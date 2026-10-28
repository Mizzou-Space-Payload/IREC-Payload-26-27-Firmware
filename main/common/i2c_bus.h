/*
 * i2c_bus.h
 *
 *  Created on: Oct 27, 2026
 *      Author: sir_l
 */

#ifndef MAIN_COMMON_I2C_BUS_H_
#define MAIN_COMMON_I2C_BUS_H_

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "payload_data.h"

esp_err_t i2c_bus_init(system_status_t * system_status); //Creates shared i2c bus

i2c_master_bus_handle_t i2c_bus_get_handle(void); //returns the handle for the already created bus

#endif /* MAIN_COMMON_I2C_BUS_H_ */
