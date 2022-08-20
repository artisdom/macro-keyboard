#ifndef MATRIX_H
#define MATRIX_H

#include "config.h"

extern uint8_t matrix_state[MATRIX_ROWS][MATRIX_COLS];


void matrix__init();
void matrix__scan();

void matrix__rtc_setup(void);
void matrix__rtc_deinit(void);

#endif