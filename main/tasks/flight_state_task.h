/*
 * flight_state_task.h
 *
 *  Created on: Nov 7, 2026
 *      Author: sir_l
 */

 #pragma once
 
 #include "esp_err.h"
 #include "freertos/FreeRTOS.h"
 #include "freertos/queue.h"
 #include "freertos/task.h"
 
 /*Struct because it makes the notified tasks easier to expand*/
 
 typedef struct{
	 TaskHandle_t adxl375_task;
	 TaskHandle_t bno085_task;
	 TaskHandle_t environmental_task;
	 TaskHandle_t logger_task;
 } flight_state_notify_targets_t;
 
 
/*flight state task:
Waits for a measurement from flight_state_queue
passes that measurement to flight_state_process_input
Check whether a transition occurred 
If so, notify each task in flight_state_notify_targets
Return to waiting for the next measurement
*/
 esp_err_t flight_state_task_start(QueueHandle_t flight_state_queue, const flight_state_notify_targets_t *notify_targets, TaskHandle_t *task_handle_out);