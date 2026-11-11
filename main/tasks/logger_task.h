
/*
 * logger_task.h
 *
 * Sensor event logging task.
 */

#pragma once

#ifndef MAIN_TASKS_LOGGER_TASK_H_
#define MAIN_TASKS_LOGGER_TASK_H_

#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "common/payload_data.h"


/*
 * Creates and starts the logger task.
 *
 * sensor_queue: Shared queue containing sensor events.
 * system_status: Pointer to shared system status.
 * task_handle_out: Receives the created logger task handle.
 */
esp_err_t logger_task_start(
    QueueHandle_t logger_queue,
    system_status_t *system_status,
    TaskHandle_t *task_handle_out
);

#endif /* MAIN_TASKS_LOGGER_TASK_H_ */
