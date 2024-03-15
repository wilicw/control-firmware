#include "inverter.h"

#ifdef USE_HAL_DRIVER

#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#endif

extern inverter_t inverter;

#ifdef INVERTER_PM100DZ
#include "stm32f4xx_hal_can.h"

extern CAN_HandleTypeDef hcan1;
CAN_TxHeaderTypeDef right_torque_handler;
CAN_TxHeaderTypeDef left_torque_handler;
static uint8_t right_torque_buffer[8];
static uint8_t left_torque_buffer[8];
#endif

void inverter_init_bsp() {
#ifdef INVERTER_PM100DZ
  right_torque_handler.StdId = 0x0C0;
  right_torque_handler.ExtId = 0x01;
  right_torque_handler.RTR = CAN_RTR_DATA;
  right_torque_handler.IDE = CAN_ID_STD;
  right_torque_handler.DLC = 8;
  right_torque_handler.TransmitGlobalTime =
      DISABLE;  // time trigger must be turned ON

  left_torque_handler.StdId = 0x070;
  left_torque_handler.ExtId = 0x01;
  left_torque_handler.RTR = CAN_RTR_DATA;
  left_torque_handler.IDE = CAN_ID_STD;
  left_torque_handler.DLC = 8;
  left_torque_handler.TransmitGlobalTime =
      DISABLE;  // time trigger must be turned ON

  right_torque_buffer[0] = 0;
  right_torque_buffer[1] = 0;
  right_torque_buffer[2] = 0;
  right_torque_buffer[3] = 0;
  right_torque_buffer[4] = 1;
  right_torque_buffer[5] = 0;
  right_torque_buffer[6] = 0;
  right_torque_buffer[7] = 0;

  left_torque_buffer[0] = 0;
  left_torque_buffer[1] = 0;
  left_torque_buffer[2] = 0;
  left_torque_buffer[3] = 0;
  left_torque_buffer[4] = 0;
  left_torque_buffer[5] = 0;
  left_torque_buffer[6] = 0;
  left_torque_buffer[7] = 0;

#endif
}

void invert_bsp_set(inverter_t *instance) {
#ifdef INVERTER_PM100DZ
  int16_t right_torque = instance->value[0];
  int16_t left_torque = instance->value[1];

  right_torque_buffer[0] = (right_torque >> 8) & 0xFF;
  right_torque_buffer[1] = right_torque & 0xFF;
  left_torque_buffer[0] = (left_torque >> 8) & 0xFF;
  left_torque_buffer[1] = left_torque & 0xFF;

  if (right_torque <= 0 || left_torque <= 0)
    right_torque_buffer[5] = left_torque_buffer[5] = 0x00;
  else
    right_torque_buffer[5] = left_torque_buffer[5] = 0x01;

  HAL_CAN_AddTxMessage(&hcan1, &right_torque_handler, right_torque_buffer,
                       (uint32_t *)CAN_TX_MAILBOX0);
  HAL_CAN_AddTxMessage(&hcan1, &left_torque_handler, left_torque_buffer,
                       (uint32_t *)CAN_TX_MAILBOX0);
#endif
}

void inverter_bsp_interrupt(void *arg) {}

#endif
