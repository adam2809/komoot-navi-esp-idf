#include "alarm.h"

#define TAG "ALARM"

void alarm_enable_task(void *pvParameter){    
    configure_mpu(MOTION_DETECTION_SENSITIVITY);
    init_mpu_interrupt();

    button_event_t ev;
    while(1){
        if (xQueueReceive((QueueHandle_t) pvParameter, &ev, 100/portTICK_PERIOD_MS)) {
            if ((ev.pin == BUTTON_PIN) && (ev.event == BUTTON_UP)) {
                go_to_deep_sleep();
            }
        }
    }

    vTaskDelete(NULL);
}
void go_to_deep_sleep(){
    rtc_gpio_hold_en(MPU6050_INTERRUPT_INPUT_PIN);
    ESP_LOGI(TAG,"Going to deep sleep");
    esp_sleep_enable_ext1_wakeup(PIN_BIT(MPU6050_INTERRUPT_INPUT_PIN)|PIN_BIT(BUTTON_PIN),ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_deep_sleep_start();
}