#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "memory.h"
#include "config.h"
#include "ble.h"


/* --------- Local Variables --------- */
static const char *TAG = "memory";

static nvs_handle_t system_handle;
static nvs_handle_t ble_handle;
static nvs_handle_t keyboard_handle;

static uint32_t restart_counter;
static uint32_t sleep_counter;


/* --------- Local Functions --------- */
static void init_handle(char *namespace, nvs_handle_t *handle);
static uint32_t increment_counter(nvs_handle_t handle, char *key);

static void mem_set_u8(nvs_handle_t handle, char* key, uint8_t value);
static void mem_set_u16(nvs_handle_t handle, char* key, uint16_t value);
static void mem_set_u32(nvs_handle_t handle, char* key, uint32_t value);
static void mem_set_u64(nvs_handle_t handle, char* key, uint64_t value);
static void mem_set_str(nvs_handle_t handle, char* key, char* value);
static void mem_set_blob(nvs_handle_t handle, char* key, void* value, size_t size);
static void mem_write_error_check(esp_err_t err, char *key);

static void mem_get_u8(nvs_handle_t handle, char *key, void *to);
static void mem_get_u16(nvs_handle_t handle, char *key, void *to);
static void mem_get_u32(nvs_handle_t handle, char *key, void *to);
static void mem_get_u64(nvs_handle_t handle, char *key, void *to);
static void mem_get_str(nvs_handle_t handle, char *key, void *to, size_t length);
static void mem_get_blob(nvs_handle_t handle, char *key, void *to, size_t size);
static void mem_read_error_check(esp_err_t err, char *key);



// init stuff
void memory__init() {
    esp_err_t ret;
    
    ESP_LOGI(TAG, "Init NVS");

    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    init_handle("system", &system_handle);
    init_handle("ble", &ble_handle);
    init_handle("keyboard", &keyboard_handle);

}


static void init_handle(char *namespace, nvs_handle_t *handle) {

    esp_err_t err = nvs_open(namespace, NVS_READWRITE, handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle for %s: 0x%x", namespace, err);
    }
    else {
        ESP_LOGD(TAG, "Successfully init NVS handle for %s", namespace);
    }
}


// Restart counter
void memory__set_restart_counter() {
    restart_counter = increment_counter(system_handle, "restart_counter");
}


uint32_t memory__get_restart_counter() {
    return restart_counter;
}


// Deep sleep counter
void memory__set_sleep_counter() {
    sleep_counter = increment_counter(system_handle, "sleep_counter");
}


uint32_t memory__get_sleep_counter() {
    return sleep_counter;
}


static uint32_t increment_counter(nvs_handle_t handle, char *key) {
    uint32_t counter = 0;

    mem_get_u32(handle, key, &counter);

    counter++;

    ESP_LOGD(TAG, "Updating %s counter in NVS ... ", key);
    mem_set_u32(handle, key, counter);

    ESP_LOGI(TAG, "%s : %ld", key, counter);

    return counter;
}


// Sleep status
// Used to know if keyboard should go back to sleep after a wakeup (when usb cable is plugged in)
void memory__set_sleep_status(bool sleep_status) {
    ESP_LOGD(TAG, "Setting sleep status: %d", sleep_status);
    mem_set_u8(system_handle, "sleep_status", sleep_status);
}


bool memory__get_sleep_status() {
    uint8_t status = 0;

    mem_get_u8(system_handle, "sleep_status", &status);

    return (bool) status;
}


// Layers
void memory__set_current_layer(uint8_t layer) {
    ESP_LOGD(TAG, "Setting current layer: %d", layer);
    mem_set_u8(keyboard_handle, "current_layer", layer);
}


uint8_t memory__get_current_layer() {
    uint8_t layer = 0;

    mem_get_u8(keyboard_handle, "current_layer", &layer);

    return layer;
}


// Keymaps / VIA
void memory__set_via_layout_options(uint32_t options) {
    ESP_LOGD(TAG, "Setting via layout options: %ld", options);
    mem_set_u8(keyboard_handle, "via_layout_options", options);
}


uint32_t  memory__get_via_layout_options() {
    uint32_t options = 0;
    
    ESP_LOGD(TAG, "Getting via layout options");
    mem_get_u32(keyboard_handle, "via_layout_options", &options);

    return options;
}


void memory__set_keymap_state(void *data, size_t length) {
    ESP_LOGD(TAG, "Setting keymap state of size %d", length);
    mem_set_blob(keyboard_handle, "keymap_state", data, length);
}


void memory__get_keymap_state(void *data, size_t length) {
    ESP_LOGD(TAG, "Getting keymap state");
    mem_get_blob(keyboard_handle, "keymap_state", data, length);
}


void memory__set_macros(void *data, size_t length) {
    ESP_LOGD(TAG, "Setting macros of size %d", length);
    mem_set_blob(keyboard_handle, "macros", data, length);
}


void memory__get_macros(void *data, size_t length) {
    ESP_LOGD(TAG, "Getting macros state");
    mem_get_blob(keyboard_handle, "macros", data, length);
}


void memory__set_keymaps(void *data, size_t length) {
    ESP_LOGD(TAG, "Setting keymaps of size %d", length);
    mem_set_blob(keyboard_handle, "keymaps", data, length);
}


