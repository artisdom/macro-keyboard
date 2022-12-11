#ifndef KEY_DEFINITIONS_H
#define KEY_DEFINITIONS_H

// taken QMK which itself comes from TMK

/*
 * Short names for ease of definition of keymap
 */
/* Transparent */
#define KC_TRANSPARENT 0x01
#define KC_TRNS KC_TRANSPARENT

/* Punctuation */
#define KC_ENT  KC_ENTER
#define KC_ESC  KC_ESCAPE
#define KC_BSPC KC_BACKSPACE
#define KC_SPC  KC_SPACE
#define KC_MINS KC_MINUS
#define KC_EQL  KC_EQUAL
#define KC_LBRC KC_LEFT_BRACKET
#define KC_RBRC KC_RIGHT_BRACKET
#define KC_BSLS KC_BACKSLASH
#define KC_NUHS KC_NONUS_HASH
#define KC_SCLN KC_SEMICOLON
#define KC_QUOT KC_QUOTE
#define KC_GRV  KC_GRAVE
#define KC_COMM KC_COMMA
#define KC_SLSH KC_SLASH
#define KC_NUBS KC_NONUS_BACKSLASH

/* Lock Keys */
#define KC_CAPS KC_CAPS_LOCK
#define KC_SCRL KC_SCROLL_LOCK
#define KC_NUM  KC_NUM_LOCK
#define KC_LCAP KC_LOCKING_CAPS_LOCK
#define KC_LNUM KC_LOCKING_NUM_LOCK
#define KC_LSCR KC_LOCKING_SCROLL_LOCK

/* Commands */
#define KC_PSCR KC_PRINT_SCREEN
#define KC_PAUS KC_PAUSE
#define KC_BRK  KC_PAUSE
#define KC_INS  KC_INSERT
#define KC_PGUP KC_PAGE_UP
#define KC_DEL  KC_DELETE
#define KC_PGDN KC_PAGE_DOWN
#define KC_RGHT KC_RIGHT
#define KC_APP  KC_APPLICATION
#define KC_EXEC KC_EXECUTE
#define KC_SLCT KC_SELECT
#define KC_AGIN KC_AGAIN
#define KC_PSTE KC_PASTE
#define KC_ERAS KC_ALTERNATE_ERASE
#define KC_SYRQ KC_SYSTEM_REQUEST
#define KC_CNCL KC_CANCEL
#define KC_CLR  KC_CLEAR
#define KC_PRIR KC_PRIOR
#define KC_RETN KC_RETURN
#define KC_SEPR KC_SEPARATOR
#define KC_CLAG KC_CLEAR_AGAIN
#define KC_CRSL KC_CRSEL
#define KC_EXSL KC_EXSEL

/* Keypad */
#define KC_PSLS KC_KP_SLASH
#define KC_PAST KC_KP_ASTERISK
#define KC_PMNS KC_KP_MINUS
#define KC_PPLS KC_KP_PLUS
#define KC_PENT KC_KP_ENTER
#define KC_P1   KC_KP_1
#define KC_P2   KC_KP_2
#define KC_P3   KC_KP_3
#define KC_P4   KC_KP_4
#define KC_P5   KC_KP_5
#define KC_P6   KC_KP_6
#define KC_P7   KC_KP_7
#define KC_P8   KC_KP_8
#define KC_P9   KC_KP_9
#define KC_P0   KC_KP_0
#define KC_PDOT KC_KP_DOT
#define KC_PEQL KC_KP_EQUAL
#define KC_PCMM KC_KP_COMMA

/* Language Specific */
#define KC_INT1 KC_INTERNATIONAL_1
#define KC_INT2 KC_INTERNATIONAL_2
#define KC_INT3 KC_INTERNATIONAL_3
#define KC_INT4 KC_INTERNATIONAL_4
#define KC_INT5 KC_INTERNATIONAL_5
#define KC_INT6 KC_INTERNATIONAL_6
#define KC_INT7 KC_INTERNATIONAL_7
#define KC_INT8 KC_INTERNATIONAL_8
#define KC_INT9 KC_INTERNATIONAL_9
#define KC_LNG1 KC_LANGUAGE_1
#define KC_LNG2 KC_LANGUAGE_2
#define KC_LNG3 KC_LANGUAGE_3
#define KC_LNG4 KC_LANGUAGE_4
#define KC_LNG5 KC_LANGUAGE_5
#define KC_LNG6 KC_LANGUAGE_6
#define KC_LNG7 KC_LANGUAGE_7
#define KC_LNG8 KC_LANGUAGE_8
#define KC_LNG9 KC_LANGUAGE_9

