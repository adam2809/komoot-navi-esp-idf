#include "morse.h"

#define TAG "MORSE"

uint8_t morse_char;
uint8_t morse_char_len;
char morse_password[MAX_PASSWORD_LENGTH+1];

char mapping[32][5] = {
    {'e','i','s','h','5'},
    {'t','n','d','b','6'},
    {'\0','a','r','l','\0'},    
    {'\0','m','g','z','7'},
    {'\0','\0','u','f','\0'},    
    {'\0','\0','k','c','\0'},
    {'\0','\0','w','p','\0'},
    {'\0','\0','o','\0','8'},
    {'\0','\0','\0','v','\0'},
    {'\0','\0','\0','x','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','q','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','y','\0'},
    {'\0','\0','\0','j','9'},    
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','4'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','3'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','2'},
    {'\0','\0','\0','\0','\0'},
    {'\0','\0','\0','\0','1'},
    {'\0','\0','\0','\0','0'}
};

char bin_morse_2_char(uint8_t bin_morse,uint8_t len){
    if(len == 0 || len > 5){
        return '\0';
    }
    return mapping[bin_morse][len-1];
}

void morse_password_input_task(void *pvParameter){
    char password[MAX_PASSWORD_LENGTH] = "des";
    char password_len = 3;

    QueueHandle_t* buttons_events = ((morse_input_params_t*)pvParameter)->buttons_events;
    TaskHandle_t* display_task_handle = ((morse_input_params_t*)pvParameter)->display_task_handle;
    
    for(int i=0;i<MAX_PASSWORD_TRIES;i++){
        memset(morse_password,'\0',sizeof(char)*MAX_PASSWORD_LENGTH);
        read_morse_word(*buttons_events,*display_task_handle);
        if(!memcmp(morse_password,password,sizeof(char)*password_len)){
            ESP_LOGI(TAG,"Password correct");
            xTaskCreate(&alarm_enable_task, "alarm_enable_task", 4098, *buttons_events, 5, NULL);
            
            strcpy(morse_password,"ok\0");
            morse_char_len = 0;
            morse_char = 0;
            xTaskNotify(
                *display_task_handle,
                NOTIFY_VALUE_MORSE,
                eSetValueWithOverwrite
            );
            vTaskDelay(pdMS_TO_TICKS(500));

            vTaskDelete(NULL);
        }else{
            ESP_LOGI(TAG,"Password wrong");

            strcpy(morse_password,"wrong\0");
            morse_char_len = 0;
            morse_char = 0;
            xTaskNotify(
                display_task_handle,
                NOTIFY_VALUE_MORSE,
                eSetValueWithOverwrite
            );
            
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
    vTaskDelete(NULL);
}

void read_morse_word(QueueHandle_t button_events,TaskHandle_t display_task_handle){
    uint8_t morse_password_len = 0;
    morse_password_len = 0;
    morse_char = 0;
    morse_char_len = 0;
    int64_t last_char_input_time = esp_timer_get_time() / 1000ULL;
    bool was_held_flag = false;
    button_event_t ev;

    xTaskNotify(
        display_task_handle,
        NOTIFY_VALUE_MORSE,
        eSetValueWithOverwrite
    );

    while (true) {
        if (esp_timer_get_time() / 1000ULL - last_char_input_time > TIME_BETWEEN_LETTERS && morse_char_len != 0){
            morse_password[morse_password_len] = bin_morse_2_char(morse_char,morse_char_len);
            xTaskNotify(
                display_task_handle,
                NOTIFY_VALUE_MORSE,
                eSetValueWithOverwrite
            );
            ESP_LOGI(TAG,"Morse password is %s", morse_password);
            morse_password_len++;
            morse_char_len = 0;
            morse_char = 0;
        }

        if (esp_timer_get_time() / 1000ULL - last_char_input_time > TIME_BETWEEN_WORDS){
            ESP_LOGI(TAG,"Morse password input finished");
            break;
        }
        
        if (xQueueReceive(button_events, &ev, 100/portTICK_PERIOD_MS)) {
            if ((ev.pin == CONFIG_BUTTON_PIN) && (ev.event == BUTTON_HELD)) {
                was_held_flag = true;
            }
            if ((ev.pin == CONFIG_BUTTON_PIN) && (ev.event == BUTTON_UP)) {
                if(was_held_flag){
                    ESP_LOGI(TAG,"Got long");
                }else{
                    ESP_LOGI(TAG,"Got short");
                }
                morse_char |= was_held_flag << morse_char_len;
                morse_char_len++;
                xTaskNotify(
                    display_task_handle,
                    NOTIFY_VALUE_MORSE,
                    eSetValueWithOverwrite
                );
                was_held_flag = false;
                last_char_input_time = esp_timer_get_time() / 1000ULL;
                ESP_LOGI(TAG,"Morse char is %c", bin_morse_2_char(morse_char,morse_char_len));
            }
        }
    }
}