#include "config.h"
#include "imu.h"
#include "tx_api.h"

#ifdef USE_HAL_DRIVER

#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#endif

extern imu_t imu;

/*
 * IMU BSP interrupt handler
 * Only works for STM32 HAL drivers
 * @param CAN_RxHeaderTypeDef* arg1 or SPI_HandleTypeDef* arg1
 * @param void* arg2 any arbitrary arguments
 */
void imu_bsp_interrupt(void *arg1, void *arg2) {
#ifdef IMU_CAN
  CAN_RxHeaderTypeDef *rx_header = (CAN_RxHeaderTypeDef *)arg1;
  uint8_t *rx_data = (uint8_t *)arg2;

  imu_update(&imu, rx_header->StdId, rx_data, rx_header->DLC);
#endif

#ifdef IMU_SPI
  /* TODO: Implement SPI interrupt */
#endif
}
#endif
