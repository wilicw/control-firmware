#include "imu.h"

#include "config.h"

extern imu_t imu;

void imu_update(uint32_t id, uint8_t *data, size_t len, uint32_t timestamp) {
  static int16_t gx, gy, gz;
  static int16_t ax, ay, az;
#ifdef IMU_MTI630
  switch (id) {
    case 0x61:
      ax = (data[0] << 8) | data[1];
      ay = (data[2] << 8) | data[3];
      az = (data[4] << 8) | data[5];
      imu.acc.x = ax / 1000.0f;
      imu.acc.y = ay / 1000.0f;
      imu.acc.z = az / 1000.0f;
      imu.acc_raw.x = ax;
      imu.acc_raw.y = ay;
      imu.acc_raw.z = az;
      imu.timestamp = timestamp;
      break;
    case 0x62:
      gx = (data[0] << 8) | data[1];
      gy = (data[2] << 8) | data[3];
      gz = (data[4] << 8) | data[5];
      imu.gyro.x = gx / 1000.0f;
      imu.gyro.y = gy / 1000.0f;
      imu.gyro.z = gz / 1000.0f;
      imu.gyro_raw.x = gx;
      imu.gyro_raw.y = gy;
      imu.gyro_raw.z = gz;
      imu.timestamp = timestamp;
      break;
    default:
      break;
  }
#endif
}
