/*
 * imu_driver.cpp
 *
 *  Created on: May 19, 2026
 *      Author: sir_l
 *
 * Edited to use the rinku404/bno085 component (I2C, CEVA SH2 protocol)
 * instead of the esp32_BNO08x submodule (SPI).
 */

#include "imu_driver.h"

#include <math.h>
#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern "C" {
#include "bno085.h"
}

static const char *TAG = "imu_driver";

/* ---- IMU hardware config: CONFIRM against your wiring/PCB ----
 * The barometer (i2cdev) already owns I2C_NUM_0 on GPIO20/21, so the IMU
 * uses I2C_NUM_1 to avoid a port conflict.
 * SCL/SDA/INT/RST below reuse the pins the old SPI config used
 * (SCK=12, MISO=13, HINT=14, RST=15). Change them if your wiring differs. */
#define IMU_I2C_PORT        I2C_NUM_1
#define IMU_SCL_PIN         GPIO_NUM_12
#define IMU_SDA_PIN         GPIO_NUM_13
#define IMU_INT_PIN         GPIO_NUM_14
#define IMU_RST_PIN         GPIO_NUM_15
#define IMU_I2C_ADDR        0x28        // AD0 low = 0x28, AD0 high = 0x29
#define IMU_I2C_SPEED_HZ    400000

// 50000 us = 50 ms = 20 Hz. main.c reads every 100 ms, so every read
// will normally find fresh data (at 10 Hz, timing jitter causes misses).
#define IMU_REPORT_INTERVAL_US  50000UL

#define IMU_SERVICE_PERIOD_MS   10      // library recommends ~10 ms
#define RAD_TO_DEG_F            57.29577951f

static bno085_handle_t imu = NULL;
static bool initialized = false;

static SemaphoreHandle_t latest_lock = NULL;

// Latest values, written by the callback and consumed by imu_read()
static struct {
	float roll_deg, pitch_deg, yaw_deg;
	float gyro_x, gyro_y, gyro_z;
	bool  new_orientation;
	bool  new_gyro;
} latest;

static void quat_to_euler_deg(float i, float j, float k, float w,
                              float *roll, float *pitch, float *yaw){
	float sinr = 2.0f * (w * i + j * k);
	float cosr = 1.0f - 2.0f * (i * i + j * j);
	*roll = atan2f(sinr, cosr) * RAD_TO_DEG_F;

	float sinp = 2.0f * (w * j - k * i);
	if(sinp > 1.0f)  sinp = 1.0f;
	if(sinp < -1.0f) sinp = -1.0f;
	*pitch = asinf(sinp) * RAD_TO_DEG_F;

	float siny = 2.0f * (w * k + i * j);
	float cosy = 1.0f - 2.0f * (j * j + k * k);
	*yaw = atan2f(siny, cosy) * RAD_TO_DEG_F;
}

// Runs inside bno085_service(), which is called from imu_service_task
static void sensor_callback(bno085_handle_t handle,
                            const bno085_sensor_value_t *value,
                            void *user_context){
	(void)handle;
	(void)user_context;

	if(value->sensor_id == BNO085_SENSOR_GAME_ROTATION_VECTOR){
		// TODO verify this union member name in bno085.h
		const auto &q = value->data.game_rotation_vector;
		float r, p, y;
		quat_to_euler_deg(q.i, q.j, q.k, q.real, &r, &p, &y);

		xSemaphoreTake(latest_lock, portMAX_DELAY);
		latest.roll_deg = r;
		latest.pitch_deg = p;
		latest.yaw_deg = y;
		latest.new_orientation = true;
		xSemaphoreGive(latest_lock);
	}
	else if(value->sensor_id == BNO085_SENSOR_GYROSCOPE_CALIBRATED){
		// TODO verify this union member name in bno085.h (values are rad/s)
		const auto &g = value->data.gyroscope;

		xSemaphoreTake(latest_lock, portMAX_DELAY);
		latest.gyro_x = g.x;
		latest.gyro_y = g.y;
		latest.gyro_z = g.z;
		latest.new_gyro = true;
		xSemaphoreGive(latest_lock);
	}
}

