#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_timer.h"

#include "toggle_switch.h"
#include "config.h"
#include "events.h"


/* --------- Local Defines --------- */
#define ESP_INTR_FLAG_DEFAULT 0

#define TOGGLE_SWITCH_DEBOUNCE_TIME (4000) // in us


/* --------- Local Variables --------- */
static const char *TAG = "toggle_switch";

static const gpio_num_t toggle_usb_gpio = GPIO_NUM_1;
static const gpio_num_t toggle_ble_gpio = GPIO_NUM_2;


/* --------- Local Functions --------- */
static void toggle_switch__gpio_init(uint8_t pin);
static void toggle_switch__gpio_isr_handler(void* arg);



static void toggle_switch__gpio_init(uint8_t pin) {

    uint64_t pin_mask = (1ULL << pin);

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = pin_mask,
        .pull_down_en = 0,
        .pull_up_en = 1
    };
  
    ESP_ERROR_CHECK(gpio_config(&io_conf));
}


void toggle_switch__init() {

    ESP_LOGI(TAG, "Init Toggle switch");

    toggle_switch__gpio_init(toggle_ble_gpio);
    toggle_switch__gpio_init(toggle_usb_gpio);

    //install gpio isr service
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
    //hook isr handler for specific gpio pin
    ESP_ERROR_CHECK(gpio_isr_handler_add(toggle_ble_gpio, toggle_switch__gpio_isr_handler, (void*) TOGGLE_BLE));
    //hook isr handler for specific gpio pin
    ESP_ERROR_CHECK(gpio_isr_handler_add(toggle_usb_gpio, toggle_switch__gpio_isr_handler, (void*) TOGGLE_USB));

}

uint8_t toggle_switch__get_state() {

    uint8_t ble_gpio = gpio_get_level(toggle_ble_gpio);
    uint8_t usb_gpio = gpio_get_level(toggle_usb_gpio);

    ESP_LOGD(TAG, "Toggle states %d %d", !usb_gpio, !ble_gpio);

    if ( !ble_gpio ) {
        return TOGGLE_BLE;
    }
    else if ( !usb_gpio ) {
        return TOGGLE_USB;
    }
    else {
        ESP_LOGE(TAG, "Impossible toggle states %d %d", usb_gpio, ble_gpio);
    }
    return TOGGLE_OFF;
}


static void IRAM_ATTR toggle_switch__gpio_isr_handler(void *arg) {
    static int64_t last_interrupt;
    int64_t time;
    event_t event = {
        .type = EVENT_TOGGLE_SWITCH,
        .data = TOGGLE_OFF,
    };
    event.data = (uint8_t) arg;

    time = esp_timer_get_time();
    if (time - last_interrupt > TOGGLE_SWITCH_DEBOUNCE_TIME) {
            xQueueSendFromISR(event_q, &event, (TickType_t) 0);
    }
    last_interrupt = time;
}
