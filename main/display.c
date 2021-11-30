#include "display.h"

#include "digit_imgs.h"
lv_obj_t* nav_scr;
lv_obj_t* passkey_scr;

lv_obj_t * passkey_digits_row_top[DIGITS_IN_ROW_COUNT] = {NULL};
lv_obj_t * passkey_digits_row_bottom[DIGITS_IN_ROW_COUNT] = {NULL};

lv_obj_t * meters_digits[DIGITS_IN_ROW_COUNT] = {NULL};

void init_lvgl_display(lv_color_t* buf) {

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
    lv_obj_t * passkey_scr  = lv_obj_create(NULL, NULL);
    lv_obj_t * nav_scr  = lv_obj_create(NULL, NULL);

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
    lv_scr_load(passk_scr);
}