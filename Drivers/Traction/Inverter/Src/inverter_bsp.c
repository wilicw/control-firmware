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
static uint8_t clear_fault_buffer[8] = {20, 0, 1, 0, 0, 0, 0, 0};
static uint32_t txmailbox;
#endif

void inverter_bsp_init() {
#ifdef INVERTER_PM100DZ
  right_torque_handler.StdId = 0x0C0;
  right_torque_handler.ExtId = 0x01;
  right_torque_handler.RTR = CAN_RTR_DATA;
  right_torque_handler.IDE = CAN_ID_STD;
  right_torque_handler.DLC = 8;
  right_torque_handler.TransmitGlobalTime = DISABLE;

  left_torque_handler.StdId = 0x070;
  left_torque_handler.ExtId = 0x01;
  left_torque_handler.RTR = CAN_RTR_DATA;
  left_torque_handler.IDE = CAN_ID_STD;
  left_torque_handler.DLC = 8;
  left_torque_handler.TransmitGlobalTime = DISABLE;

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
  left_torque_buffer[4] = 1;
  left_torque_buffer[5] = 0;
  left_torque_buffer[6] = 0;
  left_torque_buffer[7] = 0;

#endif
}

void inverter_bsp_set(inverter_t *instance) {
#ifdef INVERTER_PM100DZ
  static uint8_t fault = 1;
  static uint8_t i = 0;

  if (fault) {
    right_torque_handler.StdId ^= 0x1;
    left_torque_handler.StdId ^= 0x1;
    if (HAL_CAN_AddTxMessage(&hcan1, &right_torque_handler, clear_fault_buffer,
                             &txmailbox) != HAL_OK)
      SEGGER_RTT_printf(0, "Failed to clear right fault\n");
    if (HAL_CAN_AddTxMessage(&hcan1, &left_torque_handler, clear_fault_buffer,
                             &txmailbox) != HAL_OK)
      SEGGER_RTT_printf(0, "Failed to clear left fault\n");
    fault = 0;
    right_torque_handler.StdId ^= 0x1;
    left_torque_handler.StdId ^= 0x1;
  }

  int16_t right_torque = instance->value[0] * 10;
  int16_t left_torque = instance->value[1] * 10;

  right_torque_buffer[1] = (right_torque >> 8) & 0xFF;
  right_torque_buffer[0] = right_torque & 0xFF;
  left_torque_buffer[1] = (left_torque >> 8) & 0xFF;
  left_torque_buffer[0] = left_torque & 0xFF;

  if (right_torque <= 0 || left_torque <= 0)
    right_torque_buffer[5] = left_torque_buffer[5] = 0x00;
  else
    right_torque_buffer[5] = left_torque_buffer[5] = 0x01;

  if (HAL_CAN_AddTxMessage(&hcan1, &right_torque_handler, right_torque_buffer,
                           &txmailbox) != HAL_OK)
    SEGGER_RTT_printf(0, "Failed to send right torque\n");
  if (HAL_CAN_AddTxMessage(&hcan1, &left_torque_handler, left_torque_buffer,
                           &txmailbox) != HAL_OK)
    SEGGER_RTT_printf(0, "Failed to send left torque\n");

  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3)
    ;
#endif
}

void inverter_bsp_interrupt(void *arg1, void *arg2) {
#ifdef INVERTER_PM100D
  switch (rx_header->StdId) {
    case 0x55:
    case 0xA5:
      /* Motor Position Information
       * Byte#  Description
       * 0,1    Motor angle
       * 2,3    Motor velocity
       * 4,5    Electrical Output Frequency
       * 6,7    Delta Resolver Filtered
       */
      break;
    case 0x57:
    case 0xA7:
      /* Voltage Information
       * Byte#  Description
       * 0,1    DC Bus Voltage
       * 2,3    Output Voltage High Voltage
       * 4,5    VAB_Vd_Voltage High Voltage
       * 6,7    VBC_Vq_Voltage High Voltage
       */
      break;
    default:
      break;
  }
#endif
}

#endif
