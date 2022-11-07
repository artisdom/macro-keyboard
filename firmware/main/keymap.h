#ifndef KEYMAP_H
#define KEYMAP_H

#include <stdint.h>

#include "config.h"


/* These are defined in keyboards/<name>/keymap.c */

// Macros
extern const uint16_t macros[][MACRO_LEN];

// Layers
extern const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS];


#include "keymap_introspection.h"


#endif