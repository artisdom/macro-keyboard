#ifndef USB_H
#define USB_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "events.h"

// Tasks
extern TaskHandle_t xUSB_tinyusb_task;
extern TaskHandle_t xUSB_keyboard_task;
extern TaskHandle_t xUSB_media_task;

// Queues
extern QueueHandle_t usb_keyboard_q;
extern QueueHandle_t usb_media_q;


// USB device
void usb__init();
esp_err_t usb__deinit();

void usb__tinyusb_task(void *pvParameters);
void usb__keyboard_task(void *pvParameters);
void usb__media_task(void *pvParameters);

void usb__set_restart_mode(restart_type_t mode);
void usb__switch_to_cdc_jtag();
void usb__shutdown_handler();


// USB cable detection
void usb__init_detection();
bool usb__is_connected();
void usb__rtc_setup(uint8_t level_to_wakeup);
void usb__rtc_deinit();

#endif