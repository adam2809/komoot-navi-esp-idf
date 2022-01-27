#include "alarm.h"

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