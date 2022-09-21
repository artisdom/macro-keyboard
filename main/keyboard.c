#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"

#include "keyboard.h"
#include "keymap.h"
#include "key_definitions.h"
#include "config.h"
#include "matrix.h"
#include "events.h"


/* --------- Local Variables --------- */
static const char *TAG = "keyboard";

static uint8_t keyboard_state[MATRIX_ROWS][MATRIX_COLS] = {0};
static uint8_t keyboard_prev_state[MATRIX_ROWS][MATRIX_COLS] = {0};

static uint8_t current_layer = DEFAULT_LAYER;
static uint8_t prev_layer = DEFAULT_LAYER;

// the HID report
static uint8_t hid_report[2 + HID_REPORT_LEN] = {0};
static uint8_t hid_report_index = 2;
// store where each key is set in the HID report
static uint8_t hid_report_key_index[MATRIX_ROWS][MATRIX_COLS] = {0};

extern QueueHandle_t media_q;


/* --------- Local Functions --------- */
static uint16_t keyboard__get_keycode(uint8_t row, uint8_t col);
static uint16_t keyboard__check_modifier(uint16_t keycode);
static bool keyboard__handle_action(uint16_t keycode, uint8_t keystate);
static void keyboard__handle_media(uint16_t keycode, uint8_t keystate);



static uint16_t keyboard__get_keycode(uint8_t row, uint8_t col) {

    uint16_t keycode = keymaps[current_layer][row][col];

    return keycode;
}


// check for HID modifiers
static uint16_t keyboard__check_modifier(uint16_t keycode) {

    uint8_t cur_mod = 0;
    // these are the modifier keys
    if ((KC_LCTRL <= keycode) && (keycode <= KC_RGUI)) {
        cur_mod = (1 << (keycode - KC_LCTRL));
        return cur_mod;
    }
    return 0;

}


static bool keyboard__handle_action(uint16_t keycode, uint8_t keystate) {

    bool action_performed = false;
    event_t event;

    if (keycode > QK_ACTION) {
        action_performed = true;

        switch (keycode) {
            case QK_TO ... QK_TO_MAX:
                if (keystate == KEY_DOWN) {
                    current_layer = keycode & 0xFF;
                    ESP_LOGD(TAG, "Goto layer %d", current_layer);
                }
                break;
            case QK_MOMENTARY ... QK_MOMENTARY_MAX:
                if (keystate == KEY_DOWN) {
                    prev_layer = current_layer;
                    current_layer = keycode & 0xFF;
                    ESP_LOGD(TAG, "Momentary to layer %d", current_layer);
                }
                else { // KEY_UP
                    current_layer = prev_layer;
                    ESP_LOGD(TAG, "Momentary back to layer %d", current_layer);
                }
                break;
            case QK_BT_HOST ... QK_BT_HOST_MAX:
                event.type = EVENT_BT_CHANGE_HOST,
                event.data = keycode & 0xFF,
                xQueueSend(event_q, (void *) &event, (TickType_t) 0);
                ESP_LOGD(TAG, "EVENT_BT_CHANGE_HOST %d", keycode & 0xFF);
                break;
            case QK_BT_HOST_RESET ... QK_BT_HOST_RESET_MAX:
                event.type = EVENT_BT_RESET_HOST,
                event.data = keycode & 0xFF,
                xQueueSend(event_q, (void *) &event, (TickType_t) 0);
                ESP_LOGD(TAG, "EVENT_BT_RESET_HOST %d", keycode & 0xFF);
                break;
            case QK_BRIGHTNESS ... QK_BRIGHTNESS_MAX:
                if (keystate == KEY_DOWN) {
                    event.type = EVENT_LEDS_BRIGHTNESS;
                    event.data = keycode - QK_BRIGHTNESS; // 0 is up, 1 is down
                    xQueueSend(event_q, (void *) &event, (TickType_t) 0);
                    ESP_LOGD(TAG, "EVENT_LEDS_BRIGHTNESS %d", keycode - QK_BRIGHTNESS);
                }
            default:
                break;
        }
    }

    return action_performed;

}


static void keyboard__handle_media(uint16_t keycode, uint8_t keystate) {

    uint8_t media_state[HID_CC_REPORT_LEN] = { 0 };
    media_state[0] = keycode - KC_CC_OFFSET;
    media_state[1] = keystate;

    xQueueSend(media_q, (void*) &media_state, (TickType_t) 0);


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

            if (keyboard__handle_action(keycode, keystate) == true) {
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

                // media controls
                if (keycode >= KC_BASE_MEDIA && keycode <= KC_MAX_MEDIA) {
                    keyboard__handle_media(keycode, keystate);
                    continue;
                }

                // normal key report
                if (keycode <= 0xFF && report_index < HID_REPORT_LEN) {

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
            else { // KEY_UP

                report_index = hid_report_key_index[row][col];

                // macros
                if (keycode >= QK_MACRO && keycode <= QK_MACRO_MAX) {
                    uint16_t key;
                    uint8_t macro_id = keycode & 0xFF;

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

                // media controls
                if (keycode >= KC_BASE_MEDIA && keycode <= KC_MAX_MEDIA) {
                    keyboard__handle_media(keycode, keystate);
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