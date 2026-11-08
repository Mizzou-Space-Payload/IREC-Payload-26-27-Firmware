/*
 * flight_state_task.c
 *
 *  Created on: Nov 7, 2026
 *      Author: sir_l
 */

/*flight state task:
Waits for a measurement from flight_state_queue
passes that measurement to flight_state_process_input
Check whether a transition occurred 
If so, notify each task in flight_state_notify_targets
Return to waiting for the next measurement
*/
 esp_err_t flight_state_task_start(QueueHandle_t flight_state_queue, const flight_state_notify_targets_t *notify_targets, TaskHandle_t *task_handle_out);
