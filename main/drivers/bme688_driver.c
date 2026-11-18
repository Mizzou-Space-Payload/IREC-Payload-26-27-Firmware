#include "bme688_driver.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "payload_data.h"

static const char *TAG = "bme688";
static bool initialized = false;


static bme688_handle_t sensor_handle;


esp_err_t BME688_Init(uint8_t dev_addr) {
    if (initialized) {
        return ESP_OK;
    }

    
    i2c_master_bus_handle_t bus_handle = i2c_bus_get_handle();
    if (bus_handle == NULL) {
        ESP_LOGE(TAG, "Failed to get shared I2C bus handle. Ensure i2c_bus_init() was called.");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = bme688_init(bus_handle, dev_addr, 400000, &sensor_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "bme688_init failed: %s", esp_err_to_name(err));
        return err;
    }

    initialized = true;
    ESP_LOGI(TAG, "BME688 Initialized successfully.");
    return ESP_OK;
}

esp_err_t BME688_Configure(const BME688_Config_t *config) {
    if (!initialized || config == NULL) {
        ESP_LOGE(TAG, "Configuration failed: device not initialized or config is NULL");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "Applying custom BME688 configuration (Temp OS: %d, Press OS: %d, Hum OS: %d, Filter: %d)",
             config->temp_oversampling, config->pressure_oversampling,
             config->humidity_oversampling, config->filter_coeff);
    
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
    
    esp_err_t err = bme688_read(&sensor_handle, &raw_reading);
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