#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "leds.h"
#include "config.h"
#include "memory.h"


/* --------- Local Defines --------- */
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT // Set duty resolution to 8 bits
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz
#define LEDC_DUTY_CYCLE         (50) // in percentage

#define LEDS_RATE               (1) // in ms

#define BRIGHTNESS_STEP         (10) // increase/decrease step size


/* --------- Local Variables --------- */
static const char *TAG = "leds";

static const gpio_num_t row_gpios[LED_ROWS] = {GPIO_NUM_34, GPIO_NUM_43, GPIO_NUM_44};
static const gpio_num_t col_gpios[LED_COLS] = {GPIO_NUM_36, GPIO_NUM_37, GPIO_NUM_35};

// One channel per column
static const ledc_channel_t col_channels[LED_COLS] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2};

static uint8_t brightness;


/* --------- Local Functions --------- */
static void leds__gpio_init(uint8_t pin);
static void leds__ledc_timer_init();
static void leds__ledc_init(uint8_t pin, uint8_t col_id);



static void leds__gpio_init(uint8_t pin) {

    uint64_t pin_mask = (1ULL << pin);

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = pin_mask,
        .pull_down_en = 0,
        .pull_up_en = 0
    };
  
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    gpio_set_level(pin, 0);

}


static void leds__ledc_timer_init() {

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
}


static void leds__ledc_init(uint8_t pin, uint8_t col_id) {

    ledc_channel_t channel = col_channels[col_id];

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = channel,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = pin,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


void leds__init() {

    ESP_LOGI(TAG, "Init leds");

    brightness = memory__get_leds_brightness();

    // init rows
    ESP_LOGI(TAG, "Init row gpios");
    for (uint8_t i = 0; i < LED_ROWS; i++) {
        leds__gpio_init(row_gpios[i]);
        gpio_set_level(row_gpios[i], 0);
    }

    // init columns
    ESP_LOGI(TAG, "Init columns gpios");
    leds__ledc_timer_init();
    for (uint8_t i = 0; i < LED_COLS; i++) {
        leds__ledc_init(col_gpios[i], i);
    }

    leds__set_brightness(brightness);

}


void leds__set_brightness(uint8_t level) {

    if (level != brightness) {
        brightness = level;
        memory__set_leds_brightness(brightness);
    }
    uint16_t duty_cycle = (uint16_t) brightness * 2.55;

    ESP_LOGD(TAG, "Setting leds to %d%% = duty cycle %d", brightness, duty_cycle);

    // update all leds channels
    for (uint8_t i = 0; i < LED_COLS; i++) {
        ledc_channel_t channel = col_channels[i];
        ledc_set_duty(LEDC_MODE, channel, duty_cycle);
        ledc_update_duty(LEDC_MODE, channel);
    }
}


void leds__increase_brightness() {

    uint8_t new_brightness = brightness;
    new_brightness += BRIGHTNESS_STEP;
    if (new_brightness > 100) {
        new_brightness = 100;
    }
    leds__set_brightness(new_brightness);
}


void leds__decrease_brightness() {

    uint8_t new_brightness = brightness;
    new_brightness -= BRIGHTNESS_STEP;
    if (new_brightness > 100) { // overflow
        new_brightness = 0;
    }
    leds__set_brightness(new_brightness);
}



