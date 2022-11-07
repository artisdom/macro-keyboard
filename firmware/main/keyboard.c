#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"

#include "keyboard.h"
#include "key_definitions.h"
#include "layers.h"
#include "config.h"
#include "matrix.h"
#include "events.h"
#include "memory.h"
#include "leds.h"


/* --------- Local Variables --------- */
static const char *TAG = "keyboard";

static uint8_t keyboard_state[MATRIX_ROWS][MATRIX_COLS] = {0};
static uint8_t keyboard_prev_state[MATRIX_ROWS][MATRIX_COLS] = {0};

// the HID report
static uint8_t hid_report[2 + HID_REPORT_LEN] = {0};
static uint8_t hid_report_index = 2;
// store where each key is set in the HID report
static uint8_t hid_report_key_index[MATRIX_ROWS][MATRIX_COLS] = {0};

extern QueueHandle_t media_q;


/* --------- Local Functions --------- */
static uint16_t keyboard__get_keycode(uint8_t row, uint8_t col);
static uint16_t keyboard__check_modifier(uint16_t keycode);
static bool keyboard__handle_action(uint16_t keycode, uint8_t keystate, uint8_t position[2]);
static void keyboard__handle_media(uint16_t keycode, uint8_t keystate);



static uint16_t keyboard__get_keycode(uint8_t row, uint8_t col) {
    return layers__get_keycode(row, col);
}


// check for HID modifiers
static uint16_t keyboard__check_modifier(uint16_t keycode) {

    uint8_t modifier = 0;
    // quantum modifier combinations
    if ((keycode >= QK_MODS) && (keycode <= QK_MODS_MAX)) {
        modifier = (keycode & 0x0F00) >> 8; // grab 4 bits
        uint16_t right_mod = keycode & QK_RMODS_MIN;
        if (right_mod) { // Right modifiers are the 4 MSB bits
            modifier <<= 4;
        }
    }

    // HID modifier keys
    keycode = keycode & QK_BASIC_MAX;
    if ((keycode >= KC_MODS) && (keycode <= KC_MODS_MAX)) {
        modifier |= (1 << (keycode - KC_LEFT_CTRL));
    }
    return modifier;

}


static bool keyboard__handle_action(uint16_t keycode, uint8_t keystate, uint8_t position[2]) {

    bool action_performed = false;
    event_t event;

    if (keycode >= QK_ACTION) {
        action_performed = true;

        switch (keycode) {
            case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
                if (keystate == KEY_DOWN) {
                    layers__toggle_layer(keycode & 0xFF, true);
                }
                break;
            case QK_TO ... QK_TO_MAX:
                if (keystate == KEY_DOWN) {
                    layers__deactivate_all();
                    layers__activate_layer(keycode & 0xFF, true);
                }
                break;
            case QK_MOMENTARY ... QK_MOMENTARY_MAX:
                if (keystate == KEY_DOWN) {
                    layers__activate_layer(keycode & 0xFF, false);
                }
                else { // KEY_UP
                    layers__deactivate_layer(keycode & 0xFF, false);
                }
                break;
            case QK_DEF_LAYER ... QK_DEF_LAYER_MAX:
                layers__set_default_layer(keycode & 0xFF);
                break;
            case QK_BT_HOST ... QK_BT_HOST_MAX:
                if (keystate == KEY_DOWN) {
                    event.type = EVENT_KB_CHANGE_BT_HOST;
                    event.data = keycode & 0xFF;
                    leds__update_effect_position(position);
                    xQueueSend(event_q, (void *) &event, (TickType_t) 0);
                    ESP_LOGD(TAG, "EVENT_BT_CHANGE_HOST %d", keycode & 0xFF);
                }
                break;
            case QK_BT_HOST_RESET ... QK_BT_HOST_RESET_MAX:
                if (keystate == KEY_DOWN) {
                    event.type = EVENT_KB_RESET_BT_HOST;
                    event.data = keycode & 0xFF;
                    leds__update_effect_position(position);
                    xQueueSend(event_q, (void *) &event, (TickType_t) 0);
                    ESP_LOGD(TAG, "EVENT_BT_RESET_HOST %d", keycode & 0xFF);
                }
                break;
            case QK_BRIGHTNESS ... QK_BRIGHTNESS_MAX:
                if (keystate == KEY_DOWN) {
                    event.type = EVENT_KB_LEDS_BRIGHTNESS;
                    event.data = keycode - QK_BRIGHTNESS; // 0 is up, 1 is down
                    xQueueSend(event_q, (void *) &event, (TickType_t) 0);
                    ESP_LOGD(TAG, "EVENT_LEDS_BRIGHTNESS %d", keycode - QK_BRIGHTNESS);
                }
                break;
            default:
                break;
        }
    }

    return action_performed;

}


