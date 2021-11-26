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
#include "ssd1306.h"
#include "symbols.h"

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

SSD1306_t disp;
uint32_t curr_passkey=123456;
struct nav_data_t curr_nav_data = {0,0,{0}};

TaskHandle_t display_nav_task_handle = NULL;

void write_number_icon(uint8_t dest[MAX_PAGE_COUNT*NUMBER_WIDTH],uint8_t icon[NUMBER_PAGE_COUNT*NUMBER_WIDTH],int display_pixel,int number_pixel){
	int number_page = number_pixel/8;
	int display_page = display_pixel/8;

	// Write pixels from start pixel to the end of current page
	if(display_pixel%8!=0){
		int pixels_to_full_page_display = 8-(display_pixel%8);
		int pixels_to_full_page_number = 8-(number_pixel%8);


		for(int seg=0;seg<NUMBER_WIDTH;seg++){

			uint8_t old_right = (dest[display_page*NUMBER_WIDTH+seg] << pixels_to_full_page_display) >> pixels_to_full_page_display;
			uint8_t new_left;

			if(pixels_to_full_page_number < pixels_to_full_page_display){
				uint8_t curr_right = icon[number_page*NUMBER_WIDTH+seg] >> number_pixel%8;
				uint8_t next_left = icon[(number_page+1)*NUMBER_WIDTH+seg] << pixels_to_full_page_number;

				new_left = (next_left | curr_right)<< display_pixel%8;
			}else{
				new_left = (icon[number_page*NUMBER_WIDTH+seg] >> number_pixel%8) << (number_pixel%8+(pixels_to_full_page_number-pixels_to_full_page_display));
			}

			dest[display_page*NUMBER_WIDTH*seg] = new_left | old_right;
		}

		number_pixel+=pixels_to_full_page_display;
		display_pixel+=pixels_to_full_page_display;
		number_page = number_pixel/8;
		display_page = display_pixel/8;
	}

	// Write full pages
	while (number_pixel < NUMBER_PAGE_COUNT*8-8+1){
		for(int seg=0;seg<NUMBER_WIDTH;seg++){
			int pixels_to_full_page_number = 8-(number_pixel%8);

			uint8_t new_display_page;
			if(pixels_to_full_page_number < 8){
				uint8_t curr_right = icon[number_page*NUMBER_WIDTH+seg] >> number_pixel%8;
				uint8_t next_left = icon[(number_page+1)*NUMBER_WIDTH+seg] << pixels_to_full_page_number;

				new_display_page = next_left | curr_right;
			}else{
				new_display_page = icon[number_page*NUMBER_WIDTH+seg];
			}

			dest[display_page*NUMBER_WIDTH+seg] = new_display_page;
		}

		number_pixel+=8;
		display_pixel+=8;
		number_page = number_pixel/8;
		display_page = display_pixel/8;
	}

	// Write what is left of last page of number on the beginning of next display page
	if (number_pixel < NUMBER_PAGE_COUNT*8){
		for(int seg=0;seg<NUMBER_WIDTH;seg++){
			int pixels_to_end = NUMBER_PAGE_COUNT*8 - number_pixel;
			int pixels_from_last_page = 8 - pixels_to_end;

			uint8_t old_left = (dest[display_page*NUMBER_WIDTH+seg] >> pixels_to_end) << pixels_to_end;
			uint8_t new_right = icon[number_page*NUMBER_WIDTH+seg] >> pixels_from_last_page;

			dest[display_page*NUMBER_WIDTH+seg] = old_left | new_right;
		}
	}
}


void display_numbers(uint8_t numbers[MAX_PAGE_COUNT*NUMBER_WIDTH],int seg){
	display_partial_image(&disp,numbers,0,MAX_PAGE_COUNT,seg,NUMBER_WIDTH);
}
void display_nav_symbol(uint8_t symbol[MAX_PAGE_COUNT*MANEUVER_WIDTH]){
    display_partial_image(&disp,symbol,0,8,0,64);
}

