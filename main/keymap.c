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
    {KC_NONE, KC_NONE, KC_NONE}
};

// Layout 1
layout_t layout_1 = {
    {KC_BLUETOOTH, KC_CC_PLAY_PAUSE, KC_CC_VOL_UP}
};
layout_t shift_layout_1 = {
    {KC_SHIFT, KC_G, KC_H}
};

// Layout 2
layout_t layout_2 = {
    {KC_LAYOUT, KC_E, KC_F}
};

// Layout modifier (when clicking on the KC_LAYOUT key in one of the normal layouts)
layout_t layout_modifier_keymap = {
    {KC_LAYOUT, KC_LAYOUT_DECREMENT, KC_LAYOUT_INCREMENT}
};
// Bluetooth modifier layout (when clicking on the KC_BLUETOOTH key)
layout_t bluetooth_modifier_keymap = {
    {KC_BLUETOOTH, KC_BT_DEVICE_0, KC_BT_DEVICE_1}
};


/* --------- Layout Arrays --------- */
// Normal layouts
layout_t *keymaps[NUM_LAYOUTS] = {
	&layout_1, 
	&layout_2
};

// Shift layouts (need one per layout in the array above)
layout_t *shift_keymaps[NUM_LAYOUTS] = {
	&shift_layout_1,
	&empty_layout
};
