
/*
 * watchdog_task.h
 *
 * Watchdog monitoring task.
 */

#pragma once

#ifndef MAIN_TASKS_WATCHDOG_TASK_H_
#define MAIN_TASKS_WATCHDOG_TASK_H_

#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "common/payload_data.h"


/*
 * Creates and starts the diagnostic task.
 *
 * system_status: Pointer to shared system status.
 */
esp_err_t watchdog_task_start(
    system_status_t *system_status
);

#endif /* MAIN_TASKS_WATCHDOG_TASK_H_ */

