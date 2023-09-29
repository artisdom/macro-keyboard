#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "tinyusb.h"
#include "tinyusb_types.h"
#include "class/hid/hid_device.h"
#include "tusb_cdc_acm.h"
#include "tusb_console.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "soc/usb_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "esp32s3/rom/usb/usb_device.h"
#include "esp32s3/rom/usb/usb_persist.h"
#include "esp32s3/rom/usb/chip_usb_dw_wrapper.h"

#include "usb.h"
#include "config.h"
#include "events.h"
#include "via.h"


/* --------- Global Variables --------- */
TaskHandle_t xUSB_tinyusb_task;
TaskHandle_t xUSB_keyboard_task;
TaskHandle_t xUSB_media_task;

// Input queue for sending keyboard reports
QueueHandle_t usb_keyboard_q;
// Input queue for sending media/Consumer control reports
QueueHandle_t usb_media_q;


/* --------- Local Defines --------- */
/*
 * A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_TUSB_PID (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
    _PID_MAP(MIDI, 3) ) //| _PID_MAP(AUDIO, 4) | _PID_MAP(VENDOR, 5) )

// #define TUSB_DESC_TOTAL_LEN         (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)
// #define TUSB_DESC_TOTAL_LEN         (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_INOUT_DESC_LEN)
#define TUSB_DESC_TOTAL_LEN         (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_INOUT_DESC_LEN + TUD_CDC_DESC_LEN)
#define HID_REPORT_ID_KEYBOARD   (1)
#define HID_REPORT_ID_CONSUMER   (2)

#define ESP_INTR_FLAG_DEFAULT       (0)
#define USB_GPIO_DEBOUNCE_TIME      (4000) // in us

// Specific to VIA
#define RAW_USAGE_PAGE           0xFF60
#define RAW_USAGE_ID             0x61
#define RAW_USAGE_ID_IN          0x62
#define RAW_USAGE_ID_OUT         0x63

#define RAW_REPORT_SIZE          32

// HID Raw report descriptor
// - 1st parameter is report size (mandatory)
// - 2nd parameter is report id HID_REPORT_ID(n) (optional)
#define TUD_HID_REPORT_DESC_RAW(report_size, ...) \
    HID_USAGE_PAGE_N ( RAW_USAGE_PAGE, 2   ),\
    HID_USAGE        ( RAW_USAGE_ID        ),\
    HID_COLLECTION   ( HID_COLLECTION_APPLICATION ),\
      /* Report ID if any */\
      __VA_ARGS__ \
      /* Input */ \
      HID_USAGE       ( RAW_USAGE_ID_IN                        ),\
      HID_LOGICAL_MIN ( 0x00                                   ),\
      HID_LOGICAL_MAX_N ( 0xff, 2                              ),\
      HID_REPORT_SIZE ( 8                                      ),\
      HID_REPORT_COUNT( report_size                            ),\
      HID_INPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),\
      /* Output */ \
      HID_USAGE       ( RAW_USAGE_ID_OUT                        ),\
      HID_LOGICAL_MIN ( 0x00                                    ),\
      HID_LOGICAL_MAX_N ( 0xff, 2                               ),\
      HID_REPORT_SIZE ( 8                                       ),\
      HID_REPORT_COUNT( report_size                             ),\
      HID_OUTPUT      ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ),\
    HID_COLLECTION_END \


/* --------- Local Declarations --------- */
enum {
    // Interface numbers
    ITF_NUM_HID = 0,
    ITF_NUM_HID_RAW,
    ITF_NUM_CDC,
    ITF_NUM_CDC_DATA,
    ITF_NUM_TOTAL
};

enum {
    // Available USB Endpoints: 5 IN/OUT EPs and 1 IN EP
    EP_EMPTY = 0,
    EPNUM_0_HID,
    EPNUM_1_HID,
    EPNUM_2_CDC_NOTIF,
    EPNUM_2_CDC,
};


