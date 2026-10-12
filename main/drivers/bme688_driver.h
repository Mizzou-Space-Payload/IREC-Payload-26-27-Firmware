/*
    Author: Sharan_Fly-Forever
    Date: Oct 11, 2026
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "soldered_bme688.h"
#include "payload_data.h"

esp_err_t BME688_Init(i2c_master_bus_handle_t bus_handle, uint8_t dev_addr);
esp_err_t BME688_ReadData(BME688_Data_t *data);