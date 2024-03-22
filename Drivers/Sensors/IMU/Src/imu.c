#include "imu.h"

#include "imu_mti600.h"

#define MAX_INSTANCE 3
static imu_t instance_list[MAX_INSTANCE];

imu_t* open_imu_instance(uint32_t id) {
  if (id >= MAX_INSTANCE) return NULL;
  return &instance_list[id];
}

void imu_set_type(imu_t* imu, imu_type_t type) {
  switch (type) {
    case IMU_MTI600:
      imu->config.acc_scale = XCDI_AccelerationHR_SCALE;
      imu->config.gyro_scale = XCDI_RateOfTurnHR_SCALE;
      break;
    default:
      break;
  }

  imu->acc.timestamp = 0;
  imu->acc_raw.timestamp = 0;
  imu->gyro.timestamp = 0;
  imu->gyro_raw.timestamp = 0;
  imu->mag.timestamp = 0;
  imu->mag_raw.timestamp = 0;
}
