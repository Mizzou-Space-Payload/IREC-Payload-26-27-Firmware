/*
 * storage_driver.c
 *
 *  Created on: May 21, 2026
 *      Author: sir_l
 */
#include "storage_driver.h"
#include <stdio.h>
#include <string.h>


#include "esp_log.h"
#include "esp_err.h"

#include "driver/sdspi_host.h"
#include "driver/spi_common.h"

#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#include <sys/stat.h>
#include <unistd.h>

#define LOG_FILE_PATH "/flight.csv"
#define MOUNT_POINT "/sdcard" 
//^Where the SD card will appear in the ESP32 filesystem
#define SD_CS_PIN GPIO_NUM_40


static const char * TAG = "SD";
static FILE *log_file = NULL;
static bool storage_ready = false;

static sdmmc_card_t *card = NULL; //out_card, where ESP-IDF stores information about the mounted SD card

bool storage_init(system_status_t * status){ //Mount microSD and open log file
	if(status == NULL){
		ESP_LOGE(TAG, "Invalid system status pointer");
		return false;
	}
	

	if(storage_ready){ //If storage is already initialized return true
		status->storage_ok = true;
		return true;
	}
	
	status->storage_ok = false;
	
	esp_err_t err;
	sdmmc_host_t host = SDSPI_HOST_DEFAULT(); //use SD-over-SPI mode
	sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT(); //Where the actual SD card wiring goes
	spi_host_device_t spi_host; //Holds the spi host
	
	slot_config.gpio_cs = SD_CS_PIN; //slot_config does not set SCLK/MOSI/MISO. Those are set when you initialize the SPI bus
	
	//err = spi_bus_get_host(&spi_host); //Retrieves and validates SPI host COMMENTED OUT TILL SPI MODULE IS BUILT
	err = ESP_OK; //This line is written to bypass the line above being commented out
	if(err != ESP_OK){
		ESP_LOGE(TAG, "Could not retrieve SPI host..., %s", esp_err_to_name(err));
		return false;
	}
	
	host.slot = spi_host;
	
	//mount configuration
	esp_vfs_fat_mount_config_t mount_config = {
		.format_if_mount_failed = false, //If true, ESP-IDF may format the card if mounting fails
		.max_files = 5, //Max number of files open at once
		.allocation_unit_size = 16 * 1024, //Filesystem allocation size
	};
	
	
	
	
	err = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
	if(err != ESP_OK){
		ESP_LOGE(TAG, "SD mounting failed...: %s", esp_err_to_name(err));
		card = NULL;
		return false;
	}
	
	ESP_LOGI(TAG, "SD mounted.");
	
	log_file = fopen(LOG_FILE_PATH, "a+");
	if(log_file == NULL){
		ESP_LOGE(TAG, "Failed to open log file...");
		esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
		card = NULL;
		return false;
	}
	
	ESP_LOGI(TAG, "Log file opened: %s", LOG_FILE_PATH);
	
	storage_ready = true;
	status->storage_ok = true;
	return true;
}

bool storage_write(const char * line){//Write CSV rows
	if(!storage_ready || log_file == NULL || line == NULL){
		return false;
	}
	
	int written = fprintf(log_file, "%s\n", line);
	
	if(written < 0){
		ESP_LOGE(TAG, "Failed to write line.");
		return false;
	}
	
	return true;
}

bool storage_read(char *buffer, size_t buffer_size){ //Test reading back data
	if(!storage_ready || log_file == NULL || buffer == NULL || buffer_size == 0){
		return false;
	}
	
	rewind(log_file); //Move file pointer back to top
	
	if(fgets(buffer, buffer_size, log_file) == NULL){
		return false;
	}
	
	return true;
}

bool storage_flush(void){ //Force buffered data to card
	if(!storage_ready || log_file == NULL){
		return false;
	}
	
	return fflush(log_file) == 0;
}

bool storage_deinit(void){ // Close file and unmount card
	bool success = true;
	
	if(log_file != NULL) {
		if(fclose(log_file) != 0) {
			ESP_LOGE(TAG, "failed to close log file.");
			success = false;
		}
		
		log_file = NULL;
	}
	
	if(card != NULL){
		esp_err_t err = esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
		
		if(err != ESP_OK){
			ESP_LOGE(TAG, "Failed to unmount SD card: %s", esp_err_to_name(err));
			success = false;
		}
		
		card = NULL;
	}
	
	storage_ready = false;
	
	return success;
}

