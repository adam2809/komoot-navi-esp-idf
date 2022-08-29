#include "Statechart_required.h"
#include "alarm.h"

#define TAG "STATECHART_REQUIRED"

TaskHandle_t alarm_ringing_task_handle = NULL;

extern void statechart_disable_alarm( Statechart* handle){
    ESP_LOGI(TAG,"Disabling alarm");
    gpio_set_level(CONFIG_BUZZER_PIN, 0);
    if(alarm_ringing_task_handle != NULL){
        vTaskDelete(alarm_ringing_task_handle);
    }
}
extern void statechart_activate_alarm( Statechart* handle){
    ESP_LOGI(TAG,"Activating alarm");
    xTaskCreate(alarm_ringing_task, "alarm_ringing_task", 4098, NULL, 5, alarm_ringing_task_handle);
}
extern void statechart_display_unlocked( Statechart* handle){}
extern void statechart_display_locked( Statechart* handle){}
extern void statechart_new_password_input( Statechart* handle, const sc_integer password){}
extern sc_boolean statechart_verify_password( Statechart* handle, const sc_integer password){
    return false;
}