/* Modifiers */
#define KC_LCTL KC_LEFT_CTRL
#define KC_LSFT KC_LEFT_SHIFT
#define KC_LALT KC_LEFT_ALT
#define KC_LOPT KC_LEFT_ALT
#define KC_LGUI KC_LEFT_GUI
#define KC_LCMD KC_LEFT_GUI
#define KC_LWIN KC_LEFT_GUI
#define KC_RCTL KC_RIGHT_CTRL
#define KC_RSFT KC_RIGHT_SHIFT
#define KC_RALT KC_RIGHT_ALT
#define KC_ALGR KC_RIGHT_ALT
#define KC_ROPT KC_RIGHT_ALT
#define KC_RGUI KC_RIGHT_GUI
#define KC_RCMD KC_RIGHT_GUI
#define KC_RWIN KC_RIGHT_GUI

/* Generic Desktop Page (0x01) */
#define KC_PWR  KC_SYSTEM_POWER
#define KC_SLEP KC_SYSTEM_SLEEP
#define KC_WAKE KC_SYSTEM_WAKE

/* Consumer Page (0x0C) */
#define KC_MUTE KC_AUDIO_MUTE
#define KC_VOLU KC_AUDIO_VOL_UP
#define KC_VOLD KC_AUDIO_VOL_DOWN
#define KC_MNXT KC_MEDIA_NEXT_TRACK
#define KC_MPRV KC_MEDIA_PREV_TRACK
#define KC_MSTP KC_MEDIA_STOP
#define KC_MPLY KC_MEDIA_PLAY_PAUSE
#define KC_MSEL KC_MEDIA_SELECT
#define KC_EJCT KC_MEDIA_EJECT
#define KC_CALC KC_CALCULATOR
#define KC_MYCM KC_MY_COMPUTER
#define KC_WSCH KC_WWW_SEARCH
#define KC_WHOM KC_WWW_HOME
#define KC_WBAK KC_WWW_BACK
#define KC_WFWD KC_WWW_FORWARD
#define KC_WSTP KC_WWW_STOP
#define KC_WREF KC_WWW_REFRESH
#define KC_WFAV KC_WWW_FAVORITES
#define KC_MFFD KC_MEDIA_FAST_FORWARD
#define KC_MRWD KC_MEDIA_REWIND
#define KC_BRIU KC_BRIGHTNESS_UP
#define KC_BRID KC_BRIGHTNESS_DOWN

/* System Specific */
#define KC_BRMU KC_PAUSE
#define KC_BRMD KC_SCROLL_LOCK

/* Mouse Keys */
#define KC_MS_U KC_MS_UP
#define KC_MS_D KC_MS_DOWN
#define KC_MS_L KC_MS_LEFT
#define KC_MS_R KC_MS_RIGHT
#define KC_BTN1 KC_MS_BTN1
#define KC_BTN2 KC_MS_BTN2
#define KC_BTN3 KC_MS_BTN3
#define KC_BTN4 KC_MS_BTN4
#define KC_BTN5 KC_MS_BTN5
#define KC_BTN6 KC_MS_BTN6
#define KC_BTN7 KC_MS_BTN7
#define KC_BTN8 KC_MS_BTN8
#define KC_WH_U KC_MS_WH_UP
#define KC_WH_D KC_MS_WH_DOWN
#define KC_WH_L KC_MS_WH_LEFT
#define KC_WH_R KC_MS_WH_RIGHT
#define KC_ACL0 KC_MS_ACCEL0
#define KC_ACL1 KC_MS_ACCEL1
#define KC_ACL2 KC_MS_ACCEL2

/* Misc key aliases*/
#define KC_NONE KC_UNDEFINED


