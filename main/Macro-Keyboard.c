#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "tusb_console.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "matrix.h"
#include "keyboard.h"
#include "memory.h"
#include "leds.h"
#include "ble_hidd.h"


/* --------- Local Defines --------- */
#define USEC_TO_SEC 1000000
#define SEC_TO_MIN 60

#define DEEP_SLEEP_TASK_RATE (5000) //in ms


/* --------- Local Variables --------- */
static const char *TAG = "main";

static bool DEEP_SLEEP = true;



void keyboard_task(void *parameters) {

    uint8_t past_report[HID_REPORT_LEN] = {0};
    uint8_t *current_report;

    ESP_LOGI(TAG, "Starting keyboard task");

    while(1) {
        current_report = keyboard__check_state();

        // only send report if it changed
        if (memcmp(past_report, current_report, sizeof(past_report)) != 0) {
            void *report;
            DEEP_SLEEP = false;

            if (ble_keyboard_q == NULL) {
                ESP_LOGE(TAG, "keyboard queue not initialized");
                continue;
            }

            report = (void *) &current_report;
            memcpy(past_report, current_report, sizeof(past_report));

            ESP_LOGD(TAG, "HID report:  %d,%d, 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x",
                current_report[0], current_report[1], current_report[2], current_report[3],
                current_report[4], current_report[5], current_report[6], current_report[7]);

            if (BLE_ENABLED) {
                xQueueSend(ble_keyboard_q, report, (TickType_t) 0);
            }

        }

        vTaskDelay(KEYBOARD_RATE / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
    
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
                if (BLE_ENABLED && ble_deinit() != ESP_OK) {
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


static void logging_init() {

    if (!USB_ENABLED) {
        tinyusb_config_t tusb_cfg = { 0 }; // the configuration uses default values
        ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

        tinyusb_config_cdcacm_t amc_cfg = { 0 }; // the configuration uses default values
        ESP_ERROR_CHECK(tusb_cdc_acm_init(&amc_cfg));
        
        esp_tusb_init_console(TINYUSB_CDC_ACM_0); // log to usb
    }

    // ESP-IDF modules
    esp_log_level_set("*", ESP_LOG_INFO);

    // local modules
    esp_log_level_set("main", ESP_LOG_DEBUG);
    esp_log_level_set("matrix", ESP_LOG_INFO);
    esp_log_level_set("keyboard", ESP_LOG_INFO);
    esp_log_level_set("memory", ESP_LOG_DEBUG);
    esp_log_level_set("leds", ESP_LOG_DEBUG);
    esp_log_level_set("ble_hid", ESP_LOG_DEBUG);

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
        memory__set_restart_counter();
    }

    
    matrix__init();
    keyboard__init();
    if (BLE_ENABLED) {
        ble_init();
    }

    if (LED_ENABLED) {
        leds__init();
    }

    xTaskCreatePinnedToCore(keyboard_task, "keyboard task", 8192, NULL, configMAX_PRIORITIES, NULL, 1);
    if (DEEP_SLEEP_ENABLED) {
        xTaskCreatePinnedToCore(deep_sleep_task, "deep sleep task", 4096, NULL, configMAX_PRIORITIES, NULL, 1);
    }

}
