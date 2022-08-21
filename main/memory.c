#define LOG_LOCAL_LEVEL     ESP_LOG_DEBUG

#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "memory.h"
#include "config.h"
#include "ble_hidd.h"


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

static void mem_get_u8(nvs_handle_t handle, char *key, void *to);
static void mem_get_u16(nvs_handle_t handle, char *key, void *to);
static void mem_get_u32(nvs_handle_t handle, char *key, void *to);
static void mem_get_u64(nvs_handle_t handle, char *key, void *to);
static void mem_get_str(nvs_handle_t handle, char *key, void *to, size_t length);
static void mem_get_blob(nvs_handle_t handle, char *key, void *to, size_t size);



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
    esp_err_t err;
    uint32_t counter = 0;

    mem_get_u32(handle, key, &counter);

    counter++;

    ESP_LOGD(TAG, "Updating %s counter in NVS ... ", key);
    err = nvs_set_u32(handle, key, counter);
    ESP_ERROR_CHECK(err);
    err = nvs_commit(handle);
    ESP_ERROR_CHECK(err);

    ESP_LOGI(TAG, "%s : %d", key, counter);

    return counter;
}


// Bluetooth hosts
void memory__set_bluetooth_host(uint8_t host_id, bt_host_t host) {
    esp_err_t err;
    char key[6] = "host0";
    itoa(host_id, &(key[4]), 10);

    err = nvs_set_blob(ble_handle, key, &host, sizeof(bt_host_t));
    ESP_ERROR_CHECK(err);
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
    esp_err_t err;

    ESP_LOGD(TAG, "Setting last BT host: %d", host_id);
    err = nvs_set_u8(ble_handle, "last_host", host_id);
    ESP_ERROR_CHECK(err);
}


uint8_t memory__get_bluetooth_last_host() {
    uint8_t host = 0;

    mem_get_u8(ble_handle, "last_host", &host);

    return host;
}


static void mem_get_u8(nvs_handle_t handle, char *key, void *to) {

    esp_err_t err = nvs_get_u8(handle, key, to);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "%s not found in NVS", key);
    }
    else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting %s from NVS", key);
    }
    else {
        ESP_LOGD(TAG, "Successfully got %s from NVS", key);
    }
}


static void mem_get_u16(nvs_handle_t handle, char *key, void *to) {

    esp_err_t err = nvs_get_u16(handle, key, to);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "%s not found in NVS", key);
    }
    else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting %s from NVS", key);
    }
    else {
        ESP_LOGD(TAG, "Successfully got %s from NVS", key);
    }
}


static void mem_get_u32(nvs_handle_t handle, char *key, void *to) {

    esp_err_t err = nvs_get_u32(handle, key, to);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "%s not found in NVS", key);
    }
    else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting %s from NVS", key);
    }
    else {
        ESP_LOGD(TAG, "Successfully got %s from NVS", key);
    }
}


static void mem_get_u64(nvs_handle_t handle, char *key, void *to) {

    esp_err_t err = nvs_get_u64(handle, key, to);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "%s not found in NVS", key);
    }
    else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting %s from NVS", key);
    }
    else {
        ESP_LOGD(TAG, "Successfully got %s from NVS", key);
    }
}


static void mem_get_str(nvs_handle_t handle, char *key, void *to, size_t length) {

    size_t len = length;
    char *str = malloc(len);
    memset(str, 0x00, len);

    esp_err_t err = nvs_get_str(handle, key, str, &len);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "%s not found in NVS", key);
    }
    else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting %s from NVS", key);
    }
    else {
        ESP_LOGD(TAG, "Successfully got %s from NVS", key);
        memset(to, 0x00, length);
        strcpy(to, str);
    }
    free(str);
}


static void mem_get_blob(nvs_handle_t handle, char *key, void *to, size_t size) {

    esp_err_t err = nvs_get_blob(handle, key, to, &size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "%s not found in NVS", key);
    }
    else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting %s from NVS", key);
    }
    else {
        ESP_LOGD(TAG, "Successfully got %s from NVS", key);
    }
}


