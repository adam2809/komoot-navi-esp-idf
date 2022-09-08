#include "Statechart_required.h"
#include "alarm.h"

#define TAG "STATECHART_REQUIRED"

TaskHandle_t alarm_ringing_task_handle = NULL;
uint32_t _password = 1;

uint32_t get_password(){
    return _password;
}

extern void statechart_disable_alarm_display( Statechart* handle){
    ESP_LOGI(TAG,"Disabling alarm");
    gpio_set_level(CONFIG_BUZZER_PIN, 0);
    if(alarm_ringing_task_handle != NULL){
        vTaskDelete(alarm_ringing_task_handle);
    }
}

extern void statechart_activate_alarm_display( Statechart* handle){
    ESP_LOGI(TAG,"Activating alarm");
    xTaskCreate(alarm_display_task,
        "alarm_display_task",
        4098,
        (void*) handle->display_task_handle,
        5,
        alarm_ringing_task_handle);
}

extern void statechart_activate_alarm_horn( Statechart* handle){

}
extern void statechart_disable_alarm_horn( Statechart* handle){

}

extern void statechart_display_unlocked( Statechart* handle){
    ESP_LOGI(TAG,"Displaying unlocked");
    display_notif(NOTIFY_VALUE_UNLOCK, 1500, handle->display_task_handle);
}

extern void statechart_display_locked( Statechart* handle){
    ESP_LOGI(TAG,"Displaying locked");
    display_notif(NOTIFY_VALUE_LOCK, 1500, handle->display_task_handle);
}

extern void statechart_new_password_input( Statechart* handle, const sc_integer password){
    _password = password;
    xTaskNotify(
        handle->display_task_handle,
        NOTIFY_VALUE_MORSE,
        eSetValueWithOverwrite
    );
}

extern sc_boolean statechart_verify_password( Statechart* handle, const sc_integer password){
    bool res = password == 0b1110010;
    if(!res){
        xTaskNotify(
            handle->display_task_handle,
            NOTIFY_VALUE_WRONG_PASS,
            eSetValueWithOverwrite
        );
    }
    return res;
}
