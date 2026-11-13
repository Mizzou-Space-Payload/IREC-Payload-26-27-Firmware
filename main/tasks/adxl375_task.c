/*
 * adxl375_task.c
 *
 * Created on:	 				Nov 5, 2026
 * Author: 						sir_l, Joey_J
 *
 * Last Modified: 				Nov 13, 2026
 * User that last modified: 	Joey_J
 *
 * Desc:
 * 	ADXL375 acquisition task. Creates a dedicated FreeRTOS task that
 * 	periodically reads the ADXL375 high-g accelerometer (initial target rate of 100 Hz) using the ADXL375 driver
 *
 * 	In each cycle the task will:
 * 		I	reads ADXL375 sensor data
 * 		II	packages the measurement into a sensor_event_t
 * 		III sends the sensor event to the logger queue
 * 		IV	notifies the logger task if the event was successfully queued
 * 		V	packages the measurement into a flight_state_input_t
 * 		VI	sends the measurement to the flight-state queue
 * 		VII	waits until the next acquisition period
 *
 * Rules to follow:
 * https://spinroot.com/gerard/pdf/P10.pdf
 *
 * The Power of Ten
 * 	R1. Simple Control Flow
 * 		- Restrict control flow, do not use goto, setjmp, longjmp or recursion
 * 	R2. Fixed Upper-Bound Loops
 * 		- All loops must have a statically provable, fixed upper-bound on iterations.
 * 	R3. No Dynamic Memory After Initialization
 * 		- Do not use dynamic memory allocation (such as malloc or free) after task initialization.
 * 	R4. Limit Function Length
 * 		- Keep every function small enough to fit on a single printed page.
 * 	R5. Minimum Assertions
 * 		- Include an average of at least two side-effect-free bool assertions per function
 * 	R6. Minimal Data Scope
 * 		- Declare data objects at smallest possible of level of scope
 * 	R7. Check Return Values and Parameters
 * 		- Always check non-void function return values and explicitly verify parameter validity
 * 	R8. Limited Preprocessor Use
 * 		- Restrict the preprocessor to file inclusion and simple macro definitions.
 * 	R9. Restrict Pointers
 * 		- Limit pointer dereferencing to no more than one per expression, and prohibit function pointers.
 * 	R10. Zero-Warning Compilation and Static Analysis
 * 		- Compile daily with all compiler warnings enabled at most pedantic setting (zero warnings) and
 * 			run at least one state-of-the-art static source code analyzer.
 */

#include <stdio.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "flight/flight_state.h" // flight_state_get(), FLIGHT_STATE_LANDED

#include "adxl375_task.h" // declares adxl375_task_start()
#include "payload_data.h" // allows flight_state_input_t

#include "drivers/adxl375_driver.h" // allows adxl375_init() and adxl375_read() calls

static const char *TAG = "ADXL375_TASK";

/*
Stores the resources and dependencies required by the ADXL375 task.
A single static instance is populated by adxl375_task_start() and
read by the FreeRTOS task, so no dynamic allocation is needed.
*/

typedef struct {
	QueueHandle_t logger_queue;
	QueueHandle_t flight_state_queue;
	TaskHandle_t logger_task_handle;
	system_status_t *system_status;
} adxl375_task_config_t;

// make adxl375_task_config_t static to avoid malloc usage
static adxl375_task_config_t adxl375_config;

// tracker to see if current adxl375 is running
static bool adxl375_task_started = false;

/* 
Main execution function for the ADXL375 task
Receives the task configuration through pvParameters, 
copies the configuration locally, and then enters its acquisition loop
*/

static void adxl375_task(void *pvParameters);