typedef enum {
    REBOOT_NONE,
    REBOOT_NORMAL,
    REBOOT_BOOTLOADER,
} reboot_type_t;


/* --------- Local Variables --------- */
static const char *TAG = "usb";

static bool initialized = false;
static const gpio_num_t usb_bus_gpio = GPIO_NUM_21;
static int prev_rts_state = 0;
static reboot_type_t reboot_state = REBOOT_NONE;

static const tusb_desc_device_t descriptor_tinyusb = {
    .bLength = sizeof(descriptor_tinyusb),      // Size of this descriptor in bytes.
    .bDescriptorType = TUSB_DESC_DEVICE,        // DEVICE Descriptor Type.
    .bcdUSB = 0x0200,                           // BUSB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H).

    .bDeviceClass = TUSB_CLASS_MISC,            // Class code (assigned by the USB-IF).
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,    // Subclass code (assigned by the USB-IF).
    .bDeviceProtocol = MISC_PROTOCOL_IAD,       // Protocol code (assigned by the USB-IF).
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,  // Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid). For HS devices is fixed to 64.

    .idVendor = USB_ESPRESSIF_VID,              // Vendor ID (assigned by the USB-IF).
    .idProduct = USB_TUSB_PID,                  // Product ID (assigned by the manufacturer).
    .bcdDevice = 0x0100,                        // Device release number in binary-coded decimal.
    .iManufacturer = 0x01,                      // Index of string descriptor describing manufacturer.
    .iProduct = 0x02,                           // Index of string descriptor describing product.
    .iSerialNumber = 0x03,                      // Index of string descriptor describing the device's serial number.

    .bNumConfigurations = 0x01                  // Number of possible configurations.
};

// array of pointer to string descriptors
static tusb_desc_strarray_device_t string_descriptor = {
    (char[]){0x09, 0x04},                       // 0: supported language is English (0x0409)
    USB_MANUFACTURER_NAME,                      // 1: Manufacturer
    USB_DEVICE_NAME,                            // 2: Product
    "123456",                                   // 3: Serials, should use chip ID
    USB_HID_NAME,                               // 4: HID Interface
    USB_CDC_NAME,                               // 5: CDC Interface
    USB_MIDI_NAME                               // 6: MIDI
};

static const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_REPORT_ID_KEYBOARD)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(HID_REPORT_ID_CONSUMER))
};

static const uint8_t hid_raw_report_descriptor[] = {
    TUD_HID_REPORT_DESC_RAW(RAW_REPORT_SIZE)
};


static const uint8_t configuration_descriptor[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, boot protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 4, false, sizeof(hid_report_descriptor), 0x80 | EPNUM_0_HID, 16, 10),

    // Interface number, string index, protocol, report descriptor len, EP OUT & IN address, size & polling interval
    TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID_RAW, 4, false, sizeof(hid_raw_report_descriptor), EPNUM_1_HID, 0x80 | EPNUM_1_HID, 16, 10),

    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 5, 0x80 | EPNUM_2_CDC_NOTIF, 8, EPNUM_2_CDC, 0x80 | EPNUM_2_CDC, CFG_TUD_CDC_EP_BUFSIZE),
};

/* --------- Local Functions --------- */
static void usb__init_gpio();
static void usb__gpio_isr_handler(void *arg);



/* --------- TinyUSB callbacks --------- */
// Invoked when received GET HID REPORT DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    // We use only one interface and one HID report descriptor, so we can ignore parameter 'instance'
    // return hid_report_descriptor;
    ESP_LOGD(TAG, "tud_hid_descriptor_report_cb with instance %d", instance);
    if (instance == ITF_NUM_HID) {
        return hid_report_descriptor;
    }
    else if (instance == ITF_NUM_HID_RAW) {
        return hid_raw_report_descriptor;
    }
    else {
        ESP_LOGW(TAG, "Unhandled interface number %d", instance);
    }
    return NULL;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    ESP_LOGD(TAG, "instance:%d report_id:%d report_type:%d, len:%d", instance, report_id, report_type, bufsize);
    if (instance == ITF_NUM_HID_RAW) {
        uint8_t *new_buffer = (uint8_t *) malloc(bufsize);
        memcpy(new_buffer, buffer, bufsize); 
        via__hid_receive(new_buffer, bufsize);

        // send back modified buffer to host
        tud_hid_n_report(ITF_NUM_HID_RAW, 0, new_buffer, bufsize);
        free(new_buffer);
    }
}

