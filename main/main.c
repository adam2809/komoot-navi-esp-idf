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
#include "freertos/semphr.h"

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
#include "display.h"
#include "mpu6050.h"
#include "button.h"
#include "morse.h"

#define TAG "MAIN"
#define BUTTONS_BITMASK PIN_BIT(CONFIG_BUTTON_PIN)
#define LV_TICK_PERIOD_MS 1

TaskHandle_t display_task_handle = NULL;
QueueHandle_t button_events = NULL;


void poll_mtu_event_queue_task(void *pvParameter);
void go_to_deep_sleep();
void react_to_wakeup_reason();

void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    button_events = button_init(BUTTONS_BITMASK,false);

    xTaskCreatePinnedToCore(display_task, "display_task", 4096*2, NULL, 0, &display_task_handle, 1);
    
    alarm_wakeup(&button_events,&display_task_handle);
    // init_komoot_ble_client(&display_task_handle);
    configure_mpu();
}
