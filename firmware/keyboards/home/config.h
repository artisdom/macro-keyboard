#ifndef CONFIG_H
#define CONFIG_H

/* --------- Global configs --------- */
#define DEVICE_NAME               "Macro Keyboard" // max 15 chars for macOS
#define DEEP_SLEEP_ENABLED        (1)
#define DEEP_SLEEP_TIMEOUT        (3600) // in seconds 
#define TOGGLE_SWITCH_ENABLED     (1)
#define BATTERY_ENABLED           (1)
#define VIA_ENABLED               (1)


/* --------- Key matrix configs --------- */
#define MATRIX_ROWS               (4)
#define MATRIX_COLS               (3)

#define DEBOUNCE_TIME             (4)   // in ms
#define KEYBOARD_RATE             (1u)  // in ms
#define KEYBOARD_BLE_RATE         (10u) // in ms


/* --------- Keymap configs --------- */
#define DEFAULT_LAYER             (0)
#define MAX_LAYER                 (32)

#define MACRO_LEN                 (3)
#define MAX_MACRO                 (16) // locked by VIA's crappy macro system
#define ONESHOT_TIMEOUT           (3)  // in seconds


/* --------- BLE configs --------- */
#define BLE_DEVICE_NAME           DEVICE_NAME
#define BLE_ENABLED               (1)
#define HID_REPORT_LEN            (8)
#define HID_CC_REPORT_LEN         (2)
#define BLE_NUM_HOSTS             (3)


/* --------- USB configs --------- */
#define USB_ENABLED               (1)
#define USB_DETECT_ENABLED        (1)   // detect cable plugged in
#define USB_MANUFACTURER_NAME     "Patopest"
#define USB_DEVICE_NAME           DEVICE_NAME
#define USB_HID_NAME              DEVICE_NAME " HID"
#define USB_CDC_NAME              DEVICE_NAME " CDC"
#define USB_MIDI_NAME             DEVICE_NAME " MIDI"


/* --------- Led matrix configs --------- */
#define LED_ENABLED               (1)
#define LED_EFFECTS_ENABLED       (1)
#define LED_BT_EFFECTS_ENABLED    (1)
#define LED_LAYER_EFFECTS_ENABLED (1)
#define LED_ROWS                  (3)
#define LED_COLS                  (3)



/* --------- FreeRTOS configs --------- */
#define KEYBOARD_TASK_STACK             (8192)
#define KEYBOARD_TASK_PRIORITY          (configMAX_PRIORITIES)
#define MEDIA_TASK_STACK                (8192)
#define MEDIA_TASK_PRIORITY             (configMAX_PRIORITIES)
#define BATTERY_TASK_STACK              (4096)
#define BATTERY_TASK_PRIORITY           (configMAX_PRIORITIES)
#define EVENT_HANDLER_TASK_STACK        (8192)
#define EVENT_HANDLER_TASK_PRIORITY     (configMAX_PRIORITIES)
#define DEEP_SLEEP_TASK_STACK           (4096)
#define DEEP_SLEEP_TASK_PRIORITY        (configMAX_PRIORITIES)
#define LEDS_TASK_STACK                 (4096)
#define LEDS_TASK_PRIORITY              (configMAX_PRIORITIES)

#define BLE_KEYBOARD_TASK_STACK         (2048)
#define BLE_KEYBOARD_TASK_PRIORITY      (configMAX_PRIORITIES)
#define BLE_MEDIA_TASK_STACK            (2048)
#define BLE_MEDIA_TASK_PRIORITY         (configMAX_PRIORITIES)
#define BLE_BATTERY_TASK_STACK          (2048)
#define BLE_BATTERY_TASK_PRIORITY       (configMAX_PRIORITIES)
#define BLE_EVENT_TASK_STACK            (4096)
#define BLE_EVENT_TASK_PRIORITY         (configMAX_PRIORITIES)

#define USB_TINYUSB_TASK_STACK          (4096)
#define USB_TINYUSB_TASK_PRIORITY       (5)
#define USB_KEYBOARD_TASK_STACK         (4096)
#define USB_KEYBOARD_TASK_PRIORITY      (configMAX_PRIORITIES)
#define USB_MEDIA_TASK_STACK            (2048)
#define USB_MEDIA_TASK_PRIORITY         (configMAX_PRIORITIES)


#endif