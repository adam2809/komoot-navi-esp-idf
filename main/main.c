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
#define BUTTONS_BITMASK (PIN_BIT(CONFIG_LEFT_BUTTON_PIN) | PIN_BIT(CONFIG_RIGHT_BUTTON_PIN)) 
#define LV_TICK_PERIOD_MS 1

void wakeup();


TaskHandle_t display_task_handle = NULL;
QueueHandle_t button_events = NULL;
morse_input_params_t morse_input_params;

void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    button_events = button_init(BUTTONS_BITMASK,false);

    xTaskCreatePinnedToCore(display_task, "display_task", 4096*2, NULL, 0, &display_task_handle, 1);
    
    // init_komoot_ble_client(&display_task_handle);
    configure_mpu();
    wakeup(&button_events,&display_task_handle);
}

void wakeup(){
    morse_input_params.buttons_events = &button_events;
    morse_input_params.display_task_handle = &display_task_handle;
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_EXT1: {
            uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
            if (wakeup_pin_mask != 0) {
                int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
                ESP_LOGI(TAG,"Wake up from GPIO %d", pin);
                if(pin == CONFIG_LEFT_BUTTON_PIN){
                    if (get_lock_state()){
                        xTaskCreate(&morse_password_input_task, "morse_password_input_task", 4098, (void*) &morse_input_params, 5, NULL);
                    }else{                        
                        xTaskCreate(&lock, "lock_task", 4098, NULL, 5, NULL);
                    }
                }else if(pin == CONFIG_MPU6050_INTERRUPT_INPUT_PIN && get_lock_state()){
                    raise_alarm_state();
                }
            } else {
                ESP_LOGW(TAG,"Could not get wakeup pin number");
            }
            break;
        }
        case ESP_SLEEP_WAKEUP_EXT0: {
            ESP_LOGI(TAG,"Wake up from EXT0");
            break;
        }
        case ESP_SLEEP_WAKEUP_UNDEFINED:{
            ESP_LOGI(TAG,"Undefined wakeup reason");
            init_lock_state();
            go_to_deep_sleep(false);
            break;
        }
        default:
            ESP_LOGI(TAG,"Not a deep sleep reset");
    }
}