/*Creates and starts the ADXL375 FreeRTOS task
Validates the provided arguments.
Populates the static task configuration structure.
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
	if(logger_queue == NULL || flight_state_queue == NULL ||
	  system_status == NULL || task_handle_out == NULL) {
		return ESP_ERR_INVALID_ARG;
	}

	// only one instance is allowed since config is static
	if (adxl375_task_started) {
		return ESP_ERR_INVALID_STATE;
	}

	//Initialize output handle to NULL
	*task_handle_out = NULL;

	// initalize adxl375
	// if adxl375 fails to initalize then send an error out
	if (!adxl375_init()) {
		ESP_LOGE(TAG, "ADXL375 init failed");
		system_status->adxl_ok = false; 
		return ESP_FAIL; 
	}

	// populate static task configuration
	adxl375_config.logger_queue = logger_queue;
	adxl375_config.flight_state_queue = flight_state_queue;
	adxl375_config.logger_task_handle = logger_task_handle;
	adxl375_config.system_status = system_status;

	// create FreeRTOS task
	BaseType_t result = xTaskCreate(
		adxl375_task,		//Task function
		"ADXL375 TASK",		//Task name
		4096,				//Stack size (bytes in ESP-IDF)
		&adxl375_config,	//Task parameters
		5,					//Task priority
		task_handle_out		//Output task handle
	);

	if (result != pdPASS) {
		ESP_LOGE(TAG, "Failed to create ADXL375 task");
		return ESP_FAIL;
	}

	// adxl375 task is started set adxl375_task_started to true
	adxl375_task_started = true;

	ESP_LOGI(TAG, "ADXL375 task created successfully");

	return ESP_OK;
}

/*
Main execution function for the ADXL375 task*/

static void adxl375_task(void *pvParameters){
	//Copy configuration locally (static storage, so nothing to free)
	adxl375_task_config_t config = *(adxl375_task_config_t *)pvParameters;

	//Extract task dependencies
	QueueHandle_t logger_queue = config.logger_queue;
	QueueHandle_t flight_state_queue = config.flight_state_queue;
	TaskHandle_t logger_task_handle = config.logger_task_handle;
	system_status_t *system_status = config.system_status;

	ESP_LOGI(TAG, "ADXL375 task started");

	// loop variables 
	TickType_t last_wake_time = xTaskGetTickCount(); 
	const TickType_t sample_period = pdMS_TO_TICKS(10); 

	//Main task execution loop
	while(1){

		// respect flight-state when deciding whether to continue acquiring
		if (flight_state_get() != FLIGHT_STATE_LANDED) {

			// create zero data 
			adxl_data_t sample = {0}; // (R3)

			// for refrence again 
			// bool adxl375_read(adxl_data_t *out);
			// returns a bool if the data was read and if it was valid 
			if (adxl375_read(&sample) && sample.valid) {

				// set status adxl_okay to true
				system_status->adxl_ok = true; 

				// STEP 2 Package the measurement into a sensor_event_t.
				sensor_event_t event = {0}; 		 // lives on stack non dynamic (R3)
				event.sensor_id = SENSOR_ID_ADXL375; // set sensor to ADXL375 
				event.data.adxl_data = sample; 		 // copy the reading 

				// STEP 3 Send the sensor event to the logger queue. -> using FreeRTOS 
				// xQueueSend returns pdPASS (1) if copied into the queue, errQUEUE_FULL (0) if full
				if (xQueueSend(logger_queue, &event, 0) == pdPASS) {

					// sent to queue successfully 
					// STEP 4 Notify the logger task if the event was successfully queued.
					if (logger_task_handle != NULL) {

						// xTaskNotifyGive always returns pdPASS so no need to check R7
						(void)xTaskNotifyGive(logger_task_handle); 
					} 
				} else {
					// queue full so increase packet count... 
					system_status->dropped_packet_count++; 
				}

				// STEP 5 Package the measurement into a flight_state_input_t.
				flight_state_input_t adxl_flight_state_input = {0}; 	// create flight_state_input (R3) 
				adxl_flight_state_input.type = FLIGHT_INPUT_ADXL375; 	// set flight input to ADXL375
				adxl_flight_state_input.data.adxl_data = sample;  

				// STEP 6 Send the measurement to the flight-state queue.
				if (xQueueSend(flight_state_queue, &adxl_flight_state_input, 0) != pdPASS) {

					// flight_state_queue is full and sample is dropped
					system_status->dropped_packet_count++;
				}
			} else {

				// failed read
				system_status->adxl_ok = false;  
				system_status->adxl_read_fail_count++; 
			}
		}

		// STEP 7 Wait until the next acquisition period.
		// vTaskDelayUntil keeps a fixed 10ms period 
		vTaskDelayUntil(&last_wake_time, sample_period);
	}
} // end of adxl375_task.c 
