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
static keyboard_report_t hid_report = {0};

extern QueueHandle_t media_q;


/* --------- Local Functions --------- */
static inline uint16_t keyboard__get_keycode(uint8_t row, uint8_t col);
static uint16_t keyboard__check_modifier(uint16_t keycode);
static bool keyboard__handle_action(uint16_t keycode, uint8_t keystate, uint8_t position[2]);
static void keyboard__handle_media(uint16_t keycode, uint8_t keystate);
static void keyboard__handle_keycode(uint16_t keycode, uint8_t keystate, uint8_t position[2]);
static void keyboard__add_key(uint8_t keycode);
static void keyboard__remove_key(uint8_t keycode);



static inline uint16_t keyboard__get_keycode(uint8_t row, uint8_t col) {
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
            case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX:
                layers__set_oneshot_layer(keycode & 0xFF);
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


static void keyboard__add_key(uint8_t keycode) {

    int8_t i = 0;
    int8_t empty = -1;
    // Find first empty slot in the report
    for (; i < HID_REPORT_KEYS_LEN; i++) {
        if (hid_report.keys[i] == keycode) {
            break;
        }
        if (empty == -1 && hid_report.keys[i] == 0) {
            empty = i;
        }
    }

    if (i == HID_REPORT_KEYS_LEN) {
        if (empty != -1) {
            ESP_LOGD(TAG, "Adding keycode 0x%x to report at %d", keycode, empty);
            hid_report.keys[empty] = keycode;
        }
    }

    if (empty == -1) {
        ESP_LOGW(TAG, "Not enough space remaining in hid report! Skipping key");
    }

}


static void keyboard__remove_key(uint8_t keycode) {

    for (uint8_t i = 0; i < HID_REPORT_KEYS_LEN; i++) {
        if (hid_report.keys[i] == keycode) {
            ESP_LOGD(TAG, "Removing keycode 0x%x from report at %d", keycode, i);
            hid_report.keys[i] = 0;
        }
    }
}


static void keyboard__handle_keycode(uint16_t keycode, uint8_t keystate, uint8_t position[2]) {

    if (keyboard__handle_action(keycode, keystate, position) == true) {
        return;
    }

    if (keystate == KEY_DOWN) {

        // macros
        if (keycode >= QK_MACRO && keycode <= QK_MACRO_MAX) {
            uint16_t key;
            uint8_t macro_id = keycode & 0xFF;

            ESP_LOGD(TAG, "Adding macro %d", macro_id);

            for (uint8_t i = 0; i < MACRO_LEN; i++) {
                key = layers__get_macro_keycode(macro_id, i);
                keyboard__handle_keycode(key, keystate, position);
            }
            return;
        }

        // media controls
        if (keycode >= QK_MEDIA && keycode <= QK_MEDIA_MAX) {
            keyboard__handle_media(keycode, keystate);
            return;
        }

        // normal key report
        uint8_t modifier = keyboard__check_modifier(keycode);
        hid_report.modifiers |= modifier;

        uint8_t hid_keycode = keycode & QK_BASIC_MAX;
        // discard if key is modifier
        if ( (hid_keycode > KC_NO) && ( (hid_keycode < KC_MODS) || (hid_keycode > KC_MODS_MAX) )) {
            keyboard__add_key(hid_keycode);
        }

    }
    else { // KEY_UP

        // macros
        if (keycode >= QK_MACRO && keycode <= QK_MACRO_MAX) {
            uint16_t key;
            uint8_t macro_id = keycode & 0xFF;

            ESP_LOGD(TAG, "Removing macro %d", macro_id);

            for (uint8_t i = MACRO_LEN; i > 0; i--) {
                key = layers__get_macro_keycode(macro_id, i - 1);
                keyboard__handle_keycode(key, keystate, position);
            }
            return;
        }

        // media controls
        if (keycode >= QK_MEDIA && keycode <= QK_MEDIA_MAX) {
            keyboard__handle_media(keycode, keystate);
            return;
        }

        // normal key report
        uint8_t modifier = keyboard__check_modifier(keycode);
        hid_report.modifiers &= ~modifier;

        uint8_t hid_keycode = keycode & QK_BASIC_MAX;
        if (hid_keycode > KC_NO) {
            keyboard__remove_key(hid_keycode);
        }

    }

}


void keyboard__init() {
    ESP_LOGI(TAG, "Init keyboard");

    layers__init();
}


uint8_t *keyboard__check_state() {

    matrix__scan();

    memcpy(keyboard_state, matrix_state, sizeof(matrix_state));

    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        for(uint8_t row = 0; row < MATRIX_ROWS; row++) {

            uint8_t keystate = keyboard_state[row][col];

            if (keystate == keyboard_prev_state[row][col]) {
                continue;
            }

            // check if oneshot has timed out before getting the keycode
            if (layers__check_oneshot_timeout()) {
                layers__clear_oneshot_layer();
            }

            uint16_t keycode = keyboard__get_keycode(row, col);
            ESP_LOGD(TAG, "state: [%d] [%d] 0x%x %d", row, col, keycode, keystate);

            // handle oneshot state changes
            if ((keystate == KEY_DOWN) && (layers__get_oneshot_state() == ONESHOT_START)) {
                layers__set_oneshot_state(ONESHOT_PRESSED);
            }
            if ((keystate == KEY_UP) && (layers__get_oneshot_state() == ONESHOT_PRESSED)) {
                layers__clear_oneshot_layer();
            }

            uint8_t pos[2] = {row, col};
            keyboard__handle_keycode(keycode, keystate, pos);

            // ESP_LOGD(TAG, "state: [%d] [%d] 0x%x %d", row, col, keycode, keystate);
            // ESP_LOGD(TAG, "hid report len: %d %d", hid_report_index, report_index);
        }
    }

    memcpy(keyboard_prev_state, keyboard_state, sizeof(keyboard_state));

    hid_report.reserved = 0;

    return hid_report.raw;
}