// Keeps the BNO085 serviced at ~10 ms no matter how often imu_read() runs
static void imu_service_task(void *arg){
	(void)arg;
	TickType_t last_wake = xTaskGetTickCount();
	while(1){
		bno085_service(imu);
		vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(IMU_SERVICE_PERIOD_MS));
	}
}

bool imu_init(void){
	ESP_LOGI(TAG, "Starting BNO085 I2C init...");

	if(initialized){
		return true;   // already up (init may be retried by main.c)
	}

	if(latest_lock == NULL){
		latest_lock = xSemaphoreCreateMutex();
		if(latest_lock == NULL){
			ESP_LOGE(TAG, "Mutex creation failed");
			return false;
		}
	}

	// 1. I2C bus
	i2c_master_bus_config_t bus_config = {};
	bus_config.i2c_port = IMU_I2C_PORT;
	bus_config.sda_io_num = IMU_SDA_PIN;
	bus_config.scl_io_num = IMU_SCL_PIN;
	bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
	bus_config.glitch_ignore_cnt = 7;
	bus_config.flags.enable_internal_pullup = true;

	i2c_master_bus_handle_t bus_handle;
	esp_err_t err = i2c_new_master_bus(&bus_config, &bus_handle);
	if(err != ESP_OK){
		ESP_LOGE(TAG, "I2C bus creation failed: %s", esp_err_to_name(err));
		return false;
	}

	// 2. I2C device
	i2c_device_config_t dev_config = {};
	dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
	dev_config.device_address = IMU_I2C_ADDR;
	dev_config.scl_speed_hz = IMU_I2C_SPEED_HZ;

	i2c_master_dev_handle_t i2c_dev;
	err = i2c_master_bus_add_device(bus_handle, &dev_config, &i2c_dev);
	if(err != ESP_OK){
		ESP_LOGE(TAG, "I2C add device failed: %s", esp_err_to_name(err));
		i2c_del_master_bus(bus_handle);
		return false;
	}

	// 3. BNO085
	bno085_config_t config;
	bno085_config_default(&config);

	err = bno085_init(&config, i2c_dev, IMU_INT_PIN, IMU_RST_PIN, &imu);
	if(err != ESP_OK){
		ESP_LOGE(TAG, "BNO085 init failed (%s). Check wiring, PS0/PS1 = GND for I2C", esp_err_to_name(err));
		i2c_master_bus_rm_device(i2c_dev);
		i2c_del_master_bus(bus_handle);
		return false;
	}

	ESP_LOGI(TAG, "BNO085 initialized successfully.");

	bno085_register_sensor_callback(imu, sensor_callback, NULL);

	// Game rotation vector = fused orientation without magnetometer correction
	bno085_enable_sensor(imu, BNO085_SENSOR_GAME_ROTATION_VECTOR, IMU_REPORT_INTERVAL_US);

	// Calibrated gyro report
	bno085_enable_sensor(imu, BNO085_SENSOR_GYROSCOPE_CALIBRATED, IMU_REPORT_INTERVAL_US);

	memset(&latest, 0, sizeof(latest));

	if(xTaskCreate(imu_service_task, "imu_svc", 4096, NULL, 6, NULL) != pdPASS){
		ESP_LOGE(TAG, "Failed to create IMU service task");
		return false;
	}

	initialized = true;
	return true;
}

bool imu_read(imu_data_t * out){

	if(out == NULL || !initialized){
		return false;
	}

	memset(out, 0, sizeof(*out)); //Initializes everything in *out to zero
	out->timestamp_ms = (uint32_t)(esp_timer_get_time() / 1000ULL);

	xSemaphoreTake(latest_lock, portMAX_DELAY);

	if(latest.new_orientation){
		out->roll_deg  = latest.roll_deg;
		out->pitch_deg = latest.pitch_deg;
		out->yaw_deg   = latest.yaw_deg;
		out->has_orientation = true;
		latest.new_orientation = false;
	}

	if(latest.new_gyro){
		out->gyro_x = latest.gyro_x;
		out->gyro_y = latest.gyro_y;
		out->gyro_z = latest.gyro_z;
		out->has_gyro = true;
		latest.new_gyro = false;
	}

	xSemaphoreGive(latest_lock);

	out->valid = out->has_orientation || out->has_gyro;

	return out->valid;
}
