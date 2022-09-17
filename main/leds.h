#ifndef LEDS_H
#define LEDS_H

#include "freertos/queue.h"

#include "events.h"

void leds__init();
void leds__set_brightness(uint8_t level);
void leds__increase_brightness();
void leds__decrease_brightness();

void leds__update_host_positions(uint8_t host_id, uint8_t position[2]);

// void leds__handle_event(event_t event);
void leds__task(void *pvParameters);


extern QueueHandle_t leds_q;

#endif