#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
// #include "driver/adc.h"
// #include "esp_adc_cal.h"

#include "battery.h"
#include "config.h"


/* --------- Local Defines --------- */
#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling


/* --------- Local Variables --------- */
static const char *TAG = "battery";

static adc_oneshot_unit_handle_t adc_handle;
static const adc_channel_t channel = ADC_CHANNEL_9;
static const adc_bitwidth_t width = ADC_BITWIDTH_12;
static const adc_atten_t atten = ADC_ATTEN_DB_2_5;
static const adc_unit_t unit = ADC_UNIT_1;
static adc_cali_handle_t cali_handle;
// static esp_adc_cal_characteristics_t *adc_chars;

static const float bat_voltage_div_ratio = 0.2658; //based on TinyS3 schematic
// approximations for now
static const uint16_t bat_max_voltage = 4200; // in mV
static const uint16_t bat_min_voltage = 3400; // in mV

static const gpio_num_t usb_bus_gpio = GPIO_NUM_33;


/* --------- Local Functions --------- */
static uint32_t battery__get_voltage();
static bool battery__detect_bus_voltage();


static uint32_t battery__get_voltage() {

    uint32_t reading = 0;
    uint32_t voltage = 0;
    uint32_t bat_voltage = 0;

    for (uint16_t i = 0; i < NO_OF_SAMPLES; i++) {
        uint32_t raw = 0;
        adc_oneshot_read(adc_handle, channel, &raw);
        reading += raw;
    }
    reading /= NO_OF_SAMPLES;

    adc_cali_raw_to_voltage(cali_handle, reading, &voltage);
    // voltage = esp_adc_cal_raw_to_voltage(reading, adc_chars);
    bat_voltage = (uint32_t) (voltage / bat_voltage_div_ratio);

    ESP_LOGD(TAG, "reading: %ld   voltage on pin: %ldmV   Vbat: %ldmV", reading, voltage, bat_voltage);

    return bat_voltage;
}


uint32_t battery__get_level() {

    uint32_t percentage = 0;
    uint32_t voltage = battery__get_voltage();

    percentage = (voltage - bat_min_voltage) * 100;
    percentage /= (bat_max_voltage - bat_min_voltage);
    
    ESP_LOGI(TAG, "voltage: %ldmV -> %ld%%", voltage, percentage);

    if (percentage > 100) {
        percentage = 100;
    }

    return percentage;
}


void battery__init() {

    ESP_LOGI(TAG, "Init Battery");

    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = unit,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = width,
        .atten = atten,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel, &config));


    ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = width,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle));

    // // ADC for Battery voltage pin
 //    adc1_config_width(width);
 //    adc1_config_channel_atten(channel, atten);

 //    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    // esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

    // GPIO driver for USB 5V detection
    uint64_t pin_mask = (1ULL << usb_bus_gpio);

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = pin_mask,
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

}


static bool battery__detect_bus_voltage() {

    uint8_t level = gpio_get_level(usb_bus_gpio);
    ESP_LOGD(TAG, "USB Bus voltage is %d", level);

    if (level) {
        return true;
    }
    return false;

}


bool battery__is_charging() {

    return battery__detect_bus_voltage();
}




