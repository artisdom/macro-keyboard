#ifndef CONFIG_H
#define CONFIG_H


/* --------- Key matrix configs --------- */
#define MATRIX_ROWS         	(1)
#define MATRIX_COLS         	(3)

#define DEBOUNCE_TIME       	(4)  //in ms

#define KEYBOARD_RATE       	(1u) //in ms


/* --------- Keymap configs --------- */
#define NUM_LAYOUTS         	(2)
#define DEFAULT_LAYOUT      	(0)

#define NUM_MACROS          	(2)
#define MACRO_LEN				(3)


/* --------- BLE configs --------- */
#define BLE_DEVICE_NAME     	"Macro Keyboard"
#define BLE_ENABLED         	(1)
#define HID_REPORT_LEN      	(8)
#define HID_CC_REPORT_LEN   	(2)
#define BLE_NUM_HOSTS       	(3)


/* --------- Global configs --------- */
#define DEEP_SLEEP_ENABLED  	(0)
#define DEEP_SLEEP_TIMEOUT  	(120) //in seconds 
#define USB_ENABLED         	(0) // enable hid over usb
#define TOGGLE_SWITCH_ENABLED   (0)
#define BATTERY_ENABLED         (0)


/* --------- Led matrix configs --------- */
#define LED_ENABLED         	(1)
#define LED_EFFECTS_ENABLED     (1)
#define LED_ROWS 				(3)
#define LED_COLS            	(3)


#endif