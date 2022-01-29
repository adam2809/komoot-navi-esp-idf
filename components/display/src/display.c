#include "display.h"

#define TAG "DISPLAY"

lv_obj_t* nav_scr;
lv_obj_t* passkey_scr;
lv_obj_t* morse_input_scr; 
lv_obj_t* alarm_notifs_scr; 

lv_obj_t * passkey_digits_row_top[DIGITS_IN_ROW_COUNT] = {NULL};
lv_obj_t * passkey_digits_row_bottom[DIGITS_IN_ROW_COUNT] = {NULL};
lv_obj_t * morse_bin_label = NULL;
lv_obj_t * morse_password_label = NULL;

lv_obj_t * meters_digits[DIGITS_IN_ROW_COUNT] = {NULL};
lv_obj_t * dir_symbol = NULL;
lv_obj_t * alarm_symbol = NULL;

void init_lvgl_objs();
void display_passkey(uint32_t passkey);
void display_dir_symbol(uint8_t symbol);
void display_meters(uint32_t meters);
void display_morse(uint8_t bin_morse,uint8_t len,char* password);

void init_lvgl_display(lv_color_t* buf) {
    lvgl_i2c_locking(i2c_manager_locking());

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
}

void init_lvgl_objs(){
    passkey_scr  = lv_obj_create(NULL, NULL);
    nav_scr  = lv_obj_create(NULL, NULL);
    morse_input_scr = lv_obj_create(NULL, NULL);

    for(int i = 0;i < DIGITS_IN_ROW_COUNT;i++){
        passkey_digits_row_top[i] = lv_img_create(passkey_scr,NULL);
        passkey_digits_row_bottom[i] = lv_img_create(passkey_scr,NULL);
        meters_digits[i] = lv_img_create(nav_scr,NULL);
        
        lv_img_set_src(passkey_digits_row_top[i],digits[0]);
        lv_img_set_src(passkey_digits_row_bottom[i],digits[0]);
        lv_img_set_src(meters_digits[i],digits[0]);

        lv_obj_align(passkey_digits_row_top[i], NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_TOP_X_OFFSET,DIGIT_1_Y_OFFSET-DIGIT_Y_SPACING*i);
        lv_obj_align(passkey_digits_row_bottom[i], NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_BOTTOM_X_OFFSET,DIGIT_1_Y_OFFSET-DIGIT_Y_SPACING*i);
        lv_obj_align(meters_digits[i], NULL, LV_ALIGN_IN_BOTTOM_LEFT,DIGITS_ROW_BOTTOM_X_OFFSET,DIGIT_1_Y_OFFSET-DIGIT_Y_SPACING*i);
    }

    dir_symbol = lv_img_create(nav_scr,NULL);

    morse_bin_label = lv_label_create(morse_input_scr,NULL);
    morse_password_label = lv_label_create(morse_input_scr,NULL);

    lv_obj_align(morse_bin_label, NULL, LV_ALIGN_CENTER,0,0);
    lv_obj_align(morse_password_label, NULL, LV_ALIGN_IN_BOTTOM_MID,0,0);

    lv_label_set_text(morse_bin_label,"");
    lv_label_set_text(morse_password_label,"");

    alarm_symbol = lv_img_create(alarm_notifs_scr,NULL);
    lv_obj_align(alarm_symbol, NULL, LV_ALIGN_CENTER,0,0);

    lv_scr_load(lv_obj_create(NULL, NULL));
}

void display_number_row(uint32_t row_content,lv_obj_t* row[]){
    lv_img_set_src(row[2], digits[row_content%10]);
    if(row_content > 9){
        lv_img_set_src(row[1], digits[(row_content%100)/10]);
    }else{
        lv_img_set_src(row[1], digits[0]);
    }
    if(row_content > 99){
        lv_img_set_src(row[0], digits[row_content/100]);
    }else{
        lv_img_set_src(row[0], digits[0]);
    }
}

void display_passkey(uint32_t passkey){
    ESP_LOGI(TAG,"Displaing passkey: %d",passkey);
    lv_scr_load(passkey_scr);

    display_number_row(passkey/1000,passkey_digits_row_top);
    display_number_row(passkey%1000,passkey_digits_row_bottom);


}

void display_dir_symbol(uint8_t symbol){
    lv_img_set_src(dir_symbol,dir_symbols_imgs[symbol]);
}

void display_meters(uint32_t meters){
    ESP_LOGI(TAG,"Displaing meters: %d",meters);
    lv_scr_load(nav_scr);

    if(meters < 1000){
        display_number_row(meters < 1000 ? meters : 999,meters_digits);
    }else{
        lv_img_set_src(meters_digits[0],&greater_than);
        lv_img_set_src(meters_digits[1],digits[1]);
        lv_img_set_src(meters_digits[2],&km);
    }
}

void display_morse(uint8_t bin_morse,uint8_t len,char* password){
    ESP_LOGI(TAG,"Displaing morse");

    lv_scr_load(morse_input_scr);

    char bit_str_rep[MAX_BITS_IN_LETTER+2] = {'\0'};
    bit_str_rep[0] = bin_morse_2_char(bin_morse,len);
    for (int i = 0; i < len; i++){
        bool bit = (bin_morse >> i) % 2;
        bit_str_rep[i+1] = bit ? '_' : '.';
    }
    
    
    lv_label_set_text(morse_password_label,password);
    lv_label_set_text(morse_bin_label,bit_str_rep);
}
void display_lock_notif(){
    ESP_LOGI(TAG,"Displaing lock notification");
    lv_scr_load(alarm_notifs_scr);
    lv_img_set_src(alarm_symbol,&locked);
}

SemaphoreHandle_t xGuiSemaphore;

void display_task(void *pvParameter){
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

    ESP_LOGI(TAG,"Init lvgl done");

    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
        ESP_LOGD(TAG,"Calling task handler");
        lv_task_handler();
        xSemaphoreGive(xGuiSemaphore);
    }
    uint32_t ulNotifiedValue;
    while (1) {
        xTaskNotifyWait(
            0x00,      /* Don't clear any notification bits on entry. */
            ULONG_MAX, /* Reset the notification value to 0 on exit. */
            &ulNotifiedValue, /* Notified value pass out in ulNotifiedValue. */
            portMAX_DELAY
        );
        ESP_LOGI(TAG,"Display task got notification with value %d",ulNotifiedValue);
        switch(ulNotifiedValue){
            case NOTIFY_VALUE_NAVIGATION:{
                if(nav_data.direction!=13&&nav_data.direction!=14&&nav_data.direction!=31){
                    display_dir_symbol(nav_data.direction);
                }
                
                display_meters(nav_data.distance);
                break;
            }            
            case NOTIFY_VALUE_CLEAR:{    
                lv_scr_load(lv_obj_create(NULL, NULL));
                break;
            }             
            case NOTIFY_VALUE_PASSKEY:{
                display_passkey(passkey);
                break;
            }            
            case NOTIFY_VALUE_MORSE:{
                display_morse(morse_char,morse_char_len,morse_password);
                break;
            }          
            case NOTIFY_VALUE_LOCK:{
                display_lock_notif();
                break;
            }
        }

        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            ESP_LOGD(TAG,"Calling task handler");
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }

    free(buf);
    vTaskDelete(NULL);
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}