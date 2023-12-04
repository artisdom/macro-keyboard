#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"

#include "leds.h"
#include "config.h"
#include "memory.h"
#include "events.h"


/* --------- Global Variables --------- */
QueueHandle_t leds_q;


/* --------- Local Defines --------- */
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT // Set duty resolution to 8 bits
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

#define BRIGHTNESS_STEP         (10)   // increase/decrease step size

#define LEDS_TASK_RATE          (100)  // in ms

#define SLOW_PULSE_FADE         (500)  // in ms
#define FAST_PULSE_FADE         (200)  // in ms
#define STATIC_FADE             (1000) // in ms

#define FADE_EVENT_NOTIF_IDX    (0)


/* --------- Local Definitions --------- */
typedef enum fade_direction_e {
    FADEIN,
    FADEOUT,
} fade_direction_t;

typedef enum fade_type_e {
    NONE = 0x00,
    SLOW_PULSE,
    FAST_PULSE,
    STATIC,
} fade_type_t;

typedef struct led_effect_s {
    gpio_num_t row_gpio;
    ledc_channel_t col_channel;
    fade_direction_t direction;
    fade_type_t type;
} led_effect_t;


/* --------- Local Variables --------- */
static const char *TAG = "leds";

static const gpio_num_t row_gpios[LED_ROWS] = {GPIO_NUM_34, GPIO_NUM_43, GPIO_NUM_44};
static const gpio_num_t col_gpios[LED_COLS] = {GPIO_NUM_36, GPIO_NUM_37, GPIO_NUM_35};
// One channel per column
static const ledc_channel_t col_channels[LED_COLS] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2};

static bool backlight_enabled;        // runtime toggle of leds backlight (not including effects)
static uint8_t brightness; 
static TaskHandle_t xLED_task = NULL;

static uint8_t effect_position[2];
static bool bt_effect_enabled = LED_BT_EFFECTS_ENABLED;

// Lightness curve using the CIE 1931 lightness formula
// Generated by the python script provided in http://jared.geek.nz/2013/feb/linear-led-pwm
static const uint8_t cie1931_curve[101] = {
    0,   0,   1,   1,   1,   1,   2,   2,   2,   2, 
    3,   3,   3,   4,   4,   5,   5,   5,   6,   6, 
    7,   8,   8,   9,   10,  10,  11,  12,  13,  14, 
    15,  16,  17,  18,  19,  20,  21,  23,  24,  25, 
    27,  28,  30,  31,  33,  34,  36,  38,  40,  42, 
    44,  46,  48,  50,  52,  54,  56,  59,  61,  64, 
    66,  69,  72,  75,  77,  80,  83,  87,  90,  93, 
    96,  100, 103, 107, 110, 114, 118, 122, 126, 130, 
    134, 138, 142, 147, 151, 156, 161, 165, 170, 175, 
    180, 185, 191, 196, 201, 207, 213, 218, 224, 230, 
    236, 
};


/* --------- Local Functions --------- */
static void leds__gpio_init(uint8_t pin);
static void leds__ledc_timer_init();
static void leds__ledc_init(uint8_t pin, uint8_t col_id);
static bool leds__fade_end_event_cb(const ledc_cb_param_t *param, void *arg);
static void leds__set_backlight();
static void leds__open_all();
static void leds__close_all();
static void leds__close_columns();
static void leds__open_row(gpio_num_t row);
static void leds__close_row(gpio_num_t row);



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


static bool IRAM_ATTR leds__fade_end_event_cb(const ledc_cb_param_t *param, void *arg) {

    portBASE_TYPE taskAwoken = pdFALSE;

    if (param->event == LEDC_FADE_END_EVT) {
        vTaskGenericNotifyGiveFromISR(xLED_task, FADE_EVENT_NOTIF_IDX, &taskAwoken);
    }

    return (taskAwoken == pdTRUE);
}


void leds__init() {

    ESP_LOGI(TAG, "Init leds");

    backlight_enabled = true;
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

    ledc_fade_func_install(0);
    ledc_cbs_t callbacks = {
        .fade_cb = leds__fade_end_event_cb
    };

    for (uint8_t i = 0; i < LED_COLS; i++) {
        ledc_cb_register(LEDC_MODE, col_channels[i], &callbacks, (void *) NULL);
    }

    if (LED_EFFECTS_ENABLED) {
        leds_q = xQueueCreate(32, sizeof(event_t));
        xTaskCreatePinnedToCore(leds__task, "leds task", 4096, NULL, configMAX_PRIORITIES, &xLED_task, 1);
    }

}


