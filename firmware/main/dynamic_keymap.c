#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "esp_log.h"

#include "dynamic_keymap.h"
#include "config.h"
#include "keymap.h"
#include "memory.h"



/* --------- Global Variables --------- */
// uint16_t *dyn_macros;
// uint16_t *dyn_keymaps;


/* --------- Local Declarations --------- */
// struct to store in NVS to check that data is in sync with const data set with keyboard
typedef struct keymap_state_s {
    uint8_t num_macros;
    uint8_t macro_length;
    uint8_t num_layers;
    uint8_t layer_size;
} keymap_state_t;


/* --------- Local Variables --------- */
static const char *TAG = "dynamic_keymap";

static keymap_state_t state;
static size_t macros_size;
static size_t keymaps_size;
static uint16_t *dyn_macros;
static uint16_t *dyn_keymaps;


/* --------- Local Functions --------- */
static inline uint16_t dynamic_keymap__get_keycode_index(uint8_t layer, uint8_t row, uint8_t col);
static void dynamic_keymap__load_macros();
static void dynamic_keymap__save_macros(void *macros);
static void dynamic_keymap__load_keymaps();
static void dynamic_keymap__save_keymaps(void *keymaps);



void dynamic_keymap__init() {
    keymap_state_t init_state = {
        .num_macros = MAX_MACRO,
        .macro_length = MACRO_LEN,
        .num_layers = keymap__get_layer_count(),
        .layer_size = MATRIX_ROWS * MATRIX_COLS * sizeof(uint16_t),
    };

    ESP_LOGI(TAG, "Init dynamic keymaps");

    memset(&state, 0x00, sizeof(keymap_state_t));
    memory__get_keymap_state(&state, sizeof(keymap_state_t));

    ESP_LOGD(TAG, "keymap state -> macros:%d, macro_len:%d, layers:%d, layer_len:%d",
        state.num_macros,
        state.macro_length,
        state.num_layers,
        state.layer_size);

    macros_size = MAX_MACRO * MACRO_LEN * sizeof(uint16_t);
    keymaps_size = keymap__get_layer_count() * MATRIX_ROWS * MATRIX_COLS * sizeof(uint16_t);
    dyn_macros = malloc(macros_size);
    dyn_keymaps = malloc(keymaps_size);

    bool difference = false;
    // check if NVS keymaps have the same settings
    if (state.num_macros != init_state.num_macros) {
        ESP_LOGW(TAG, "Macro states not equal: init:%d != nvs:%d",init_state.num_macros, state.num_macros);
        difference = true;
    }
    if (state.macro_length != init_state.macro_length) {
        ESP_LOGW(TAG, "Macro length not equal: init:%d != nvs:%d",init_state.macro_length, state.macro_length);
        difference = true;
    }
    if (state.num_layers != init_state.num_layers) {
        ESP_LOGW(TAG, "Macro length not equal: init:%d != nvs:%d",init_state.num_layers, state.num_layers);
        difference = true;
    }
    if (state.macro_length != init_state.macro_length) {
        ESP_LOGW(TAG, "Macro length not equal: init:%d != nvs:%d",init_state.macro_length, state.macro_length);
        difference = true;
    }

    if (difference == true) {
        dynamic_keymap__reset();
        memory__set_keymap_state(&init_state, sizeof(keymap_state_t));
    }
    else {
        dynamic_keymap__load_macros();
        dynamic_keymap__load_keymaps();
        // ESP_LOGD(TAG, "dyn_keymaps:");
        // ESP_LOG_BUFFER_HEX_LEVEL(TAG, dyn_keymaps, keymaps_size, ESP_LOG_DEBUG);
        // ESP_LOGD(TAG, "keymaps:");
        // ESP_LOG_BUFFER_HEX_LEVEL(TAG, keymaps, keymaps_size, ESP_LOG_DEBUG);
    }

}


static inline uint16_t dynamic_keymap__get_keycode_index(uint8_t layer, uint8_t row, uint8_t col) {
    return (layer * MATRIX_ROWS * MATRIX_COLS) + (row * MATRIX_COLS) + col;
}


void dynamic_keymap__reset() {
    size_t static_macros_size = keymap__get_macro_count() * MACRO_LEN * sizeof(uint16_t);

    dynamic_keymap__save_macros((void *) &macros);
    memset(dyn_macros, 0x00, macros_size);
    memcpy(dyn_macros, &macros, static_macros_size);
    dynamic_keymap__save_keymaps((void *) &keymaps);
    memcpy(dyn_keymaps, &keymaps, keymaps_size);
}


uint16_t dynamic_keymap__get_keycode(uint8_t layer, uint8_t row, uint8_t col) {
    ESP_LOGD(TAG, "Getting key at layer %d (%d, %d)", layer, row, col);
    uint16_t index = dynamic_keymap__get_keycode_index(layer, row, col);
    uint16_t keycode = dyn_keymaps[index];
    return keycode;
}