// USB HID Keyboard/Keypad Usage(0x07)
enum hid_keyboard_keypad_usage {
    KC_NO = 0x00,
    KC_ROLL_OVER,
    KC_POST_FAIL,
    KC_UNDEFINED,
    KC_A,
    KC_B,
    KC_C,
    KC_D,
    KC_E,
    KC_F,
    KC_G,
    KC_H,
    KC_I,
    KC_J,
    KC_K,
    KC_L,
    KC_M, // 0x10
    KC_N,
    KC_O,
    KC_P,
    KC_Q,
    KC_R,
    KC_S,
    KC_T,
    KC_U,
    KC_V,
    KC_W,
    KC_X,
    KC_Y,
    KC_Z,
    KC_1,
    KC_2,
    KC_3, // 0x20
    KC_4,
    KC_5,
    KC_6,
    KC_7,
    KC_8,
    KC_9,
    KC_0,
    KC_ENTER,
    KC_ESCAPE,
    KC_BACKSPACE,
    KC_TAB,
    KC_SPACE,
    KC_MINUS,
    KC_EQUAL,
    KC_LEFT_BRACKET,
    KC_RIGHT_BRACKET, // 0x30
    KC_BACKSLASH,
    KC_NONUS_HASH,
    KC_SEMICOLON,
    KC_QUOTE,
    KC_GRAVE,
    KC_COMMA,
    KC_DOT,
    KC_SLASH,
    KC_CAPS_LOCK,
    KC_F1,
    KC_F2,
    KC_F3,
    KC_F4,
    KC_F5,
    KC_F6,
    KC_F7, // 0x40
    KC_F8,
    KC_F9,
    KC_F10,
    KC_F11,
    KC_F12,
    KC_PRINT_SCREEN,
    KC_SCROLL_LOCK,
    KC_PAUSE,
    KC_INSERT,
    KC_HOME,
    KC_PAGE_UP,
    KC_DELETE,
    KC_END,
    KC_PAGE_DOWN,
    KC_RIGHT,
    KC_LEFT, // 0x50
    KC_DOWN,
    KC_UP,
    KC_NUM_LOCK,
    KC_KP_SLASH,
    KC_KP_ASTERISK,
    KC_KP_MINUS,
    KC_KP_PLUS,
    KC_KP_ENTER,
    KC_KP_1,
    KC_KP_2,
    KC_KP_3,
    KC_KP_4,
    KC_KP_5,
    KC_KP_6,
    KC_KP_7,
    KC_KP_8, // 0x60
    KC_KP_9,
    KC_KP_0,
    KC_KP_DOT,
    KC_NONUS_BACKSLASH,
    KC_APPLICATION,
    KC_KB_POWER,
    KC_KP_EQUAL,
    KC_F13,
    KC_F14,
    KC_F15,
    KC_F16,
    KC_F17,
    KC_F18,
    KC_F19,
    KC_F20,
    KC_F21, // 0x70
    KC_F22,
    KC_F23,
    KC_F24,
    KC_EXECUTE,
    KC_HELP,
    KC_MENU,
    KC_SELECT,
    KC_STOP,
    KC_AGAIN,
    KC_UNDO,
    KC_CUT,
    KC_COPY,
    KC_PASTE,
    KC_FIND,
    KC_KB_MUTE,
    KC_KB_VOLUME_UP, // 0x80
    KC_KB_VOLUME_DOWN,
    KC_LOCKING_CAPS_LOCK,
    KC_LOCKING_NUM_LOCK,
    KC_LOCKING_SCROLL_LOCK,
    KC_KP_COMMA,
    KC_KP_EQUAL_AS400,
    KC_INTERNATIONAL_1,
    KC_INTERNATIONAL_2,
    KC_INTERNATIONAL_3,
    KC_INTERNATIONAL_4,
    KC_INTERNATIONAL_5,
    KC_INTERNATIONAL_6,
    KC_INTERNATIONAL_7,
    KC_INTERNATIONAL_8,
    KC_INTERNATIONAL_9,
    KC_LANGUAGE_1, // 0x90
    KC_LANGUAGE_2,
    KC_LANGUAGE_3,
    KC_LANGUAGE_4,
    KC_LANGUAGE_5,
    KC_LANGUAGE_6,
    KC_LANGUAGE_7,
    KC_LANGUAGE_8,
    KC_LANGUAGE_9,
    KC_ALTERNATE_ERASE,
    KC_SYSTEM_REQUEST,
    KC_CANCEL,
    KC_CLEAR,
    KC_PRIOR,
    KC_RETURN,
    KC_SEPARATOR,
    KC_OUT, // 0xA0
    KC_OPER,
    KC_CLEAR_AGAIN,
    KC_CRSEL,
    KC_EXSEL,

#if 0
  // ***************************************************************
  // These keycodes are present in the HID spec, but are           *
  // nonfunctional on modern OSes. QMK uses this range (0xA5-0xDF) *
  // for the media and function keys instead - see below.          *
  // ***************************************************************

