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


#define NAV_TAG "NAVIGATION_DISPLAY"

#define LV_TICK_PERIOD_MS 1

uint32_t curr_passkey=123456;
struct nav_data_t curr_nav_data = {0,543,{0}};

TaskHandle_t display_nav_task_handle = NULL;




#define LV_TICK_PERIOD_MS 1

void display_task(void *pvParameter);
static void lv_tick_task(void *arg);
void  display_task_new(void *pvParameter);
void test_task(void *pvParameter);



void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    // init_komoot_ble_client(&curr_passkey,&curr_nav_data,&display_nav_task_handle);
    
    xTaskCreatePinnedToCore(display_task_new, "display_task", 4096*2, NULL, 0, &display_nav_task_handle, 1);
    xTaskCreate(&test_task, "test_task", 4098, NULL, 5, NULL);
}


void display_task(void *pvParameter){
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
            display_passkey(curr_passkey);
        }
    }
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
            if(curr_nav_data.direction!=0){
                display_nav_symbol(curr_nav_data.direction);
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

void test_task(void *pvParameter){
    bool flag = false;
    while(1){
        vTaskDelay(1000/portTICK_PERIOD_MS);
        
        xTaskNotify(
            display_nav_task_handle,
            flag ? NOTIFY_VALUE_NAVIGATION : NOTIFY_VALUE_PASSKEY,
            eSetValueWithOverwrite
        );
        ESP_LOGI(GATTC_TAG,"Sending notification with value %d",flag ? NOTIFY_VALUE_NAVIGATION : NOTIFY_VALUE_PASSKEY);

        flag = !flag;
    }
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
