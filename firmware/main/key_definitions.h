#ifndef KEY_DEFINITIONS_H
#define KEY_DEFINITIONS_H

/*
File is generated using the keycodes definition json or toml files
  Generated on: 24-01-2024 18:28:34
  Version: v1.11
  Git commit (keycodes repository): 408e8af
*/

enum keycode_ranges {
	QK_BASIC                       = 0x0000,
	QK_BASIC_MAX                   = 0x00FF,
	QK_MODS                        = 0x0100,
	QK_MODS_MAX                    = 0x1FFF,
	QK_CONSUMER                    = 0x2000,
	QK_CONSUMER_MAX                = 0x2FFF,
	QK_MACRO                       = 0x3000,
	QK_MACRO_MAX                   = 0x30FF,
	QK_TO                          = 0x5000,
	QK_TO_MAX                      = 0x50FF,
	QK_MOMENTARY                   = 0x5100,
	QK_MOMENTARY_MAX               = 0x51FF,
	QK_DEF_LAYER                   = 0x5200,
	QK_DEF_LAYER_MAX               = 0x52FF,
	QK_TOGGLE_LAYER                = 0x5300,
	QK_TOGGLE_LAYER_MAX            = 0x53FF,
	QK_ONE_SHOT_LAYER              = 0x5400,
	QK_ONE_SHOT_LAYER_MAX          = 0x54FF,
	QK_BT_HOST                     = 0x6000,
	QK_BT_HOST_MAX                 = 0x60FF,
	QK_BT_HOST_RESET               = 0x6100,
	QK_BT_HOST_RESET_MAX           = 0x61FF,
	QK_LIGHTING                    = 0x7000,
	QK_LIGHTING_MAX                = 0x70FF,
	QK_DEBUG                       = 0x7100,
	QK_DEBUG_MAX                   = 0x710F,
};

// Switch statement helpers
#define QK_BASIC_RANGE             QK_BASIC ... QK_BASIC_MAX
#define QK_MODS_RANGE              QK_MODS ... QK_MODS_MAX
#define QK_CONSUMER_RANGE          QK_CONSUMER ... QK_CONSUMER_MAX
#define QK_MACRO_RANGE             QK_MACRO ... QK_MACRO_MAX
#define QK_TO_RANGE                QK_TO ... QK_TO_MAX
#define QK_MOMENTARY_RANGE         QK_MOMENTARY ... QK_MOMENTARY_MAX
#define QK_DEF_LAYER_RANGE         QK_DEF_LAYER ... QK_DEF_LAYER_MAX
#define QK_TOGGLE_LAYER_RANGE      QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX
#define QK_ONE_SHOT_LAYER_RANGE    QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX
#define QK_BT_HOST_RANGE           QK_BT_HOST ... QK_BT_HOST_MAX
#define QK_BT_HOST_RESET_RANGE     QK_BT_HOST_RESET ... QK_BT_HOST_RESET_MAX
#define QK_LIGHTING_RANGE          QK_LIGHTING ... QK_LIGHTING_MAX
#define QK_DEBUG_RANGE             QK_DEBUG ... QK_DEBUG_MAX



