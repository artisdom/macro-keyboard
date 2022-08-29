#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_system.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"

#include "toggle_switch.h"
#include "config.h"
#include "events.h"


/* --------- Local Defines --------- */
#define ESP_INTR_FLAG_DEFAULT 0


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

    if ( !(gpio_get_level(toggle_ble_gpio)) ) {
        return TOGGLE_BLE;
    }
    else if ( !(gpio_get_level(toggle_usb_gpio)) ) {
        return TOGGLE_USB;
    }
    else {
        ESP_LOGW(TAG, "Impossible toggle states");
    }
    return TOGGLE_OFF;
}


static void IRAM_ATTR toggle_switch__gpio_isr_handler(void *arg) {
    event_t event = {
        .type = EVENT_TOGGLE_SWITCH,
        .data = TOGGLE_OFF,
    };
    event.data = *((uint8_t *)arg);
    xQueueSendFromISR(event_q, &event, (TickType_t) 0);
}
