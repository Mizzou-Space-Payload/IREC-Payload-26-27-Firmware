/*
 * status_buzzer.c
 *
 *  Created on: Oct 21, 2026
 *      Author: owenneff
 */
#include "status_buzzer.h"
#include "buzzer_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void status_buzzer(int pitch_hz)
{
	buzzer_tone(pitch_hz, STATUS_BUZZER_DURATION);
	vTaskDelay(pdMS_TO_TICKS(STATUS_BUZZER_DELAY));
}

void status_buzzer_init(void) //initialization start
{
		status_buzzer(STATUS_BUZZER_INIT_PITCH);
}

void status_buzzer_fail(void) //initialization failed (3 buzzes)
{
	for(int i = 0; i < 3; i++)
	{
		status_buzzer(STATUS_BUZZER_FAIL_PITCH);
	}
}

void status_buzzer_success(void) //initialization success (2 buzzes)
{
	for(int i = 0; i < 2; i++)
	{
		status_buzzer(STATUS_BUZZER_SUCCESS_PITCH);
	}
}