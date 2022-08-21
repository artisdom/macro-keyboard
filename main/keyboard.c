#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "keyboard.h"
#include "config.h"
#include "matrix.h"
#include "ble_hidd.h"


/* --------- Global Variables --------- */
enum macro_keycodes {
    KC_MACRO_COPY = KC_BASE_MACRO,
    KC_MACRO_PASTE,
};

uint8_t macros[NUM_MACROS][MACRO_LEN] = {
    {KC_LGUI, KC_C, KC_NO},
    {KC_LGUI, KC_V, KC_NO},
};


layout_t empty_layout = {
    {KC_NONE, KC_NONE, KC_NONE}
};

layout_t layout_1 = {
    {KC_BLUETOOTH, KC_C, KC_MACRO_PASTE}
};
layout_t shift_layout_1 = {
    {KC_SHIFT, KC_G, KC_H}
};
layout_t layout_2 = {
    {KC_LAYOUT, KC_E, KC_F}
};

layout_t layout_modifier_keymap = {
    {KC_LAYOUT, KC_LAYOUT_DECREMENT, KC_LAYOUT_INCREMENT}
};
layout_t bluetooth_modifier_keymap = {
    {KC_BLUETOOTH, KC_BT_DEVICE_0, KC_BT_DEVICE_1}
};

layout_t *keymaps[NUM_LAYOUTS] = {&layout_1, &layout_2};
layout_t *shift_keymaps[NUM_LAYOUTS] = {&shift_layout_1, &empty_layout};


/* --------- Local Variables --------- */
static const char *TAG = "keyboard";

static uint8_t keyboard_state[MATRIX_ROWS][MATRIX_COLS] = {0};
static uint8_t keyboard_prev_state[MATRIX_ROWS][MATRIX_COLS] = {0};

static uint8_t current_layout = DEFAULT_LAYOUT;

static bool layout_modifier = false;
static bool shift_modifier = false;
static bool bluetooth_modifier = false;


// the HID report
static uint8_t hid_report[2 + HID_REPORT_LEN] = {0};
static uint8_t hid_report_index = 2;
// store where each key is set in the HID report
static uint8_t hid_report_key_index[MATRIX_ROWS][MATRIX_COLS] = {0};


/* --------- Local Functions --------- */
static void keyboard__change_layout(uint16_t keycode);
static uint16_t keyboard__get_keycode(uint8_t row, uint8_t col);
static bool keyboard__handle_modifier(uint16_t keycode, uint8_t keystate);


// Unused
// static void keyboard__set_layout(uint8_t layout_id) {
//     if (layout_id < NUM_LAYOUTS) {
//         current_layout = layout_id;
//     }
//     else {
//         ESP_LOGE(TAG, "Layout number outside possible range %d > %d", layout_id, NUM_LAYOUTS);
//     }
// }

static void keyboard__change_layout(uint16_t keycode) {

    uint8_t new_layout;

    switch(keycode) {
        case KC_LAYOUT_INCREMENT:
            if (current_layout == NUM_LAYOUTS - 1) {
                current_layout = 0;    
            }
            else {
                current_layout++;
            }
            break;
        case KC_LAYOUT_DECREMENT:
            if (current_layout == 0) {
                current_layout = NUM_LAYOUTS - 1;    
            }
            else {
                current_layout--;
            }
            break;
        default:
            new_layout = keycode - KC_MIN_LAYOUT;
            if (new_layout >= NUM_LAYOUTS) {
                ESP_LOGW(TAG, "New layout is out of bounds %d (max %d)", new_layout, NUM_LAYOUTS - 1);
                current_layout = 0;
            }
            else {
                current_layout = keycode - KC_MIN_LAYOUT;
            }
    }
    ESP_LOGD(TAG, "Current layout changed %d", current_layout);
}


static uint16_t keyboard__get_keycode(uint8_t row, uint8_t col) {

    layout_t *layout;

    if (layout_modifier == true) {
        layout = &layout_modifier_keymap;
    }
    else if (shift_modifier == true) {
        layout = shift_keymaps[current_layout];
    }
    else if (bluetooth_modifier == true) {
        layout = &bluetooth_modifier_keymap;
    }
    else {
        layout = keymaps[current_layout];
    }

    uint16_t keycode = (*layout)[row][col];

    return keycode;
}


// check for HID modifiers
uint16_t keyboard__check_modifier(uint16_t keycode) {

    uint8_t cur_mod = 0;
    // these are the modifier keys
    if ((KC_LCTRL <= keycode) && (keycode <= KC_RGUI)) {
        cur_mod = (1 << (keycode - KC_LCTRL));
        return cur_mod;
    }
    return 0;

}