void IRAM_ATTR tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rts = event->line_state_changed_data.rts;
    ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
    if (!rts && prev_rts_state) {
        if (dtr) {
            ESP_LOGW(TAG, "Reboot in bootloader");
            reboot_state = REBOOT_BOOTLOADER;
        }
        else {
            ESP_LOGW(TAG, "Reboot normal");
            reboot_state = REBOOT_NORMAL;
        }
    }
    prev_rts_state = rts;
}

void IRAM_ATTR tinyusb_cdc_callback(int itf, cdcacm_event_t *event) {
    if (event->type == CDC_EVENT_RX) {
        ESP_LOGD(TAG, "CDC_EVENT_RX");
    }
    else if (event->type == CDC_EVENT_RX_WANTED_CHAR) {
        ESP_LOGD(TAG, "CDC_EVENT_RX_WANTED_CHAR");
    }
    else if (event->type == CDC_EVENT_LINE_CODING_CHANGED) {
        ESP_LOGD(TAG, "CDC_EVENT_LINE_CODING_CHANGED");
        prev_rts_state = 0;
    }
}

void restart_handler(void) {

    usb_dc_prepare_persist();
    if (reboot_state == REBOOT_BOOTLOADER) {
        chip_usb_set_persist_flags(USBDC_PERSIST_ENA);
        REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
    }
    else if (reboot_state == REBOOT_NORMAL) {
        chip_usb_set_persist_flags(USBDC_PERSIST_ENA);
    }

}



void usb__init() {
    esp_err_t ret;

    ESP_LOGI(TAG, "Init USB");

    if (initialized == false) {
        tinyusb_config_t tusb_cfg = {
            .device_descriptor = &descriptor_tinyusb,
            .string_descriptor = string_descriptor,
            .external_phy = false,
            .configuration_descriptor = configuration_descriptor,
            .self_powered = true,
            .vbus_monitor_io = GPIO_NUM_33,
        };

        ret = tinyusb_driver_install(&tusb_cfg);
        if (ret) {
            ESP_LOGE(TAG, "install tinyusb driver failed");
            return;
        }
        initialized = true;

        tinyusb_config_cdcacm_t acm_cfg = {
            .usb_dev = TINYUSB_USBDEV_0,
            .cdc_port = TINYUSB_CDC_ACM_0,
            .rx_unread_buf_sz = 64,
            .callback_rx = &tinyusb_cdc_callback,
            .callback_rx_wanted_char = &tinyusb_cdc_callback,
            .callback_line_state_changed = &tinyusb_cdc_line_state_changed_callback,
            .callback_line_coding_changed = &tinyusb_cdc_callback,
        };
        ret = tusb_cdc_acm_init(&acm_cfg);
        if (ret) {
            ESP_LOGE(TAG, "init CDC ACM driver failed");
        } 
        else {
            // ret = esp_tusb_init_console(TINYUSB_CDC_ACM_0);
            // if (ret) {
            //     ESP_LOGE(TAG, "init console to redirect output to CDC failed");
            // }
            // else {
            //     ESP_LOGI(TAG, "init tusb console");
            // }
            // ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
            //             TINYUSB_CDC_ACM_0,
            //             CDC_EVENT_LINE_STATE_CHANGED,
            //             &tinyusb_cdc_line_state_changed_callback));
        }
    }
    tud_connect();

    usb_keyboard_q = xQueueCreate(32, HID_REPORT_LEN * sizeof(uint8_t));
    usb_media_q = xQueueCreate(32, HID_CC_REPORT_LEN * sizeof(uint8_t));

    xTaskCreatePinnedToCore(usb__tinyusb_task, "usb_tinyusb_task", USB_TINYUSB_TASK_STACK, NULL, USB_TINYUSB_TASK_PRIORITY, &xUSB_tinyusb_task, 0);
    xTaskCreatePinnedToCore(usb__keyboard_task, "usb_keyboard_task", USB_KEYBOARD_TASK_STACK, NULL, USB_KEYBOARD_TASK_PRIORITY, &xUSB_keyboard_task, 0);
    xTaskCreatePinnedToCore(usb__media_task, "usb_media_task", USB_MEDIA_TASK_STACK, NULL, USB_MEDIA_TASK_PRIORITY, &xUSB_media_task, 0);
}


