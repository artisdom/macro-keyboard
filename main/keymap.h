#ifndef KEYMAP_h
#define KEYMAP_H

#include <stdint.h>

#include "config.h"


// Layout type
typedef uint16_t layout_matrix[MATRIX_ROWS][MATRIX_COLS];
typedef layout_matrix layout_t;


// Macro
extern uint8_t macros[NUM_MACROS][MACRO_LEN];


// Modifier layouts
extern layout_t layout_modifier_keymap;
extern layout_t bluetooth_modifier_keymap;

// Layout arrays
extern layout_t *keymaps[NUM_LAYOUTS];
extern layout_t *shift_keymaps[NUM_LAYOUTS];




#endif