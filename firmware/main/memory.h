#ifndef MEMORY_H
#define MEMORY_H

#include "ble.h"


void      memory__init();

void      memory__set_restart_counter();
uint32_t  memory__get_restart_counter();
void      memory__set_sleep_counter();
uint32_t  memory__get_sleep_counter();
void      memory__set_sleep_status(bool sleep_status);
bool      memory__get_sleep_status();

void      memory__set_current_layer(uint8_t layer);
uint8_t   memory__get_current_layer();

void      memory__set_via_layout_options(uint32_t options);
uint32_t  memory__get_via_layout_options();
void      memory__set_keymap_state(void *data, size_t length);
void      memory__get_keymap_state(void *data, size_t length);
void      memory__set_macros(void *data, size_t length);
void      memory__get_macros(void *data, size_t length);
void      memory__set_keymaps(void *data, size_t length);
void      memory__get_keymaps(void *data, size_t length);

void      memory__set_bluetooth_host(uint8_t host_id, bt_host_t host);
bt_host_t memory__get_bluetooth_host(uint8_t host_id);
void      memory__set_bluetooth_last_host(uint8_t host_id);
uint8_t   memory__get_bluetooth_last_host();

void      memory__set_leds_brightness(uint8_t brightness);
uint8_t   memory__get_leds_brightness();


#endif