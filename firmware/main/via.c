#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_log.h"

#include "config.h"
#include "keymap.h" // to remove
#include "dynamic_keymap.h"
#include "memory.h"


/* --------- Local Defines --------- */
#define VIA_PROTOCOL_VERSION    (0x000B) // = 11

// #define MAX_VIA_MACRO        (16)


/* --------- Local Variables --------- */
static const char *TAG = "via";


/* --------- Local Declarations --------- */
enum via_command_id {
    id_get_protocol_version                 = 0x01, // IMPLEMENTED 
    id_get_keyboard_value                   = 0x02, // see below
    id_set_keyboard_value                   = 0x03, // see below
    id_dynamic_keymap_get_keycode           = 0x04, // IMPLEMENTED
    id_dynamic_keymap_set_keycode           = 0x05, // IMPLEMENTED
    id_dynamic_keymap_reset                 = 0x06, // IMPLEMENTED
    id_lighting_set_value                   = 0x07, // NOT IMPLEMENTED
    id_lighting_get_value                   = 0x08, // NOT IMPLEMENTED
    id_lighting_save                        = 0x09, // NOT IMPLEMENTED
    id_eeprom_reset                         = 0x0A, // IMPLEMENTED
    id_bootloader_jump                      = 0x0B, // NOT IMPLEMENTED
    id_dynamic_keymap_macro_get_count       = 0x0C, // IMPLEMENTED
    id_dynamic_keymap_macro_get_buffer_size = 0x0D, // IMPLEMENTED
    id_dynamic_keymap_macro_get_buffer      = 0x0E, // IMPLEMENTED
    id_dynamic_keymap_macro_set_buffer      = 0x0F, // IMPLEMENTED
    id_dynamic_keymap_macro_reset           = 0x10, // IMPLEMENTED
    id_dynamic_keymap_get_layer_count       = 0x11, // IMPLEMENTED
    id_dynamic_keymap_get_buffer            = 0x12, // IMPLEMENTED
    id_dynamic_keymap_set_buffer            = 0x13, // IMPLEMENTED
    id_dynamic_keymap_get_encoder           = 0x14, // NOT IMPLEMENTED
    id_dynamic_keymap_set_encoder           = 0x15, // NOT IMPLEMENTED
    id_unhandled                            = 0xFF,
};

enum via_keyboard_value_id {
    id_uptime              = 0x01, // NOT IMPLEMENTED
    id_layout_options      = 0x02, // IMPLEMENTED
    id_switch_matrix_state = 0x03, // NOT IMPLEMENTED
};


/* --------- Local Functions --------- */
static uint8_t via__get_macro_count();



void via__init() {
    //
}


void via__hid_receive(uint8_t *data, uint8_t length) {
    uint8_t *command_id   = &(data[0]);
    uint8_t *command_data = &(data[1]);

    ESP_LOGI(TAG, "Received command id: 0x%02x", *command_id);

    switch (*command_id) {
        case id_get_protocol_version: {
            command_data[0] = VIA_PROTOCOL_VERSION >> 8;
            command_data[1] = VIA_PROTOCOL_VERSION & 0xFF;
            break;
        }
        case id_get_keyboard_value: {
            switch (command_data[0]) {
                case id_layout_options: {
                    uint32_t value = memory__get_via_layout_options();
                    command_data[1] = (value >> 24) & 0xFF;
                    command_data[2] = (value >> 16) & 0xFF;
                    command_data[3] = (value >> 8) & 0xFF;
                    command_data[4] = value & 0xFF;
                    break;
                }
                default: {
                    ESP_LOGW(TAG, "Unhandled keyboard value (%d) command: %d", id_get_keyboard_value, command_data[0]);
                    break;
                }
            }
            break;
        }
        case id_set_keyboard_value: {
            switch (command_data[0]) {
                case id_layout_options: {
                    uint32_t value = ((uint32_t)command_data[1] << 24) | ((uint32_t)command_data[2] << 16) | ((uint32_t)command_data[3] << 8) | (uint32_t)command_data[4];
                    memory__set_via_layout_options(value);
                    break;
                }
                default: {
                    ESP_LOGW(TAG, "Unhandled keyboard value (0x03) command: %d", command_data[0]);
                    break;
                }
            }
            break;
        }
        case id_dynamic_keymap_get_keycode: {
            uint16_t keycode = dynamic_keymap__get_keycode(command_data[0], command_data[1], command_data[2]);
            command_data[3]  = keycode >> 8;
            command_data[4]  = keycode & 0xFF;
            break;
        }
        case id_dynamic_keymap_set_keycode: {
            dynamic_keymap__set_keycode(command_data[0], command_data[1], command_data[2], (command_data[3] << 8) | command_data[4]);
            break;
        }
        case id_dynamic_keymap_reset: {
            dynamic_keymap__layers_reset();
            break;
        }
        case id_eeprom_reset: {
            dynamic_keymap__reset();
            break;
        }
        case id_dynamic_keymap_macro_get_count: {
            command_data[0] = via__get_macro_count();
            ESP_LOGD(TAG, "macro count: %d", command_data[0]);
            break;
        }
        case id_dynamic_keymap_macro_get_buffer_size: {
            uint16_t size = dynamic_keymap__get_macros_size();
            ESP_LOGD(TAG, "macro buffer size: %d", size);
            command_data[0] = size >> 8;
            command_data[1] = size & 0xFF;
            break;
        }
        case id_dynamic_keymap_macro_get_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint8_t size = command_data[2]; // size <= 28
            ESP_LOGD(TAG, "macro get buffer -> offset: %d, size: %d", offset, size);
            dynamic_keymap__get_macros(offset, size, &(command_data[3]));
            break;
        }
        case id_dynamic_keymap_macro_set_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint8_t size = command_data[2]; // size <= 28
            ESP_LOGD(TAG, "macro set buffer -> offset: %d, size: %d ", offset, size);
            dynamic_keymap__set_macros(offset, size, &(command_data[3]));
            break;
        }
        case id_dynamic_keymap_macro_reset: {
            dynamic_keymap__macro_reset();
            break;
        }
        case id_dynamic_keymap_get_layer_count: {
            command_data[0] = dynamic_keymap__get_layers_count();
            break;
        }
        case id_dynamic_keymap_get_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size = command_data[2]; // size <= 28
            ESP_LOGD(TAG, "offset: %d, size: %d", offset, size);
            dynamic_keymap__get_layers(offset, size, &(command_data[3]));
            break;
        }
        case id_dynamic_keymap_set_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size = command_data[2]; // size <= 28
            ESP_LOGD(TAG, "offset: %d, size: %d", offset, size);
            dynamic_keymap__set_layers(offset, size, &(command_data[3]));
            break;

        }
        default: {
            ESP_LOGW(TAG, "Unhandled command: %d", *command_id);
            *command_id = id_unhandled;
            break;
        }
    }

    ESP_LOGD(TAG, "Returning buffer");
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, data, length, ESP_LOG_DEBUG);
}


static uint8_t via__get_macro_count() {
    return MAX_MACRO;
}


