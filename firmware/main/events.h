#ifndef MACRO_KEYBOARD_H
#define MACRO_KEYBOARD_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Contains declarations used by the event handler task

typedef enum event_type_e {
    EVENT_KB_CHANGE_BT_HOST = 0x01,
    EVENT_KB_RESET_BT_HOST,
    EVENT_KB_LEDS,
    EVENT_LAYERS_CHANGED,
    EVENT_TOGGLE_SWITCH,
    EVENT_BT_ADV,
    EVENT_BT_ADV_ALL,
    EVENT_BT_CONNECTED,
    EVENT_BT_LEDS_EFFECT_TOGGLE,
    EVENT_USB_PORT,
    EVENT_RESTART,

    EVENT_MAX = 0xFF,
} event_type_t;


// Event specific Data values
// EVENT_KB_LEDS
typedef enum {
    LEDS_ON = 0x00,
    LEDS_OFF,
    LEDS_TOGGLE,
    LEDS_DECREMENT,
    LEDS_INCREMENT,
} led_control_t;


// EVENT_RESTART
typedef enum {
    RESTART_APP = 0x00,
    RESTART_USB,
    RESTART_BOOTLOADER,
    RESTART_BOOTLOADER_DFU,
    
    RESTART_TYPE_MAX = 0xFF,
} restart_type_t;


typedef struct event_s {
    event_type_t type;
    uint8_t data;
} event_t;


extern QueueHandle_t event_q;



#endif