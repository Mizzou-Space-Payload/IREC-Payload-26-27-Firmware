/*
 * adxl375_driver.c
 *
 *  Created on: Oct 28, 2026
 *      Author: kylerzook
 */

#include "adxl375_driver.h"
#include "i2c_bus.h"
#include "esp_log.h"
#include "esp_timer.h"

#define ADXL375_ADDRESS 0x53 //SDO tied to GND on the payload board
#define ADXL375_I2C_SPEED_HZ 400000
#define ADXL375_TIMEOUT_MS 100

//Registers
#define REG_DEVID 0x00
#define REG_BW_RATE 0x2C
#define REG_POWER_CTL 0x2D
#define REG_DATA_FORMAT 0x31
#define REG_DATAX0 0x32

#define ADXL375_DEVICE_ID 0xE5
#define BW_RATE_100HZ 0x0A
#define DATA_FORMAT_VALUE 0x0B //Range bits must be 11 on the ADXL375 (fixed +-200g)
#define POWER_CTL_MEASURE 0x08

#define ADXL375_G_PER_LSB 0.049f //49 mg per count

static const char *TAG = "adxl375";

static i2c_master_dev_handle_t device = NULL; //ADXL375 on the shared bus
static bool initialized = false; //shows if ADXL375 is ready to use

static esp_err_t write_reg(uint8_t reg, uint8_t value){
	uint8_t buf[2] = {reg, value};
	return i2c_master_transmit(device, buf, sizeof(buf), ADXL375_TIMEOUT_MS);
}

static esp_err_t read_regs(uint8_t reg, uint8_t *data, size_t len){
	//Chip auto-increments the register address, so multi-byte reads come back in order
	return i2c_master_transmit_receive(device, &reg, 1, data, len, ADXL375_TIMEOUT_MS);
}

bool adxl375_init(void){
	esp_err_t err;

	if(device == NULL){ //Only add the device to the bus once, even if init is retried
		i2c_master_bus_handle_t bus = i2c_bus_get_handle();
		if(bus == NULL){
			ESP_LOGE(TAG, "Shared I2C bus not initialized");
			return false;
		}

		i2c_device_config_t dev_config = {
			.dev_addr_length = I2C_ADDR_BIT_LEN_7,
			.device_address = ADXL375_ADDRESS,
			.scl_speed_hz = ADXL375_I2C_SPEED_HZ,
		};

		err = i2c_master_bus_add_device(bus, &dev_config, &device);
		if(err != ESP_OK){
			ESP_LOGE(TAG, "i2c_master_bus_add_device failed: %s", esp_err_to_name(err));
			device = NULL;
			return false;
		}
	}

	//Confirm the chip is there and is the right part
	uint8_t device_id = 0;
	err = read_regs(REG_DEVID, &device_id, 1);
	if(err != ESP_OK){
		ESP_LOGE(TAG, "Failed to read DEVID: %s", esp_err_to_name(err));
		return false;
	}
	if(device_id != ADXL375_DEVICE_ID){
		ESP_LOGE(TAG, "Unexpected DEVID 0x%02X (expected 0x%02X)", device_id, ADXL375_DEVICE_ID);
		return false;
	}

	err = write_reg(REG_BW_RATE, BW_RATE_100HZ);
	if(err != ESP_OK){
		ESP_LOGE(TAG, "Failed to set data rate: %s", esp_err_to_name(err));
		return false;
	}

	err = write_reg(REG_DATA_FORMAT, DATA_FORMAT_VALUE);
	if(err != ESP_OK){
		ESP_LOGE(TAG, "Failed to set data format: %s", esp_err_to_name(err));
		return false;
	}

	//Leaves standby and starts measuring, must be written last
	err = write_reg(REG_POWER_CTL, POWER_CTL_MEASURE);
	if(err != ESP_OK){
		ESP_LOGE(TAG, "Failed to enable measurement: %s", esp_err_to_name(err));
		return false;
	}

	initialized = true;
	ESP_LOGI(TAG, "ADXL375 Initialized");
	return true;
}

bool adxl375_read(adxl_data_t *out){
	if(out == NULL || !initialized){
		return false;
	}

	uint8_t data[6];

	//Read all 6 bytes at once so X, Y and Z come from the same sample
	esp_err_t err = read_regs(REG_DATAX0, data, sizeof(data));
	if(err != ESP_OK){
		ESP_LOGE(TAG, "Failed to read acceleration: %s", esp_err_to_name(err));
		out->valid = false;
		return false;
	}

	//Low byte first
	int16_t raw_x = (int16_t)((data[1] << 8) | data[0]);
	int16_t raw_y = (int16_t)((data[3] << 8) | data[2]);
	int16_t raw_z = (int16_t)((data[5] << 8) | data[4]);

	out->timestamp_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);
	out->ax_g = raw_x * ADXL375_G_PER_LSB;
	out->ay_g = raw_y * ADXL375_G_PER_LSB;
	out->az_g = raw_z * ADXL375_G_PER_LSB;
	out->valid = true;

	return true;
}
