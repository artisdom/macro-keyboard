#ifndef BLE_HIDD_H
#define BLE_HIDD_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_bt_defs.h"

// Tasks
extern TaskHandle_t xBLE_keyboard_task;
extern TaskHandle_t xBLE_media_task;
extern TaskHandle_t xBLE_battery_task;
extern TaskHandle_t xBLE_event_task;

// Queues
extern QueueHandle_t ble_keyboard_q;
extern QueueHandle_t ble_media_q;
extern QueueHandle_t ble_battery_q;
extern QueueHandle_t ble_event_q;

// Struct to store in NVS
typedef struct bt_host_s {
    esp_bd_addr_t addr;
    esp_ble_addr_type_t type;
} bt_host_t;


// Definitions for events
enum bt_event_type_e {
    BT_EVENT_CHANGE_HOST = 0x01,
    BT_EVENT_RESET_HOST,
};
typedef struct bt_event_s {
    uint8_t type;
    uint8_t host_id;    
} bt_event_t;


void ble_init(void);
esp_err_t ble_deinit(void);

void ble_keyboard_task(void *pvParameters);
void ble_media_task(void *pvParameters);
void ble_battery_task(void *pvParameters);
void ble_event_task(void *pvParameters);



#endif