void leds__enable_backlight(bool enable) {
    backlight_enabled = enable;
    leds__set_backlight();
}


void leds__toggle_backlight() {
    backlight_enabled = !backlight_enabled;
    leds__set_backlight();
}


bool leds__get_backlight() {
    return backlight_enabled;
}


static void leds__set_backlight() {
    ESP_LOGD(TAG, "Backlight %d", backlight_enabled);
    if (backlight_enabled) {
        leds__open_all();
    }
    else {
        leds__close_all();
    }
}


void leds__set_brightness(uint8_t level) {

    if (level != brightness) {
        brightness = level;
        memory__set_leds_brightness(brightness);
    }
    uint16_t duty_cycle = cie1931_curve[brightness];

    ESP_LOGD(TAG, "Setting leds to %d%% = duty cycle %d", brightness, duty_cycle);

    // update all leds channels
    for (uint8_t i = 0; i < LED_COLS; i++) {
        ledc_channel_t channel = col_channels[i];
        ledc_set_duty(LEDC_MODE, channel, duty_cycle);
        ledc_update_duty(LEDC_MODE, channel);
    }
}


uint8_t leds__get_brightness() {
    return brightness;
}


void leds__increase_brightness() {

    uint8_t new_brightness = brightness;
    new_brightness += BRIGHTNESS_STEP;
    if (new_brightness > 100) {
        new_brightness = 100;
    }
    ESP_LOGI(TAG, "Setting leds to %d%%", new_brightness);
    leds__set_brightness(new_brightness);
}


void leds__decrease_brightness() {

    uint8_t new_brightness = brightness;
    new_brightness -= BRIGHTNESS_STEP;
    if (new_brightness > 100) { // overflow
        new_brightness = 0;
    }
    ESP_LOGI(TAG, "Setting leds to %d%%", new_brightness);
    leds__set_brightness(new_brightness);
}


static void leds__open_all() {

    for (uint8_t i = 0; i < LED_ROWS; i++) {
        gpio_set_level(row_gpios[i], 0);
    }

    leds__set_brightness(brightness);
}


static void leds__close_all() {

    for (uint8_t i = 0; i < LED_ROWS; i++) {
        gpio_set_level(row_gpios[i], 1);
    }

    leds__close_columns();
}


static void leds__close_columns() {

    for (uint8_t i = 0; i < LED_COLS; i++) {
        ledc_channel_t channel = col_channels[i];
        ledc_set_duty(LEDC_MODE, channel, 0);
        ledc_update_duty(LEDC_MODE, channel);
    }
}


static void leds__open_row(gpio_num_t row) {
    gpio_set_level(row, 0);
}


static void leds__close_row(gpio_num_t row) {
    gpio_set_level(row, 1);
}


void leds__update_effect_position(uint8_t position[2]) {

    if (position[0] >= LED_ROWS || position[1] >= LED_COLS) {
        ESP_LOGE(TAG, "Unable to update effect position: outside of led matrix range");
    }
    else {
        ESP_LOGD(TAG, "Updating effect position to [%d, %d]", position[0], position[1]);
        effect_position[0] = position[0];
        effect_position[1] = position[1];
    }
}


