#ifndef IMU_H
#define IMU_H

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
} imu_t;

void imu_init(imu_t *imu);
void imu_update(imu_t *imu, uint32_t id, uint8_t *data, size_t len,
                uint32_t timestamp);

void imu_bsp_interrupt(void *arg);

#endif
