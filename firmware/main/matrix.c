#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_timer.h"

#include "matrix.h"
#include "config.h"


/* --------- Local Variables --------- */
static const char *TAG = "matrix";

static const gpio_num_t row_gpios[MATRIX_ROWS] = {GPIO_NUM_9, GPIO_NUM_8, GPIO_NUM_7, GPIO_NUM_6};
static const gpio_num_t col_gpios[MATRIX_COLS] = {GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_3};

uint8_t matrix_state[MATRIX_ROWS][MATRIX_COLS] = {0};
static uint8_t prev_matrix_state[MATRIX_ROWS][MATRIX_COLS] = {0};
static uint32_t debounce_matrix[MATRIX_ROWS][MATRIX_COLS] = {0};


/* --------- Local Functions --------- */
static void matrix__gpio_row_init(uint8_t pin);
static void matrix__gpio_col_init(uint8_t pin);
static uint32_t millis();



static uint32_t millis() {
    return esp_timer_get_time() / 1000;
}


static void matrix__gpio_row_init(uint8_t pin) {

    uint64_t pin_mask = (1ULL << pin);

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pin_bit_mask = pin_mask,
        .pull_down_en = 0,
        .pull_up_en = 0
    };
  
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    gpio_set_level(pin, 0);

}


static void matrix__gpio_col_init(uint8_t pin) {

    uint64_t pin_mask = (1ULL << pin);

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = pin_mask,
        .pull_down_en = 0,
        .pull_up_en = 1
    };
  
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    gpio_set_drive_capability(pin, GPIO_DRIVE_CAP_0);
    gpio_set_level(pin, 1);

}


void matrix__init() {

    ESP_LOGI(TAG, "Init matrix");

    // init rows
    ESP_LOGI(TAG, "Init row gpios");
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        matrix__gpio_row_init(row_gpios[i]);
    }

    // init columns
    ESP_LOGI(TAG, "Init columns gpios");
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        matrix__gpio_col_init(col_gpios[i]);
    }

}


void matrix__scan() {

    // Setting row pin as low, and checking if the column pin changes.
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        gpio_set_level(row_gpios[row], 0);

        for (uint8_t col = 0; col < MATRIX_COLS; col++) {

            uint8_t state = !(gpio_get_level(col_gpios[col]));

            if (prev_matrix_state[row][col] != state) {
                debounce_matrix[row][col] = millis();
            }
            prev_matrix_state[row][col] = state;

            if ((millis() - debounce_matrix[row][col]) > DEBOUNCE_TIME) {
                if (matrix_state[row][col] != state) {
                    matrix_state[row][col] = state;
                }
            }
        }

        gpio_set_level(row_gpios[row], 1);
    }

    // ESP_LOGD(TAG, "%ld states: %d, %d, %d", millis(), matrix_state[0][0], matrix_state[0][1], matrix_state[0][2]);

}



// RTC gpio matrix for deep sleep wake up
void matrix__rtc_setup(void) {
    uint64_t rtc_mask = 0;

    ESP_LOGI(TAG, "Init RTC matrix for deep sleep");

    // init columns
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        gpio_num_t pin = col_gpios[col];
        if (rtc_gpio_is_valid_gpio(pin) == 1) {
            rtc_gpio_init(pin);
            rtc_gpio_set_direction(pin, RTC_GPIO_MODE_INPUT_OUTPUT);
            rtc_gpio_set_drive_capability(pin, GPIO_DRIVE_CAP_3); // need to have enough current go through the diode
            rtc_gpio_set_level(pin, 1);

            ESP_LOGD(TAG,"%d is level %d", pin, gpio_get_level(pin));
        }
        else {
            ESP_LOGW(TAG, "gpio col pin %d is not a valid RTC pin", pin);
        }
    }

    // init rows
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        gpio_num_t pin = row_gpios[row];
        if (rtc_gpio_is_valid_gpio(pin) == 1) {
            rtc_gpio_init(pin);
            rtc_gpio_set_direction(pin, RTC_GPIO_MODE_INPUT_OUTPUT);
            rtc_gpio_set_level(pin, 0);
            rtc_gpio_wakeup_enable(pin, GPIO_INTR_HIGH_LEVEL);

            // setting bit
            rtc_mask |= 1llu << pin;

            ESP_LOGD(TAG,"%d is level %d", pin, gpio_get_level(pin));
        }
        else {
            ESP_LOGW(TAG, "gpio row pin %d is not a valid RTC pin", pin);
        }

        esp_sleep_enable_ext1_wakeup(rtc_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
    }

}


void matrix__rtc_deinit() {

    // deinit rows
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        gpio_num_t pin = row_gpios[row];
        if (rtc_gpio_is_valid_gpio(pin) == 1) {
            rtc_gpio_set_level(pin, 0);
            rtc_gpio_set_direction(pin, RTC_GPIO_MODE_DISABLED);
            rtc_gpio_deinit(pin);
            gpio_reset_pin(pin);
        }
    }

    // deinit columns
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        gpio_num_t pin = col_gpios[col];
        if (rtc_gpio_is_valid_gpio(pin) == 1) {
            rtc_gpio_set_level(pin, 0);
            rtc_gpio_set_direction(pin, RTC_GPIO_MODE_DISABLED);
            rtc_gpio_deinit(pin);
            gpio_reset_pin(pin);
        }
    }

}
