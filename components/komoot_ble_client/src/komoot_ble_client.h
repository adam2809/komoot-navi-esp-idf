#pragma once

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "display.h"

#define PROFILE_NUM      1
#define PROFILE_A_APP_ID 0
#define INVALID_HANDLE   0
#define SCAN_DURATION 30
#define EXPECTED_ADV_DATA_LEN 21
#define GATTC_TAG "GATTC_DEMO"


struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bd_addr_t remote_bda;
};



struct nav_data_t{
    uint8_t direction;
    uint32_t distance;
    char street[100];
};

esp_err_t init_komoot_ble_client(TaskHandle_t* _display_nav_task_handle_pointer);
void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
void resolve_nav_data(uint8_t* data,struct nav_data_t target);
char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req);
const char *esp_key_type_to_str(esp_ble_key_type_t key_type);
uint8_t* resolve_service_from_adv_data(uint8_t* adv_data,uint8_t adv_data_length);

uint32_t get_passkey();
struct nav_data_t* get_nav_data();