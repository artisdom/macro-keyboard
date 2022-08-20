#ifndef CONFIG_H
#define CONFIG_H


/* --------- Key matrix configs --------- */
#define MATRIX_ROWS         (1)
#define MATRIX_COLS         (3)

#define DEBOUNCE_TIME       (4)  //in ms

#define KEYBOARD_RATE       (1u) //in ms


/* --------- Keymap configs --------- */
#define NUM_LAYOUTS         (2)
#define DEFAULT_LAYOUT      (0)
#define NUM_MACROS          (2)


/* --------- BLE configs --------- */
#define BLE_DEVICE_NAME     "Macro Keyboard"
#define BLE_ENABLED         (1)
#define HID_REPORT_LEN      (8)
#define BLE_NUM_HOSTS       (3)


/* --------- Global configs --------- */
#define DEEP_SLEEP_ENABLED  (1)
#define DEEP_SLEEP_TIMEOUT  (120) //in seconds 

#endif