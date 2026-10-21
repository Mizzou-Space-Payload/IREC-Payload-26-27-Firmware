/*
 * tmp117_driver.h
 *
 *  Created on: October 11, 2026
 *      Author: TriGalx
 
 *  Last updated: October 11, 2026
 */

#ifndef TMP117_DRIVER_H
#define TMP117_DRIVER_H
#include <stdbool.h>

bool tmp117_driver_init(void); // Configure I2C bus, check sensor connection
bool tmp117_driver_read(float *temperature); // Read temperature in Celsius

#endif // TMP117_DRIVER_H
