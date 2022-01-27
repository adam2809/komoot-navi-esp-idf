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

#include "driver/rtc_io.h"
#include "esp_sleep.h"

#include "esp_log.h"

#include "komoot_ble_client.h"
#include "display.h"
#include "mpu6050.h"
#include "button.h"
#include "morse.h"

#define NAV_TAG "NAVIGATION_DISPLAY"
#define BUTTON_PIN GPIO_NUM_35
#define BUTTONS_BITMASK PIN_BIT(BUTTON_PIN)
#define LV_TICK_PERIOD_MS 1

uint32_t curr_passkey=123456;
struct nav_data_t curr_nav_data = {0,0,{0}};
TaskHandle_t display_nav_task_handle = NULL;
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

    xTaskCreatePinnedToCore(display_task, "display_task", 4096*2, NULL, 0, &display_nav_task_handle, 1);

    react_to_wakeup_reason();
    // init_komoot_ble_client(&curr_passkey,&curr_nav_data,&display_nav_task_handle);

    // xTaskCreate(&poll_mtu_event_queue_task, "poll_mtu_event_queue_task", 4098, NULL, 5, NULL);
}

void react_to_wakeup_reason(){
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_EXT1: {
            uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
            if (wakeup_pin_mask != 0) {
                int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
                ESP_LOGI(GATTC_TAG,"Wake up from GPIO %d", pin);
                if(pin != BUTTON_PIN){
                    ESP_LOGI(GATTC_TAG,"Turning on alarm");
                }
                xTaskCreate(&morse_password_input_task, "morse_password_input_task", 4098, (void*) button_events, 5, NULL);
            } else {
                ESP_LOGW(GATTC_TAG,"Could not get wakeup pin number");
            }
            break;
        }
        case ESP_SLEEP_WAKEUP_EXT0: {
            ESP_LOGI(GATTC_TAG,"Wake up from EXT0");
            break;
        }
        case ESP_SLEEP_WAKEUP_UNDEFINED:{
            xTaskCreate(&alarm_enable_task, "alarm_enable_task", 4098, NULL, 5, NULL);
            ESP_LOGI(GATTC_TAG,"Undefined wakeup reason");
            break;
        }
        default:
            ESP_LOGI(GATTC_TAG,"Not a deep sleep reset");
    }
}





void poll_mtu_event_queue_task(void *pvParameter){  
    configure_mpu(MOTION_DETECTION_SENSITIVITY);  
    init_mpu_interrupt();

    gpio_num_t interrupt_gpio = MPU6050_INTERRUPT_INPUT_PIN;
    while(1){
        if(xQueueReceive(mpu_event_queue,&interrupt_gpio, portMAX_DELAY)) {
            ESP_LOGI(NAV_TAG,"Got interrpupt from mtu");
        }
    }
}


void go_to_deep_sleep(){
    rtc_gpio_hold_en(MPU6050_INTERRUPT_INPUT_PIN);
    ESP_LOGI(NAV_TAG,"Going to deep sleep");
    esp_sleep_enable_ext1_wakeup(PIN_BIT(MPU6050_INTERRUPT_INPUT_PIN)|PIN_BIT(BUTTON_PIN),ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_deep_sleep_start();
}

