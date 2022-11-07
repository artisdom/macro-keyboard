#ifndef DYNAMIC_KEYMAP_H
#define DYNAMIC_KEYMAP_H


void     dynamic_keymap__init();
void     dynamic_keymap__reset();

uint16_t dynamic_keymap__get_keycode(uint8_t layer, uint8_t row, uint8_t col);
void     dynamic_keymap__set_keycode(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode);
uint8_t  dynamic_keymap__get_layers_count();
uint16_t dynamic_keymap__get_layers_size();
void     dynamic_keymap__get_layers(uint16_t offset, uint8_t size, uint8_t *dest);
void     dynamic_keymap__set_layers(uint16_t offset, uint8_t size, uint8_t *src);
void     dynamic_keymap__layers_reset();

uint16_t dynamic_keymap__get_macro_keycode(uint8_t macro, uint8_t key);
uint16_t dynamic_keymap__get_macros_size();
void     dynamic_keymap__get_macros(uint16_t offset, uint8_t size, uint8_t *dest);
void     dynamic_keymap__set_macros(uint16_t offset, uint8_t size, uint8_t *src);
void     dynamic_keymap__macro_reset();



#endif