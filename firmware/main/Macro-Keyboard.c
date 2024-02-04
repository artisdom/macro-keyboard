#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "matrix.h"
#include "keyboard.h"
#include "memory.h"
#include "leds.h"
#include "events.h"
#include "toggle_switch.h"
#include "battery.h"
#include "ble.h"
#include "usb.h"


/* --------- Global Variables --------- */
QueueHandle_t event_q;
QueueHandle_t media_q;


/* --------- Local Defines --------- */
#define USEC_TO_SEC 1000000
#define SEC_TO_MIN 60

#define DEEP_SLEEP_TASK_RATE (5000) //in ms
#define BATTERY_TASK_RATE (DEEP_SLEEP_TIMEOUT * 1000 / 2)  //in ms


/* --------- Local Variables --------- */
static const char *TAG = "main";

static bool DEEP_SLEEP = true;

static uint8_t mode = TOGGLE_BLE;
static bool usb_connected;


/* --------- Local Functons --------- */
static void sleep(bool rtc_keyboard, bool rtc_toggle_switch, bool rtc_usb, bool rtc_usb_direction);
static void restart(restart_type_t type);


void keyboard_task(void *parameters) {

    uint8_t past_report[HID_REPORT_LEN] = {0};
    uint8_t *current_report;

    ESP_LOGI(TAG, "Starting keyboard task");

    while(1) {
        current_report = keyboard__check_state();

        // only send report if it changed
        if (memcmp(past_report, current_report, sizeof(past_report)) != 0) {
            DEEP_SLEEP = false;

            memcpy(past_report, current_report, sizeof(past_report));

            ESP_LOGD(TAG, "HID report:  0x%x,0x%x,  0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",
                current_report[0], current_report[1], current_report[2], current_report[3],
                current_report[4], current_report[5], current_report[6], current_report[7]);

            if (BLE_ENABLED && (mode == TOGGLE_BLE)) {
                xQueueSend(ble_keyboard_q, (void *) current_report, (TickType_t) 0);
            }
            if (USB_ENABLED && (mode == TOGGLE_USB)) {
                xQueueSend(usb_keyboard_q, (void *) current_report, (TickType_t) 0);
            }

        }

        if (usb_connected == true) {
            vTaskDelay(KEYBOARD_RATE / portTICK_PERIOD_MS);
        }
        else {
            vTaskDelay(KEYBOARD_BLE_RATE / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
    
}


void media_task(void *pvParameters) {

    uint8_t report[HID_CC_REPORT_LEN];
    uint8_t past_report[HID_CC_REPORT_LEN];

    ESP_LOGI(TAG, "Starting media task");

    while(1) {

        if (xQueueReceive(media_q, &report, (TickType_t) 100)) {
            // only send report if it changed
            if(memcmp(past_report, report, sizeof(past_report)) != 0) {
                DEEP_SLEEP = false;
                memcpy(past_report, report, sizeof(past_report));

                ESP_LOGD(TAG, "HID CC report:  %d,%d", report[0], report[1]);

                if (BLE_ENABLED && (mode == TOGGLE_BLE)) {
                    xQueueSend(ble_media_q, (void *) report, (TickType_t) 0);
                }
                if (USB_ENABLED && (mode == TOGGLE_USB)) {
                    xQueueSend(usb_media_q, (void *) report, (TickType_t) 0);
                }

            }    
        }

        if (usb_connected == true) {
            vTaskDelay(KEYBOARD_RATE / portTICK_PERIOD_MS);
        }
        else {
            vTaskDelay(KEYBOARD_BLE_RATE / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);

}


void battery_task(void *pvParameters) {

    ESP_LOGI(TAG, "Starting battery task");

    while(1) {
        uint32_t battery_level = battery__get_level();

        if (battery__is_charging()) {
            // if charging, disable DEEP_SLEEP
            DEEP_SLEEP = false;
        }

        if (BLE_ENABLED && (mode == TOGGLE_BLE)) {
            xQueueSend(ble_battery_q, &battery_level, (TickType_t) 0);
        }

        vTaskDelay(BATTERY_TASK_RATE / portTICK_PERIOD_MS);
    }

}


void deep_sleep_task(void *pvParameters) {
    uint64_t initial_time = esp_timer_get_time(); // time in micro-seconds
    uint64_t current_time_passed = 0;

    ESP_LOGI(TAG, "Starting deep sleep task");

    while (1) {
        current_time_passed = (esp_timer_get_time() - initial_time);

        if (DEEP_SLEEP == false) {
            current_time_passed = 0;
            initial_time = esp_timer_get_time();
            DEEP_SLEEP = true;
        }

        if (((double)current_time_passed/USEC_TO_SEC) >= (double)(DEEP_SLEEP_TIMEOUT)) {
            if (DEEP_SLEEP == true) {
                ESP_LOGW(TAG, "Deep sleep triggered");

                // deinits
                if (BLE_ENABLED && (mode == TOGGLE_BLE) && (ble__deinit() != ESP_OK)) {
                    ESP_LOGE(TAG, "Unable to go to sleep!");
                    DEEP_SLEEP = false;
                    continue;                    
                }
                if (USB_ENABLED && (mode == TOGGLE_USB) && (usb__deinit() != ESP_OK)) {
                    ESP_LOGE(TAG, "Unable to go to sleep!");
                    DEEP_SLEEP = false;
                    continue;                    
                }

                sleep(true, false, false, 0);
            }
            if (DEEP_SLEEP == false) {
                current_time_passed = 0;
                initial_time = esp_timer_get_time();
                DEEP_SLEEP = true;
            }
        }
        vTaskDelay(DEEP_SLEEP_TASK_RATE / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);

}


void event_handler_task(void *parameters) {

    bt_event_t bt_event;

    ESP_LOGI(TAG, "Starting event handler task");

    if (event_q == NULL) {
        ESP_LOGW(TAG, "event queue not initialised, resetting...");
        xQueueReset(event_q);
    }

    while(1) {
        event_t event;
        event_t new_event;
        memset(&event, 0x00, sizeof(event_t));

        if(xQueueReceive(event_q, &event, portMAX_DELAY)) {
            
            switch(event.type) {
                case EVENT_KB_CHANGE_BT_HOST: {
                    bt_event.type = BT_EVENT_CHANGE_HOST;
                    bt_event.host_id = event.data;

                    if (BLE_ENABLED && (mode == TOGGLE_BLE)) {
                        ESP_LOGI(TAG, "Changing BLE device to %d", event.data);
                        xQueueSend(ble_event_q, &bt_event, (TickType_t) 0);
                    }
                    break;
                }
                case EVENT_KB_RESET_BT_HOST: {
                    bt_event.type = BT_EVENT_RESET_HOST;
                    bt_event.host_id = event.data;

                    if (BLE_ENABLED && (mode == TOGGLE_BLE)) {
                        ESP_LOGI(TAG, "Resetting BLE device at %d", event.data);
                        xQueueSend(ble_event_q, &bt_event, (TickType_t) 0);
                    }
                    break;
                }
                case EVENT_TOGGLE_SWITCH: {
                    if ((event.data == TOGGLE_BLE)) {
                        // ESP_LOGW(TAG, "event toggle ble");
                        if (USB_ENABLED) {
                            usb__deinit();
                        }
                        if (BLE_ENABLED) {
                            ESP_LOGI(TAG, "Toggle BLE");
                            if (LED_ENABLED && LED_EFFECTS_ENABLED) {
                                new_event.type = EVENT_BT_LEDS_EFFECT_TOGGLE,
                                new_event.data = true;
                                xQueueSend(leds_q, &new_event, (TickType_t) 0);
                            }
                            mode = event.data;
                            ble__init();
                        }
                    }
                    else if ((event.data == TOGGLE_USB)) {
                        // ESP_LOGW(TAG, "event toggle usb");
                        usb_connected = usb__is_connected();
                        if (BLE_ENABLED) {
                            if (LED_ENABLED && LED_EFFECTS_ENABLED) {
                                new_event.type = EVENT_BT_LEDS_EFFECT_TOGGLE,
                                new_event.data = false;
                                xQueueSend(leds_q, &new_event, (TickType_t) 0);
                            }
                            ble__deinit();
                        }
                        if (USB_ENABLED) {
                            if (usb_connected) {
                                ESP_LOGI(TAG, "Toggle USB");
                                mode = event.data;
                                usb__init();
                            }
                            else {
                                sleep(false, true, true, !usb_connected);
                            }
                        }
                        else {
                            sleep(false, true, false, 0);
                        }
                    }
                    else {
                        ESP_LOGW(TAG, "Unknown toggle event type %d", event.data);
                    }
                    break;
                }
                case EVENT_KB_LEDS: {
                    if (LED_ENABLED) {
                        switch (event.data) {
                            case LEDS_ON:
                                ESP_LOGD(TAG, "Leds on");
                                leds__enable_backlight(true);
                                break;
                            case LEDS_OFF:
                                ESP_LOGD(TAG, "Leds off");
                                leds__enable_backlight(false);
                                break;
                            case LEDS_TOGGLE:
                                ESP_LOGD(TAG, "Leds toggle");
                                leds__toggle_backlight();
                                break;
                            case LEDS_INCREMENT:
                                ESP_LOGD(TAG, "Leds increase brightness");
                                leds__increase_brightness();
                                break;
                            case LEDS_DECREMENT:
                                ESP_LOGD(TAG, "Leds decrease brightness");
                                leds__decrease_brightness();
                                break;
                            default:
                                ESP_LOGE(TAG, "Unrecognised KB_LEDS event data %d", event.data);
                                break;
                        }
                    }
                    break;
                }
                case EVENT_BT_ADV:
                case EVENT_BT_ADV_ALL:
                case EVENT_BT_CONNECTED:
                case EVENT_LAYERS_CHANGED: {
                    if (LED_ENABLED && LED_BT_EFFECTS_ENABLED) {
                        ESP_LOGI(TAG, "Leds FX event");
                        xQueueSend(leds_q, &event, (TickType_t) 0);
                    }
                    break;
                }
                case EVENT_USB_PORT: {
                    vTaskDelay(1 / portTICK_PERIOD_MS); // delay to wait for voltage to stabilise on pin
                    usb_connected = usb__is_connected();
                    if (usb_connected == true) {
                        ESP_LOGI(TAG, "USB Connected");
                    }
                    else {
                        ESP_LOGI(TAG, "USB Disconnected");
                        if (mode == TOGGLE_USB) {
                            sleep(false, true, true, 1);
                        }

                    }
                    break;
                }
                case EVENT_RESTART: {
                    if (event.data > 0) {
                        restart((restart_type_t) event.data);
                    }
                    else {
                        ESP_LOGE(TAG, "Unrecognised EVENT_RESTART event data %d", event.data);
                    }
                    break;
                }
                default:
                    ESP_LOGW(TAG, "Unhandled event type");
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}


static void logging_init() {

    // ESP-IDF modules
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("ledc", ESP_LOG_INFO);
    esp_log_level_set("TinyUSB", ESP_LOG_INFO);

    // local modules
    esp_log_level_set("main", ESP_LOG_DEBUG);
    esp_log_level_set("matrix", ESP_LOG_INFO);
    esp_log_level_set("keyboard", ESP_LOG_INFO);
    esp_log_level_set("layers", ESP_LOG_INFO);
    esp_log_level_set("dynamic_keymap", ESP_LOG_INFO);
    esp_log_level_set("memory", ESP_LOG_INFO);
    esp_log_level_set("leds", ESP_LOG_INFO);
    esp_log_level_set("toggle_switch", ESP_LOG_INFO);
    esp_log_level_set("battery", ESP_LOG_INFO);
    esp_log_level_set("usb", ESP_LOG_DEBUG);
    esp_log_level_set("via", ESP_LOG_INFO);
    esp_log_level_set("ble", ESP_LOG_INFO);
    esp_log_level_set("hid_le_prf", ESP_LOG_INFO);

}


void app_main(void) {

    // run first to disable interrupts
    matrix__rtc_deinit();
    toggle_switch__rtc_deinit();
    usb__rtc_deinit();

    logging_init();
    ESP_LOGI(TAG, "Hello");

    memory__init();

    esp_sleep_wakeup_cause_t wakeup = esp_sleep_get_wakeup_cause();
    if (wakeup == ESP_SLEEP_WAKEUP_EXT1) {
        ESP_LOGI(TAG, "Wakeup from deep-sleep");
        uint64_t bit_mask = esp_sleep_get_ext1_wakeup_status();
        ESP_LOGD(TAG, "wakeup gpio bit mask: 0x%llx", bit_mask);
        memory__set_sleep_counter();
    }
    else if (wakeup == ESP_SLEEP_WAKEUP_EXT0) {
         ESP_LOGI(TAG, "Wakeup from USB bus GPIO");
         if (memory__get_sleep_status() == true) {
            if (USB_ENABLED) {
                sleep(false, true, true, 1);
            }
            else {
                sleep(false, true, false, 0);
            }
         }
    }
    else {
        ESP_LOGI(TAG, "Wakeup from normal boot");
        ESP_LOGD(TAG, "wakeup cause: %d", esp_sleep_get_wakeup_cause());
        memory__set_restart_counter();
    }

    event_q = xQueueCreate(32, sizeof(event_t));
    media_q = xQueueCreate(32, HID_CC_REPORT_LEN * sizeof(uint8_t));

    matrix__init();
    keyboard__init();

    if (TOGGLE_SWITCH_ENABLED) {
        toggle_switch__init();
        mode = toggle_switch__get_state();
    }
    if (BLE_ENABLED && (mode == TOGGLE_BLE)) {
        ble__init();
    }
    if (USB_ENABLED && (mode == TOGGLE_USB)) {
        usb__init();
    }

    if (USB_DETECT_ENABLED) {
        usb__init_detection();
        usb_connected = usb__is_connected();
    }

    if (LED_ENABLED) {
        leds__init();
    }

    if (BATTERY_ENABLED) {
        battery__init();
        xTaskCreatePinnedToCore(battery_task, "battery task", BATTERY_TASK_STACK, NULL, BATTERY_TASK_PRIORITY, NULL, 1);
    }

    xTaskCreatePinnedToCore(keyboard_task, "keyboard task", KEYBOARD_TASK_STACK, NULL, KEYBOARD_TASK_PRIORITY, NULL, 1);
    xTaskCreatePinnedToCore(media_task, "media task", MEDIA_TASK_STACK, NULL, MEDIA_TASK_PRIORITY, NULL, 1);
    xTaskCreatePinnedToCore(event_handler_task, "event handler task", EVENT_HANDLER_TASK_STACK, NULL, EVENT_HANDLER_TASK_PRIORITY, NULL, 1);

    if (DEEP_SLEEP_ENABLED) {
        xTaskCreatePinnedToCore(deep_sleep_task, "deep sleep task", DEEP_SLEEP_TASK_STACK, NULL, DEEP_SLEEP_TASK_PRIORITY, NULL, 1);
    }

}



static void sleep(bool rtc_keyboard, bool rtc_toggle_switch, bool rtc_usb, bool rtc_usb_direction) {

    ESP_LOGD(TAG, "Setting up for sleep...");
    if (rtc_keyboard) {
        matrix__rtc_setup();
    }
    if (rtc_toggle_switch && TOGGLE_SWITCH_ENABLED) {
        toggle_switch__rtc_setup();
    }
    if (rtc_usb && USB_DETECT_ENABLED) {
        memory__set_sleep_status(!rtc_usb_direction);
        usb__rtc_setup(rtc_usb_direction);
    }

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    ESP_LOGW(TAG, "Going to sleep................!");
    esp_deep_sleep_start();

}


static void restart(restart_type_t type) {

    ESP_LOGD(TAG, "Setting up for restart...");

    usb__set_restart_mode(type);
    if (type >= RESTART_BOOTLOADER) {
        ESP_ERROR_CHECK(esp_register_shutdown_handler(usb__shutdown_handler));
        usb__switch_to_cdc_jtag();
    }
    if (type == RESTART_USB) {
        usb__switch_to_cdc_jtag();
        ESP_LOGW(TAG, "Skipping restart");
        return;
    }

    ESP_LOGW(TAG, "Restarting................!");
    esp_restart();
}


