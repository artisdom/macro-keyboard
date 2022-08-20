#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#include "matrix.h"
#include "config.h"


/* --------- Local Variables --------- */
static const char *TAG = "matrix";

static const gpio_num_t row_gpios[MATRIX_ROWS] = {GPIO_NUM_32};
static const gpio_num_t col_gpios[MATRIX_COLS] = {GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27};

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
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = pin_mask,
        .pull_down_en = 0,
        .pull_up_en = 1
    };
  
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    gpio_set_drive_capability(pin, GPIO_DRIVE_CAP_0);
    gpio_set_level(pin, 1);

}


static void matrix__gpio_col_init(uint8_t pin) {

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

    // Setting column pin as low, and checking if the input of a row pin changes.
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        gpio_set_level(col_gpios[col], 0);

        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {

            uint8_t state = !(gpio_get_level(row_gpios[row]));

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

        gpio_set_level(col_gpios[col], 1);
    }

    ESP_LOGD(TAG, "%d states: %d, %d, %d", millis(), matrix_state[0][0], matrix_state[0][1], matrix_state[0][2]);

}
