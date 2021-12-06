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
#define LV_TICK_PERIOD_MS 1
#define BUTTON_PIN GPIO_NUM_35

uint32_t curr_passkey=123456;
struct nav_data_t curr_nav_data = {0,0,{0}};
TaskHandle_t display_nav_task_handle = NULL;

#define LV_TICK_PERIOD_MS 1

void display_task(void *pvParameter);
static void lv_tick_task(void *arg);
void  display_task_new(void *pvParameter);
void poll_mtu_task_queue_task(void *pvParameter);
void alarm_task(void *pvParameter);
void morse_password_input_task(void *pvParameter);



void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    // init_komoot_ble_client(&curr_passkey,&curr_nav_data,&display_nav_task_handle);
    
    // xTaskCreatePinnedToCore(display_task_new, "display_task", 4096*2, NULL, 0, &display_nav_task_handle, 1);
    // xTaskCreate(&alarm_task, "alarm_task", 4098, NULL, 5, NULL);
    xTaskCreate(&morse_password_input_task, "morse_password_input_task", 4098, NULL, 5, NULL);
    // xTaskCreate(&poll_mtu_task_queue_task, "poll_mtu_task_queue_task", 4098, NULL, 5, NULL);
}


SemaphoreHandle_t xGuiSemaphore;

void  display_task_new(void *pvParameter){
    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();
    
    lv_color_t* buf=NULL;
    init_lvgl_display(buf);
    
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
    
    init_lvgl_objs();

    ESP_LOGI(GATTC_TAG,"Init lvgl done");

    uint32_t ulNotifiedValue;
    while (1) {
        xTaskNotifyWait(
            0x00,      /* Don't clear any notification bits on entry. */
            ULONG_MAX, /* Reset the notification value to 0 on exit. */
            &ulNotifiedValue, /* Notified value pass out in ulNotifiedValue. */
            portMAX_DELAY
        );
        ESP_LOGI(GATTC_TAG,"Display task got notification with value %d",ulNotifiedValue);

        if(ulNotifiedValue == NOTIFY_VALUE_NAVIGATION){
            if(curr_nav_data.direction!=13&&curr_nav_data.direction!=14&&curr_nav_data.direction!=31){
                display_dir_symbol(curr_nav_data.direction);
            }
            
            display_meters(curr_nav_data.distance);
        }else if(ulNotifiedValue == NOTIFY_VALUE_PASSKEY){
            display_passkey(curr_passkey);
        }

        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            ESP_LOGD(NAV_TAG,"Calling task handler");
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
       }
    }

    free(buf);
    vTaskDelete(NULL);
}

void poll_mtu_task_queue_task(void *pvParameter){    
    init_mpu_interrupt();

    gpio_num_t interrupt_gpio = MPU6050_INTERRUPT_INPUT_PIN;
    while(1){
        if(xQueueReceive(mpu_event_queue,&interrupt_gpio, portMAX_DELAY)) {
            ESP_LOGI(NAV_TAG,"Got interrpupt from mtu");
        }
    }
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void go_to_deep_sleep(){
    if(rtc_gpio_pullup_en(MPU6050_INTERRUPT_INPUT_PIN)){
        ESP_LOGE(NAV_TAG,"Could not pull up gpio %d",MPU6050_INTERRUPT_INPUT_PIN);
    }
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_13,0);
    esp_deep_sleep_start();
}

void alarm_task(void *pvParameter){
    vTaskDelay(100/portTICK_PERIOD_MS);
    
    configure_mpu(10);

    uint8_t read_data[6];
    val_3d accel_val;
    while(1){
        vTaskDelay(100/portTICK_PERIOD_MS);

        read_3d_reg_value(&accel_val,MPU6050_ACCEL_XOUT_H,read_data);
		ESP_LOGI(NAV_TAG, "x: %d, y: %d, z: %d", accel_val.x, accel_val.y, accel_val.z);
    }

    vTaskDelete(NULL);
}

void morse_password_input_task(void *pvParameter){
    button_event_t ev;
    QueueHandle_t button_events = button_init(PIN_BIT(BUTTON_PIN));
    char morse_password[MAX_PASSWORD_LENGTH] = {'\0'};
    uint8_t morse_password_len = 0;
    uint8_t morse_char = 0;
    uint8_t morse_char_len = 0;
    int64_t last_char_input_time = 0;
    char was_held_flag = false;
    
    while (true) {
        if (esp_timer_get_time() / 1000ULL - last_char_input_time > TIME_BETWEEN_LETTERS && morse_char_len != 0){
            morse_password[morse_password_len] = bin_morse_2_char(morse_char,morse_char_len);
            ESP_LOGI(NAV_TAG,"Morse password is %s", morse_password);
            morse_password_len++;
            morse_char_len = 0;
            morse_char = 0;
        }

        if (esp_timer_get_time() / 1000ULL - last_char_input_time > TIME_BETWEEN_WORDS){
            ESP_LOGI(NAV_TAG,"Morse password input finished");
            break;
        }
        
        if (xQueueReceive(button_events, &ev, 100/portTICK_PERIOD_MS)) {
            if ((ev.pin == BUTTON_PIN) && (ev.event == BUTTON_HELD)) {
                was_held_flag = true;
            }
            if ((ev.pin == BUTTON_PIN) && (ev.event == BUTTON_UP)) {
                if(was_held_flag){
                    ESP_LOGI(NAV_TAG,"Got long");
                }else{
                    ESP_LOGI(NAV_TAG,"Got short");
                }
                morse_char |= was_held_flag << morse_char_len;
                morse_char_len++;
                was_held_flag = false;
                last_char_input_time = esp_timer_get_time() / 1000ULL;
                ESP_LOGI(NAV_TAG,"Morse char is %c", bin_morse_2_char(morse_char,morse_char_len));
            }
        }
    }
    vTaskDelete(NULL);
}