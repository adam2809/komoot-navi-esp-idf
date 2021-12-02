#include "display.h"

#include "digit_imgs.h"
lv_obj_t* nav_scr;
lv_obj_t* passkey_scr;

lv_obj_t * passkey_digits_row_top[DIGITS_IN_ROW_COUNT] = {NULL};
lv_obj_t * passkey_digits_row_bottom[DIGITS_IN_ROW_COUNT] = {NULL};

lv_obj_t * meters_digits[DIGITS_IN_ROW_COUNT] = {NULL};
lv_obj_t * dir_symbol = NULL;

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
    ESP_LOGI(DISPLAY_TAG,"Displaing passkey: %d",passkey);
    lv_scr_load(passkey_scr);

    display_number_row(passkey/1000,passkey_digits_row_top);
    display_number_row(passkey%1000,passkey_digits_row_bottom);


}

void display_dir_symbol(uint8_t symbol){
    lv_img_set_src(dir_symbol,dir_symbols_imgs[symbol]);
}

void display_meters(uint32_t meters){
    ESP_LOGI(DISPLAY_TAG,"Displaing meters: %d",meters);
    lv_scr_load(nav_scr);

    if(meters < 1000){
        display_number_row(meters < 1000 ? meters : 999,meters_digits);
    }else{
        lv_img_set_src(meters_digits[0],&greater_than);
        lv_img_set_src(meters_digits[1],digits[1]);
        lv_img_set_src(meters_digits[2],&km);
    }
}