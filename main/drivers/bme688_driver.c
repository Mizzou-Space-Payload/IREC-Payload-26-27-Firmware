#include "bme688_driver.h"
#include "payload_data.h"
#include "soldered_bme688.h"
#include "esp_log.h"

static const char *TAG = "bme688";
static bool initialized = false;

esp_err_t BME688_Init(i2c_master_bus_handle_t bus_handle, uint8_t dev_addr) {
    if (initialized) {
        return ESP_OK;
    }

    esp_err_t err = bme688_init(bus_handle, dev_addr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "bme688_init failed: %s", esp_err_to_name(err));
        return err;
    }

    initialized = true;
    ESP_LOGI(TAG, "BME688 Initialized successfully");
    return ESP_OK;
}

esp_err_t BME688_ReadData(BME688_Data_t *data) {
    if (data == NULL || !initialized) {
        ESP_LOGE(TAG, "Read failed: device not initialized or data pointer is NULL");
        if (data != NULL) {
            data->valid = false;
        }
        return ESP_ERR_INVALID_STATE;
    }

    bme688_data_t raw_reading;
    esp_err_t err = bme688_read(&raw_reading);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "bme688_read failed: %s", esp_err_to_name(err));
        data->valid = false;
        return err;
    }

    data->timestamp_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);
    data->temperature  = raw_reading.temperature;
    data->pressure     = raw_reading.pressure;
    data->humidity     = raw_reading.humidity;
    data->valid        = true;

    return ESP_OK;
}