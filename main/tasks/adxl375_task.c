/*
 * adxl375_task.c
 *
 *  Created on: Nov 5, 2026
 *      Author: sir_l
 */

#include <stdlib.h>

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "adxl375_task.h"
#include "payload_data.h"
#include "flight_state.h"
#include "adxl375.h"

static const char *TAG = "ADXL375_TASK";

/*
Stores the resources and dependencies required by the ADXL375 task
This structure is allocated by adxl375_task_start() and passed to the 
FreeRTOS task when it is created*/

typedef struct {
	QueueHandle_t logger_queue;
	QueueHandle_t flight_state_queue;
	TaskHandle_t logger_task_handle;
	system_status_t *system_status;
} adxl375_task_config_t;

/* 
Main execution function for the ADXL375 tas
Receives the task configuration through pvParameters, 
copies the configuration locally, and then enters its acquisition loop
*/

static void adxl375_task(void *pvParameters);

/*Creates and starts the ADXL375 FreeRTOS task
Validates the provided arguments.
allocates and populates the task configuration structure.
creates the freertos task using xTaskCreate().
Stores the created task handle in task_handle_out.
*/
esp_err_t adxl375_task_start(
    QueueHandle_t logger_queue,
    QueueHandle_t flight_state_queue,
    TaskHandle_t logger_task_handle,
    system_status_t *system_status,
    TaskHandle_t *task_handle_out
) {
	
	//Validate required arguments
	if(logger_queue == NULL || flight_state_queue == NULL || system_status == NULL || task_handle_out == NULL){
		return ESP_ERR_INVALID_ARG;
		}
		
	//Initialize output handle to NULL
	*task_handle_out = NULL;
	
	//Allocate memory for the task configuration
	adxl375_task_config_t *config = malloc(sizeof(adxl375)task_config_t));
	
	if(config == NULL){
		return ESP_ERR_NO_MEM;
		}
		
	//Populate task configuration
	config->logger_queue = logger_queue;
    config->flight_state_queue = flight_state_queue;
    config->logger_task_handle = logger_task_handle;
    config->system_status = system_status;
    
    //Create the FreeRTOS task
    BaseType_t result = xTaskCreate(
		adxl375_task,		//Task function
		"ADXL375 TASK",		//Task name
		4096,				//Stack size (bytes in ESP-IDF)
		config,				//Task parameters
		5,					//Task priority
		task_handle_out		//Output task handle
		);
		
	// Check whether task creation succeeded
    if (result != pdPASS) {

        // Free configuration memory if task creation failed
        free(config);

        ESP_LOGE(TAG, "Failed to create ADXL375 task");

        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "ADXL375 task created successfully");

    return ESP_OK;
}

/*
Main execution function for the ADXL375 task*/

static void adxl375_task(void *pvParameters){
	//Copy configuration locally
	adxl375_task_config_t config = *(adxl375_task_config_t *)pvParameters;
	
	//Free dynamically allocated configuration memory
	free(pvParameters);
	
	//Extract task dependencies
	QueueHandle_t logger_queue = config.logger_queue;
	QueueHandle_t flight_state_queue = config.flight_state_queue;
	TaskHandle_t logger_task_handle = config.logger_task_handle;
	system_status_t *system_status = config.system_status;
	
	ESP_LOGI(TAG, "ADXL375 task started");
	
	//Main task execution loop
	while(1){
		 /*
         * 1. Read ADXL375 sensor data.
         *
         * 2. Package the measurement into a sensor_event_t.
         *
         * 3. Send the sensor event to the logger queue.
         *
         * 4. Notify the logger task if the event was successfully queued.
         *
         * 5. Package the measurement into a flight_state_input_t.
         *
         * 6. Send the measurement to the flight-state queue.
         *
         * 7. Wait until the next acquisition period.
         */
	}
}
	
	
