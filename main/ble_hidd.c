/* This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this software is
   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_bt.h"

#include "esp_hidd_prf_api.h"
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "driver/gpio.h"
#include "hid_dev.h"
#include "nvs_flash.h"

#include "ble_hidd.h"
#include "config.h"
#include "memory.h"



/* --------- Global Variables --------- */
TaskHandle_t xBLE_keyboard_task;
TaskHandle_t xBLE_event_task;

// Input queue for sending keyboard reports
QueueHandle_t ble_keyboard_q;
// Input queue for receiving ble events to handle
QueueHandle_t ble_event_q;


/* --------- Local Defines --------- */
#define CHAR_DECLARATION_SIZE   (sizeof(uint8_t))       


/* --------- Local Variables --------- */
static const char *TAG = "ble_hid";

static uint16_t hid_conn_id = 0;
static bool sec_conn = false;
static uint8_t current_host_id = 0;

static uint8_t hidd_service_uuid128[] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x12, 0x18, 0x00, 0x00,
};

static esp_ble_adv_data_t hidd_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x03c0,       //HID Generic,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(hidd_service_uuid128),
    .p_service_uuid = hidd_service_uuid128,
    .flag = 0x6,
};

static esp_ble_adv_params_t hidd_adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x30,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    // .peer_addr          = {0xa0, 0x99, 0x9b, 0x02, 0xf0, 0x32},
    // .peer_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// static esp_ble_adv_params_t hidd_adv_params_directed = {
//     .adv_int_min        = 0x20,
//     .adv_int_max        = 0x30,
//     .adv_type           = ADV_TYPE_DIRECT_IND_LOW,
//     .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
//     .peer_addr          = {0xa0, 0x99, 0x9b, 0x02, 0xf0, 0x32},
//     .peer_addr_type     = BLE_ADDR_TYPE_PUBLIC,
//     .channel_map        = ADV_CHNL_ALL,
//     .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
// };


/* --------- Local Function --------- */
static void hidd_event_callback(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param);
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

static void ble_set_host(uint8_t host_id);
static void ble_change_host(uint8_t host_id);
static void ble_save_host(esp_bd_addr_t addr, esp_ble_addr_type_t addr_type);
// static uint64_t ble_addr_to_u64(esp_bd_addr_t addr);
// static void ble_u64_to_addr(uint64_t addr, esp_bd_addr_t to_addr);


static void hidd_event_callback(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param) {
    switch(event) {
        case ESP_HIDD_EVENT_REG_FINISH: {
            if (param->init_finish.state == ESP_HIDD_INIT_OK) {
                //esp_bd_addr_t rand_addr = {0x04,0x11,0x11,0x11,0x11,0x05};
                esp_ble_gap_set_device_name(BLE_DEVICE_NAME);
                esp_ble_gap_config_adv_data(&hidd_adv_data);

            }
            break;
        }
        case ESP_BAT_EVENT_REG: {
            break;
        }
        case ESP_HIDD_EVENT_DEINIT_FINISH:
	     break;
		case ESP_HIDD_EVENT_BLE_CONNECT: {
            ESP_LOGI(TAG, "ESP_HIDD_EVENT_BLE_CONNECT");
            hid_conn_id = param->connect.conn_id;
            break;
        }
        case ESP_HIDD_EVENT_BLE_DISCONNECT: {
            sec_conn = false;
            ESP_LOGI(TAG, "ESP_HIDD_EVENT_BLE_DISCONNECT");
            esp_ble_gap_start_advertising(&hidd_adv_params);
            break;
        }
        case ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT: {
            ESP_LOGI(TAG, "%s, ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT", __func__);
            ESP_LOG_BUFFER_HEX(TAG, param->vendor_write.data, param->vendor_write.length);
        }
        default:
            break;
    }
    return;
}


