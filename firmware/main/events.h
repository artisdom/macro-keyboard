#ifndef MACRO_KEYBOARD_H
#define MACRO_KEYBOARD_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Contains declarations used by the event handler task

typedef enum event_type_e {
    EVENT_KB_CHANGE_BT_HOST = 0x01,
    EVENT_KB_RESET_BT_HOST,
    EVENT_KB_LEDS_BRIGHTNESS,
    EVENT_LAYERS_CHANGED,
    EVENT_TOGGLE_SWITCH,
    EVENT_BT_ADV,
    EVENT_BT_ADV_ALL,
    EVENT_BT_CONNECTED,
    EVENT_BT_LEDS_EFFECT_TOGGLE,
    EVENT_USB_PORT,

    EVENT_MAX = 0xFF,
} event_type_t;


typedef struct event_s {
    event_type_t type;
    uint8_t data;
} event_t;


extern QueueHandle_t event_q;



#endif