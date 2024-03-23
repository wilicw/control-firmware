#include "imu.h"
#include "imu_mti600.h"

#ifdef USE_HAL_DRIVER

#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#endif

static inline void imu_bsp_interrupt_mti600(imu_t *imu,
                                            CAN_RxHeaderTypeDef *rx_header,
                                            uint8_t *rx_data) {
  int16_t gx, gy, gz;
  int16_t ax, ay, az;

  switch (rx_header->StdId) {
    case XCDI_AccelerationHR:
      ax = (rx_data[0] << 8) | rx_data[1];
      ay = (rx_data[2] << 8) | rx_data[3];
      az = (rx_data[4] << 8) | rx_data[5];
      imu->acc.x = ax * imu->config.acc_scale;
      imu->acc.y = ay * imu->config.acc_scale;
      imu->acc.z = az * imu->config.acc_scale;
      imu->acc_raw.x = ax;
      imu->acc_raw.y = ay;
      imu->acc_raw.z = az;
      imu->acc.timestamp++;
      imu->acc_raw.timestamp++;
      break;
    case XCDI_RateOfTurnHR:
      gx = (rx_data[0] << 8) | rx_data[1];
      gy = (rx_data[2] << 8) | rx_data[3];
      gz = (rx_data[4] << 8) | rx_data[5];
      imu->gyro.x = gx * imu->config.gyro_scale;
      imu->gyro.y = gy * imu->config.gyro_scale;
      imu->gyro.z = gz * imu->config.gyro_scale;
      imu->gyro_raw.x = gx;
      imu->gyro_raw.y = gy;
      imu->gyro_raw.z = gz;
      imu->gyro.timestamp++;
      imu->gyro_raw.timestamp++;
      break;
    default:
      break;
  }
}

/*
 * IMU BSP interrupt handler
 * Only works for STM32 HAL drivers
 * @param CAN_RxHeaderTypeDef* arg1 or SPI_HandleTypeDef* arg1
 * @param void* arg2 any arbitrary arguments
 */
void imu_bsp_interrupt(imu_t *imu, void *arg1, void *arg2) {
  switch (imu->type) {
    case IMU_MTI600:
      imu_bsp_interrupt_mti600(imu, arg1, arg2);
      break;
    default:
      break;
  }
}
#endif