void write_digits_to_buffer(uint32_t number,uint8_t buffer[MAX_PAGE_COUNT*NUMBER_WIDTH]){
	int curr_pixel = 0;
	while(number != 0){
		uint32_t digit = number%10;
        ESP_LOGD(GATTC_TAG,"Writing num");

		write_number_icon(buffer,numbers[digit],curr_pixel,3);

		number/=10;
		curr_pixel+=21;
	}
}

void display_meters(uint32_t meters){
	char meters_str[10];
	sprintf(meters_str,"%dm",meters);
	ESP_LOGI(GATTC_TAG,"Displaying %d meters",meters);
	uint8_t meters_display[MAX_PAGE_COUNT*NUMBER_WIDTH] = {0};

	if(meters == 0){	
		write_number_icon(meters_display,numbers[0],0,3);
		display_numbers(meters_display,METER_DISPLAY_SEG);
	}else if(meters >= 1000){	
		write_number_icon(meters_display,greater_than,42,3);
		write_number_icon(meters_display,numbers[1],21,3);
		write_number_icon(meters_display,km,0,3);
		display_numbers(meters_display,METER_DISPLAY_SEG);
	}else{
        write_digits_to_buffer(meters,meters_display);
    }

	display_numbers(meters_display,METER_DISPLAY_SEG);
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
                display_nav_symbol(nav_symbols[curr_nav_data.direction]);
            }
            
            display_meters(curr_nav_data.distance);
        }else if(ulNotifiedValue == NOTIFY_VALUE_PASSKEY){
	        uint8_t passkey_display[MAX_PAGE_COUNT*NUMBER_WIDTH] = {0};
            write_digits_to_buffer(curr_passkey/1000,passkey_display);

            display_numbers(passkey_display,PASSKEY_LINE_1_DISPLAY_SEG);

            memset(passkey_display,0,MAX_PAGE_COUNT*NUMBER_WIDTH);
            write_digits_to_buffer(curr_passkey%1000,passkey_display);
            display_numbers(passkey_display,PASSKEY_LINE_2_DISPLAY_SEG);
        }
    }
}


void clear_display(){
	ssd1306_clear_screen(&disp, false);
	ssd1306_contrast(&disp, 0xff);
}

void config_display(){
	#if CONFIG_I2C_INTERFACE
		ESP_LOGI(NAV_TAG, "INTERFACE is i2c");
		ESP_LOGI(NAV_TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
		ESP_LOGI(NAV_TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
		ESP_LOGI(NAV_TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
		i2c_master_init(&disp, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
	#endif // CONFIG_I2C_INTERFACE

	#if CONFIG_SPI_INTERFACE
		ESP_LOGI(tag, "INTERFACE is SPI");
		ESP_LOGI(tag, "CONFIG_MOSI_GPIO=%d",CONFIG_MOSI_GPIO);
		ESP_LOGI(tag, "CONFIG_SCLK_GPIO=%d",CONFIG_SCLK_GPIO);
		ESP_LOGI(tag, "CONFIG_CS_GPIO=%d",CONFIG_CS_GPIO);
		ESP_LOGI(tag, "CONFIG_DC_GPIO=%d",CONFIG_DC_GPIO);
		ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
		spi_master_init(&disp, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO);
	#endif // CONFIG_SPI_INTERFACE

	#if CONFIG_FLIP
		disp._flip = true;
		ESP_LOGW(tag, "Flip upside down");
	#endif

	#if CONFIG_SSD1306_128x64
		ESP_LOGI(NAV_TAG, "Panel is 128x64");
		ssd1306_init(&disp, 128, 64);
	#endif // CONFIG_SSD1306_128x64
	#if CONFIG_SSD1306_128x32
		ESP_LOGI(tag, "Panel is 128x32");
		ssd1306_init(&disp, 128, 32);
	#endif // CONFIG_SSD1306_128x32

	clear_display();
}

void  test_task(void *pvParameter){
    bool flag = true;
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
 

void app_main(void){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    init_komoot_ble_client(&curr_passkey,&curr_nav_data,&display_nav_task_handle);
    
    config_display();
    xTaskCreate(&display_nav_task, "display_nav_task", 4098, NULL, 5, &display_nav_task_handle);
}