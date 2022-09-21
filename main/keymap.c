#include "keymap.h"
#include "config.h"
#include "key_definitions.h"


/* --------- Macros --------- */
enum macro_keycodes {
    COPY,
    PASTE,
};

uint16_t macros[][MACRO_LEN] = {
    [COPY]  = { KC_LGUI, KC_C, KC_NO },
    [PASTE] = { KC_LGUI, KC_V, KC_NO },
};


/* --------- Layers --------- */
enum layers {
    LAYER_0,
    LAYER_1,
    LAYER_2,
    BT,
    BT_RESET,
    LEDS,
};


#define _______ KC_TRNS
#define xxxxxxx KC_NO


const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = {
        { KC_A,        KC_B,        KC_C },
        { KC_A,        KC_B,        KC_C },
        { M(COPY),     M(PASTE),    KC_C },
        { TO(LAYER_0), TO(LAYER_1), TO(LEDS) },
    },

    [LAYER_1] = {
        { KC_D,        KC_E,        KC_F },
        { KC_D,        KC_E,        KC_F },
        { KC_D,        KC_E,        KC_F },
        { TO(LAYER_0), TO(LAYER_1), TO(LEDS) },
    },

    [LAYER_2] = {
        { KC_G,        KC_H,        KC_I },
        { KC_G,        KC_H,        KC_I },
        { KC_G,        KC_H,        KC_I },
        { TO(LAYER_0), TO(LAYER_1), TO(LEDS) },
    },

    [BT] = {
        { BT(0),         BT(1),         BT(2) },
        { xxxxxxx,       MO(BT_RESET),  xxxxxxx },
        { xxxxxxx,       xxxxxxx,       xxxxxxx },
        { TO(LAYER_0),   TO(LAYER_1),   TO(LAYER_2) },
    },

    [BT_RESET] = {
        { BT_RESET(0),   BT_RESET(1),   BT_RESET(2) },
        { xxxxxxx,       MO(BT_RESET),  xxxxxxx },
        { xxxxxxx,       xxxxxxx,       xxxxxxx },
        { TO(LAYER_0),   TO(LAYER_1),   TO(LAYER_2) },
    },

    [LEDS] = {
        { KC_BRID,      KC_BRIU,      xxxxxxx },
        { xxxxxxx,      xxxxxxx,      xxxxxxx },
        { xxxxxxx,      xxxxxxx,      xxxxxxx },
        { TO(LAYER_0),  TO(LAYER_1),  TO(LAYER_2) },
    },

};

