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

#define BRIGHTNESS_STEP         (10) // increase/decrease step size

#define LEDS_TASK_RATE          (100) // in ms

#define SLOW_PULSE_FADE         (500) // in ms
#define FAST_PULSE_FADE         (200) // in ms
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
    uint8_t host_id;
} led_effect_t;


/* --------- Local Variables --------- */
static const char *TAG = "leds";

static const gpio_num_t row_gpios[LED_ROWS] = {GPIO_NUM_34, GPIO_NUM_43, GPIO_NUM_44};
static const gpio_num_t col_gpios[LED_COLS] = {GPIO_NUM_36, GPIO_NUM_37, GPIO_NUM_35};
// One channel per column
static const ledc_channel_t col_channels[LED_COLS] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2};

static uint8_t brightness;
static TaskHandle_t xLED_task = NULL;

// Dumb stuff for dev, to be changed
static uint8_t bt_host_positions[BLE_NUM_HOSTS][2] = {
    {0, 1}, {0, 2}, {0, 0}
};


/* --------- Local Functions --------- */
static void leds__gpio_init(uint8_t pin);
static void leds__ledc_timer_init();
static void leds__ledc_init(uint8_t pin, uint8_t col_id);
static bool leds__fade_end_event_cb(const ledc_cb_param_t *param, void *arg);
static void leds__open_all();
static void leds__close_all();
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


static bool leds__fade_end_event_cb(const ledc_cb_param_t *param, void *arg) {

    portBASE_TYPE taskAwoken = pdFALSE;

    if (param->event == LEDC_FADE_END_EVT) {
        vTaskGenericNotifyGiveFromISR(xLED_task, FADE_EVENT_NOTIF_IDX, &taskAwoken);
    }

    return (taskAwoken == pdTRUE);
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


void leds__set_brightness(uint8_t level) {

    if (level != brightness) {
        brightness = level;
        memory__set_leds_brightness(brightness);
    }
    uint16_t duty_cycle = (uint16_t) brightness * 2.55; // 8 bit PWM resolution

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


// Is that necessay? probably
void leds__update_host_positions(uint8_t host_id, uint8_t position[2]) {

    ESP_LOGD(TAG, "Updating host %d positions to [%d,%d]", host_id, position[0], position[1]);

    bt_host_positions[host_id][0] = position[0];
    bt_host_positions[host_id][1] = position[1];

}


void leds__task(void *pvParameters) {

    led_effect_t effect;
    uint8_t host_id;

    ESP_LOGI(TAG, "Starting led task");

    while(1) {
        event_t event;

        if(uxQueueMessagesWaiting(leds_q)) {
            ESP_LOGD(TAG, "New event in queue");

            memset(&event, 0x00, sizeof(event_t));
            xQueueReceive(leds_q, &event, 0);

            host_id = event.data;
            if (host_id != effect.host_id) {
                effect.row_gpio = row_gpios[host_id];
                effect.col_channel = col_channels[host_id];
                effect.host_id = host_id;
            }

            if (effect.type == NONE) {
                leds__close_all();
                leds__open_row(effect.row_gpio);
            }
            switch(event.type) {
                case EVENT_LEDS_BT_ADV: {
                    ESP_LOGD(TAG, "Led Event adv -> slow pulse");
                    effect.type = SLOW_PULSE;
                    break;
                }
                case EVENT_LEDS_BT_ADV_ALL: {
                    ESP_LOGD(TAG, "Led Event adv all -> fast pulse");
                    effect.type = FAST_PULSE;
                    break;
                }
                case EVENT_LEDS_BT_CONNECTED: {
                    ESP_LOGD(TAG, "Led Event connected -> static");
                    effect.type = STATIC;
                    break;
                }
                default:
                    ESP_LOGW(TAG, "Unhandled event %d", event.type);
                    break;
            }
        }

        if (effect.type != NONE) {
            uint16_t duty_cycle = (uint16_t) brightness * 2.55; // 8 bit PWM resolution

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
                    ledc_set_duty(LEDC_MODE, effect.col_channel, duty_cycle);
                    ledc_update_duty(LEDC_MODE, effect.col_channel);

                    vTaskDelay(STATIC_FADE / portTICK_PERIOD_MS);
                    effect.type = NONE;
                    leds__open_all();
                }
                default:
                    break;
            }
        }

        ulTaskGenericNotifyTake(FADE_EVENT_NOTIF_IDX, pdTRUE, LEDS_TASK_RATE / portTICK_PERIOD_MS);

    }
    vTaskDelete(NULL);
}


