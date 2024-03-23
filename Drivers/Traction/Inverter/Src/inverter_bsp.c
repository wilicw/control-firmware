#include "inverter.h"

#ifdef USE_HAL_DRIVER

#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#endif

extern CAN_HandleTypeDef hcan1;

typedef struct {
  CAN_TxHeaderTypeDef tx_handler;
  uint8_t buffer[8];
  uint32_t txmailbox;
} pm100_t;

void inverter_bsp_init(inverter_t *instance) {
  switch (instance->type) {
    case INVERTER_PM100: {
      pm100_t priv = {
          .tx_handler =
              {
                  .StdId = 0x000,
                  .ExtId = 0x01,
                  .RTR = CAN_RTR_DATA,
                  .IDE = CAN_ID_STD,
                  .DLC = 8,
                  .TransmitGlobalTime = DISABLE,
              },
          .buffer = {0, 0, 0, 0, 0, 0, 0, 0},
          .txmailbox = 0,
      };
      *((pm100_t *)(&instance->priv_pool)) = priv;
      break;
    }
    default:
      break;
  }
}

void inverter_bsp_set_hw_id(inverter_t *instance) {
  switch (instance->type) {
    case INVERTER_PM100: {
      ((pm100_t *)(&instance->priv_pool))->tx_handler.StdId =
          instance->hw_id + 0x20;
      break;
    }
    default:
      break;
  }
}

void inverter_bsp_set_direction(inverter_t *instance) {
  switch (instance->type) {
    case INVERTER_PM100: {
      ((pm100_t *)(&instance->priv_pool))->buffer[4] = instance->direction;
      break;
    }
    default:
      break;
  }
}

void inverter_bsp_send_torque(inverter_t *instance) {
  switch (instance->type) {
    case INVERTER_PM100: {
      int16_t __torque = instance->torque * 10;
      uint16_t torque = *(uint16_t *)&__torque;
      ((pm100_t *)(&instance->priv_pool))->buffer[1] = (torque >> 8) & 0xFF;
      ((pm100_t *)(&instance->priv_pool))->buffer[0] = torque & 0xFF;
      if (!torque) ((pm100_t *)(&instance->priv_pool))->buffer[5] = 0x00;
      HAL_CAN_AddTxMessage(&hcan1,
                           &((pm100_t *)(&instance->priv_pool))->tx_handler,
                           ((pm100_t *)(&instance->priv_pool))->buffer,
                           &((pm100_t *)(&instance->priv_pool))->txmailbox);
      while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3)
        ;
      break;
    }
    default:
      break;
  }
}

void inverter_bsp_interrupt(inverter_t *instance, void *arg1, void *arg2) {
  switch (instance->type) {
    case INVERTER_PM100: {
      CAN_RxHeaderTypeDef *rx_header = (CAN_RxHeaderTypeDef *)arg1;
      uint8_t *rx_data = (uint8_t *)arg2;
      const uint8_t rx_id = rx_header->StdId;
      if (rx_id == 0x05 + instance->hw_id) {
        /* Motor Position Information
         * Byte#  Description
         * 0,1    Motor angle
         * 2,3    Motor velocity
         * 4,5    Electrical Output Frequency
         * 6,7    Delta Resolver Filtered
         */
        instance->speed = *(uint16_t *)&rx_data[2];
      } else if (rx_id == 0x07 + instance->hw_id) {
        /* Voltage Information
         * Byte#  Description
         * 0,1    DC Bus Voltage
         * 2,3    Output Voltage High Voltage
         * 4,5    VAB_Vd_Voltage High Voltage
         * 6,7    VBC_Vq_Voltage High Voltage
         */
        instance->voltage = *(uint16_t *)&rx_data[0];
      }
      break;
    }
    default:
      break;
  }
}

#endif
