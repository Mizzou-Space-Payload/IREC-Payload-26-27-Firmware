/*
 * adxl375_driver.h
 *
 *  Created on: Oct 28, 2026
 *      Author: kylerzook
 */
#pragma once

#ifndef MAIN_DRIVERS_ADXL375_DRIVER_H_
#define MAIN_DRIVERS_ADXL375_DRIVER_H_

#include <stdbool.h>
#include "payload_data.h"

bool adxl375_init(void); //Requires i2c_bus_init() to have been called first
bool adxl375_read(adxl_data_t *out);

#endif /* MAIN_DRIVERS_ADXL375_DRIVER_H_ */
