#ifndef TOGGLE_SWITCH_H
#define TOGGLE_SWITCH_H


enum toggle_types_e {
    TOGGLE_OFF = 0x00,
    TOGGLE_BLE,
    TOGGLE_USB,
};

void toggle_switch__init();
uint8_t toggle_switch__get_state();

#endif