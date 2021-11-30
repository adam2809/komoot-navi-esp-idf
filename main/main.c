/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/



/****************************************************************************
*
* This demo showcases BLE GATT client. It can scan BLE devices and connect to one device.
* Run the gatt_server demo, the client demo will automatically connect to the gatt_server demo.
* Client demo will enable gatt_server's notify after connection. The two devices will then exchange
* data.
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"

#include "komoot_ble_client.h"

#define NAV_TAG "NAVIGATION_DISPLAY"


#define MAX_PAGE_COUNT 8
#define MAX_WIDTH 128
#define NUMBER_PAGE_COUNT 3
#define NUMBER_WIDTH 30
#define NUMBER_IMAGE_WIDTH 21

#define MANEUVER_PAGE_COUNT 8
#define MANEUVER_WIDTH 64
#define METER_DISPLAY_SEG 70
#define PASSKEY_LINE_1_DISPLAY_SEG 35
#define PASSKEY_LINE_2_DISPLAY_SEG 70

uint32_t curr_passkey=123456;
struct nav_data_t curr_nav_data = {0,0,{0}};

TaskHandle_t display_nav_task_handle = NULL;

void display_numbers(uint8_t numbers[MAX_PAGE_COUNT*NUMBER_WIDTH],int seg){
}
void display_nav_symbol(uint8_t symbol[MAX_PAGE_COUNT*MANEUVER_WIDTH]){
}

void display_meters(uint32_t meters){
}


void  display_nav_task(void *pvParameter){
    uint32_t ulNotifiedValue;
    while(1){
        xTaskNotifyWait(
            0x00,      /* Don't clear any notification bits on entry. */
            ULONG_MAX, /* Reset the notification value to 0 on exit. */
            &ulNotifiedValue, /* Notified value pass out in ulNotifiedValue. */
            portMAX_DELAY
        );
        ESP_LOGI(GATTC_TAG,"Display task got notification with value %d",ulNotifiedValue);

        if(ulNotifiedValue == NOTIFY_VALUE_NAVIGATION){
            if(curr_nav_data.direction!=0){
                // display_nav_symbol(nav_symbols[curr_nav_data.direction]);
            }
            
            display_meters(curr_nav_data.distance);
        }else if(ulNotifiedValue == NOTIFY_VALUE_PASSKEY){
            // display passkey
        }
    }
}
void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    init_komoot_ble_client(&curr_passkey,&curr_nav_data,&display_nav_task_handle);
    
    xTaskCreate(&display_nav_task, "display_nav_task", 4098, NULL, 5, &display_nav_task_handle);
}