#include <stdint.h>

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
    BT_RST,
    LEDS,
};


#define _______ KC_TRNS
#define xxxxxxx KC_NO


const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = LAYOUT(
        TO(BT),           KC_A,     KC_B,         KC_C,
        TG(LAYER_1),    LSFT(KC_A), LSC(KC_DOWN), LAG(KC_DOWN),
        xxxxxxx,          xxxxxxx,  RSC(KC_DOWN), xxxxxxx),

    [LAYER_1] = LAYOUT(
        TO(BT),           KC_AT,    KC_LPRN,  KC_RPRN,
        TG(LAYER_2),      KC_D,     KC_E,     KC_F,
        TG(LAYER_1),      M(COPY),  M(PASTE), KC_F),

    [LAYER_2] = LAYOUT(
        TO(BT),           KC_G,     KC_H,     KC_J,
        xxxxxxx,          KC_G,     KC_H,     KC_J,
        TG(LAYER_2),      _______,  _______,  _______),

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

