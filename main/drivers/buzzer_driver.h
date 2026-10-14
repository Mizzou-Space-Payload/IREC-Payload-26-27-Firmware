/*
 * buzzer_driver.h
 *
 *  Created on: Oct 14, 2026
 *      Author: Vu and ChatGPT :)
 */

#pragma once

#include <stdbool.h>

bool buzzer_init(void);
void buzzer_tone(int pitch_hz, int duration_ms);
