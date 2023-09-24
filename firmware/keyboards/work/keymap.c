#include <stdint.h>

#include "layout.h"
#include "config.h"
#include "key_definitions.h"


/* --------- Macros names --------- */
enum macro_keycodes {
    COPY,
    PASTE,
    MOOML,
    SNITCH,
    SUBL_GROUP_R,
    SUBL_GROUP_L,
    SUBL_GROUP_FILTER,
};


/* --------- Layer names --------- */
enum layers {
    LAYER_0,
    LAYER_SUBL,
    LAYER_MOOM,
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
    [COPY]   = { KC_LGUI, KC_C },
    [PASTE]  = { KC_LGUI, KC_V },
    [MOOML]  = { OSL(LAYER_MOOM), KC_MOOM },
    [SNITCH] = { LCAG(KC_M) },
    [SUBL_GROUP_R] = { LCMD(KC_K), LCMD(KC_RIGHT) },
    [SUBL_GROUP_L] = { LCMD(KC_K), LCMD(KC_LEFT) },
    [SUBL_GROUP_FILTER] = { LCMD(KC_K), LCMD(KC_F) },
};


/* --------- Layers --------- */
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = LAYOUT(
        TO(BT),           MACRO(MOOML),  xxxxxxx,      xxxxxxx,
        TG(LAYER_SUBL),   MACRO(COPY),   MACRO(PASTE), M(SNITCH),
        xxxxxxx,          LCTL(KC_LEFT), KC_CC_PLAY,   LCTL(KC_RGHT)
    ),
    
    [LAYER_SUBL] = LAYOUT(
        TO(BT),           xxxxxxx,         LSFT(LAG(KC_DOWN)), M(SUBL_GROUP_FILTER),
        TG(LAYER_MOOM),   M(SUBL_GROUP_L),   LCAG(KC_DOWN),    M(SUBL_GROUP_R),
        TG(LAYER_SUBL),   LAG(KC_LEFT),       LAG(KC_DOWN),    LAG(KC_RIGHT)
    ),

    [LAYER_MOOM] = LAYOUT(
        TO(BT),           KC_MOOM,       KC_SPC, M_FUSC,
        xxxxxxx,          LGUI(KC_LEFT), KC_4,   LGUI(KC_RGHT),
        TG(LAYER_MOOM),   LALT(KC_LEFT), KC_1,   LALT(KC_RGHT)
    ),

    [BT] = LAYOUT(
        TO(BT),           BT(0),    BT(1),     BT(2),
        TO(LAYER_0),      xxxxxxx, MO(BT_RST), xxxxxxx,
        TO(LAYER_0),      xxxxxxx,  MO(LEDS),  xxxxxxx
    ),

    [BT_RST] = LAYOUT(
        TO(BT),          BT_RST(0), BT_RST(1), BT_RST(2),
        TO(LAYER_0),      _______,   _______,  _______,
        TO(LAYER_0),      _______,   _______,  _______
    ),

    [LEDS] = LAYOUT(
        TO(BT),           KC_LEDD,  KC_LEDU,  xxxxxxx,
        TO(LAYER_0),      _______,  _______,  _______,
        TO(LAYER_0),      _______,  _______,  _______
    ),

};

