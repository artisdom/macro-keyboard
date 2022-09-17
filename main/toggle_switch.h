#ifndef TOGGLE_SWITCH_H
#define TOGGLE_SWITCH_H


enum toggle_types_e {
    TOGGLE_OFF = 0x00,
    TOGGLE_BLE,
    TOGGLE_USB,

    TOGGLE_MAX = 0xFF, // make sure it's 8bit
};

void toggle_switch__init();
uint8_t toggle_switch__get_state();

#endif