static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&hidd_adv_params);
        break;
     case ESP_GAP_BLE_SEC_REQ_EVT:
        for(int i = 0; i < ESP_BD_ADDR_LEN; i++) {
             ESP_LOGD(TAG, "%x:",param->ble_security.ble_req.bd_addr[i]);
        }
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
	 break;
     case ESP_GAP_BLE_AUTH_CMPL_EVT:
        sec_conn = true;
        esp_bd_addr_t bd_addr;
        esp_ble_addr_type_t bd_addr_type;

        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        bd_addr_type = param->ble_security.auth_cmpl.addr_type;
        // ESP_LOGI(TAG, "remote BD address: %08x%04x",
        //         (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
        //         (bd_addr[4] << 8) + bd_addr[5]);
        ESP_LOGI(TAG, "remote host address: "ESP_BD_ADDR_STR, ESP_BD_ADDR_HEX(bd_addr));
        ESP_LOGI(TAG, "address type = %d", bd_addr_type);
        ESP_LOGI(TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
        if(!param->ble_security.auth_cmpl.success) {
            ESP_LOGE(TAG, "fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
        }

        // onyl save if address is Public or Random
        if (bd_addr_type <= BLE_ADDR_TYPE_RANDOM) {
            ble_save_host(bd_addr, bd_addr_type);
        }
        else {
            ESP_LOGW(TAG, "Unable to save host in memory as the addr type is %d", bd_addr_type);
        }


        break;
    default:
        break;
    }
}



// Helpers
// static uint64_t ble_addr_to_u64(esp_bd_addr_t addr) {
//     uint64_t to_addr = (addr[0] << 40) + (addr[1] << 32) + (addr[2] << 24)
//         + (addr[3] << 16) + (addr[4] << 8) + (addr[5]);

//     return to_addr;
// }


// static void ble_u64_to_addr(uint64_t addr, esp_bd_addr_t to_addr) {

//     for (uint8_t i = 0, j = ESP_BD_ADDR_LEN-1; i < ESP_BD_ADDR_LEN; i++, j--) {
//         to_addr[i] = (addr >> (8 * j)) & 0xff;
//     }   
// }


static void ble_set_host(uint8_t host_id) {

    bt_addr_t host;

    ESP_LOGI(TAG, "Setting host to %d", host_id);

    host = memory__get_bluetooth_host(host_id);

    memcpy(hidd_adv_params.peer_addr, host.addr, sizeof(host.addr));
    hidd_adv_params.peer_addr_type = host.type;

    ESP_LOGD(TAG, "peer addr: "ESP_BD_ADDR_STR, ESP_BD_ADDR_HEX(hidd_adv_params.peer_addr));
    ESP_LOGD(TAG, "peer addr type: %d", hidd_adv_params.peer_addr_type);

    if (host.addr[0] == 0) { //terrible check
        ESP_LOGW(TAG, "No host stored in memory, advertising to all...");
    }

}


static void ble_change_host(uint8_t host_id) {

    ESP_LOGI(TAG, "Changing host to %d", host_id);

    esp_bd_addr_t current_host;
    memcpy(current_host, hidd_adv_params.peer_addr, sizeof(esp_bd_addr_t));

    ble_set_host(host_id);

    current_host_id = host_id;
    memory__set_bluetooth_last_host(host_id);

    // TODO
    // stop then start BT + adv
    esp_ble_gap_stop_advertising();
    // esp_ble_gap_disconnect(current_host);
    // esp_ble_gap_start_advertising(&hidd_adv_params);

}


static void ble_save_host(esp_bd_addr_t addr, esp_ble_addr_type_t addr_type) {
    bt_addr_t host;

    memcpy(host.addr, addr, sizeof(esp_bd_addr_t));
    host.type = addr_type;

    ESP_LOGI(TAG, "Saving host at id %d", current_host_id);

    memory__set_bluetooth_host(current_host_id, host);
}


