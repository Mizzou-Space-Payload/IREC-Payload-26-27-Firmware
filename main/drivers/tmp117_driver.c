/*
 *  tmp117_driver.c
 *
 *  Created on: October 11, 2026
 *      Author: TriGalx
 
 *  Last updated: October 31, 2026
 *	Added i2c_bus handler from i2c_bus.c
 *  Commented out old handler
 */
 
#include "tmp117_driver.h"
#include <stdio.h>
#include <string.h>

#include "soldered_tmp117.h"  // Access 3rd party library
#include "i2c_bus.h" 
#include "esp_log.h"

static const char *TAG = "tmp";

static tmp117_t sensor_device; //Creates tmp "object"
static bool tmp117_initialized = false; //Check if tmp is ready

/*
#include "driver/i2c_master.h" // ESP-IDF core I2C driver
static i2c_master_bus_handle_t i2c_bus_handle; //Tracks physical hardware instance

#define I2C_MASTER_SDA_IO  21  
#define I2C_MASTER_SCL_IO  22  
*/

#define TMP117_ADDR_GND  0x48  // Default address
#define TMP117_ADDR_VCC  0x49  // Address if ADDR pin is pulled high

bool tmp117_driver_init(void){
	esp_err_t err;//ESP-IDF's error/status type
	/*
	i2c_master_bus_config_t bus_cfg = {  // Set-up ports
		.i2c_port = I2C_NUM_0,
		.sda_io_num = I2C_MASTER_SDA_IO,
		.scl_io_num = I2C_MASTER_SCL_IO,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,			 // Ignore elec. noise less than 7 cycles
		.flags.enable_internal_pullup = true,
	};
	
	
	err = i2c_new_master_bus(&bus_cfg, &i2c_bus_handle); // Check if ports are open
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Hardware Error: i2c_new_master_bus failed: %s", esp_err_to_name(err));
		return false;
	}
	*/
	
	i2c_master_bus_handle_t i2c_bus_handle = i2c_bus_get_handle(); // New initializer for i2c_bus
	if (i2c_bus_handle == NULL){
		ESP_LOGE(TAG, "Shared Bus Error: i2c_bus_get_handle returned NULL. Make sure i2c_bus_init is called first!");
		return false;
	}
	
	err = tmp117_init(&sensor_device, i2c_bus_handle, TMP117_ADDR_GND);// Check for mem. space and allocation
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Library Error: tmp117_init failed: %s", esp_err_to_name(err));
		return false;
	}
	
	err = tmp117_begin(&sensor_device); // Check for chip, power, wire failure
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Communication Error: tmp117_begin failed: %s", esp_err_to_name(err));
		return false;
	}

	ESP_LOGI(TAG, "TMP117 driver successfully initialized.");
	tmp117_initialized = true;
	return tmp117_initialized = true;
}

bool tmp117_driver_read(float *temperature) {
	if (temperature == NULL || !tmp117_initialized) {
		return false;
	}

	if (tmp117_get_temperature(&sensor_device, temperature) == ESP_OK) {
		return true;
	}

	ESP_LOGE(TAG, "Data Error: Failed executing I2C read transaction.");
	return false;
}
