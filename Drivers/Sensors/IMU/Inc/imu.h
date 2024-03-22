#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint16_t acc_sample_rate;
  uint16_t gyro_sample_rate;
  uint16_t mag_sample_rate;
  double acc_scale;
  double gyro_scale;
  double mag_scale;
} imu_config_t;

typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
  uint32_t timestamp;
} imu_acc_raw_t;

typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
  uint32_t timestamp;
} imu_gyro_raw_t;

typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
  uint32_t timestamp;
} imu_mag_raw_t;

typedef struct {
  double x;
  double y;
  double z;
  uint32_t timestamp;
} imu_acc_t;

typedef struct {
  double x;
  double y;
  double z;
  uint32_t timestamp;
} imu_gyro_t;

typedef struct {
  double x;
  double y;
  double z;
  uint32_t timestamp;
} imu_mag_t;

typedef enum {
  IMU_MTI600,
  IMU_ADIS16,
} imu_type_t;

typedef struct {
  /* physical values in SI units */
  imu_acc_t acc;
  imu_gyro_t gyro;
  imu_mag_t mag;

  /* raw values from sensor */
  imu_acc_raw_t acc_raw;
  imu_gyro_raw_t gyro_raw;
  imu_mag_raw_t mag_raw;

  imu_config_t config;
  imu_type_t type;
} imu_t;

imu_t *open_imu_instance(uint32_t id);
void imu_set_type(imu_t *imu, imu_type_t type);

void imu_bsp_interrupt(imu_t *imu, void *arg1, void *arg2);
