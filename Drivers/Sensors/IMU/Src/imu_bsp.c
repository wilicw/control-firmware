#include "config.h"
#include "imu.h"
#include "tx_api.h"

#ifdef USE_HAL_DRIVER

#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#endif

extern imu_t imu;

void imu_bsp_interrupt(void *arg) {
  static uint8_t rx_data[128];

#ifdef IMU_CAN
  static CAN_RxHeaderTypeDef rx_header;

  HAL_CAN_GetRxMessage((CAN_HandleTypeDef *)arg, CAN_RX_FIFO0, &rx_header,
                       rx_data);

  imu_update(&imu, rx_header.StdId, rx_data, rx_header.DLC, tx_time_get());
#endif
#ifdef IMU_SPI
  /* TODO: Implement SPI interrupt */
#endif
}
#endif
