/*
 * status_buzzer.h
 *
 *  Created on: Oct 21, 2026
 *      Author: owenneff
 */
 
//buzzer pattern settings
#define STATUS_BUZZER_INIT_PITCH 	2000
#define STATUS_BUZZER_FAIL_PITCH 	4000
#define STATUS_BUZZER_SUCCESS_PITCH 1000
#define STATUS_BUZZER_DURATION  100
#define STATUS_BUZZER_DELAY 	100

void status_buzzer_init(void);
void status_buzzer_fail(void);
void status_buzzer_success(void);

