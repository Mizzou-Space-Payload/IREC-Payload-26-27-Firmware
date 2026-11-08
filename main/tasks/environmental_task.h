
/*
 * environmental_task.h
 *
 * Environmental sensor acquisition task.
 *
 * Handles:
 * BMP390
 * BME688
 * TMP117
 */

#pragma once

#ifndef MAIN_TASKS_ENVIRONMENTAL_TASK_H_
#define MAIN_TASKS_ENVIRONMENTAL_TASK_H_

#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "common/payload_data.h"


/*
 * Creates and starts the environmental acquisition task.
 *
 * sensor_queue: Shared queue used to publish sensor events.
 * logger_task_handle: Logger task handle used for notifications.
 * system_status: Pointer to shared system status.
 */
esp_err_t environmental_task_start(
	QueueHandle_t flight_state_queue,
    QueueHandle_t logger_queue,
    TaskHandle_t logger_task_handle,
    system_status_t *system_status,
    TaskHandle_t *task_handle_out
);

#endif /* MAIN_TASKS_ENVIRONMENTAL_TASK_H_ */
