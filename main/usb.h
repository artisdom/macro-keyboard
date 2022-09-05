#ifndef USB_H
#define USB_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Tasks
extern TaskHandle_t xUSB_keyboard_task;
extern TaskHandle_t xUSB_media_task;

// Queues
extern QueueHandle_t usb_keyboard_q;
extern QueueHandle_t usb_media_q;



void usb__init();
esp_err_t usb__deinit(void);

void usb__keyboard_task(void *pvParameters);
void usb__media_task(void *pvParameters);






#endif