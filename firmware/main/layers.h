#ifndef LAYERS_h
#define LAYERS_H

#include <stdbool.h>


enum oneshot_layer_state_e {
    ONESHOT_INACTIVE = 0b000,
    ONESHOT_START    = 0b001,
    ONESHOT_PRESSED  = 0b011,
    ONESHOT_END      = 0b111,
};


void     layers__init();
void     layers__set_default_layer(uint8_t layer);
void     layers__activate_layer(uint8_t layer, bool send_event);
void     layers__deactivate_layer(uint8_t layer, bool send_event);
void     layers__deactivate_all();
void     layers__toggle_layer(uint8_t layer, bool send_event);

void     layers__set_oneshot_layer(uint8_t layer);
void     layers__clear_oneshot_layer();
uint8_t  layers__get_oneshot_state();
void     layers__set_oneshot_state(uint8_t state);
bool     layers__check_oneshot_timeout();

uint16_t layers__get_keycode(uint8_t row, uint8_t col);
uint16_t layers__get_macro_keycode(uint8_t macro_id, uint8_t key_id);


#endif