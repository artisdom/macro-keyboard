#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG


// include c file to be able to compute the sizeof of the arrays
#include "keymap.c"

#include "keymap_introspection.h"


#define NUM_LAYERS  ((uint8_t)(sizeof(keymaps) / ((MATRIX_ROWS) * (MATRIX_COLS) * sizeof(uint16_t))))


uint8_t keymap__get_layer_count() {
    return NUM_LAYERS;
}


#define NUM_MACROS  ((uint8_t)(sizeof(macros) / ((MACRO_LEN) * sizeof(uint16_t))))


uint8_t keymap__get_macro_count() {
    return NUM_MACROS;
}