/*
 * flight_state.h
 *
 *  Created on: Nov 5, 2026
 *      Author: sir_l
 */

#pragma once

#include <stdbool.h>
#include "payload_data.h"

void flight_state_init(void);

flight_state_id_t flight_state_get(void);

bool flight_state_process_input(const flight_state_input_t *input);

