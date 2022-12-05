#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "config.h"

#define KEY_UP              (0)
#define KEY_DOWN            (1)

#define HID_REPORT_KEYS_LEN (HID_REPORT_LEN - 2)


/*
 * keyboard report is 8-byte array retains state of 8 modifiers and 6 keys.
 *
 * byte |0       |1       |2       |3       |4       |5       |6       |7
 * -----+--------+--------+--------+--------+--------+--------+--------+--------
 * desc |mods    |reserved|keys[0] |keys[1] |keys[2] |keys[3] |keys[4] |keys[5]
 */
typedef union __attribute__ ((packed)) keyboard_report_u {
    uint8_t raw[HID_REPORT_LEN];
    struct {
        uint8_t modifiers;
        uint8_t reserved;
        uint8_t keys[HID_REPORT_KEYS_LEN];
    };
} keyboard_report_t;



void     keyboard__init();
uint8_t *keyboard__check_state();




#endif