enum keycodes {
	// Keycodes
	/* internal */
	KC_NO           = 0x0000,
	KC_TRANSPARENT  = 0x0001,
	/* basic */
	KC_A            = 0x0004,
	KC_B            = 0x0005,
	KC_C            = 0x0006,
	KC_D            = 0x0007,
	KC_E            = 0x0008,
	KC_F            = 0x0009,
	KC_G            = 0x000A,
	KC_H            = 0x000B,
	KC_I            = 0x000C,
	KC_J            = 0x000D,
	KC_K            = 0x000E,
	KC_L            = 0x000F,
	KC_M            = 0x0010,
	KC_N            = 0x0011,
	KC_O            = 0x0012,
	KC_P            = 0x0013,
	KC_Q            = 0x0014,
	KC_R            = 0x0015,
	KC_S            = 0x0016,
	KC_T            = 0x0017,
	KC_U            = 0x0018,
	KC_V            = 0x0019,
	KC_W            = 0x001A,
	KC_X            = 0x001B,
	KC_Y            = 0x001C,
	KC_Z            = 0x001D,
	KC_1            = 0x001E,
	KC_2            = 0x001F,
	KC_3            = 0x0020,
	KC_4            = 0x0021,
	KC_5            = 0x0022,
	KC_6            = 0x0023,
	KC_7            = 0x0024,
	KC_8            = 0x0025,
	KC_9            = 0x0026,
	KC_0            = 0x0027,
	KC_ENTER        = 0x0028,
	KC_ESCAPE       = 0x0029,
	KC_BACKSPACE    = 0x002A,
	KC_TAB          = 0x002B,
	KC_SPACE        = 0x002C,
	KC_MINUS        = 0x002D,
	KC_EQUAL        = 0x002E,
	KC_LEFT_BRACKET = 0x002F,
	KC_RIGHT_BRACKET = 0x0030,
	KC_BACKSLASH    = 0x0031,
	KC_NONUS_HASH   = 0x0032,
	KC_SEMICOLON    = 0x0033,
	KC_QUOTE        = 0x0034,
	KC_GRAVE        = 0x0035,
	KC_COMMA        = 0x0036,
	KC_DOT          = 0x0037,
	KC_SLASH        = 0x0038,
	KC_CAPS_LOCK    = 0x0039,
	KC_F1           = 0x003A,
	KC_F2           = 0x003B,
	KC_F3           = 0x003C,
	KC_F4           = 0x003D,
	KC_F5           = 0x003E,
	KC_F6           = 0x003F,
	KC_F7           = 0x0040,
	KC_F8           = 0x0041,
	KC_F9           = 0x0042,
	KC_F10          = 0x0043,
	KC_F11          = 0x0044,
	KC_F12          = 0x0045,
	KC_PRINT_SCREEN = 0x0046,
	KC_SCROLL_LOCK  = 0x0047,
	KC_PAUSE        = 0x0048,
	KC_INSERT       = 0x0049,
	KC_HOME         = 0x004A,
	KC_PAGE_UP      = 0x004B,
	KC_DELETE       = 0x004C,
	KC_END          = 0x004D,
	KC_PAGE_DOWN    = 0x004E,
	KC_RIGHT        = 0x004F,
	KC_LEFT         = 0x0050,
	KC_DOWN         = 0x0051,
	KC_UP           = 0x0052,
	KC_NUM_LOCK     = 0x0053,
	KC_KP_SLASH     = 0x0054,
	KC_KP_ASTERISK  = 0x0055,
	KC_KP_MINUS     = 0x0056,
	KC_KP_PLUS      = 0x0057,
	KC_KP_ENTER     = 0x0058,
	KC_KP_1         = 0x0059,
	KC_KP_2         = 0x005A,
	KC_KP_3         = 0x005B,
	KC_KP_4         = 0x005C,
	KC_KP_5         = 0x005D,
	KC_KP_6         = 0x005E,
	KC_KP_7         = 0x005F,
	KC_KP_8         = 0x0060,
	KC_KP_9         = 0x0061,
	KC_KP_0         = 0x0062,
	KC_KP_DOT       = 0x0063,
	KC_NONUS_BACKSLASH = 0x0064,
	KC_APPLICATION  = 0x0065,
	KC_KB_POWER     = 0x0066,
	KC_KP_EQUAL     = 0x0067,
	KC_F13          = 0x0068,
	KC_F14          = 0x0069,
	KC_F15          = 0x006A,
	KC_F16          = 0x006B,
	KC_F17          = 0x006C,
	KC_F18          = 0x006D,
	KC_F19          = 0x006E,
	KC_F20          = 0x006F,
	KC_F21          = 0x0070,
	KC_F22          = 0x0071,
	KC_F23          = 0x0072,
	KC_F24          = 0x0073,
	KC_EXECUTE      = 0x0074,
	KC_HELP         = 0x0075,
	KC_MENU         = 0x0076,
	KC_SELECT       = 0x0077,
	KC_STOP         = 0x0078,
	KC_AGAIN        = 0x0079,
	KC_UNDO         = 0x007A,
	KC_CUT          = 0x007B,
	KC_COPY         = 0x007C,
	KC_PASTE        = 0x007D,
	KC_FIND         = 0x007E,
	KC_KB_MUTE      = 0x007F,
	KC_KB_VOLUME_UP = 0x0080,
	KC_KB_VOLUME_DOWN = 0x0081,
	KC_LOCKING_CAPS_LOCK = 0x0082,
	KC_LOCKING_NUM_LOCK = 0x0083,
	KC_LOCKING_SCROLL_LOCK = 0x0084,
	KC_KP_COMMA     = 0x0085,
	KC_KP_EQUAL_AS400 = 0x0086,
	KC_INTERNATIONAL_1 = 0x0087,
	KC_INTERNATIONAL_2 = 0x0088,
	KC_INTERNATIONAL_3 = 0x0089,
	KC_INTERNATIONAL_4 = 0x008A,
	KC_INTERNATIONAL_5 = 0x008B,
	KC_INTERNATIONAL_6 = 0x008C,
	KC_INTERNATIONAL_7 = 0x008D,
	KC_INTERNATIONAL_8 = 0x008E,
	KC_INTERNATIONAL_9 = 0x008F,
	KC_LANGUAGE_1   = 0x0090,
	KC_LANGUAGE_2   = 0x0091,
	KC_LANGUAGE_3   = 0x0092,
	KC_LANGUAGE_4   = 0x0093,
	KC_LANGUAGE_5   = 0x0094,
	KC_LANGUAGE_6   = 0x0095,
	KC_LANGUAGE_7   = 0x0096,
	KC_LANGUAGE_8   = 0x0097,
	KC_LANGUAGE_9   = 0x0098,
	KC_ALTERNATE_ERASE = 0x0099,
	KC_SYSTEM_REQUEST = 0x009A,
	KC_CANCEL       = 0x009B,
	KC_CLEAR        = 0x009C,
	KC_PRIOR        = 0x009D,
	KC_RETURN       = 0x009E,
	KC_SEPARATOR    = 0x009F,
	KC_OUT          = 0x00A0,
	KC_OPER         = 0x00A1,
	KC_CLEAR_AGAIN  = 0x00A2,
	KC_CRSEL        = 0x00A3,
	KC_EXSEL        = 0x00A4,
	/* modifiers */
	KC_LEFT_CTRL    = 0x00E0,
	KC_LEFT_SHIFT   = 0x00E1,
	KC_LEFT_ALT     = 0x00E2,
	KC_LEFT_GUI     = 0x00E3,
	KC_RIGHT_CTRL   = 0x00E4,
	KC_RIGHT_SHIFT  = 0x00E5,
	KC_RIGHT_ALT    = 0x00E6,
	KC_RIGHT_GUI    = 0x00E7,
	QK_LCTL         = 0x0100,
	QK_LSFT         = 0x0200,
	QK_LALT         = 0x0400,
	QK_LGUI         = 0x0800,
	QK_RMODS_MIN    = 0x1000,
	QK_RCTL         = 0x1100,
	QK_RSFT         = 0x1200,
	QK_RALT         = 0x1400,
	QK_RGUI         = 0x1800,
	/* system */
	KC_SYSTEM_POWER = 0x2030,
	KC_SYSTEM_RESET = 0x2031,
	KC_SYSTEM_SLEEP = 0x2032,
	/* menu */
	KC_MENU_MENU    = 0x2040,
	KC_MENU_PICK    = 0x2041,
	KC_MENU_UP      = 0x2042,
	KC_MENU_DOWN    = 0x2043,
	KC_MENU_LEFT    = 0x2044,
	KC_MENU_RIGHT   = 0x2045,
	KC_MENU_ESCAPE  = 0x2046,
	KC_MENU_VALUE_INCREASE = 0x2047,
	KC_MENU_VALUE_DECREASE = 0x2048,
	/* display */
	KC_SNAPSHOT     = 0x2065,
	KC_PIP          = 0x2067,
	KC_BRIGHTNESS_UP = 0x206F,
	KC_BRIGHTNESS_DOWN = 0x2070,
	/* keyboard */
	KC_KEYBOARD_BRIGHTNESS_UP = 0x2079,
	KC_KEYBOARD_BRIGHTNESS_DOWN = 0x207A,
	/* selection */
	KC_SELECTION    = 0x2080,
	KC_ASSIGN_SEL   = 0x2081,
	KC_MODE_STEP    = 0x2082,
	KC_RECALL_LAST  = 0x2083,
	/* media */
	KC_MEDIA_QUIT   = 0x2094,
	KC_MEDIA_HELP   = 0x2095,
	KC_CHANNEL_UP   = 0x209C,
	KC_CHANNEL_DOWN = 0x209D,
	KC_MEDIA_PLAY   = 0x20B0,
	KC_MEDIA_PAUSE  = 0x20B1,
	KC_MEDIA_RECORD = 0x20B2,
	KC_MEDIA_FAST_FORWARD = 0x20B3,
	KC_MEDIA_REWIND = 0x20B4,
	KC_MEDIA_NEXT_TRACK = 0x20B5,
	KC_MEDIA_PREV_TRACK = 0x20B6,
	KC_MEDIA_STOP   = 0x20B7,
	KC_MEDIA_EJECT  = 0x20B8,
	KC_MEDIA_RANDOM_PLAY = 0x20B9,
	KC_MEDIA_SELECT_DISK = 0x20BA,
	KC_MEDIA_ENTER_DISK = 0x20BB,
	KC_MEDIA_REPEAT = 0x20BC,
	KC_MEDIA_STOP_EJECT = 0x20CC,
	KC_MEDIA_PLAY_PAUSE = 0x20CD,
	KC_MEDIA_PLAY_SKIP = 0x20CE,
	KC_AUDIO_VOLUME = 0x20E0,
	KC_AUDIO_BALANCE = 0x20E1,
	KC_AUDIO_MUTE   = 0x20E2,
	KC_AUDIO_BASS   = 0x20E3,
	KC_AUDIO_VOL_UP = 0x20E9,
	KC_AUDIO_VOL_DOWN = 0x20EA,
	/* backlight */
	QK_BACKLIGHT_ON = 0x7000,
	QK_BACKLIGHT_OFF = 0x7001,
	QK_BACKLIGHT_TOGGLE = 0x7002,
	QK_BACKLIGHT_DOWN = 0x7003,
	QK_BACKLIGHT_UP = 0x7004,
	/* debug */
	QK_RESTART_APP  = 0x7100,
	QK_RESTART_USB  = 0x7101,
	QK_RESTART_BOOTLOADER = 0x7102,
	QK_RESTART_BOOTLOADER_DFU = 0x7103,


