#ifndef MACRO_KEYBOARD_H
#define MACRO_KEYBOARD_H

#include "freertos/queue.h"

// Contains declarations used by the event handler task

typedef enum event_type_e {
	EVENT_BT_CHANGE_DEVICE = 0x01,
	EVENT_BT_RESET_DEVICE,
	EVENT_TOGGLE_SWITCH,

	EVENT_MAX = 0xFF, // make sure it's 16 bits
} event_type_t;


typedef struct event_s {
	event_type_t type;
	uint8_t data;
} event_t;


extern QueueHandle_t event_q;



#endif