
/*
 * payload_data.h
 *
 * Shared payload data structures and sensor event definitions.
 */

#pragma once

#ifndef MAIN_COMMON_PAYLOAD_DATA_H_
#define MAIN_COMMON_PAYLOAD_DATA_H_

#include <stdint.h>
#include <stdbool.h>

/*SENSOR DATA STRUCTURES*/

/* BMP390 barometer measurements */
typedef struct {
    uint32_t timestamp_ms;

    float pressure_pa;
    float temperature_c;
    float altitude_m;

    bool valid;

} barometer_data_t;


/* BNO085 IMU measurements */
typedef struct {
    uint32_t timestamp_ms;

    float roll_deg;
    float pitch_deg;
    float yaw_deg;

    float gyro_x;
    float gyro_y;
    float gyro_z;

    bool has_orientation;
    bool has_gyro;
    bool valid;

} imu_data_t;


/* ADXL375 high-G accelerometer measurements */
typedef struct {
    uint32_t timestamp_ms;

    float ax_g;
    float ay_g;
    float az_g;

    bool valid;

} adxl_data_t;


/* BME688 environmental measurements */
typedef struct {
    uint32_t timestamp_ms;

    float pressure_pa;
    float temperature_c;
    float humidity_pct;
    float gas_resistance_ohms;

    bool valid;

} bme688_data_t;


/* TMP117 temperature measurements */
typedef struct {
    uint32_t timestamp_ms;

    float temperature_c;

    bool valid;

} tmp117_data_t;


/* SENSOR IDENTIFICATION*/

/*
  Identifies which sensor generated a sensor event.
 
  The identifier determines which member of the
  sensor_event_t data union contains valid data.
 */

typedef enum {
    SENSOR_ID_BMP390,
    SENSOR_ID_BNO085,
    SENSOR_ID_ADXL375,
    SENSOR_ID_BME688,
    SENSOR_ID_TMP117

} sensor_id_t;


/* ---------------- SENSOR EVENT ---------------- */

/*
 Represents one individual sensor reading.
 
 Each queue entry contains data from ONE sensor.
 */

typedef struct {

    sensor_id_t sensor_id;

    union {

        barometer_data_t baro_data;
        imu_data_t imu_data;
        adxl_data_t adxl_data;
        bme688_data_t bme688_data;
        tmp117_data_t tmp117_data;

    } data;

} sensor_event_t;

/*FLIGHT STATE EVENT*/
/*Carries the readings required for the flight state
decision task to make a choice for the current flight stage
Contains enum for flight states*/

typedef enum {
	FLIGHT_INPUT_ADXL375,
	FLIGHT_INPUT_BMP390
} flight_input_type_t;

typedef struct{
	flight_input_type_t type;
	
	union{
		adxl_data_t adxl_data;
		barometer_data_t baro_data;
	} data;
	
}flight_state_input_t;

typedef enum {
    FLIGHT_STATE_LAUNCHPAD,
    FLIGHT_STATE_BOOST,
    FLIGHT_STATE_COAST,
    FLIGHT_STATE_SUSTAINER,
    FLIGHT_STATE_DESCENT,
    FLIGHT_STATE_LANDED
} flight_state_id_t;

/*AGGREGATE SENSOR DATA*/

/*
 Optional aggregate snapshot containing the latest
 available measurements from all sensor groups.
 
 This is NOT the format used by the sensor event queue.
 */

typedef struct {

    barometer_data_t baro_data;
    imu_data_t imu_data;
    adxl_data_t adxl_data;
    bme688_data_t bme688_data;
    tmp117_data_t tmp117_data;

} sensor_data_t;


/*SYSTEM STATUS*/

typedef struct {

    bool imu_ok;
    bool barometer_ok;
    bool adxl_ok;
    bool bme688_ok;
    bool tmp117_ok;

    bool storage_ok;
    bool i2c_ok;
    bool spi_ok;

    uint32_t imu_read_fail_count;
    uint32_t barometer_read_fail_count;
    uint32_t adxl_read_fail_count;
    uint32_t bme688_read_fail_count;
    uint32_t tmp117_read_fail_count;

    uint32_t dropped_packet_count;

    uint32_t sensor_task_loops;
    uint32_t logger_task_loops;
    uint32_t health_task_loops;

} system_status_t;

#endif /* MAIN_COMMON_PAYLOAD_DATA_H_ */

