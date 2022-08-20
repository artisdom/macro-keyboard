#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "config.h"
#include "key_definitions.h"


#define KEY_UP     (1)
#define KEY_DOWN   (0)

#define MACRO_LEN  (3)


typedef uint16_t layout_matrix[MATRIX_ROWS][MATRIX_COLS];
typedef layout_matrix layout_t;

extern layout_t layout_1;
extern layout_t layout_2;

extern layout_t *keymaps[NUM_LAYOUTS];


void 	 keyboard__init();
uint8_t *keyboard__check_state();

#endif