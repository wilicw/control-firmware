#include "imu.h"

#include "config.h"

#ifdef IMU_MTI630
#include "imu_mti600.h"
#endif

void imu_init(imu_t* imu) {
#ifdef IMU_MTI630
  imu->config.acc_scale = XCDI_AccelerationHR_SCALE;
  imu->config.gyro_scale = XCDI_RateOfTurnHR_SCALE;
#endif
}

void imu_update(imu_t* imu, uint32_t id, uint8_t* data, size_t len,
                uint32_t timestamp) {
#ifdef IMU_MTI630
  static int16_t gx, gy, gz;
  static int16_t ax, ay, az;

  switch (id) {
    case XCDI_AccelerationHR:
      ax = (data[0] << 8) | data[1];
      ay = (data[2] << 8) | data[3];
      az = (data[4] << 8) | data[5];
      imu->acc.x = ax * imu->config.acc_scale;
      imu->acc.y = ay * imu->config.acc_scale;
      imu->acc.z = az * imu->config.acc_scale;
      imu->acc_raw.x = ax;
      imu->acc_raw.y = ay;
      imu->acc_raw.z = az;
      imu->acc.timestamp = timestamp;
      imu->acc_raw.timestamp = timestamp;
      break;
    case XCDI_RateOfTurnHR:
      gx = (data[0] << 8) | data[1];
      gy = (data[2] << 8) | data[3];
      gz = (data[4] << 8) | data[5];
      imu->gyro.x = gx * imu->config.gyro_scale;
      imu->gyro.y = gy * imu->config.gyro_scale;
      imu->gyro.z = gz * imu->config.gyro_scale;
      imu->gyro_raw.x = gx;
      imu->gyro_raw.y = gy;
      imu->gyro_raw.z = gz;
      imu->gyro.timestamp = timestamp;
      imu->gyro_raw.timestamp = timestamp;
      break;
    default:
      break;
  }
#endif
}