static void keyboard__handle_media(uint16_t keycode, uint8_t keystate) {

    uint8_t media_state[HID_CC_REPORT_LEN] = { 0 };
    media_state[0] = keycode & 0xFF;
    media_state[1] = keystate;

    xQueueSend(media_q, (void*) &media_state, (TickType_t) 0);

}


void keyboard__init() {
    ESP_LOGI(TAG, "Init keyboard");

    layers__init();
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

            uint8_t pos[2] = {row, col};
            if (keyboard__handle_action(keycode, keystate, pos) == true) {
                continue;
            }

            if (keystate == KEY_DOWN) {

                // macros
                if (keycode >= QK_MACRO && keycode <= QK_MACRO_MAX) {
                    uint16_t key;
                    uint8_t macro_id = keycode & 0xFF;

                    if (report_index < HID_REPORT_LEN - MACRO_LEN) {

                        hid_report_key_index[row][col] = report_index;

                        for (uint8_t i = 0; i < MACRO_LEN; i++) {
                            key = layers__get_macro_keycode(macro_id, i);
                            
                            uint8_t modifier = keyboard__check_modifier(key);
                            hid_report[0] |= modifier;

                            uint8_t hid_keycode = key & QK_BASIC_MAX;
                            if ((hid_keycode < KC_MODS) || (hid_keycode > KC_MODS_MAX)) {
                                // ESP_LOGD(TAG, "Macro %d, adding key 0x%x at %d", macro_id, hid_keycode, report_index);
                                hid_report[report_index] = hid_keycode;
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

                // media controls
                if (keycode >= QK_MEDIA && keycode <= QK_MEDIA_MAX) {
                    keyboard__handle_media(keycode, keystate);
                    continue;
                }

                // normal key report
                if (report_index < HID_REPORT_LEN) {

                    uint8_t modifier = keyboard__check_modifier(keycode);
                    hid_report[0] |= modifier;
   
                    uint8_t hid_keycode = keycode & QK_BASIC_MAX;
                    if ((hid_keycode < KC_MODS) || (hid_keycode > KC_MODS_MAX)) {
                        hid_report[report_index] = hid_keycode;
                        hid_report_key_index[row][col] = report_index;
                        hid_report_index++;
                    }
                }

            }
            else { // KEY_UP

                report_index = hid_report_key_index[row][col];

                // macros
                if (keycode >= QK_MACRO && keycode <= QK_MACRO_MAX) {
                    uint16_t key;
                    uint8_t macro_id = keycode & 0xFF;

                    for (uint8_t i = 0; i < MACRO_LEN; i++) {
                        key = layers__get_macro_keycode(macro_id, i);
                        uint8_t modifier = keyboard__check_modifier(key);
                        hid_report[0] &= ~modifier;
                        
                        uint8_t hid_keycode = key & QK_BASIC_MAX;
                        if ((hid_keycode < KC_MODS) || (hid_keycode > KC_MODS_MAX)) {
                            // ESP_LOGD(TAG, "Macro %d, removing key 0x%x at %d", macro_id, hid_keycode, report_index);
                            hid_report[report_index] = 0;
                            report_index++;
                            hid_report_index--;
                        }
                    }
                    hid_report_key_index[row][col] = 0;
                    continue;
                }

                // media controls
                if (keycode >= QK_MEDIA && keycode <= QK_MEDIA_MAX) {
                    keyboard__handle_media(keycode, keystate);
                    continue;
                }

                // normal key report
                uint8_t modifier = keyboard__check_modifier(keycode);
                hid_report[0] &= ~modifier;

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