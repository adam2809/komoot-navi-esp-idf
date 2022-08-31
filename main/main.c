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

#include <driver/gpio.h>
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
static QueueHandle_t monitor_mpu_interrupt_q;

void raise_button_events_on_click(void *pvParameter);
void monitor_mpu_interrupt(void *ignore);

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
    xTaskCreate(raise_button_events_on_click, "raise_button_events_on_click", 4098, NULL, 5, NULL);
    xTaskCreate(monitor_mpu_interrupt, "monitor_mpu_interrupt", 4098, NULL, 5, NULL);

    configure_mpu();

    statechart_init(&statechart);
    statechart_enter(&statechart);
}

bool is_held = false;
bool is_left_down = false;
bool is_right_down= false;
void raise_button_events_on_click(void *pvParameter){
    button_event_t ev;
    while(true){
        if (xQueueReceive(button_events, &ev, 100/portTICK_PERIOD_MS)) {
            if(ev.event == BUTTON_DOWN){
                if(ev.pin == CONFIG_LEFT_BUTTON_PIN){
                    ESP_LOGD(TAG,"Left button down event");
                    is_left_down = true;
                }else if(ev.pin == CONFIG_RIGHT_BUTTON_PIN){
                    ESP_LOGD(TAG,"Right button down event");
                    is_right_down = true;
                }else{
                    ESP_LOGD(TAG,"Undefined button pin");
                }
            }else if(ev.event == BUTTON_HELD && !is_held){
                ESP_LOGI(TAG,"Either button hold event");
                statechart_raise_double_button_clicked(&statechart);
                is_held = true;
            }else if(ev.event == BUTTON_UP){
                if(is_left_down && !is_held){
                    ESP_LOGD(TAG,"Right button up but not held event");
                    statechart_raise_left_button_clicked(&statechart);
                }
                if(is_right_down && !is_held){
                    ESP_LOGD(TAG,"Right button up not but held event");
                    statechart_raise_right_button_clicked(&statechart);
                }
                is_held = false;
                is_left_down = false;
                is_right_down = false;
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

static void handler(void *args) {
    gpio_num_t gpio;
    gpio = CONFIG_MPU6050_INTERRUPT_INPUT_PIN;
    xQueueSendToBackFromISR(monitor_mpu_interrupt_q, &gpio, NULL);
}

void monitor_mpu_interrupt(void *ignore) {
    gpio_num_t gpio;
    monitor_mpu_interrupt_q = xQueueCreate(10, sizeof(gpio_num_t));

    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = 1ULL << CONFIG_MPU6050_INTERRUPT_INPUT_PIN;
    gpioConfig.mode         = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en   = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpioConfig.intr_type    = GPIO_INTR_POSEDGE;
    gpio_config(&gpioConfig);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(CONFIG_MPU6050_INTERRUPT_INPUT_PIN, handler, NULL);
    while(1) {
        ESP_LOGD(TAG, "Waiting on mpu interrupt");
        BaseType_t rc = xQueueReceive(monitor_mpu_interrupt_q, &gpio, portMAX_DELAY);
        ESP_LOGI(TAG, "Got mpu interrupt: %d", rc);
        statechart_raise_motion_detected(&statechart);
    }
    vTaskDelete(NULL);
}
