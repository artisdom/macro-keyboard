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


// To be moved to other file, once project supports multi-keyboards
#define LAYOUT( \
    K32,   K00, K01, K02, \
    K31,   K10, K11, K12, \
    K30,   K20, K21, K22  \
  ) { \
    { K00, K01, K02 }, \
    { K10, K11, K12 }, \
    { K20, K21, K22 }, \
    { K30, K31, K32 }  \
}


#define _______ KC_TRNS
#define xxxxxxx KC_NO


const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = LAYOUT(
        TO(BT),           KC_A,     KC_B,     KC_C,
        TO(LAYER_1),      KC_A,     KC_B,     KC_C,
        TO(LAYER_0),      M(COPY),  M(PASTE), KC_C),

    [LAYER_1] = LAYOUT(
        TO(BT),           KC_D,     KC_E,     KC_F,
        TO(LAYER_2),      KC_D,     KC_E,     KC_F,
        TO(LAYER_0),      KC_D,     KC_E,     KC_F),

    [LAYER_2] = LAYOUT(
        TO(BT),           KC_G,     KC_H,     KC_J,
        TO(LAYER_2),      KC_G,     KC_H,     KC_J,
        TO(LAYER_1),      KC_G,     KC_H,     KC_J),

    [BT] = LAYOUT(
        TO(BT),           BT(0),    BT(1),     BT(2),
        TO(LAYER_0),      xxxxxxx, MO(BT_RST), xxxxxxx,
        TO(LAYER_0),      xxxxxxx,  MO(LEDS),  xxxxxxx),

    [BT_RST] = LAYOUT(
        TO(BT),          BT_RST(0), BT_RST(1), BT_RST(2),
        TO(LAYER_0),      xxxxxxx, MO(BT_RST), xxxxxxx,
        TO(LAYER_0),      xxxxxxx,  xxxxxxx,   xxxxxxx),

    [LEDS] = LAYOUT(
        TO(BT),           KC_BRID,  KC_BRIU,  xxxxxxx,
        TO(LAYER_0),      xxxxxxx,  xxxxxxx,  xxxxxxx,
        TO(LAYER_0),      xxxxxxx,  MO(LEDS), xxxxxxx),

};

