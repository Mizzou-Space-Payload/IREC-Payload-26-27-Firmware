/*
 * flight_state.c
 *
 *  Created on: Nov 5, 2026
 *      Author: sir_l
 */

#include "payload_data.h"
#include <stdatomic.h>


void flight_state_init(void){ //establishes the initial state
	static atomic_int current_flight_state = ATOMIC_VAR_INIT(FLIGHT_STATE_LAUNCHPAD);
};

flight_state_id_t flight_state_get(void){ //retrieves the current flight state
	return (flight_state_id_t)atomic_load(&current_flight_state);
}

bool flight_state_process_input(const flight_state_input_t *input){ //processes one incoming measurement and returns true if it causes a transition
	if(input == NULL){
		return false;
	}
	
	//Flight-state transistion logic will go here
	
	return false;
}


