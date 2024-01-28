#ifndef IMU_H
#define IMU_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint16_t acc_sample_rate;
  uint16_t gyro_sample_rate;
  uint16_t mag_sample_rate;
  float acc_scale;
  float gyro_scale;
  float mag_scale;
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
  float x;
  float y;
  float z;
  uint32_t timestamp;
} imu_acc_t;

typedef struct {
  float x;
  float y;
  float z;
  uint32_t timestamp;
} imu_gyro_t;

typedef struct {
  float x;
  float y;
  float z;
  uint32_t timestamp;
} imu_mag_t;

typedef struct {
  imu_acc_t acc;
  imu_gyro_t gyro;
  imu_mag_t mag;

  imu_acc_raw_t acc_raw;
  imu_gyro_raw_t gyro_raw;
  imu_mag_raw_t mag_raw;

  imu_config_t config;
  uint32_t timestamp;
} imu_t;

void imu_init();
void imu_update(uint32_t id, uint8_t *data, size_t len, uint32_t timestamp);

void imu_bsp_interrupt(void *arg);

#endif
