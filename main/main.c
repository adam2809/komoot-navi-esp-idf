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

#include "digit_imgs.h"



#define NAV_TAG "NAVIGATION_DISPLAY"

#define LV_TICK_PERIOD_MS 1
#define DIGITS_IN_ROW_COUNT 3

uint32_t curr_passkey=123456;
struct nav_data_t curr_nav_data = {0,0,{0}};

TaskHandle_t display_nav_task_handle = NULL;

lv_obj_t * digit_row_top[DIGITS_IN_ROW_COUNT] = {NULL};
lv_obj_t * digit_row_bottom[DIGITS_IN_ROW_COUNT] = {NULL};


#define LV_TICK_PERIOD_MS 1

void display_task(void *pvParameter);
static void init_lvgl_display();
static void init_lvgl_objs();
static void lv_tick_task(void *arg);
void  display_task_new(void *pvParameter);


void display_numbers(uint8_t numbers[0],int seg){
}
void display_nav_symbol(uint8_t symbol[0]){
}

void display_meters(uint32_t meters){
}


void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    init_komoot_ble_client(&curr_passkey,&curr_nav_data,&display_nav_task_handle);
    
    xTaskCreatePinnedToCore(display_task_new, "display_task", 4096*2, NULL, 0, NULL, 1);
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
            // display passkey
        }
    }
}

SemaphoreHandle_t xGuiSemaphore;

void  display_task_new(void *pvParameter){
    (void) pvParameter;
    lv_color_t* buf=NULL;
    init_lvgl_display(buf);
    init_lvgl_objs();

    while (1) {
        // int counter = 0;

        // /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        // vTaskDelay(pdMS_TO_TICKS(10));

        // counter%=1000;
        // ESP_LOGI(NAV_TAG,"Counter: %d",counter);
        // lv_img_set_src(digit_row_top[2], digits[counter%10]);
        // if(counter > 9){
        //   lv_img_set_src(digit_row_top[1], digits[(counter%100)/10]);
        // }else{
        //   lv_img_set_src(digit_row_top[1], digits[0]);
        // }
        // if(counter > 99){
        //   lv_img_set_src(digit_row_top[0], digits[counter/100]);
        // }else{
        //   lv_img_set_src(digit_row_top[0], digits[0]);
        // }
        // counter++;
        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
       }
    }

    free(buf);
    vTaskDelete(NULL);
}

static void init_lvgl_display(lv_color_t* buf) {
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    buf = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf != NULL);

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE*8;

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf, NULL, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
}

static void init_lvgl_objs(){
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);

    for(int i = 0;i < DIGITS_IN_ROW_COUNT;i++){
        if(digit_row_bottom[i] != NULL || digit_row_top[i] != NULL){
            ESP_LOGI(NAV_TAG,"DUPADUASODJIASDJIA");
        }
        digit_row_top[i] = lv_img_create(scr,NULL);
        digit_row_bottom[i] = lv_img_create(scr,NULL);
        
        lv_img_set_src(digit_row_top[i],digits[i]);
        lv_img_set_src(digit_row_bottom[i],digits[i+4]);

        
    }

    lv_obj_align(digit_row_top[0], NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_TOP_X_OFFSET,DIGIT_1_Y_OFFSET);
    lv_obj_align(digit_row_top[1], NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_TOP_X_OFFSET, DIGIT_2_Y_OFFSET);
    lv_obj_align(digit_row_top[2], NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_TOP_X_OFFSET, DIGIT_3_Y_OFFSET);

    lv_obj_align(digit_row_bottom[0], NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_BOTTOM_X_OFFSET,DIGIT_1_Y_OFFSET);
    lv_obj_align(digit_row_bottom[1], NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_BOTTOM_X_OFFSET, DIGIT_2_Y_OFFSET);
    lv_obj_align(digit_row_bottom[2], NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_BOTTOM_X_OFFSET, DIGIT_3_Y_OFFSET);
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
