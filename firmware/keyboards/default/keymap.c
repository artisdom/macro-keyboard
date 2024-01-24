#include <stdint.h>

#include "layout.h"
#include "config.h"
#include "key_definitions.h"


/* --------- Macro names --------- */
enum macro_keycodes {
    COPY,
    PASTE,
};


/* --------- Layer names --------- */
enum layers {
    LAYER_0,
    LAYER_1,
    LAYER_2,
    BT,
    BT_RST,
    LEDS,
};


/* --------- Keycode names --------- */
#define _______ KC_TRNS
#define xxxxxxx KC_NO


#define KC_FUSC LCG(KC_F)    // toggle fullscreen (macos)


/* --------- Macros --------- */
const uint16_t macros[][MACRO_LEN] = {
    [COPY]   = { KC_LGUI, KC_C, KC_NO },
    [PASTE]  = { KC_LGUI, KC_V, KC_NO },
};


/* --------- Layers --------- */
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = LAYOUT(
        TO(BT),           OSL(LAYER_1),  xxxxxxx,      KC_FUSC,
        TG(LAYER_1),      MACRO(COPY),   MACRO(PASTE), xxxxxxx,
        xxxxxxx,          LCTL(KC_LEFT), KC_PLPS,      LCTL(KC_RGHT)
    ),
    [LAYER_1] = LAYOUT(
        TO(BT),           KC_KP_1,   KC_KP_2,   KC_KP_3,
        TG(LAYER_2),      KC_KP_4,   KC_KP_5,   KC_KP_6,
        TG(LAYER_1),      KC_KP_7,   KC_KP_8,   KC_KP_9
    ),
    [LAYER_2] = LAYOUT(
        TO(BT),           _______,  _______,  _______,
        xxxxxxx,          _______,  KC_UP,    _______,
        TG(LAYER_2),      KC_LEFT,  KC_DOWN,  KC_RIGHT
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
        TO(BT),           BL_DOWN,  BL_UP,    xxxxxxx,
        TO(LAYER_0),      BL_ON,    BL_OFF,   BL_TOGG,
        TO(LAYER_0),      _______,  _______,  _______
    ),

};