esp_err_t usb__deinit() {
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Deinit USB");

    tud_disconnect();

    ESP_LOGI(TAG, "Deleting USB tasks");
    if (xUSB_tinyusb_task != NULL) {
        ESP_LOGW(TAG, "Stopping tinyusb task");
        vTaskDelete(xUSB_tinyusb_task);
        xUSB_tinyusb_task = NULL;
    }
    if (xUSB_keyboard_task != NULL) {
        ESP_LOGW(TAG, "Stopping keyboard task");
        vTaskDelete(xUSB_keyboard_task);
        xUSB_keyboard_task = NULL;
    }
    if (xUSB_media_task != NULL) {
        ESP_LOGW(TAG, "Stopping media task");
        vTaskDelete(xUSB_media_task);
        xUSB_media_task = NULL;
    }

    ESP_LOGI(TAG, "Deleting USB queues");
    vQueueDelete(usb_keyboard_q);
    vQueueDelete(usb_media_q);

    ESP_LOGI(TAG, "Successful USB deinit");

    return ret;
}



void usb__tinyusb_task(void *pvParameters){

    ESP_LOGI(TAG, "Starting tinyusb task");

    esp_err_t ret = esp_register_shutdown_handler(restart_handler);
        if (ret) {
            ESP_LOGE(TAG, "Error init shutdown handler");
        }

    while (1) {
        tud_task();

        if (reboot_state != REBOOT_NONE) {
            ESP_LOGW(TAG, "REBOOOT");
            esp_restart();
        }
    }
}


