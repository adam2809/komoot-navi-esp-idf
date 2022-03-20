#include "alarm.h"

#define TAG "ALARM"

bool alarm_state = false;
RTC_NOINIT_ATTR bool lock_state;
TaskHandle_t alarm_ringing_task_handle;
morse_input_params_t morse_input_params;
void display_notif(uint8_t notify_val,int display_time_ms);

void alarm_init(morse_input_params_t params){
    morse_input_params = params;
}

void alarm_button_disable_task(void *pvParameter){
    button_event_t ev;
    while(1){
        QueueHandle_t queue = *morse_input_params.buttons_events;
        if (queue != NULL && xQueueReceive(queue, &ev, 100/portTICK_PERIOD_MS)) {
            if ((ev.pin == CONFIG_LEFT_BUTTON_PIN) && (ev.event == BUTTON_UP)) {                        
                xTaskCreate(&morse_password_input_task, "morse_password_input_task", 4098, (void*) &morse_input_params, 5, NULL);

                if(alarm_ringing_task_handle != NULL){
                    vTaskDelete(alarm_ringing_task_handle);
                }
                vTaskDelete(NULL);
            }
        }
        vTaskDelay(10 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

void alarm_ringing_task(void *pvParameter){
    while(1){  
        display_notif(NOTIFY_VALUE_ALARM,ALARM_NOTIF_FLASHING_FREQ);

        vTaskDelay(ALARM_NOTIF_FLASHING_FREQ / portTICK_RATE_MS);
    }
}

void go_to_deep_sleep(bool locked){
    rtc_gpio_hold_en(CONFIG_MPU6050_INTERRUPT_INPUT_PIN);
    ESP_LOGI(TAG,"Going to deep sleep");
    lock_state = locked;
    esp_sleep_enable_ext1_wakeup(
        (locked ? PIN_BIT(CONFIG_MPU6050_INTERRUPT_INPUT_PIN) : 0)|PIN_BIT(CONFIG_LEFT_BUTTON_PIN)|PIN_BIT(CONFIG_RIGHT_BUTTON_PIN),ESP_EXT1_WAKEUP_ANY_HIGH
    );
    esp_deep_sleep_start();
}

bool get_alarm_state(){
    return alarm_state;
}

void raise_alarm_state(){
    ESP_LOGI(TAG,"Turning alarm on");
    alarm_state = true;


    gpio_set_level(GPIO_NUM_25, 0);
    gpio_set_level(GPIO_NUM_4, 1);
    ppposInit();
    xTaskCreate(&sms_task, "sms_task", 4096, NULL, 3, NULL);
    xTaskCreate(&alarm_button_disable_task, "alarm_button_disable_task", 4098, NULL, 5, NULL);
    xTaskCreate(&alarm_ringing_task, "alarm_ringing_task", 4098, NULL, 5, &alarm_ringing_task_handle);
}
void lower_alarm_state(){
    ESP_LOGI(TAG,"Turning alarm off");
    alarm_state = false;
}

void lock(){
    ESP_LOGI(TAG,"Locking");
    display_notif(NOTIFY_VALUE_LOCK,1500);

    go_to_deep_sleep(true);
}

void unlock(){
    ESP_LOGI(TAG,"Unlocking");
    display_notif(NOTIFY_VALUE_UNLOCK,1500);
    go_to_deep_sleep(false);
}

void display_notif(uint8_t notify_val,int display_time_ms){
    if (*morse_input_params.display_task_handle != NULL){
        xTaskNotify(
            *morse_input_params.display_task_handle,
            notify_val,
            eSetValueWithOverwrite
        );
        vTaskDelay(pdMS_TO_TICKS(display_time_ms));
        xTaskNotify(
            *morse_input_params.display_task_handle,
            NOTIFY_VALUE_CLEAR,
            eSetValueWithOverwrite
        );
        vTaskDelay(pdMS_TO_TICKS(100));
    }else{
        ESP_LOGE(GATTC_TAG,"NULL task handle");
    }
}

bool get_lock_state(){
    return lock_state;
}
void init_lock_state(){
    lock_state = false;
}
