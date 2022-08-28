#ifndef KEYBOARD_H
#define KEYBOARD_H


#define KEY_UP     (1)
#define KEY_DOWN   (0)


void     keyboard__init();
uint8_t *keyboard__check_state();

#endif