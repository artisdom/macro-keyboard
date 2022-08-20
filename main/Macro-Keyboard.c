#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"

#include "matrix.h"
#include "keyboard.h"
#include "memory.h"
#include "ble_hidd.h"

/* --------- Global Variables --------- */



/* --------- Local Variables --------- */
static const char *TAG = "main";



void keyboard__task(void *parameters) {

    uint8_t past_report[HID_REPORT_LEN] = {0};
    uint8_t *current_report;

    ESP_LOGI(TAG, "Starting keyboard task");

    while(1) {
        current_report = keyboard__check_state();

        // only send report if it changed
        if (memcmp(past_report, current_report, sizeof(past_report)) != 0) {
            void *report;

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


static void logging_init() {
    // ESP-IDF modules
    esp_log_level_set("*", ESP_LOG_INFO);

    // local modules
    esp_log_level_set("main", ESP_LOG_DEBUG);
    esp_log_level_set("matrix", ESP_LOG_INFO);
    esp_log_level_set("keyboard", ESP_LOG_DEBUG);
    esp_log_level_set("memory", ESP_LOG_DEBUG);
    esp_log_level_set("ble_hid", ESP_LOG_DEBUG);

}

void app_main(void) {

    logging_init();

    ESP_LOGI(TAG, "Hello");

    memory__init();
    memory__set_restart_counter();
    matrix__init();
    keyboard__init();
    if (BLE_ENABLED) {
        ble_init();
    }

    xTaskCreatePinnedToCore(keyboard__task, "keyboard task", 8192, NULL, configMAX_PRIORITIES, NULL, 1);

}
