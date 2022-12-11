#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_timer.h"
#include "esp_log.h"

#include "layers.h"
#include "config.h"
#include "keymap.h"
#include "dynamic_keymap.h"
#include "key_definitions.h"
#include "events.h"


/* --------- Local Variables --------- */
static const char *TAG = "layers";


static uint32_t default_layer_state;
static uint32_t layer_state;

// LLLL LSSS with layer num (5 bits) and state (3 bits)
static uint8_t  oneshot_layer_state;
static uint32_t oneshot_time;


/* --------- Local Functions --------- */
static void layers__send_event();
static void layers__debug_stack();
static inline uint32_t millis();



static inline uint32_t millis() {
    return esp_timer_get_time() / 1000;
}


void layers__init() {

    ESP_LOGI(TAG, "Init layers");

    default_layer_state = (1 << DEFAULT_LAYER);
    layer_state = default_layer_state;
    oneshot_layer_state = ONESHOT_INACTIVE;

    if (VIA_ENABLED) {
        dynamic_keymap__init();
    }

    // ESP_LOG_BUFFER_HEX_LEVEL(TAG, &layer_state, sizeof(uint32_t), ESP_LOG_DEBUG);
    layers__debug_stack();

}


void layers__set_default_layer(uint8_t layer) {
    default_layer_state = (1 << layer);
}


void layers__deactivate_all() {
    layer_state = default_layer_state;
    layers__debug_stack();
}


void layers__activate_layer(uint8_t layer, bool send_event) {
    layer_state |= (1 << layer);
    if (send_event) {
        layers__send_event();
    }
    layers__debug_stack();
}


void layers__deactivate_layer(uint8_t layer, bool send_event) {
    layer_state &= ~(1 << layer);
    if (send_event) {
        layers__send_event();
    }
    layers__debug_stack();
}


void layers__toggle_layer(uint8_t layer, bool send_event) {
    uint32_t layer_active = layer_state & (1 << layer);
    if (layer_active) {
        layer_state &= ~(1 << layer); // deactivate
    }
    else {
        layer_state |= (1 << layer); // activate
    }
    if (send_event) {
        layers__send_event();
    }
    layers__debug_stack();
}


void layers__set_oneshot_layer(uint8_t layer) {
    oneshot_layer_state = (layer << 3) | ONESHOT_START;
    layers__activate_layer(layer, false);
    oneshot_time = millis();
}


void layers__clear_oneshot_layer() {
    uint8_t oneshot_layer = oneshot_layer_state >> 3;
    layers__deactivate_layer(oneshot_layer, false);
    oneshot_layer_state = ONESHOT_INACTIVE;
}


uint8_t layers__get_oneshot_state() {
    return oneshot_layer_state & 0b111;
}


void layers__set_oneshot_state(uint8_t state) {
    oneshot_layer_state |= (state & 0b111);
}


bool layers__check_oneshot_timeout() {
    if (layers__get_oneshot_state() != ONESHOT_INACTIVE) {
        if ((millis() - oneshot_time) > (ONESHOT_TIMEOUT * 1000)) {
            return true;
        }
    }
    return false;
}


uint16_t layers__get_keycode(uint8_t row, uint8_t col) {

    uint16_t keycode = KC_NO;
    uint32_t layers = layer_state | default_layer_state;

    for (int8_t i = MAX_LAYER - 1; i >= 0; i--) {
        uint32_t layer_active = layers & (1 << i);
        if (layer_active) {
            if (VIA_ENABLED) {
                keycode = dynamic_keymap__get_keycode(i, row, col);
            }
            else {
                keycode = keymaps[i][row][col];
            }

            if (keycode != KC_TRANSPARENT) {
                ESP_LOGD(TAG, "Keycode 0x%x on layer %d", keycode, i);
                break;
            }
        }
    }

    return keycode;
}


uint16_t layers__get_macro_keycode(uint8_t macro_id, uint8_t key_id) {
    uint16_t keycode = KC_NO;
    if (VIA_ENABLED) {
        keycode = dynamic_keymap__get_macro_keycode(macro_id, key_id);
    }
    else {
        keycode = macros[macro_id][key_id];
    }
    return keycode;
}


static void layers__send_event() {
    event_t event = {
        .type = EVENT_LAYERS_CHANGED,
        .data = layer_state & 0xFF,
    };
    xQueueSend(event_q, (void *) &event, (TickType_t) 0);
}


static void layers__debug_stack() {
    char str[41];

    for (int8_t i = MAX_LAYER - 1, j = 0; i >= 0; i--, j++) {
        uint32_t layer_active = layer_state & (1 << i);
        if (layer_active) {
            str[j] = '1';
        }
        else {
            str[j] = '0';
        }

        if (!(i % 4)) {
            j++;
            str[j] = ' ';
        }
    }
    str[40] = '\0';

    ESP_LOGD(TAG, "stack: %s", str);
}