void leds__task(void *pvParameters) {

    led_effect_t effect;
    uint16_t counter = 0;

    ESP_LOGI(TAG, "Starting led task");

    while(1) {
        event_t event;

        if(uxQueueMessagesWaiting(leds_q)) {
            led_effect_t new_effect;

            ESP_LOGD(TAG, "New event in queue");

            memset(&event, 0x00, sizeof(event_t));
            xQueueReceive(leds_q, &event, 0);

            switch (event.type) {
                case EVENT_BT_ADV:
                    if (bt_effect_enabled) {
                        ESP_LOGD(TAG, "Led Event adv -> slow pulse");
                        new_effect.type = SLOW_PULSE;
                        new_effect.row_gpio = row_gpios[effect_position[0]];
                        new_effect.col_channel = col_channels[effect_position[1]];
                    }
                    break;
                 case EVENT_BT_ADV_ALL:
                    if (bt_effect_enabled) {
                        ESP_LOGD(TAG, "Led Event adv all -> fast pulse");
                        new_effect.type = FAST_PULSE;
                        new_effect.row_gpio = row_gpios[effect_position[0]];
                        new_effect.col_channel = col_channels[effect_position[1]];
                    }
                    break;
                case EVENT_BT_CONNECTED:
                    if (bt_effect_enabled) {
                        ESP_LOGD(TAG, "Led Event connected -> static");
                        new_effect.type = STATIC;
                        new_effect.row_gpio = row_gpios[effect_position[0]];
                        new_effect.col_channel = col_channels[effect_position[1]];
                        counter = 0;
                    }
                    break;
                case EVENT_BT_LEDS_EFFECT_TOGGLE:
                    if (LED_BT_EFFECTS_ENABLED) {
                        ESP_LOGD(TAG, "Led Event toggle ble effects %d", event.data);
                        bt_effect_enabled = event.data;
                        new_effect.type = NONE;
                    }
                    break;
                case EVENT_LAYERS_CHANGED:
                    if (LED_LAYER_EFFECTS_ENABLED) {
                        ESP_LOGD(TAG, "Led Event layer changed effect: 0x%x", event.data);
                        for (uint8_t i = 8; i != 0; i--) {
                            if ((event.data >> (i-1)) & 1) {
                                uint8_t row_id = (i-1) / LED_ROWS;
                                uint8_t col_id = (i-1) % LED_COLS;
                                // ESP_LOGD(TAG, "row: %d col: %d", row_id, col_id);
                                new_effect.type = STATIC;
                                new_effect.row_gpio = row_gpios[row_id];
                                new_effect.col_channel = col_channels[col_id];
                                counter = 0;
                                break;
                            }
                        }
                    }
                    break;
                default:
                    ESP_LOGW(TAG, "Unhandled LED event type");
            }

            if (effect.type == NONE && new_effect.type != NONE) {
                leds__close_all();
                leds__open_row(new_effect.row_gpio);
            }
            else if (new_effect.type == NONE) {
                leds__set_backlight();
            }
            else {
                if (effect.row_gpio != new_effect.row_gpio) {
                    leds__close_row(effect.row_gpio);
                    leds__open_row(new_effect.row_gpio);
                }
                if (effect.col_channel != new_effect.col_channel) {
                    leds__close_columns();
                }
            }

            effect = new_effect;

        }

        uint16_t duty_cycle = cie1931_curve[brightness];
        if (effect.type != NONE && duty_cycle > 0) {

            switch (effect.type) {
                case (SLOW_PULSE): {
                    if (effect.direction == FADEIN) {
                        effect.direction = FADEOUT;
                        ledc_set_fade_with_time(LEDC_MODE, effect.col_channel, 0, SLOW_PULSE_FADE);
                        ledc_fade_start(LEDC_MODE, effect.col_channel, LEDC_FADE_NO_WAIT);
                    }
                    else if (effect.direction == FADEOUT) {
                        effect.direction = FADEIN;
                        ledc_set_fade_with_time(LEDC_MODE, effect.col_channel, duty_cycle, SLOW_PULSE_FADE);
                        ledc_fade_start(LEDC_MODE, effect.col_channel, LEDC_FADE_NO_WAIT);
                    }
                    break;
                }
                case(FAST_PULSE): {
                    if (effect.direction == FADEIN) {
                        effect.direction = FADEOUT;
                        ledc_set_fade_with_time(LEDC_MODE, effect.col_channel, 0, FAST_PULSE_FADE);
                        ledc_fade_start(LEDC_MODE, effect.col_channel, LEDC_FADE_NO_WAIT);
                    }
                    else if (effect.direction == FADEOUT) {
                        effect.direction = FADEIN;
                        ledc_set_fade_with_time(LEDC_MODE, effect.col_channel, duty_cycle, FAST_PULSE_FADE);
                        ledc_fade_start(LEDC_MODE, effect.col_channel, LEDC_FADE_NO_WAIT);
                    }
                    break;
                }
                case(STATIC): {
                    if (counter == 0) {
                        ledc_set_duty(LEDC_MODE, effect.col_channel, duty_cycle);
                        ledc_update_duty(LEDC_MODE, effect.col_channel);
                    }
                    if (counter >= STATIC_FADE / LEDS_TASK_RATE) {
                        counter = 0;
                        effect.type = NONE;
                        leds__set_backlight();
                    }
                    counter++;
                }
                default:
                    break;
            }
        }

        // wait for fade end notification from isr handler, otherwise timeout at specific rate
        ulTaskGenericNotifyTake(FADE_EVENT_NOTIF_IDX, pdTRUE, LEDS_TASK_RATE / portTICK_PERIOD_MS);

    }
    vTaskDelete(NULL);
}


