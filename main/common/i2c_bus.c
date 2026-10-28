/*
 * i2c_bus.c
 *
 *  Created on: Oct 27, 2026
 *      Author: sir_l
 */

#include "i2c_bus.h"
#include "esp_log.h"

static const char *TAG = "i2c_bus"; //Tag used to identify messages from this module

static i2c_master_bus_handle_t bus_handle = NULL; //Variable that stores handle to the shared bus

esp_err_t i2c_bus_init(system_status_t * system_status){//Creates shared i2c bus
	if(system_status == NULL){
		return ESP_ERR_INVALID_ARG;
	}
	
	system_status->i2c_ok = false;
	
	if (bus_handle != NULL){ //Avoid creating the bus more than once
		ESP_LOGW(TAG, "I2C bus is already initialized.");
		system_status->i2c_ok = true;
		return ESP_OK;
	}
	
	i2c_master_bus_config_t bus_config = { //Sets bus configuration
		.i2c_port = I2C_NUM_0,
		.sda_io_num = GPIO_NUM_5,
		.scl_io_num = GPIO_NUM_6,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup = true,
		};
		
		esp_err_t err = i2c_new_master_bus(&bus_config, &bus_handle); //Creates bus
		
		if(err != ESP_OK){
			ESP_LOGE(TAG, "Failed to initialize I2C bus: %s", esp_err_to_name(err));
			bus_handle = NULL;
			return err;
		}
		
	ESP_LOGI(TAG, "I2C master bus initialized successfully");
	system_status->i2c_ok = true;
	
	return ESP_OK;
}

i2c_master_bus_handle_t i2c_bus_get_handle(void) { //returns the handle for the already created bus
	if(bus_handle == NULL){
		ESP_LOGE(TAG, "I2C bus has not been initialized.");
		return NULL;
		}
		
		return bus_handle;
	}