  KC_KP_00                = 0xB0,
  KC_KP_000,
  KC_THOUSANDS_SEPARATOR,
  KC_DECIMAL_SEPARATOR,
  KC_CURRENCY_UNIT,
  KC_CURRENCY_SUB_UNIT,
  KC_KP_LEFT_PARENTHESIS,
  KC_KP_RIGHT_PARENTHESIS,
  KC_KP_LEFT_BRACE,
  KC_KP_RIGHT_BRACE,
  KC_KP_TAB,
  KC_KP_BACKSPACE,
  KC_KP_A,
  KC_KP_B,
  KC_KP_C,
  KC_KP_D,
  KC_KP_E,                //0xC0
  KC_KP_F,
  KC_KP_XOR,
  KC_KP_HAT,
  KC_KP_PERCENT,
  KC_KP_LESS_THAN,
  KC_KP_GREATER_THAN,
  KC_KP_AND,
  KC_KP_LAZY_AND,
  KC_KP_OR,
  KC_KP_LAZY_OR,
  KC_KP_COLON,
  KC_KP_HASH,
  KC_KP_SPACE,
  KC_KP_AT,
  KC_KP_EXCLAMATION,
  KC_KP_MEM_STORE,        //0xD0
  KC_KP_MEM_RECALL,
  KC_KP_MEM_CLEAR,
  KC_KP_MEM_ADD,
  KC_KP_MEM_SUB,
  KC_KP_MEM_MUL,
  KC_KP_MEM_DIV,
  KC_KP_PLUS_MINUS,
  KC_KP_CLEAR,
  KC_KP_CLEAR_ENTRY,
  KC_KP_BINARY,
  KC_KP_OCTAL,
  KC_KP_DECIMAL,
  KC_KP_HEXADECIMAL,
#endif

    /* Modifiers */
    KC_LEFT_CTRL = 0xE0,
    KC_LEFT_SHIFT,
    KC_LEFT_ALT,
    KC_LEFT_GUI,
    KC_RIGHT_CTRL,
    KC_RIGHT_SHIFT,
    KC_RIGHT_ALT,
    KC_RIGHT_GUI

    // **********************************************
    // * 0xF0-0xFF are unallocated in the HID spec. *
    // * QMK uses these for Mouse Keys - see below. *
    // **********************************************
};

#define KC_MODS         KC_LEFT_CTRL
#define KC_MODS_MAX     KC_RIGHT_GUI

enum hid_mouse_keycodes {
    /* Mousekey */
    KC_MS_UP = 0xF0,
    KC_MS_DOWN,
    KC_MS_LEFT,
    KC_MS_RIGHT,
    KC_MS_BTN1,
    KC_MS_BTN2,
    KC_MS_BTN3,
    KC_MS_BTN4,
    KC_MS_BTN5, /* 0xF8 */
    /* Mousekey wheel */
    KC_MS_WH_UP,
    KC_MS_WH_DOWN,
    KC_MS_WH_LEFT,
    KC_MS_WH_RIGHT, /* 0xFC */
    /* Mousekey accel */
    KC_MS_ACCEL0,
    KC_MS_ACCEL1,
    KC_MS_ACCEL2, /* 0xFF */
};


/* NOTE: in order to not overlap with keyboard keycodes, all these keycodes are in the 0x2000 range
but & 0xFF will yield the spec keycode */

enum hid_consumer_control_keycodes {

    KC_CC_POWER = 0x2030,
    KC_CC_RESET,
    KC_CC_SLEEP,

    KC_CC_MENU = 0x2040,

    KC_CC_SELECTION = 0x2080,
    KC_CC_ASSIGN_SEL,
    KC_CC_MODE_STEP,
    KC_CC_RECALL_LAST,

    KC_CC_QUIT = 0x2094,
    KC_CC_HELP,

    KC_CC_CHANNEL_UP = 0x209C,
    KC_CC_CHANNEL_DOWN,

    KC_CC_PLAY = 0x20B0,
    KC_CC_PAUSE,
    KC_CC_RECORD,
    KC_CC_FAST_FORWARD,
    KC_CC_REWIND,
    KC_CC_NEXT_TRACK,
    KC_CC_PREV_TRACK,
    KC_CC_STOP,
    KC_CC_EJECT,
    KC_CC_RANDOM_PLAY,
    KC_CC_SELECT_DISK,
    KC_CC_ENTER_DISK,
    KC_CC_REPEAT,

    KC_CC_STOP_EJECT = 0x20CC,
    KC_CC_PLAY_PAUSE,
    KC_CC_PLAY_SKIP,

