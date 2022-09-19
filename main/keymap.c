#include "keymap.h"
#include "config.h"
#include "key_definitions.h"


/* --------- Macros --------- */
enum macro_keycodes {
    KC_MACRO_COPY = KC_BASE_MACRO,
    KC_MACRO_PASTE,
};

uint8_t macros[NUM_MACROS][MACRO_LEN] = {
    {KC_LGUI, KC_C, KC_NO},
    {KC_LGUI, KC_V, KC_NO},
};


/* --------- Layouts --------- */
// Empty layout to use when needed in layout arrays below
layout_t empty_layout = {
    {KC_NONE, KC_NONE, KC_NONE},
    {KC_NONE, KC_NONE, KC_NONE},
    {KC_NONE, KC_NONE, KC_NONE},
    {KC_NONE, KC_NONE, KC_NONE},
};

// Layout 0

layout_t layout_0 = {
    {KC_A, KC_B, KC_C},
    {KC_A, KC_B, KC_C},
    {KC_A, KC_B, KC_C},
    {KC_LAYOUT, KC_SHIFT, KC_BLUETOOTH},
};
layout_t shift_layout_0 = {
    {KC_G, KC_H, KC_I},
    {KC_G, KC_H, KC_I},
    {KC_G, KC_H, KC_I},
    {KC_LAYOUT, KC_SHIFT, KC_BLUETOOTH},
};

// Layout 1
layout_t layout_1 = {
    {KC_D, KC_E, KC_F},
    {KC_D, KC_E, KC_F},
    {KC_D, KC_E, KC_F},
    {KC_LAYOUT, KC_SHIFT, KC_BLUETOOTH},
};

// Layout modifier (when clicking on the KC_LAYOUT key in one of the normal layouts)
layout_t layout_modifier_keymap = {
    {KC_LAYOUT_0, KC_LAYOUT_1, KC_NONE},
    {KC_NONE, KC_NONE, KC_NONE},
    {KC_LAYOUT_DECREMENT, KC_LAYOUT_INCREMENT, KC_NONE},
    {KC_LAYOUT, KC_SHIFT, KC_BLUETOOTH},
};
// Bluetooth modifier layout (when clicking on the KC_BLUETOOTH key)
layout_t bluetooth_modifier_keymap = {
    {KC_BT_DEVICE_0, KC_BT_DEVICE_1, KC_BT_DEVICE_2},
    {KC_NONE, KC_BT_DEVICE_RESET, KC_NONE},
    {KC_NONE, KC_NONE, KC_NONE},
    {KC_LAYOUT, KC_SHIFT, KC_BLUETOOTH},
};


/* --------- Layout Arrays --------- */
// Normal layouts
layout_t *keymaps[NUM_LAYOUTS] = {
    &layout_0, 
    &layout_1
};

// Shift layouts (need one per layout in the array above)
layout_t *shift_keymaps[NUM_LAYOUTS] = {
    &shift_layout_0,
    &empty_layout
};