	// Aliases
	/* internal */
	xxxxxxx = KC_NO,
	_______ = KC_TRANSPARENT,
	KC_TRNS = KC_TRANSPARENT,
	/* basic */
	KC_ENT  = KC_ENTER,
	KC_ESC  = KC_ESCAPE,
	KC_BSPC = KC_BACKSPACE,
	KC_SPC  = KC_SPACE,
	KC_MINS = KC_MINUS,
	KC_EQL  = KC_EQUAL,
	KC_LBRC = KC_LEFT_BRACKET,
	KC_RBRC = KC_RIGHT_BRACKET,
	KC_BSLS = KC_BACKSLASH,
	KC_NUHS = KC_NONUS_HASH,
	KC_SCLN = KC_SEMICOLON,
	KC_QUOT = KC_QUOTE,
	KC_GRV  = KC_GRAVE,
	KC_COMM = KC_COMMA,
	KC_SLSH = KC_SLASH,
	KC_CAPS = KC_CAPS_LOCK,
	KC_PSCR = KC_PRINT_SCREEN,
	KC_SCRL = KC_SCROLL_LOCK,
	KC_BRMD = KC_SCROLL_LOCK,
	KC_PAUS = KC_PAUSE,
	KC_BRK  = KC_PAUSE,
	KC_BRMU = KC_PAUSE,
	KC_INS  = KC_INSERT,
	KC_PGUP = KC_PAGE_UP,
	KC_DEL  = KC_DELETE,
	KC_DELT = KC_DELETE,
	KC_PGDN = KC_PAGE_DOWN,
	KC_RGHT = KC_RIGHT,
	KC_NUM  = KC_NUM_LOCK,
	KC_PSLS = KC_KP_SLASH,
	KC_PAST = KC_KP_ASTERISK,
	KC_PMNS = KC_KP_MINUS,
	KC_PPLS = KC_KP_PLUS,
	KC_PENT = KC_KP_ENTER,
	KC_P1   = KC_KP_1,
	KC_P2   = KC_KP_2,
	KC_P3   = KC_KP_3,
	KC_P4   = KC_KP_4,
	KC_P5   = KC_KP_5,
	KC_P6   = KC_KP_6,
	KC_P7   = KC_KP_7,
	KC_P8   = KC_KP_8,
	KC_P9   = KC_KP_9,
	KC_P0   = KC_KP_0,
	KC_PDOT = KC_KP_DOT,
	KC_NUBS = KC_NONUS_BACKSLASH,
	KC_APP  = KC_APPLICATION,
	KC_PEQL = KC_KP_EQUAL,
	KC_EXEC = KC_EXECUTE,
	KC_SLCT = KC_SELECT,
	KC_AGIN = KC_AGAIN,
	KC_PSTE = KC_PASTE,
	KC_LCAP = KC_LOCKING_CAPS_LOCK,
	KC_LNUM = KC_LOCKING_NUM_LOCK,
	KC_LSCR = KC_LOCKING_SCROLL_LOCK,
	KC_PCMM = KC_KP_COMMA,
	KC_INT1 = KC_INTERNATIONAL_1,
	KC_INT2 = KC_INTERNATIONAL_2,
	KC_INT3 = KC_INTERNATIONAL_3,
	KC_INT4 = KC_INTERNATIONAL_4,
	KC_INT5 = KC_INTERNATIONAL_5,
	KC_INT6 = KC_INTERNATIONAL_6,
	KC_INT7 = KC_INTERNATIONAL_7,
	KC_INT8 = KC_INTERNATIONAL_8,
	KC_INT9 = KC_INTERNATIONAL_9,
	KC_LNG1 = KC_LANGUAGE_1,
	KC_LNG2 = KC_LANGUAGE_2,
	KC_LNG3 = KC_LANGUAGE_3,
	KC_LNG4 = KC_LANGUAGE_4,
	KC_LNG5 = KC_LANGUAGE_5,
	KC_LNG6 = KC_LANGUAGE_6,
	KC_LNG7 = KC_LANGUAGE_7,
	KC_LNG8 = KC_LANGUAGE_8,
	KC_LNG9 = KC_LANGUAGE_9,
	KC_ERAS = KC_ALTERNATE_ERASE,
	KC_SYRQ = KC_SYSTEM_REQUEST,
	KC_CNCL = KC_CANCEL,
	KC_CLR  = KC_CLEAR,
	KC_PRIR = KC_PRIOR,
	KC_RETN = KC_RETURN,
	KC_SEPR = KC_SEPARATOR,
	KC_CLAG = KC_CLEAR_AGAIN,
	KC_CRSL = KC_CRSEL,
	KC_EXSL = KC_EXSEL,
	/* modifiers */
	KC_LCTL = KC_LEFT_CTRL,
	KC_MODS = KC_LEFT_CTRL,
	KC_LSFT = KC_LEFT_SHIFT,
	KC_LALT = KC_LEFT_ALT,
	KC_LOPT = KC_LEFT_ALT,
	KC_LGUI = KC_LEFT_GUI,
	KC_LCMD = KC_LEFT_GUI,
	KC_LWIN = KC_LEFT_GUI,
	KC_RCTL = KC_RIGHT_CTRL,
	KC_RSFT = KC_RIGHT_SHIFT,
	KC_RALT = KC_RIGHT_ALT,
	KC_ROPT = KC_RIGHT_ALT,
	KC_ALGR = KC_RIGHT_ALT,
	KC_RGUI = KC_RIGHT_GUI,
	KC_RCMD = KC_RIGHT_GUI,
	KC_RWIN = KC_RIGHT_GUI,
	KC_MODS_MAX = KC_RIGHT_GUI,
	/* system */
	/* menu */
	/* display */
	KC_SNAP = KC_SNAPSHOT,
	KC_BRIU = KC_BRIGHTNESS_UP,
	KC_BRID = KC_BRIGHTNESS_DOWN,
	/* keyboard */
	KC_KBRU = KC_KEYBOARD_BRIGHTNESS_UP,
	KC_KBRD = KC_KEYBOARD_BRIGHTNESS_DOWN,
	/* selection */
	/* media */
	KC_MPLY = KC_MEDIA_PLAY,
	KC_MPSE = KC_MEDIA_PAUSE,
	KC_MREC = KC_MEDIA_RECORD,
	KC_MFFD = KC_MEDIA_FAST_FORWARD,
	KC_MRWD = KC_MEDIA_REWIND,
	KC_MNXT = KC_MEDIA_NEXT_TRACK,
	KC_MPRV = KC_MEDIA_PREV_TRACK,
	KC_MSTP = KC_MEDIA_STOP,
	KC_EJCT = KC_MEDIA_EJECT,
	KC_MRDM = KC_MEDIA_RANDOM_PLAY,
	KC_ENTD = KC_MEDIA_ENTER_DISK,
	KC_MRPT = KC_MEDIA_REPEAT,
	KC_PLPS = KC_MEDIA_PLAY_PAUSE,
	KC_MSKP = KC_MEDIA_PLAY_SKIP,
	KC_MUTE = KC_AUDIO_MUTE,
	KC_VOLU = KC_AUDIO_VOL_UP,
	KC_VOLD = KC_AUDIO_VOL_DOWN,
	/* backlight */
	BL_ON   = QK_BACKLIGHT_ON,
	BL_OFF  = QK_BACKLIGHT_OFF,
	BL_TOGG = QK_BACKLIGHT_TOGGLE,
	BL_DOWN = QK_BACKLIGHT_DOWN,
	BL_UP   = QK_BACKLIGHT_UP,
	/* debug */
	QK_REST = QK_RESTART_APP,
	QK_RAPP = QK_RESTART_APP,
	QK_RUSB = QK_RESTART_USB,
	QK_BOOT = QK_RESTART_BOOTLOADER,
	QK_BOOD = QK_RESTART_BOOTLOADER_DFU,
};