    KC_CC_VOLUME = 0x20E0,
    KC_CC_BALANCE,
    KC_CC_MUTE,
    KC_CC_BASS,

    KC_CC_VOL_UP = 0x20E9,
    KC_CC_VOL_DOWN,
};


/* Special keycodes */
// All keycodes corresponding to actions/shortcuts to be performed internally
#define QK_ACTION      (0x5000)

#define KC_LEDU QK_BRIGHTNESS_UP
#define KC_LEDD QK_BRIGHTNESS_DOWN

enum internal_special_keycodes {
    // Ranges used in shortcuts - not to be used directly
    QK_BASIC                = 0x0000,
    QK_BASIC_MAX            = 0x00FF,
    QK_MODS                 = 0x0100,
    QK_LCTL                 = 0x0100,
    QK_LSFT                 = 0x0200,
    QK_LALT                 = 0x0400,
    QK_LGUI                 = 0x0800,
    QK_RMODS_MIN            = 0x1000,
    QK_RCTL                 = 0x1100,
    QK_RSFT                 = 0x1200,
    QK_RALT                 = 0x1400,
    QK_RGUI                 = 0x1800,
    QK_MODS_MAX             = 0x1FFF,

    QK_MEDIA                = 0x2000,
    QK_MEDIA_MAX            = 0x20FF,

    QK_MACRO                = 0x3000,    
    QK_MACRO_MAX            = 0x30FF,

    QK_TO                   = 0x5000,
    QK_TO_MAX               = 0x50FF,
    QK_MOMENTARY            = 0x5100,
    QK_MOMENTARY_MAX        = 0x51FF,
    QK_DEF_LAYER            = 0x5200,
    QK_DEF_LAYER_MAX        = 0x52FF,
    QK_TOGGLE_LAYER         = 0x5300,
    QK_TOGGLE_LAYER_MAX     = 0x53FF,
    QK_ONE_SHOT_LAYER       = 0x5400,
    QK_ONE_SHOT_LAYER_MAX   = 0x54FF,
    QK_ONE_SHOT_MOD         = 0x5500,
    QK_ONE_SHOT_MOD_MAX     = 0x55FF,

    QK_BT_HOST              = 0x6000,
    QK_BT_HOST_MAX          = 0x60FF,
    QK_BT_HOST_RESET        = 0x6100,
    QK_BT_HOST_RESET_MAX    = 0x61FF,

    QK_BRIGHTNESS           = 0x7000,
    QK_BRIGHTNESS_MAX       = 0x70FF,


    // Definitions - can be used directly
    QK_BRIGHTNESS_UP        = 0x7000,
    QK_BRIGHTNESS_DOWN,


};

/* Keycode modifiers & aliases */
#define LCTL(kc) (QK_LCTL | (kc))
#define LSFT(kc) (QK_LSFT | (kc))
#define LALT(kc) (QK_LALT | (kc))
#define LGUI(kc) (QK_LGUI | (kc))
#define LOPT(kc) LALT(kc)
#define LCMD(kc) LGUI(kc)
#define LWIN(kc) LGUI(kc)
#define RCTL(kc) (QK_RCTL | (kc))
#define RSFT(kc) (QK_RSFT | (kc))
#define RALT(kc) (QK_RALT | (kc))
#define RGUI(kc) (QK_RGUI | (kc))
#define ALGR(kc) RALT(kc)
#define ROPT(kc) RALT(kc)
#define RCMD(kc) RGUI(kc)
#define RWIN(kc) RGUI(kc)

// Modified keycode aliases
#define C(kc) LCTL(kc)
#define S(kc) LSFT(kc)
#define A(kc) LALT(kc)
#define G(kc) LGUI(kc)

#define HYPR(kc) (QK_LCTL | QK_LSFT | QK_LALT | QK_LGUI | (kc))
#define MEH(kc) (QK_LCTL | QK_LSFT | QK_LALT | (kc))
#define LCAG(kc) (QK_LCTL | QK_LALT | QK_LGUI | (kc))
#define LSG(kc) (QK_LSFT | QK_LGUI | (kc))
#define SGUI(kc) LSG(kc)
#define SCMD(kc) LSG(kc)
#define SWIN(kc) LSG(kc)
#define LAG(kc) (QK_LALT | QK_LGUI | (kc))
#define RSG(kc) (QK_RSFT | QK_RGUI | (kc))
#define RAG(kc) (QK_RALT | QK_RGUI | (kc))
#define LCA(kc) (QK_LCTL | QK_LALT | (kc))
#define LSA(kc) (QK_LSFT | QK_LALT | (kc))
#define RSA(kc) (QK_RSFT | QK_RALT | (kc))
#define LSC(kc) (QK_LSFT | QK_LCTL | (kc))
#define RSC(kc) (QK_RSFT | QK_RCTL | (kc))
#define RCS(kc)  RSC(kc) // for uniformity with macro system
#define LCG(kc) (QK_LCTL | QK_LGUI | (kc))
#define RCG(kc) (QK_RCTL | QK_RGUI | (kc))
#define SAGR(kc) RSA(kc)


