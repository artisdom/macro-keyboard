#ifndef LAYERS_h
#define LAYERS_H

#include <stdbool.h>


void     layers__init();
void     layers__set_default_layer(uint8_t layer);
void     layers__activate_layer(uint8_t layer, bool send_event);
void     layers__deactivate_layer(uint8_t layer, bool send_event);
void     layers__deactivate_all();
void     layers__toggle_layer(uint8_t layer, bool send_event);

uint16_t layers__get_keycode(uint8_t row, uint8_t col);
uint16_t layers__get_macro_keycode(uint8_t macro_id, uint8_t key_id);


#endif