void ble_keyboard_task(void *pvParameters) {

    uint8_t *report;

    ESP_LOGI(TAG, "Starting ble keyboard task");

    if (ble_keyboard_q != NULL)
        ESP_LOGW(TAG, "keyboard queue not initialised, resetting...");
        xQueueReset(ble_keyboard_q);

    //check if queue is initialized
    if (ble_keyboard_q != NULL) {
        while (1) {
            //pend on MQ, if timeout triggers, just wait again.
            if (xQueueReceive(ble_keyboard_q, &report, portMAX_DELAY)) {
                //if we are not connected, discard.
                if (sec_conn == false)
                    continue;
                // esp_hidd_send_keyboard_value(hid_conn_id, 0, &report, HID_REPORT_LEN);
                hid_dev_send_report(hidd_le_env.gatt_if, hid_conn_id, HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT, HID_REPORT_LEN, report);
            }

        }
    }
    else {
        ESP_LOGE(TAG, "keyboard queue not initialized, retry in 1s");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}


void ble_event_task(void *pvParameters) {

    // only BT host id change for now
    uint8_t event;

    ESP_LOGI(TAG, "Starting ble event task");

    if (ble_event_q != NULL)
        ESP_LOGW(TAG, "ble_event queue not initialised, resetting...");
        xQueueReset(ble_event_q);

    //check if queue is initialized
    if (ble_event_q != NULL) {
        while (1) {
            //pend on MQ, if timeout triggers, just wait again.
            if (xQueueReceive(ble_event_q, &event, portMAX_DELAY)) {
                ble_change_host(event);
            }

        }
    }
    else {
        ESP_LOGE(TAG, "ble_event queue not initialized, retry in 1s");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}


void ble_init(void) {
    esp_err_t ret;

    // Create queues
    ble_keyboard_q = xQueueCreate(32, HID_REPORT_LEN * sizeof(uint8_t));
    ble_event_q = xQueueCreate(32, sizeof(uint8_t));

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed\n", __func__);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed\n", __func__);
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluedroid failed\n", __func__);
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluedroid failed\n", __func__);
        return;
    }

    if((ret = esp_hidd_profile_init()) != ESP_OK) {
        ESP_LOGE(TAG, "%s init hidd profile failed\n", __func__);
    }

    // set host to connect to
    // current_host_id = memory__get_bluetooth_last_host();
    // ble_set_host(current_host_id);

    // register the callback function to the gap module
    esp_ble_gap_register_callback(gap_event_handler);
    esp_hidd_register_callbacks(hidd_event_callback);

    /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_BOND;     //bonding with peer device after authentication
    esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input
    uint8_t key_size = 16;      //the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    /* If your BLE device act as a Slave, the init_key means you hope which types of key of the master should distribute to you,
    and the response key means which key you can distribute to the Master;
    If your BLE device act as a master, the response key means you hope which types of key of the slave should distribute to you,
    and the init key means which key you can distribute to the slave. */
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));

    // Create tasks
    // xTaskCreate(&hid_demo_task, "hid_task", 2048, NULL, 5, NULL);
    xTaskCreatePinnedToCore(ble_keyboard_task, "ble_keyboard_task", 2048, NULL, configMAX_PRIORITIES, &xBLE_keyboard_task, 0);
    xTaskCreatePinnedToCore(ble_event_task, "ble_event_task", 2048, NULL, configMAX_PRIORITIES, &xBLE_event_task, 0);
}

esp_err_t ble_deinit() {
    esp_err_t ret;

    ret = esp_hidd_profile_deinit();
    if (ret) {
        ESP_LOGE(TAG, "%s deinit hidd profile failed\n", __func__);
        return ret;
    }

    ret = esp_bluedroid_disable();
    if (ret) {
        ESP_LOGE(TAG, "%s disable bluedroid failed\n", __func__);
        return ret;
    }

    ret = esp_bluedroid_deinit();
    if (ret) {
        ESP_LOGE(TAG, "deinit bluedroid failed");
        return ret;
    }

    ret = esp_bt_controller_disable();
    if (ret) {
        ESP_LOGE(TAG, "disable controller failed");
        return ret;
    }

    ret = esp_bt_controller_deinit();
    if (ret) {
        ESP_LOGE(TAG, "deinit controller failed");
        return ret;
    }

    return ESP_OK;
}




// void hid_demo_task(void *pvParameters) {
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//     while(1) {
//         vTaskDelay(2000 / portTICK_PERIOD_MS);
//         if (sec_conn) {
//             ESP_LOGI(TAG, "Send the volume");
//             send_volum_up = true;
//             uint8_t key_vaule = {HID_KEY_A};
//             esp_hidd_send_keyboard_value(hid_conn_id, 0, &key_vaule, 1);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//             key_vaule = 0;
//             esp_hidd_send_keyboard_value(hid_conn_id, 0, &key_vaule, 1);
//             // esp_hidd_send_consumer_value(hid_conn_id, HID_CONSUMER_VOLUME_UP, true);
//             vTaskDelay(3000 / portTICK_PERIOD_MS);
//             // if (send_volum_up) {
//             //     send_volum_up = false;
//             //     esp_hidd_send_consumer_value(hid_conn_id, HID_CONSUMER_VOLUME_UP, false);
//             //     esp_hidd_send_consumer_value(hid_conn_id, HID_CONSUMER_VOLUME_DOWN, true);
//             //     vTaskDelay(3000 / portTICK_PERIOD_MS);
//             //     esp_hidd_send_consumer_value(hid_conn_id, HID_CONSUMER_VOLUME_DOWN, false);
//             // }
//         }
//     }
// }