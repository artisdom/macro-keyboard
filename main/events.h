#ifndef MACRO_KEYBOARD_H
#define MACRO_KEYBOARD_H

#include "freertos/queue.h"

// Contains declarations used by the event handler task

typedef enum event_type_e {
    EVENT_BT_CHANGE_HOST = 0x01,
    EVENT_BT_RESET_HOST,
    EVENT_TOGGLE_SWITCH,
    EVENT_LEDS_BRIGHTNESS,
    EVENT_LEDS_BT_ADV,
    EVENT_LEDS_BT_ADV_ALL,
    EVENT_LEDS_BT_CONNECTED,

    EVENT_MAX = 0xFF,
} event_type_t;


typedef struct event_s {
    event_type_t type;
    uint8_t data;
} event_t;


extern QueueHandle_t event_q;



#endif