#ifndef MEMORY_H
#define MEMORY_H

#include "ble_hidd.h"

void      memory__init();

void      memory__set_restart_counter();
uint32_t  memory__get_restart_counter();
void      memory__set_sleep_counter();
uint32_t  memory__get_sleep_counter();
void      memory__set_bluetooth_host(uint8_t host_id, bt_host_t host);
bt_host_t memory__get_bluetooth_host(uint8_t host_id);
void      memory__set_bluetooth_last_host(uint8_t host_id);
uint8_t   memory__get_bluetooth_last_host();
void      memory__set_leds_brightness(uint8_t brightness);
uint8_t   memory__get_leds_brightness();

#endif