#ifndef KEYBOARD_H
#define KEYBOARD_H


#define KEY_UP     (0)
#define KEY_DOWN   (1)


void     keyboard__init();
uint8_t *keyboard__check_state();

#endif