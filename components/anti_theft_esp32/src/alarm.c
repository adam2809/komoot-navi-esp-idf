#include "alarm.h"

#define TAG "ALARM"

bool alarm_state = false;
RTC_NOINIT_ATTR bool lock_state;
morse_input_params_t morse_input_params;

void alarm_button_disable_task(void *pvParameter){
    button_event_t ev;
    while(1){
        QueueHandle_t queue = *morse_input_params.buttons_events;
        if (queue != NULL && xQueueReceive(queue, &ev, 100/portTICK_PERIOD_MS)) {
            if ((ev.pin == CONFIG_BUTTON_PIN) && (ev.event == BUTTON_UP)) {                        
                xTaskCreate(&morse_password_input_task, "morse_password_input_task", 4098, (void*) &morse_input_params, 5, NULL);
                vTaskDelete(NULL);
            }
        }
        vTaskDelay(10 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}
void go_to_deep_sleep(bool locked){
    rtc_gpio_hold_en(MPU6050_INTERRUPT_INPUT_PIN);
    ESP_LOGI(TAG,"Going to deep sleep");
    lock_state = locked;
    esp_sleep_enable_ext1_wakeup(
        (locked ? PIN_BIT(MPU6050_INTERRUPT_INPUT_PIN) : 0)|
        PIN_BIT(CONFIG_BUTTON_PIN),ESP_EXT1_WAKEUP_ANY_HIGH
    );
    esp_deep_sleep_start();
}

bool get_alarm_state(){
    return alarm_state;
}


void raise_alarm_state(){
    ESP_LOGI(TAG,"Turning alarm on");
    alarm_state = true;
    xTaskCreate(&alarm_button_disable_task, "alarm_button_disable_task", 4098, NULL, 5, NULL);
}
void lower_alarm_state(){
    ESP_LOGI(TAG,"Turning alarm off");
    alarm_state = false;
}

void lock(void *pvParameter){
    ESP_LOGI(TAG,"Locking");
    vTaskDelay(pdMS_TO_TICKS(1000));
    if (*morse_input_params.display_task_handle != NULL){
        xTaskNotify(
            *morse_input_params.display_task_handle,
            NOTIFY_VALUE_LOCK,
            eSetValueWithOverwrite
        );
    }else{
        ESP_LOGE(GATTC_TAG,"NULL task handle");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    go_to_deep_sleep(true);
}
void unlock(){
    ESP_LOGI(TAG,"Unlocking");
    go_to_deep_sleep(false);
}


void alarm_wakeup(QueueHandle_t* buttons_events,TaskHandle_t* display_task_handle){
    morse_input_params.buttons_events = buttons_events;
    morse_input_params.display_task_handle = display_task_handle;
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_EXT1: {
            uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
            if (wakeup_pin_mask != 0) {
                int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
                ESP_LOGI(TAG,"Wake up from GPIO %d", pin);
                if(pin == CONFIG_BUTTON_PIN){
                    if (lock_state){
                        xTaskCreate(&morse_password_input_task, "morse_password_input_task", 4098, (void*) &morse_input_params, 5, NULL);
                    }else{                        
                        xTaskCreate(&lock, "lock_task", 4098, NULL, 5, NULL);
                    }
                }else if(pin == MPU6050_INTERRUPT_INPUT_PIN && lock_state == true){
                    raise_alarm_state();
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
            ESP_LOGI(TAG,"Undefined wakeup reason");
            lock_state = false;
            go_to_deep_sleep(false);
            break;
        }
        default:
            ESP_LOGI(TAG,"Not a deep sleep reset");
    }
}