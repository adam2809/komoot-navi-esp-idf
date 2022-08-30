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


#include "esp_log.h"

#include "komoot_ble_client.h"
#include "display.h"
#include "mpu6050.h"
#include "button.h"
#include "morse.h"
#include "Statechart.h"

#define TAG "MAIN"
#define BUTTONS_BITMASK (PIN_BIT(CONFIG_LEFT_BUTTON_PIN) | PIN_BIT(CONFIG_RIGHT_BUTTON_PIN)) 
#define LV_TICK_PERIOD_MS 1

void wakeup();

QueueHandle_t button_events = NULL;
morse_input_params_t morse_input_params;
Statechart statechart;

void raise_button_events_on_click(void *pvParameter);

void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    gpio_init();
    button_events = button_init(BUTTONS_BITMASK,false);

    xTaskCreatePinnedToCore(display_task, "display_task", 4096*2, NULL, 0, &statechart.display_task_handle, 1);
    xTaskCreate(&raise_button_events_on_click, "raise_button_events_on_click", 4098, NULL, 5, NULL);

    configure_mpu();

    statechart_init(&statechart);
    statechart_enter(&statechart);
}

void raise_button_events_on_click(void *pvParameter){
    button_event_t ev;
    while(true){
        if (xQueueReceive(button_events, &ev, 100/portTICK_PERIOD_MS) && ev.event == BUTTON_UP) {
            if(ev.pin == CONFIG_LEFT_BUTTON_PIN){
                ESP_LOGI(TAG,"Got left");
                statechart_raise_left_button_clicked(&statechart);
            }else if(ev.pin == CONFIG_RIGHT_BUTTON_PIN){
                ESP_LOGI(TAG,"Got right");
                statechart_raise_right_button_clicked(&statechart);
            }else{
                ESP_LOGI(TAG,"Undefined button pin");
            }
        }
    }
}

void gpio_init(){
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL<<GPIO_NUM_25 | 1ULL<<GPIO_NUM_4  | 1ULL<<CONFIG_BUZZER_PIN;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);
    gpio_set_level(GPIO_NUM_4, 0);
    gpio_set_level(GPIO_NUM_25, 1);
    gpio_set_level(CONFIG_BUZZER_PIN, 0);
}