void usb__keyboard_task(void *pvParameters) {

    uint8_t report[HID_REPORT_LEN];

    ESP_LOGI(TAG, "Starting usb keyboard task");

    if (usb_keyboard_q == NULL) {
        ESP_LOGW(TAG, "keyboard queue not initialised, resetting...");
        xQueueReset(usb_keyboard_q);
    }

    while (1) {
        //check if queue is initialized
        if (usb_keyboard_q != NULL) {
            //pend on MQ, if timeout triggers, just wait again.
            if (xQueueReceive(usb_keyboard_q, &report, (TickType_t) 100)) {
                //if we are not connected, discard.
                if (tud_ready()) {
                    // ESP_LOGD(TAG, "HID report:  %d,%d, 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",
                    // report[0], report[1], report[2], report[3],
                    // report[4], report[5], report[6], report[7]);

                    tud_hid_keyboard_report(HID_REPORT_ID_KEYBOARD, report[0], &(report[2]));
                }
                else {
                    ESP_LOGD(TAG, "Tinyusb ready: %d", tud_ready());
                    ESP_LOGD(TAG, "Tinyusb connected: %d", tud_connected());
                    ESP_LOGD(TAG, "Tinyusb mounted: %d", tud_mounted());
                    ESP_LOGD(TAG, "Tinyusb suspended: %d", tud_suspended());
                }
            }
        }
        else {
            ESP_LOGE(TAG, "keyboard queue not initialized, retry in 1s");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    ESP_LOGW(TAG, "Stopping usb keyboard task");
    vTaskDelete(NULL);

}


void usb__media_task(void *pvParameters) {

    uint8_t report[HID_CC_REPORT_LEN];

    ESP_LOGI(TAG, "Starting usb media task");

    if (usb_media_q == NULL) {
        ESP_LOGW(TAG, "media queue not initialised, resetting...");
        xQueueReset(usb_media_q);
    }

    while (1) {
        //check if queue is initialized
        if (usb_media_q != NULL) {
            //pend on MQ, if timeout triggers, just wait again.
            if (xQueueReceive(usb_media_q, &report, (TickType_t) 100)) {
                //if we are not connected, discard.
                if (tud_ready()) {
                    uint8_t cc_report[HID_CC_REPORT_LEN] = { 0 };
                
                    if (report[1] == 1) {
                        cc_report[0] = report[0];
                    }
                    ESP_LOGD(TAG, "HID CC report:  %d,%d", cc_report[0], cc_report[1]);
                    tud_hid_report(HID_REPORT_ID_CONSUMER, cc_report, HID_CC_REPORT_LEN);
                }
            }
        }
        else {
            ESP_LOGE(TAG, "media queue not initialized, retry in 1s");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    ESP_LOGW(TAG, "Stopping usb media task");
    vTaskDelete(NULL);

}



// --------- USB Detection ---------
// Bus Voltage is routed to a RTC GPIO pin to when cable is plugged in

static void usb__init_gpio() {

    uint64_t pin_mask = (1ULL << usb_bus_gpio);

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = pin_mask,
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

}


void usb__init_detection() {

    ESP_LOGI(TAG, "Init USB Detection");

    usb__init_gpio();

    //install gpio isr service
    // ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
    //hook isr handler for specific gpio pin
    ESP_ERROR_CHECK(gpio_isr_handler_add(usb_bus_gpio, usb__gpio_isr_handler, (void*) NULL));
}


bool usb__is_connected() {

    uint8_t level = gpio_get_level(usb_bus_gpio);
    ESP_LOGD(TAG, "USB Bus voltage is %d", level);

    if (level) {
        return true;
    }
    return false;
}


bool usb__is_pin_mask(uint64_t pin_mask) {

    if (pin_mask & (1ull << usb_bus_gpio)) {
        return true;
    }
    return false;
}


static void IRAM_ATTR usb__gpio_isr_handler(void *arg) {
    static int64_t last_interrupt;
    int64_t time;
    event_t event = {
        .type = EVENT_USB_PORT,
        .data = 0,
    };

    time = esp_timer_get_time();
    if (time - last_interrupt > USB_GPIO_DEBOUNCE_TIME) {
            xQueueSendFromISR(event_q, &event, (TickType_t) 0);
    }
    last_interrupt = time;
}


void usb__rtc_setup(uint8_t level_to_wakeup) {
    uint64_t rtc_mask = 0;

    ESP_LOGI(TAG, "Init RTC USB bus for deep sleep");

    gpio_num_t pin = usb_bus_gpio;
    if (rtc_gpio_is_valid_gpio(pin) == 1) {
        rtc_gpio_init(pin);
        rtc_gpio_set_direction(pin, RTC_GPIO_MODE_INPUT_ONLY);
        rtc_gpio_wakeup_enable(pin, GPIO_INTR_HIGH_LEVEL);

        rtc_mask |= 1llu << pin;
    }
    else {
        ESP_LOGW(TAG, "gpio %d is not a valid RTC pin", pin);
    }

    // Wakeup on specified level
    esp_sleep_enable_ext0_wakeup(usb_bus_gpio, level_to_wakeup);
}


void usb__rtc_deinit() {

    gpio_num_t pin = usb_bus_gpio;
    if (rtc_gpio_is_valid_gpio(pin) == 1) {
        rtc_gpio_set_direction(pin, RTC_GPIO_MODE_DISABLED);
        rtc_gpio_deinit(pin);
        gpio_reset_pin(pin);
    }
}


