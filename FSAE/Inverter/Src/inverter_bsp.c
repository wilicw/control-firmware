#include "SEGGER_RTT.h"
#include "inverter.h"
#ifdef USE_HAL_DRIVER

#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#endif

extern CAN_HandleTypeDef hcan1;
static uint32_t txmailbox;

typedef struct {
  CAN_TxHeaderTypeDef tx_handler;
  uint8_t buffer[8];
  uint8_t fault;
} pm100_t;

static inline void inverter_bsp_set_direction(inverter_t *instance) {
  switch (instance->type) {
    case INVERTER_PM100: {
      ((pm100_t *)(&instance->priv_pool))->buffer[4] = instance->direction;
      break;
    }
    default:
      break;
  }
}

void inverter_bsp_write_parameter(inverter_t *instance, uint16_t address,
                                  uint16_t value) {
  switch (instance->type) {
    case INVERTER_PM100: {
      uint8_t buffer[8] = {address & 0xFF, address >> 8, 1, 0,
                           value & 0xFF,   value >> 8,   0, 0};
      ((pm100_t *)(&instance->priv_pool))->tx_handler.StdId ^= 0x01;
      if (HAL_CAN_AddTxMessage(&hcan1,
                               &((pm100_t *)(&instance->priv_pool))->tx_handler,
                               buffer, &txmailbox) != HAL_OK) {
        SEGGER_RTT_printf(0, "Failed to write parameter\n");
      }
      while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3)
        ;
      ((pm100_t *)(&instance->priv_pool))->tx_handler.StdId ^= 0x01;
      break;
    }
    default:
      break;
  }
}

void inverter_bsp_read_parameter(inverter_t *instance, uint16_t address) {
  switch (instance->type) {
    case INVERTER_PM100: {
      uint8_t buffer[8] = {address & 0xFF, address >> 8, 0, 0, 0, 0, 0, 0};
      ((pm100_t *)(&instance->priv_pool))->tx_handler.StdId ^= 0x01;
      if (HAL_CAN_AddTxMessage(&hcan1,
                               &((pm100_t *)(&instance->priv_pool))->tx_handler,
                               buffer, &txmailbox) != HAL_OK) {
        SEGGER_RTT_printf(0, "Failed to read parameter\n");
      }
      while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3)
        ;
      ((pm100_t *)(&instance->priv_pool))->tx_handler.StdId ^= 0x01;
      break;
    }
    default:
      break;
  }
}

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
          .fault = 1,
      };
      *((pm100_t *)(&instance->priv_pool)) = priv;
      ((pm100_t *)(&instance->priv_pool))->tx_handler.StdId =
          instance->hw_id + 0x20;
      break;
    }
    default:
      break;
  }
  inverter_bsp_set_direction(instance);
  // IQ Limit
  inverter_bsp_read_parameter(instance, 100);
}

void inverter_bsp_send_torque(inverter_t *instance) {
  switch (instance->type) {
    case INVERTER_PM100: {
      if (((pm100_t *)(&instance->priv_pool))->fault) {
        SEGGER_RTT_printf(0, "Fault detected, clearing fault\n");
        inverter_bsp_write_parameter(instance, 20, 0);
        ((pm100_t *)(&instance->priv_pool))->fault = 0;
      }

      int16_t __torque = instance->torque * 10;
      uint16_t torque = *(uint16_t *)&__torque;
      ((pm100_t *)(&instance->priv_pool))->buffer[1] = (torque >> 8) & 0xFF;
      ((pm100_t *)(&instance->priv_pool))->buffer[0] = torque & 0xFF;
      if (!torque)
        ((pm100_t *)(&instance->priv_pool))->buffer[5] = 0x00;
      else
        ((pm100_t *)(&instance->priv_pool))->buffer[5] = 0x01;
      if (HAL_CAN_AddTxMessage(&hcan1,
                               &((pm100_t *)(&instance->priv_pool))->tx_handler,
                               ((pm100_t *)(&instance->priv_pool))->buffer,
                               &txmailbox) != HAL_OK) {
        SEGGER_RTT_printf(0, "Failed to send torque\n");
      }
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
      const uint16_t rx_id = rx_header->StdId;
      if (rx_id == 0x05 + instance->hw_id) {
        /* Motor Position Information
         * Byte#  Description
         * 0,1    Motor angle
         * 2,3    Motor velocity
         * 4,5    Electrical Output Frequency
         * 6,7    Delta Resolver Filtered
         */
        instance->speed = *(uint16_t *)&rx_data[2];
      } else if (rx_id == 0x06 + instance->hw_id) {
        /* Motor Current Information
         * Byte#  Description
         * 0,1    Phase A Current
         * 2,3    Phase B Current
         * 4,5    Phase C Current
         * 6,7    DC Current
         */
        instance->current = *(uint16_t *)&rx_data[6];
      } else if (rx_id == 0x07 + instance->hw_id) {
        /* Voltage Information
         * Byte#  Description
         * 0,1    DC Bus Voltage
         * 2,3    Output Voltage High Voltage
         * 4,5    VAB_Vd_Voltage High Voltage
         * 6,7    VBC_Vq_Voltage High Voltage
         */
        instance->voltage = *(uint16_t *)&rx_data[0];
      } else if (rx_id == 0x0A + instance->hw_id) {
        /* Motor Status Information
         * Byte#  Description
         * 0,1    VSM State
         * 2      Inverter State
         */
        ((pm100_t *)(&instance->priv_pool))->fault =
            (*(uint16_t *)&rx_data[0] == 7);
      } else if (rx_id == 0x0C + instance->hw_id) {
        /* Torque & Timer Information
         * Byte#  Description
         * 0,1    Torque Command
         * 2,3    Torque Feedback
         * 4,5,6,7Timer
         */
        instance->torque_feedback = *(int16_t *)&rx_data[2];
        instance->timestamp = *(uint32_t *)&rx_data[4];
      } else if (rx_id == 0x22 + instance->hw_id) {
        uint16_t address = (rx_data[1] << 8) | rx_data[0];
        uint16_t value = (rx_data[5] << 8) | rx_data[4];
        uint8_t status = rx_data[2];
        SEGGER_RTT_printf(0, "I(0x%02x)Parameter %d (%d) 0x%04x\n",
                          instance->hw_id, address, status, value);
      }
      break;
    }
    default:
      break;
  }
}

#endif