static bool keyboard__handle_modifier(uint16_t keycode, uint8_t keystate) {

    bool modifier_changed = false;

    if (keycode >= KC_BASE_MODIFIERS && keycode <= KC_MAX_MODIFIERS) {
        modifier_changed = true;

        switch (keycode) {
            case KC_LAYOUT:
                if (keystate == KEY_UP) {
                    layout_modifier = true;
                }
                else {
                    layout_modifier = false;
                }
                ESP_LOGD(TAG, "layout modifier %d", layout_modifier);
                break;
            case KC_SHIFT:
                if (keystate == KEY_UP) {
                    shift_modifier = true;
                }
                else {
                    shift_modifier = false;
                }
                ESP_LOGD(TAG, "shift modifier %d", shift_modifier);
                break;
            case KC_BLUETOOTH:
                if (keystate == KEY_UP) {
                    bluetooth_modifier = true;
                }
                else {
                    bluetooth_modifier = false;
                }
                ESP_LOGD(TAG, "bluetooth modifier %d", bluetooth_modifier);
                break;
            default:
                // ESP_LOGW(TAG, "Unrecognised modifier 0x%x", keycode);
                modifier_changed = false;
                break;
        }
    }

    return modifier_changed;

}


void keyboard__init() {
    // current_layout = 1;
}


uint8_t *keyboard__check_state() {

    uint8_t report_index;

    // memset(hid_report, 0x00, sizeof(hid_report));

    matrix__scan();

    memcpy(keyboard_state, matrix_state, sizeof(matrix_state));

    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        for(uint8_t row = 0; row < MATRIX_ROWS; row++) {

            uint8_t keystate = keyboard_state[row][col];

            if (keystate == keyboard_prev_state[row][col]) {
                continue;
            }

            uint16_t keycode = keyboard__get_keycode(row, col);
            report_index = hid_report_index;

            // ESP_LOGD(TAG, "state: [%d] [%d] 0x%x %d", row, col, keycode, keystate);

            if (keyboard__handle_modifier(keycode, keystate) == true) {
                continue;
            }

            if (keystate == KEY_UP) {
                // layouts
                if (layout_modifier == true && keycode > KC_BASE_MODIFIERS && keycode <= KC_MAX_LAYOUT) {
                    keyboard__change_layout(keycode);
                    continue;
                }

                // bluetooth
                if (bluetooth_modifier == true && keycode >= KC_MIN_BLUETOOTH && keycode <= KC_MAX_BLUETOOTH) {
                    uint8_t bt_host = keycode - KC_MIN_BLUETOOTH;
                    if (BLE_ENABLED) {
                        xQueueSend(ble_event_q, (void *) &bt_host, (TickType_t) 0);
                    }
                    continue;
                }

                // macros
                if (keycode >= KC_BASE_MACRO && keycode <= KC_MAX_MACRO) {
                    uint16_t key;
                    uint8_t macro_id = keycode - KC_BASE_MACRO;

                    if (report_index < HID_REPORT_LEN - MACRO_LEN) {

                        hid_report_key_index[row][col] = report_index;

                        for (uint8_t i = 0; i < MACRO_LEN; i++) {
                            key = macros[macro_id][i];
                            
                            uint8_t modifier = keyboard__check_modifier(key);
                            if (modifier) {
                                // ESP_LOGD(TAG, "Macro %d, modifier 0x%x", macro_id, modifier);
                                hid_report[0] |= modifier;
                            }
                            else {
                                // ESP_LOGD(TAG, "Macro %d, adding key %d at %d", macro_id, key, report_index);
                                hid_report[report_index] = key;
                                report_index++;
                                hid_report_index++;
                            }

                        }
                    }
                    else {
                        ESP_LOGW(TAG, "Unable to fit macro in report, not enough space available");
                    }
                    continue;
                }

                // normal key report
                if (report_index < HID_REPORT_LEN) {

                    uint8_t modifier = keyboard__check_modifier(keycode);
                    if (modifier) {
                        hid_report[0] |= modifier;
                    }
                    else {
                        hid_report[report_index] = keycode;
                        hid_report_key_index[row][col] = report_index;
                        hid_report_index++;
                    }
                }

            }
            else { // KEY_DOWN

                report_index = hid_report_key_index[row][col];

                // macros
                if (keycode >= KC_BASE_MACRO && keycode <= KC_MAX_MACRO) {
                    uint16_t key;
                    uint8_t macro_id = keycode - KC_BASE_MACRO;

                    for (uint8_t i = 0; i < MACRO_LEN; i++) {
                        key = macros[macro_id][i];
                        uint8_t modifier = keyboard__check_modifier(key);
                        if (modifier) {
                            // ESP_LOGD(TAG, "Macro %d, ~modifier 0x%x", macro_id, modifier);
                            hid_report[0] &= ~modifier;
                        }
                        else {
                            // ESP_LOGD(TAG, "Macro %d, removing key %d at %d", macro_id, key, report_index);
                            hid_report[report_index] = 0;
                            report_index++;
                            hid_report_index--;
                        }
                    }
                    hid_report_key_index[row][col] = 0;
                    continue;
                }

                // normal key report
                uint8_t modifier = keyboard__check_modifier(keycode);
                if (modifier) {
                    hid_report[0] &= ~modifier;
                }
                if (report_index >= 2) {
                    hid_report[report_index] = 0;
                    hid_report_key_index[row][col] = 0;
                    hid_report_index--;
                }

            }

            // ESP_LOGD(TAG, "state: [%d] [%d] 0x%x %d", row, col, keycode, keystate);
            // ESP_LOGD(TAG, "hid report len: %d %d", hid_report_index, report_index);
        }
    }

    memcpy(keyboard_prev_state, keyboard_state, sizeof(keyboard_state));

    hid_report[1] = 0;

    return hid_report;
}