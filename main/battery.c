#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "battery.h"
#include "config.h"


/* --------- Local Defines --------- */
#define DEFAULT_VREF    3300        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling


/* --------- Local Variables --------- */
static const char *TAG = "battery";

static const adc_channel_t channel = ADC1_CHANNEL_9;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_2_5;
static const adc_unit_t unit = ADC_UNIT_1;
static esp_adc_cal_characteristics_t *adc_chars;

static const float voltage_divider_ratio = 0.2658; //based on TinyS3 schematic
// approximations for now
static const uint16_t bat_max_voltage = 4200; // in mV
static const uint16_t bat_min_voltage = 3400; // in mV


/* --------- Local Functions --------- */
static uint32_t battery__get_voltage();


static uint32_t battery__get_voltage() {

	uint32_t reading = 0;
	uint32_t voltage = 0;
	uint32_t bat_voltage = 0;

	for (uint16_t i = 0; i < NO_OF_SAMPLES; i++) {
		reading += adc1_get_raw(channel);
	}
	reading /= NO_OF_SAMPLES;

	voltage = esp_adc_cal_raw_to_voltage(reading, adc_chars);
	bat_voltage = (uint32_t) (voltage / voltage_divider_ratio);

	ESP_LOGD(TAG, "reading: %d   voltage on pin: %dmV   Vbat: %dmV", reading, voltage, bat_voltage);

	return bat_voltage;
}


uint32_t battery__get_level() {

	uint32_t percentage = 0;
	uint32_t voltage = battery__get_voltage();

	percentage = (voltage - bat_min_voltage) * 100;
	percentage /= (bat_max_voltage - bat_min_voltage);
	
	ESP_LOGI(TAG, "voltage: %dmV -> %d%%", voltage, percentage);

	return percentage;
}


void battery__init() {

	ESP_LOGI(TAG, "Init Battery");

    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);

    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

}