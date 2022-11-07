#include <stdint.h>

#include "layout.h"
#include "config.h"
#include "key_definitions.h"


/* --------- Macros --------- */
enum macro_keycodes {
    COPY,
    PASTE,
    MOOM,
    SNITCH,
};

const uint16_t macros[][MACRO_LEN] = {
    [COPY]   = { KC_LGUI, KC_C },
    [PASTE]  = { KC_LGUI, KC_V },
    [MOOM]   = { KC_LSFT, KC_LGUI, KC_4 },
    [SNITCH] = { LCAG(KC_M)},
};


/* --------- Layers --------- */
enum layers {
    LAYER_0,
    LAYER_1,
    BT,
    BT_RST,
    LEDS,
};


#define _______ KC_TRNS
#define xxxxxxx KC_NO


const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = LAYOUT(
        TO(BT),           xxxxxxx,       xxxxxxx,      LCTL(KC_SPC),
        TG(LAYER_1),      MACRO(COPY),   MACRO(PASTE), M(SNITCH),
        xxxxxxx,          LCTL(KC_LEFT), KC_CC_PLAY,   LCTL(KC_RGHT)),

    [LAYER_1] = LAYOUT(
        TO(BT),           MACRO(MOOM),   KC_SPC, xxxxxxx,
        xxxxxxx,          LALT(KC_LEFT), KC_4,   LALT(KC_RGHT),
        TG(LAYER_1),      LGUI(KC_LEFT), KC_1,   LGUI(KC_RGHT)),

    [BT] = LAYOUT(
        TO(BT),           BT(0),    BT(1),     BT(2),
        TO(LAYER_0),      xxxxxxx, MO(BT_RST), xxxxxxx,
        TO(LAYER_0),      xxxxxxx,  MO(LEDS),  xxxxxxx),

    [BT_RST] = LAYOUT(
        TO(BT),          BT_RST(0), BT_RST(1), BT_RST(2),
        TO(LAYER_0),      _______,   _______,  _______,
        TO(LAYER_0),      _______,   _______,  _______),

    [LEDS] = LAYOUT(
        TO(BT),           KC_LEDD,  KC_LEDU,  xxxxxxx,
        TO(LAYER_0),      _______,  _______,  _______,
        TO(LAYER_0),      _______,  _______,  _______),

};

