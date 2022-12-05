#include <stdint.h>

#include "layout.h"
#include "config.h"
#include "key_definitions.h"


/* --------- Macro names --------- */
enum macro_keycodes {
    COPY,
    PASTE,
    MOOML,
    SNITCH,
};


/* --------- Layer names --------- */
enum layers {
    LAYER_0,
    LAYER_1,
    LAYER_2,
    LAYER_3,
    BT,
    BT_RST,
    LEDS,
};


/* --------- Keycode names --------- */
#define _______ KC_TRNS
#define xxxxxxx KC_NO


#define KC_MOOM LCA(KC_SPC)
#define KC_FUSC LCG(KC_F)    // toggle fullscreen (macos)
#define M_FUSC  KC_7         // toggle fullscreen (moom)


/* --------- Macros --------- */
const uint16_t macros[][MACRO_LEN] = {
    [COPY]   = { KC_LGUI, KC_C, KC_NO },
    [PASTE]  = { KC_LGUI, KC_V, KC_NO },
    [MOOML]  = { TG(LAYER_3), KC_MOOM },
    [SNITCH] = { LCAG(KC_M) },
};


/* --------- Layers --------- */
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = LAYOUT(
        TO(BT),           M(MOOML),    KC_B,         KC_C,
        TG(LAYER_1),      LSFT(KC_A),  LSC(KC_DOWN), M(SNITCH),
        xxxxxxx,          xxxxxxx,     RSC(KC_DOWN), LAG(KC_DOWN)),

    [LAYER_1] = LAYOUT(
        TO(BT),           KC_AT,    KC_LPRN,  KC_RPRN,
        TG(LAYER_2),      KC_D,     KC_E,     KC_F,
        TG(LAYER_1),      M(COPY),  M(PASTE), KC_F),

    [LAYER_2] = LAYOUT(
        TO(BT),           KC_G,     KC_H,     KC_J,
        TG(LAYER_3),      KC_G,     KC_H,     KC_J,
        TG(LAYER_2),      _______,  _______,  _______),

    [LAYER_3] = LAYOUT(
        TO(BT),           KC_MOOM,       KC_SPC, M_FUSC,
        xxxxxxx,          LALT(KC_LEFT), KC_4,   LALT(KC_RGHT),
        TG(LAYER_3),      LGUI(KC_LEFT), KC_1,   LGUI(KC_RGHT)),

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