// Macros

/* macro */
#define MACRO(macro) (QK_MACRO | ((macro) & 0xFF))
#define MA(macro)  MACRO(macro)
#define M(macro)   MACRO(macro)

/* layers */
// Toggle layer - 256 layer max
#define TG(layer)  (QK_TOGGLE_LAYER | ((layer) & 0xFF))
// Toggle layer and deactivate all others - 256 layer max
#define TO(layer)  (QK_TO | ((layer) & 0xFF))
// Momentary toggle layer - 256 layer max
#define MO(layer)  (QK_MOMENTARY | ((layer) & 0xFF))
// Set default layer - 256 layer max
#define DF(layer)  (QK_DEF_LAYER | ((layer) & 0xFF))
// One-shot layer - 256 layer max
#define OSL(layer) (QK_ONE_SHOT_LAYER | ((layer) & 0xFF))

/* bluetooth */
// Change to BT HOST - 256 hosts max
#define BT(host)   (QK_BT_HOST | ((host) & 0xFF))
// Reset BT HOST config - 256 hosts max
#define BT_RESET(host) (QK_BT_HOST_RESET | ((host) & 0xFF))
#define BT_RST(host) BT_RESET(host)

/* modifiers */
#define LCTL(kc)   (QK_LCTL | (kc))
#define C(kc)      LCTL(kc)
#define LSFT(kc)   (QK_LSFT | (kc))
#define S(kc)      LSFT(kc)
#define LALT(kc)   (QK_LALT | (kc))
#define LOPT(kc)   LALT(kc)
#define A(kc)      LALT(kc)
#define LGUI(kc)   (QK_LGUI | (kc))
#define LCMD(kc)   LGUI(kc)
#define LWIN(kc)   LGUI(kc)
#define G(kc)      LGUI(kc)
#define RCTL(kc)   (QK_RCTL | (kc))
#define RSFT(kc)   (QK_RSFT | (kc))
#define RALT(kc)   (QK_RALT | (kc))
#define ROPT(kc)   RALT(kc)
#define ALGR(kc)   RALT(kc)
#define RGUI(kc)   (QK_RGUI | (kc))
#define RCMD(kc)   RGUI(kc)
#define RWIN(kc)   RGUI(kc)
#define HYPR(kc)   (QK_LCTL | QK_LSFT | QK_LALT | QK_LGUI | (kc))
#define MEH(kc)    (QK_LCTL | QK_LSFT | QK_LALT | (kc))
#define LCAG(kc)   (QK_LCTL | QK_LALT | QK_LGUI | (kc))
#define LSG(kc)    (QK_LSFT | QK_LGUI | (kc))
#define SGUI(kc)   LSG(kc)
#define SCMD(kc)   LSG(kc)
#define SWIN(kc)   LSG(kc)
#define LAG(kc)    (QK_LALT | QK_LGUI | (kc))
#define RSG(kc)    (QK_RSFT | QK_RGUI | (kc))
#define RAG(kc)    (QK_RALT | QK_RGUI | (kc))
#define LCA(kc)    (QK_LCTL | QK_LALT | (kc))
#define LSA(kc)    (QK_LSFT | QK_LALT | (kc))
#define RSA(kc)    (QK_RSFT | QK_RALT | (kc))
#define SAGR(kc)   RSA(kc)
#define LSC(kc)    (QK_LSFT | QK_LCTL | (kc))
#define RSC(kc)    (QK_RSFT | QK_RCTL | (kc))
#define RCS(kc)    RSC(kc)
#define LCG(kc)    (QK_LCTL | QK_LGUI | (kc))
#define RCG(kc)    (QK_RCTL | QK_RGUI | (kc))

