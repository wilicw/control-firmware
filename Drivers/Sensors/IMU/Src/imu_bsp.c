#include "config.h"
#include "imu.h"

#ifdef USE_HAL_DRIVER

#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#endif

extern imu_t imu;

void imu_bsp_interrupt(void *arg) {
#ifdef IMU_CAN
  CAN_HandleTypeDef *hcan = (CAN_HandleTypeDef *)arg;
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[128];

  int16_t gx, gy, gz;
  int16_t ax, ay, az;

  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);

  imu_update(&imu, rx_header.StdId, rx_data, rx_header.DLC, HAL_GetTick());
#endif
#ifdef IMU_SPI
  /* TODO: Implement SPI interrupt */
#endif
}
#endif