/* C Macros to for keycode ranges */
// Macro - 256 macros max
#define MACRO(macro) (QK_MACRO | ((macro) & 0xFF))
#define M   MACRO
#define MA  MACRO


// Toggle layer - 256 layer max
#define TG(layer) (QK_TOGGLE_LAYER | ((layer) & 0xFF))

// Toggle layer and deactivate all others - 256 layer max
#define TO(layer) (QK_TO | ((layer) & 0xFF))

// Momentary toggle layer - 256 layer max
#define MO(layer) (QK_MOMENTARY | ((layer) & 0xFF))

// Set default layer - 256 layer max
#define DF(layer) (QK_DEF_LAYER | ((layer) & 0xFF))

// One-shot layer - 256 layer max
#define OSL(layer) (QK_ONE_SHOT_LAYER | ((layer) & 0xFF))


// Change to BT HOST - 256 hosts max
#define BT(host)   (QK_BT_HOST | ((host) & 0xFF))

// Reset BT Host config - 256 hosts max
#define BT_RESET(host)   (QK_BT_HOST_RESET | ((host) & 0xFF))
#define BT_RST BT_RESET


/* US ANSI shifted keycode aliases */
#define KC_TILDE LSFT(KC_GRAVE) // ~
#define KC_TILD KC_TILDE

#define KC_EXCLAIM LSFT(KC_1) // !
#define KC_EXLM KC_EXCLAIM

#define KC_AT LSFT(KC_2) // @

#define KC_HASH LSFT(KC_3) // #

#define KC_DOLLAR LSFT(KC_4) // $
#define KC_DLR KC_DOLLAR

#define KC_PERCENT LSFT(KC_5) // %
#define KC_PERC KC_PERCENT

#define KC_CIRCUMFLEX LSFT(KC_6) // ^
#define KC_CIRC KC_CIRCUMFLEX

#define KC_AMPERSAND LSFT(KC_7) // &
#define KC_AMPR KC_AMPERSAND

#define KC_ASTERISK LSFT(KC_8) // *
#define KC_ASTR KC_ASTERISK

#define KC_LEFT_PAREN LSFT(KC_9) // (
#define KC_LPRN KC_LEFT_PAREN

#define KC_RIGHT_PAREN LSFT(KC_0) // )
#define KC_RPRN KC_RIGHT_PAREN

#define KC_UNDERSCORE LSFT(KC_MINUS) // _
#define KC_UNDS KC_UNDERSCORE

#define KC_PLUS LSFT(KC_EQUAL) // +

#define KC_LEFT_CURLY_BRACE LSFT(KC_LEFT_BRACKET) // {
#define KC_LCBR KC_LEFT_CURLY_BRACE

#define KC_RIGHT_CURLY_BRACE LSFT(KC_RIGHT_BRACKET) // }
#define KC_RCBR KC_RIGHT_CURLY_BRACE

#define KC_LEFT_ANGLE_BRACKET LSFT(KC_COMMA) // <
#define KC_LABK KC_LEFT_ANGLE_BRACKET
#define KC_LT KC_LEFT_ANGLE_BRACKET

#define KC_RIGHT_ANGLE_BRACKET LSFT(KC_DOT) // >
#define KC_RABK KC_RIGHT_ANGLE_BRACKET
#define KC_GT KC_RIGHT_ANGLE_BRACKET

#define KC_COLON LSFT(KC_SEMICOLON) // :
#define KC_COLN KC_COLON

#define KC_PIPE LSFT(KC_BACKSLASH) // |

#define KC_QUESTION LSFT(KC_SLASH) // ?
#define KC_QUES KC_QUESTION

#define KC_DOUBLE_QUOTE LSFT(KC_QUOTE) // "
#define KC_DQUO KC_DOUBLE_QUOTE
#define KC_DQT KC_DOUBLE_QUOTE

#define KC_DELT KC_DELETE // Del key (four letter code)



#endif