void memory__get_keymaps(void *data, size_t length) {
    ESP_LOGD(TAG, "Getting keymaps");
    mem_get_blob(keyboard_handle, "keymaps", data, length);
}



// Bluetooth hosts
void memory__set_bluetooth_host(uint8_t host_id, bt_host_t host) {
    char key[6] = "host0";
    itoa(host_id, &(key[4]), 10);

    mem_set_blob(ble_handle, key, &host, sizeof(bt_host_t));
}


bt_host_t memory__get_bluetooth_host(uint8_t host_id) {
    bt_host_t host;
    size_t size = sizeof(bt_host_t);

    memset(&host, 0x00, size);

    char key[6] = "host0";
    itoa(host_id, &(key[4]), 10);

    mem_get_blob(ble_handle, key, &host, size);

    return host;
}


void memory__set_bluetooth_last_host(uint8_t host_id) {

    ESP_LOGD(TAG, "Setting last BT host: %d", host_id);
    mem_set_u8(ble_handle, "last_host", host_id);
}


uint8_t memory__get_bluetooth_last_host() {
    uint8_t host = 0;

    mem_get_u8(ble_handle, "last_host", &host);

    return host;
}


// Leds
void memory__set_leds_brightness(uint8_t brightness) {

    ESP_LOGD(TAG, "Setting leds brightness: %d", brightness);
    mem_set_u8(keyboard_handle, "brightness", brightness);
}


uint8_t memory__get_leds_brightness() {
    uint8_t brightness = 0;

    mem_get_u8(keyboard_handle, "brightness", &brightness);

    return brightness;
}


// --------- Writes ---------
static void mem_set_u8(nvs_handle_t handle, char* key, uint8_t value) {
    esp_err_t err = nvs_set_u8(handle, key, value);
    mem_write_error_check(err, key);
    err = nvs_commit(handle);
    ESP_ERROR_CHECK(err);
}


static void mem_set_u16(nvs_handle_t handle, char* key, uint16_t value) {
    esp_err_t err = nvs_set_u16(handle, key, value);
    mem_write_error_check(err, key);
    err = nvs_commit(handle);
    ESP_ERROR_CHECK(err);
}


static void mem_set_u32(nvs_handle_t handle, char* key, uint32_t value) {
    esp_err_t err = nvs_set_u32(handle, key, value);
    mem_write_error_check(err, key);
    err = nvs_commit(handle);
    ESP_ERROR_CHECK(err);
}


static void mem_set_u64(nvs_handle_t handle, char* key, uint64_t value) {
    esp_err_t err = nvs_set_u64(handle, key, value);
    mem_write_error_check(err, key);
    err = nvs_commit(handle);
    ESP_ERROR_CHECK(err);
}


static void mem_set_str(nvs_handle_t handle, char* key, char* value) {
    esp_err_t err = nvs_set_str(handle, key, value);
    mem_write_error_check(err, key);
    err = nvs_commit(handle);
    ESP_ERROR_CHECK(err);
}


static void mem_set_blob(nvs_handle_t handle, char* key, void* value, size_t size) {
    esp_err_t err = nvs_set_blob(handle, key, value, size);
    mem_write_error_check(err, key);
    err = nvs_commit(handle);
    ESP_ERROR_CHECK(err);
}


static void mem_write_error_check(esp_err_t err, char *key) {
    if (err == ESP_ERR_NVS_NOT_ENOUGH_SPACE) {
        ESP_LOGE(TAG, "Error setting %s in NVS. Not enough space!", key);
    }
    else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error setting %s in NVS: %d", key, err);
    }
    else {
        ESP_LOGD(TAG, "Succesfully wrote %s to NVS", key);
    }
}


// --------- Reads ---------
static void mem_get_u8(nvs_handle_t handle, char *key, void *to) {

    esp_err_t err = nvs_get_u8(handle, key, to);
    mem_read_error_check(err, key);
}


static void mem_get_u16(nvs_handle_t handle, char *key, void *to) {

    esp_err_t err = nvs_get_u16(handle, key, to);
    mem_read_error_check(err, key);
}


static void mem_get_u32(nvs_handle_t handle, char *key, void *to) {

    esp_err_t err = nvs_get_u32(handle, key, to);
    mem_read_error_check(err, key);
}


static void mem_get_u64(nvs_handle_t handle, char *key, void *to) {

    esp_err_t err = nvs_get_u64(handle, key, to);
    mem_read_error_check(err, key);
}


static void mem_get_str(nvs_handle_t handle, char *key, void *to, size_t length) {

    size_t len = length;
    char *str = malloc(len);
    memset(str, 0x00, len);

    esp_err_t err = nvs_get_str(handle, key, str, &len);
    mem_read_error_check(err, key);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "Successfully got %s from NVS", key);
        memset(to, 0x00, length);
        strcpy(to, str);
    }
    free(str);
}


static void mem_get_blob(nvs_handle_t handle, char *key, void *to, size_t size) {

    esp_err_t err = nvs_get_blob(handle, key, to, &size);
    mem_read_error_check(err, key);
}


static void mem_read_error_check(esp_err_t err, char *key) {
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "%s not found in NVS", key);
    }
    else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting %s from NVS %d", key, err);
    }
    else {
        ESP_LOGD(TAG, "Successfully got %s from NVS", key);
    }
}


