#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

// #include "esp_system.h"
#include "esp_log.h"

#include "layers.h"
#include "config.h"
#include "keymap.h"
#include "key_definitions.h"


/* --------- Local Variables --------- */
static const char *TAG = "layers";


static uint32_t default_layer_state;
static uint32_t layer_state;


/* --------- Local Functions --------- */
static void layers__debug_stack();


void layers__init() {

    ESP_LOGI(TAG, "Init layers");

    default_layer_state = (1 << DEFAULT_LAYER);

    layer_state = default_layer_state;

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


void layers__activate_layer(uint8_t layer) {
    layer_state |= (1 << layer);
    layers__debug_stack();
}


void layers__deactivate_layer(uint8_t layer) {
    layer_state &= ~(1 << layer);
    layers__debug_stack();
}


void layers__toggle_layer(uint8_t layer) {
    uint32_t layer_active = layer_state & (1 << layer);
    if (layer_active) {
        layer_state &= ~(1 << layer); // deactivate
    }
    else {
        layer_state |= (1 << layer); // activate
    }
    layers__debug_stack();
}


uint16_t layers__get_keycode(uint8_t row, uint8_t col) {

    uint16_t keycode = KC_NO;
    uint32_t layers = layer_state | default_layer_state;

    for (int8_t i = MAX_LAYER - 1; i >= 0; i--) {
        uint32_t layer_active = layers & (1 << i);
        if (layer_active) {
            keycode = keymaps[i][row][col];
            if (keycode != KC_TRANSPARENT) {
                ESP_LOGD(TAG, "Keycode 0x%x on layer %d", keycode, i);
                break;
            }
        }
    }

    return keycode;
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
