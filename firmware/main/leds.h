#ifndef LEDS_H
#define LEDS_H

#include "freertos/queue.h"

#include "events.h"

void    leds__init();
void    leds__set_brightness(uint8_t level);
uint8_t leds__get_brightness();
void    leds__increase_brightness();
void    leds__decrease_brightness();

void    leds__update_effect_position(uint8_t position[2]);

void    leds__task(void *pvParameters);


extern QueueHandle_t leds_q;

#endif