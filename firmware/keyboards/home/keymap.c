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
    SUBL_GROUP_R,
    SUBL_GROUP_L,
    SUBL_GROUP_FILTER,
    SUBL_GROUP_MOVE_R,
    SUBL_GROUP_MOVE_L,
};


/* --------- Layer names --------- */
enum layers {
    LAYER_0,
    LAYER_SUBL,
    LAYER_BLEND,
    LAYER_3,
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
#define SUBL_GOTO_DEF LAG(KC_DOWN)       // Sublime Text Goto Definition
#define SUBL_GOTO_REF LSFT(LAG(KC_DOWN)) // Sublime Text Goto Definition
#define SUBL_GOTO_BCK LCAG(KC_DOWN)      // Sublime Text Goto Backwards
#define SUBL_SEL_EXPD SCMD(KC_A)         // Sublime Text Expand Selection
#define SUBL_SEL_LINE LCMD(KC_L)         // Sublime Text Expand Selection to Line
#define SUBL_SWAP_LINES LCG(KC_UP)       // Sublime Text Swap Line Up


/* --------- Macros --------- */
const uint16_t macros[][MACRO_LEN] = {
    [COPY]   = { KC_LGUI, KC_C },
    [PASTE]  = { KC_LGUI, KC_V },
    [MOOML]  = { OSL(LAYER_MOOM), KC_MOOM },
    [SNITCH] = { LCAG(KC_M) },
    [SUBL_GROUP_R] = { LCMD(KC_K), LCMD(KC_RIGHT) },
    [SUBL_GROUP_L] = { LCMD(KC_K), LCMD(KC_LEFT) },
    [SUBL_GROUP_FILTER] = { LCMD(KC_K), LCMD(KC_F) },
    [SUBL_GROUP_MOVE_R] = { LCMD(KC_K), S(KC_RIGHT) },   // Does not work... why?
    [SUBL_GROUP_MOVE_L] = { KC_LCMD, KC_K, S(KC_LEFT) }, // Does not work... why?
};


/* --------- Layers --------- */
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_0] = LAYOUT(
        TO(BT),           MACRO(MOOML),  xxxxxxx,      LCTL(KC_SPC),
        TG(LAYER_SUBL),   xxxxxxx,       KC_PLPS,      M(SNITCH),
        xxxxxxx,          LCTL(KC_LEFT), LCTL(KC_UP),  LCTL(KC_RGHT)
    ),

    [LAYER_SUBL] = LAYOUT(
        TO(BT),           SUBL_SEL_EXPD,    SUBL_GOTO_REF,  M(SUBL_GROUP_FILTER),
        TG(LAYER_BLEND),  SUBL_SEL_LINE,    SUBL_GOTO_BCK,  SUBL_SWAP_LINES,
        TG(LAYER_SUBL),   M(SUBL_GROUP_L),  SUBL_GOTO_DEF,  M(SUBL_GROUP_R)
    ),

    [LAYER_BLEND] = LAYOUT(
        TO(BT),           KC_X,      KC_Y,      KC_Z,
        TG(LAYER_3),      xxxxxxx,   KC_LSFT,   LCMD(KC_P0),
        TG(LAYER_BLEND),  KC_PSLS,   KC_KP_DOT, KC_P0
    ),

    [LAYER_3] = LAYOUT(
        TO(BT),           xxxxxxx,   xxxxxxx,   xxxxxxx,
        TG(LAYER_MOOM),   xxxxxxx,   xxxxxxx,   xxxxxxx,
        TG(LAYER_3),      xxxxxxx,   xxxxxxx,   xxxxxxx
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
        TO(BT),           BL_DOWN,  BL_UP,    xxxxxxx,
        TO(LAYER_0),      BL_ON,    BL_OFF,   BL_TOGG,
        TO(LAYER_0),      _______,  _______,  _______
    ),

};

