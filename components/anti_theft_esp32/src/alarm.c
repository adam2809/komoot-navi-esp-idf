#include "alarm.h"

#define TAG "ALARM"

RTC_NOINIT_ATTR bool alarm_state =0;

void alarm_enable_task(void *pvParameter){    
    configure_mpu(MOTION_DETECTION_SENSITIVITY);
    init_mpu_interrupt();

    button_event_t ev;
    while(1){
        QueueHandle_t queue = (QueueHandle_t) pvParameter;
        if (queue != NULL && xQueueReceive(queue, &ev, 100/portTICK_PERIOD_MS)) {
            if ((ev.pin == CONFIG_BUTTON_PIN) && (ev.event == BUTTON_UP)) {
                go_to_deep_sleep();
            }
        }
        vTaskDelay(10 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}
void go_to_deep_sleep(){
    rtc_gpio_hold_en(MPU6050_INTERRUPT_INPUT_PIN);
    ESP_LOGI(TAG,"Going to deep sleep");
    esp_sleep_enable_ext1_wakeup(PIN_BIT(MPU6050_INTERRUPT_INPUT_PIN)|PIN_BIT(CONFIG_BUTTON_PIN),ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_deep_sleep_start();
}

bool get_alarm_state(){
    return alarm_state;
}


void raise_alarm_state(){
    ESP_LOGI(TAG,"Turning alarm on");
    alarm_state = true;
}
void lower_alarm_state(){
    ESP_LOGI(TAG,"Turning alarm off");
    alarm_state = false;
}

morse_input_params_t morse_input_params; 

void alarm_wakeup(QueueHandle_t* buttons_events,TaskHandle_t* display_task_handle){
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_EXT1: {
            uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
            if (wakeup_pin_mask != 0) {
                int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
                ESP_LOGI(TAG,"Wake up from GPIO %d", pin);
                if(pin == CONFIG_BUTTON_PIN){
                    morse_input_params.buttons_events = buttons_events;
                    morse_input_params.display_task_handle = display_task_handle;
                    xTaskCreate(&morse_password_input_task, "morse_password_input_task", 4098, (void*) &morse_input_params, 5, NULL);
                }else if(pin == MPU6050_INTERRUPT_INPUT_PIN){
                    ESP_LOGI(TAG,"Turning on alarm");
                    xTaskCreate(&alarm_enable_task, "alarm_enable_task", 4098, (void*) *buttons_events, 5, NULL);
                    alarm_state = 1;
                }
            } else {
                ESP_LOGW(TAG,"Could not get wakeup pin number");
            }
            break;
        }
        case ESP_SLEEP_WAKEUP_EXT0: {
            ESP_LOGI(TAG,"Wake up from EXT0");
            break;
        }
        case ESP_SLEEP_WAKEUP_UNDEFINED:{
            xTaskCreate(&alarm_enable_task, "alarm_enable_task", 4098, (void*)  *buttons_events, 5, NULL);
            ESP_LOGI(TAG,"Undefined wakeup reason");
            alarm_state = 0;
            break;
        }
        default:
            ESP_LOGI(TAG,"Not a deep sleep reset");
    }
}