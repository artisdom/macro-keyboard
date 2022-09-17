#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "matrix.h"
#include "keyboard.h"
#include "memory.h"
#include "leds.h"
#include "events.h"
#include "toggle_switch.h"
#include "battery.h"
#include "ble_hidd.h"
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




void keyboard_task(void *parameters) {

    uint8_t past_report[HID_REPORT_LEN] = {0};
    uint8_t *current_report;

    ESP_LOGI(TAG, "Starting keyboard task");

    while(1) {
        current_report = keyboard__check_state();

        // only send report if it changed
        if (memcmp(past_report, current_report, sizeof(past_report)) != 0) {
            DEEP_SLEEP = false;

            // if (ble_keyboard_q == NULL) {
            //     ESP_LOGE(TAG, "keyboard queue not initialized");
            //     continue;
            // }

            memcpy(past_report, current_report, sizeof(past_report));

            ESP_LOGD(TAG, "HID report:  %d,%d, 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",
                current_report[0], current_report[1], current_report[2], current_report[3],
                current_report[4], current_report[5], current_report[6], current_report[7]);

            if (BLE_ENABLED && (mode == TOGGLE_BLE)) {
                xQueueSend(ble_keyboard_q, (void *) current_report, (TickType_t) 0);
            }
            if (USB_ENABLED && (mode == TOGGLE_USB)) {
                xQueueSend(usb_keyboard_q, (void *) current_report, (TickType_t) 0);
            }

        }

        vTaskDelay(KEYBOARD_RATE / portTICK_PERIOD_MS);
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

        vTaskDelay(KEYBOARD_RATE / portTICK_PERIOD_MS);
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

                // wake up esp32 using rtc gpio
                if (BLE_ENABLED && (mode == TOGGLE_BLE) && (ble_deinit() != ESP_OK)) {
                    ESP_LOGE(TAG, "Unable to go to sleep!");
                    DEEP_SLEEP = false;
                    continue;                    
                }
                if (USB_ENABLED && (mode == TOGGLE_USB) && (usb__deinit() != ESP_OK)) {
                    ESP_LOGE(TAG, "Unable to go to sleep!");
                    DEEP_SLEEP = false;
                    continue;                    
                }
                matrix__rtc_setup();
                esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
                ESP_LOGW(TAG, "Going to sleep................!");
                esp_deep_sleep_start();
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
        memset(&event, 0x00, sizeof(event_t));

        if(xQueueReceive(event_q, &event, portMAX_DELAY)) {
            
            switch(event.type) {
                case EVENT_BT_CHANGE_DEVICE: {
                    bt_event.type = BT_EVENT_CHANGE_HOST;
                    bt_event.host_id = event.data;

                    if (BLE_ENABLED && (mode == TOGGLE_BLE)) {
                        ESP_LOGI(TAG, "Changing BLE device to %d", event.data);
                        xQueueSend(ble_event_q, &bt_event, (TickType_t) 0);
                    }
                    break;
                }
                case EVENT_BT_RESET_DEVICE: {
                    bt_event.type = BT_EVENT_RESET_HOST;
                    bt_event.host_id = event.data;

                    if (BLE_ENABLED && (mode == TOGGLE_BLE)) {
                        ESP_LOGI(TAG, "Resetting BLE device at %d", event.data);
                        xQueueSend(ble_event_q, &bt_event, (TickType_t) 0);
                    }
                    break;
                }
                case EVENT_TOGGLE_SWITCH: {
                    if ((event.data == TOGGLE_BLE) && BLE_ENABLED) {
                        mode = event.data;
                        ESP_LOGI(TAG, "Toggle BLE");
                        ble_init();
                        usb__deinit();
                    }
                    else if ((event.data == TOGGLE_USB) && USB_ENABLED) {
                        mode = event.data;
                        ESP_LOGI(TAG, "Toggle USB");
                        ble_deinit();
                        usb__init();
                    }
                    else {
                        ESP_LOGW(TAG, "Unknown toggle event type");
                    }
                    break;
                }
                case EVENT_LEDS_BRIGHTNESS: {
                    if (LED_ENABLED) {
                        if (event.data == 0) {
                            ESP_LOGI(TAG, "Leds increase brightness");
                            leds__increase_brightness();
                        }
                        else {
                            ESP_LOGI(TAG, "Leds decrease brightness");
                            leds__decrease_brightness();
                        }
                    }
                    break;
                }
                case EVENT_LEDS_BT_ADV:
                case EVENT_LEDS_BT_ADV_ALL:
                case EVENT_LEDS_BT_CONNECTED: {
                    if (LED_ENABLED && LED_EFFECTS_ENABLED) {
                        ESP_LOGI(TAG, "Leds BT event");
                        xQueueSend(leds_q, &event, (TickType_t) 0);
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

    if (USB_ENABLED) {
        // change UART default pins
        uart_config_t uart_config = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_APB,
        };

        ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 2048, 2048, 0, NULL, 0));
        ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
        ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, 0, 21, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    }

    // ESP-IDF modules
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("ledc", ESP_LOG_DEBUG);
    esp_log_level_set("TinyUSB", ESP_LOG_INFO);

    // local modules
    esp_log_level_set("main", ESP_LOG_DEBUG);
    esp_log_level_set("matrix", ESP_LOG_INFO);
    esp_log_level_set("keyboard", ESP_LOG_INFO);
    esp_log_level_set("memory", ESP_LOG_DEBUG);
    esp_log_level_set("leds", ESP_LOG_DEBUG);
    esp_log_level_set("toggle_switch", ESP_LOG_INFO);
    esp_log_level_set("battery", ESP_LOG_DEBUG);
    esp_log_level_set("usb", ESP_LOG_INFO);
    esp_log_level_set("ble_hid", ESP_LOG_DEBUG);
    esp_log_level_set("hid_le_prf", ESP_LOG_INFO);

}

void app_main(void) {

    matrix__rtc_deinit(); // first to disable interrupts

    logging_init();
    ESP_LOGI(TAG, "Hello");

    memory__init();

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1) {
        ESP_LOGI(TAG, "Wakeup from deep-sleep");
        uint64_t bit_mask = esp_sleep_get_ext1_wakeup_status();
        ESP_LOGD(TAG, "wakeup gpio bit mask: 0x%llx", bit_mask);
        memory__set_sleep_counter();
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
        ble_init();
    }
    if (USB_ENABLED && (mode == TOGGLE_USB)) {
        usb__init();
    }

    if (LED_ENABLED) {
        leds__init();
    }

    if (BATTERY_ENABLED) {
        battery__init();
        xTaskCreatePinnedToCore(battery_task, "battery task", 4096, NULL, configMAX_PRIORITIES, NULL, 1);
    }

    xTaskCreatePinnedToCore(keyboard_task, "keyboard task", 8192, NULL, configMAX_PRIORITIES, NULL, 1);
    xTaskCreatePinnedToCore(media_task, "media task", 8192, NULL, configMAX_PRIORITIES, NULL, 1);
    xTaskCreatePinnedToCore(event_handler_task, "event handler task", 8192, NULL, configMAX_PRIORITIES, NULL, 1);

    if (DEEP_SLEEP_ENABLED) {
        xTaskCreatePinnedToCore(deep_sleep_task, "deep sleep task", 4096, NULL, configMAX_PRIORITIES, NULL, 1);
    }

}
