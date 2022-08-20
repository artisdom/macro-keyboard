#ifndef MATRIX_H
#define MATRIX_H

#include "config.h"

extern uint8_t matrix_state[MATRIX_ROWS][MATRIX_COLS];


void matrix__init();
void matrix__scan();

#endif