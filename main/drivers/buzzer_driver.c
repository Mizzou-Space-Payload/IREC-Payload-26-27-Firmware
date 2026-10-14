//include anything important
//declare pins that buzzer is on
//Create a function that initializes buzzer
//	If initializes, output true and info message
//	If no, output false and info message
//Create a function (buzzer) that turns on the buzzer, with these variables: volume and period

#include "buzzer_driver.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUZZ_PIN GPIO_NUM_3
#define BUZZ_SPEED_MODE LEDC_LOW_SPEED_MODE
#define BUZZ_TIMER      LEDC_TIMER_0
#define BUZZ_CHANNEL    LEDC_CHANNEL_0

static const char *TAG = "buzzer";

static bool initialized = false;


bool buzzer_init(void){
	esp_err_t err;

	ledc_timer_config_t timer_config = {
	    .speed_mode = BUZZ_SPEED_MODE,
	    .duty_resolution = LEDC_TIMER_10_BIT,
	    .timer_num = BUZZ_TIMER,
	    .freq_hz = 4000,
	    .clk_cfg = LEDC_AUTO_CLK,
	};

	err = ledc_timer_config(&timer_config);

	if(err != ESP_OK){
		ESP_LOGE(TAG, "buzzer timer config failed %s", esp_err_to_name(err));
		initialized = false;
		return false;
	}

	
	ledc_channel_config_t channel_config = {
		.gpio_num = BUZZ_PIN,
		.speed_mode = BUZZ_SPEED_MODE,
		.channel = BUZZ_CHANNEL,
		.timer_sel = BUZZ_TIMER,
		.duty = 0,
		.hpoint = 0,
	};
	
	err = ledc_channel_config(&channel_config);
	
	if(err != ESP_OK){
		ESP_LOGE(TAG, "buzzer channel config failed %s", esp_err_to_name(err));
		initialized = false;
		return false;
	}
	
	
	initialized = true;
	ESP_LOGI(TAG, "Buzzer Initialized/Configured"); //Prints an informational message
	return true;
}


static void stop_buzzer(void){
	esp_err_t err = ledc_stop(BUZZ_SPEED_MODE, BUZZ_CHANNEL, 0);
	if (err != ESP_OK) {
	    ESP_LOGE(TAG, "Stopping buzzer failed: %s",
	             esp_err_to_name(err));
	    return;
		}
}

void buzzer_tone(int pitch_hz, int duration_ms){
	if(initialized == false){
		ESP_LOGI(TAG, "Buzzer not initialized/configured."); //Prints an informational message
		return;
	}
	
	if(pitch_hz <= 0){
		ESP_LOGE(TAG, "pitch frequency must be positive, got %d", pitch_hz);
		return;
	}

	if(duration_ms <= 0){
		ESP_LOGE(TAG, "duration cannot be 0 or negative, got %d", duration_ms);
		return;
	}
	
	//
	esp_err_t err;
		
	err = ledc_set_freq(BUZZ_SPEED_MODE, BUZZ_TIMER, pitch_hz);
	if (err != ESP_OK) {
	    ESP_LOGE(TAG, "Setting buzzer frequency failed: %s",
	             esp_err_to_name(err));
	    return;
	}
	
	
	err = ledc_set_duty(BUZZ_SPEED_MODE, BUZZ_CHANNEL, 512);
	if (err != ESP_OK) {
	    ESP_LOGE(TAG, "Setting buzzer duty failed: %s",
	             esp_err_to_name(err));
	    return;
	}
	
	err = ledc_update_duty(BUZZ_SPEED_MODE, BUZZ_CHANNEL);
	if (err != ESP_OK) {
	    ESP_LOGE(TAG, "Updating buzzer duty failed: %s",
	             esp_err_to_name(err));
				 stop_buzzer();
	    return;
	}
	
	
	vTaskDelay(pdMS_TO_TICKS(duration_ms));
	
	stop_buzzer();

}
