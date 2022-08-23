#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "leds.h"
#include "config.h"


/* --------- Local Defines --------- */
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT // Set duty resolution to 8 bits
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

#define LEDS_RATE               (1) // in ms


/* --------- Local Variables --------- */
static const char *TAG = "leds";

static const gpio_num_t row_gpios[LED_ROWS] = {GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_33};
static const gpio_num_t col_gpios[LED_COLS] = {GPIO_NUM_13};


/* --------- Local Functions --------- */
static void leds__gpio_init(uint8_t pin);
static void leds__ledc_timer_init();
static void leds__ledc_init(uint8_t pin);



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
    // gpio_set_drive_capability(pin, GPIO_DRIVE_CAP_0);
    gpio_set_level(pin, 0);

}


static void leds__ledc_timer_init() {

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
}


static void leds__ledc_init(uint8_t pin) {

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = pin,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


void leds__init() {

    ESP_LOGI(TAG, "Init leds");

    // init rows
    ESP_LOGI(TAG, "Init row gpios");
    for (uint8_t i = 0; i < LED_ROWS; i++) {
        leds__gpio_init(row_gpios[i]);
        gpio_set_level(row_gpios[i], 1);
    }

    // init columns
    leds__ledc_timer_init();
    ESP_LOGI(TAG, "Init columns gpios");
    for (uint8_t i = 0; i < LED_COLS; i++) {
        // leds__gpio_init(col_gpios[i]);
        // gpio_set_level(col_gpios[i], 1);
        leds__ledc_init(col_gpios[i]);

    }

    // xTaskCreatePinnedToCore(leds__task, "led task", 2048, NULL, configMAX_PRIORITIES, NULL, 1);
}


void leds__set_brightness(uint8_t level) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, level);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

// void leds__task(void *pvParameters) {

//     ESP_LOGI(TAG, "Starting leds task");

//     while(1) {

//         for (uint8_t col = 0; col < LED_COLS; col++) {

//             for (uint8_t row = 0; row < LED_ROWS; row++) {
//                 gpio_set_level(row_gpios[row], 0);
//                 vTaskDelay(LEDS_RATE / portTICK_PERIOD_MS);
//                 gpio_set_level(row_gpios[row], 1);

//             }

//         }

//     }
//     vTaskDelete(NULL);

// }