/* basic */
// ~
#define KC_TILDE   LSFT(KC_GRAVE)
#define KC_TILD    KC_TILDE
// !
#define KC_EXCLAIM LSFT(KC_1)
#define KC_EXLM    KC_EXCLAIM
// @
#define KC_AT      LSFT(KC_2)
// #
#define KC_HASH    LSFT(KC_3)
// $
#define KC_DOLLAR  LSFT(KC_4)
#define KC_DLR     KC_DOLLAR
// %
#define KC_PERCENT LSFT(KC_5)
#define KC_PERC    KC_PERCENT
// ^
#define KC_CIRCUMFLEX LSFT(KC_6)
#define KC_CIRC    KC_CIRCUMFLEX
// &
#define KC_AMPERSAND LSFT(KC_7)
#define KC_AMPR    KC_AMPERSAND
// *
#define KC_ASTERISK LSFT(KC_8)
#define KC_ASTR    KC_ASTERISK
// (
#define KC_LEFT_PAREN LSFT(KC_9)
#define KC_LPRN    KC_LEFT_PAREN
// )
#define KC_RIGHT_PAREN LSFT(KC_0)
#define KC_RPRN    KC_RIGHT_PAREN
// _
#define KC_UNDERSCORE LSFT(KC_MINUS)
#define KC_UNDS    KC_UNDERSCORE
// +
#define KC_PLUS    LSFT(KC_EQUAL)
// {
#define KC_LEFT_CURLY_BRACE LSFT(KC_LEFT_BRACKET)
#define KC_LCBR    KC_LEFT_CURLY_BRACE
// }
#define KC_RIGHT_CURLY_BRACE LSFT(KC_RIGHT_BRACKET)
#define KC_RCBR    KC_RIGHT_CURLY_BRACE
// <
#define KC_LEFT_ANGLE_BRACKET LSFT(KC_COMMA)
#define KC_LABK    KC_LEFT_ANGLE_BRACKET
#define KC_LT      KC_LEFT_ANGLE_BRACKET
// >
#define KC_RIGHT_ANGLE_BRACKET LSFT(KC_DOT)
#define KC_RABK    KC_RIGHT_ANGLE_BRACKET
#define KC_GT      KC_RIGHT_ANGLE_BRACKET
// :
#define KC_COLON   LSFT(KC_SEMICOLON)
#define KC_COLN    KC_COLON
// |
#define KC_PIPE    LSFT(KC_BACKSLASH)
// ?
#define KC_QUESTION LSFT(KC_SLASH)
#define KC_QUES    KC_QUESTION
// "
#define KC_DOUBLE_QUOTE LSFT(KC_QUOTE)
#define KC_DQUO    KC_DOUBLE_QUOTE
#define KC_DQT     KC_DOUBLE_QUOTE


#endif