void dynamic_keymap__set_keycode(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode) {
    ESP_LOGD(TAG, "Setting key 0x%x at layer %d (%d, %d)", keycode, layer, row, col);
    uint16_t index = dynamic_keymap__get_keycode_index(layer, row, col);
    dyn_keymaps[index] = keycode;

    // TBD, should we always save back here
    dynamic_keymap__save_keymaps((void *) dyn_keymaps);
}


uint8_t dynamic_keymap__get_layers_count() {
    return keymap__get_layer_count();
}


uint16_t dynamic_keymap__get_layers_size() {
    return (uint16_t) keymaps_size;
}


void dynamic_keymap__get_layers(uint16_t offset, uint8_t size, uint8_t *dest) {
    uint8_t *src = ((uint8_t *)dyn_keymaps) + offset;
    for (uint8_t i = 0; i < size; i++) {
        if (i + offset < keymaps_size) {
            // Converting from little-endian (esp32) to big-endian
            if (i % 2 == 0) {
                *dest++ = *(src + 1);
            }
            else {
                *dest++ = *(src - 1);
            } 
            src++;
        }
        else {
            *dest++ = 0x00;
        }
    }
}


void dynamic_keymap__set_layers(uint16_t offset, uint8_t size, uint8_t *src) {
    uint8_t *dest = ((uint8_t *)dyn_keymaps) + offset;
    for (uint8_t i = 0; i < size; i++) {
        if (i + offset < keymaps_size) {
            // Converting from big-endian to little-endian (esp32)
            if (i % 2 == 0) {
                *dest++ = *(src + 1);
            }
            else {
                *dest++ = *(src - 1);
            } 
            src++;
        }
    }

    // only save on last byte
    if (offset + size >= keymaps_size) {
        dynamic_keymap__save_keymaps((void *) dyn_keymaps);
    }
}


void dynamic_keymap__layers_reset() {
    memset(dyn_keymaps, 0x00, keymaps_size);

    // TBD, should we always save back here
    dynamic_keymap__save_keymaps((void *) dyn_keymaps);
}


uint16_t dynamic_keymap__get_macro_keycode(uint8_t macro, uint8_t key) {
    uint16_t index = (macro * MACRO_LEN) + key;
    ESP_LOGD(TAG, "Getting macro %d key: %d -> index: %d = 0x%x", macro, key, index, dyn_macros[index]);
    return dyn_macros[index];
}


uint16_t dynamic_keymap__get_macros_size() {
    return (uint16_t) macros_size;
}


void dynamic_keymap__get_macros(uint16_t offset, uint8_t size, uint8_t *dest) {
    uint8_t *src = ((uint8_t *)dyn_macros) + offset;
    for (uint16_t i = 0; i < size; i++) {
        if (i + offset < macros_size) {
            // Converting from little-endian (esp32) to big-endian
            if (i % 2 == 0) {
                *dest++ = *(src + 1);
            }
            else {
                *dest++ = *(src - 1);
            } 
            src++;
        }
        else {
            *dest++ = 0x00;
        }
    }
}


void dynamic_keymap__set_macros(uint16_t offset, uint8_t size, uint8_t *src) {
    uint8_t *dest = ((uint8_t *)dyn_macros) + offset;
    for (uint16_t i = 0; i < size; i++) {
        if (i + offset < macros_size) {
            // Converting from big-endian to little-endian (esp32)
            if (i % 2 == 0) {
                *dest++ = *(src + 1);
            }
            else {
                *dest++ = *(src - 1);
            } 
            src++;
        }
    }

    // only save on last byte
    if (offset + size >= macros_size) {
        dynamic_keymap__save_macros((void *) dyn_macros);
    }
}


void dynamic_keymap__macro_reset() {
    memset(dyn_macros, 0x00, macros_size);

    // TBD, should we always save back here
    dynamic_keymap__save_macros((void *) dyn_macros);
}


static void dynamic_keymap__load_macros() {
    ESP_LOGD(TAG, "Loading macros from NVS (size %d)", macros_size);
    memory__get_macros(dyn_macros, macros_size);
}


static void dynamic_keymap__save_macros(void *macros) {
    ESP_LOGD(TAG, "Saving macros to NVS (size %d)", macros_size);
    memory__set_macros(macros, macros_size);
}


static void dynamic_keymap__load_keymaps() {
    ESP_LOGD(TAG, "Loading keymaps from NVS (size %d)", keymaps_size);
    memory__get_keymaps(dyn_keymaps, keymaps_size);
}


static void dynamic_keymap__save_keymaps(void *keymaps) {
    ESP_LOGD(TAG, "Saving keymaps to NVS (size %d)", keymaps_size);
    memory__set_keymaps(keymaps, keymaps_size);
}


