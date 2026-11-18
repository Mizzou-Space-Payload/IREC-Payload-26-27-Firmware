#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "i2c_bus.h"
#include "soldered_bme688.h"
#include "payload_data.h"

typedef struct {
    uint8_t temp_oversampling;
    uint8_t pressure_oversampling;
    uint8_t humidity_oversampling;
    uint8_t filter_coeff;
} BME688_Config_t;

esp_err_t BME688_Init(uint8_t dev_addr);

esp_err_t BME688_Configure(const BME688_Config_t *config);

esp_err_t BME688_ReadData(BME688_Data_t *data);