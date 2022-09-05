#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "tinyusb.h"
#include "tusb_tasks.h"
#include "class/hid/hid_device.h"

#include "usb.h"
#include "config.h"


/* --------- Global Variables --------- */
TaskHandle_t xUSB_keyboard_task;
TaskHandle_t xUSB_media_task;

// Input queue for sending keyboard reports
QueueHandle_t usb_keyboard_q;
// Input queue for sending media/Consumer control reports
QueueHandle_t usb_media_q;


/* --------- Local Defines --------- */
#define TUSB_DESC_TOTAL_LEN         (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)
#define HID_ITF_PROTOCOL_CONSUMER   (3)


/* --------- Local Variables --------- */
static const char *TAG = "usb";

static bool run_tasks = true;

static const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(HID_ITF_PROTOCOL_CONSUMER))
};
static const uint8_t hid_configuration_descriptor[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, boot protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(0, 0, false, sizeof(hid_report_descriptor), 0x81, 16, 10),
};



/* --------- Local Functions --------- */



/* --------- TinyUSB callbacks --------- */
// Invoked when received GET HID REPORT DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    // We use only one interface and one HID report descriptor, so we can ignore parameter 'instance'
    return hid_report_descriptor;
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
}






void usb__init() {

    ESP_LOGI(TAG, "Init USB");
    tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = hid_configuration_descriptor,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    usb_keyboard_q = xQueueCreate(32, HID_REPORT_LEN * sizeof(uint8_t));
    usb_media_q = xQueueCreate(32, HID_CC_REPORT_LEN * sizeof(uint8_t));

    xTaskCreatePinnedToCore(usb__keyboard_task, "usb_keyboard_task", 2048, NULL, configMAX_PRIORITIES, &xUSB_keyboard_task, 0);
    xTaskCreatePinnedToCore(usb__media_task, "usb_media_task", 2048, NULL, configMAX_PRIORITIES, &xUSB_media_task, 0);
}


esp_err_t usb__deinit() {
    esp_err_t ret;

    ESP_LOGI(TAG, "Deinit USB");

    run_tasks = false; // used to stop all USB tasks

    ret = tusb_stop_task();
    if (ret) {
        ESP_LOGE(TAG, "stop tusb task failed");
        return ret;
    }

    ESP_LOGI(TAG, "Deleting USB queues");
    vQueueDelete(usb_keyboard_q);
    vQueueDelete(usb_media_q);

    ESP_LOGI(TAG, "Successful USB deinit");

    return ret;
}



void usb__keyboard_task(void *pvParameters) {

    uint8_t report[HID_REPORT_LEN];

    ESP_LOGI(TAG, "Starting usb keyboard task");

    if (usb_keyboard_q == NULL) {
        ESP_LOGW(TAG, "keyboard queue not initialised, resetting...");
        xQueueReset(usb_keyboard_q);
    }

    while (run_tasks) {
        //check if queue is initialized
        if (usb_keyboard_q != NULL) {
            //pend on MQ, if timeout triggers, just wait again.
            if (xQueueReceive(usb_keyboard_q, &report, (TickType_t) 100)) {
                //if we are not connected, discard.
                if (tud_mounted()) {
                    ESP_LOGD(TAG, "HID report:  %d,%d, 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",
                    report[0], report[1], report[2], report[3],
                    report[4], report[5], report[6], report[7]);

                    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, report[0], &(report[2]));
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

    while (run_tasks) {
        //check if queue is initialized
        if (usb_media_q != NULL) {
            //pend on MQ, if timeout triggers, just wait again.
            if (xQueueReceive(usb_media_q, &report, (TickType_t) 100)) {
                //if we are not connected, discard.
                if (tud_mounted()) {
                    uint8_t cc_report[HID_CC_REPORT_LEN] = { 0 };
                
                    if (report[1] == 1) {
                        cc_report[0] = report[0];
                    }
                    ESP_LOGD(TAG, "HID CC report:  %d,%d", cc_report[0], cc_report[1]);
                    tud_hid_report(HID_ITF_PROTOCOL_CONSUMER, cc_report, HID_CC_REPORT_LEN);
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