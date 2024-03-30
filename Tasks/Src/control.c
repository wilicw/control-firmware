/*
----------------------------------------------------------------------
File    : control.c
Purpose : Source file for the main control loop.
Revision: $Rev: 2024.11$
----------------------------------------------------------------------
*/

#include "control.h"

#include "adc.h"
#include "events.h"
#include "inverter.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "tx_port.h"

static const float MAX_TORQUE = 30.0f;
TX_THREAD control_thread;
extern TX_EVENT_FLAGS_GROUP event_flags;
static control_state_t control_state = CONTROL_STOPPED;
static adc_t *apps_l = NULL;
static adc_t *apps_r = NULL;
static adc_t *bpps_l = NULL;
static adc_t *bpps_r = NULL;
static inverter_t *inverter_R = NULL;
static inverter_t *inverter_L = NULL;
static ULONG recv_events_flags = 0;

static inline void control_stopped() {
  inverter_R->torque = 0;
  inverter_L->torque = 0;

  static const float CALIBRATION_APPS = 10.0f;
  static const float RTD_BPPS = 50.0f;
  uint8_t apps_triggered =
      apps_l->value < -CALIBRATION_APPS && apps_r->value > CALIBRATION_APPS;
  uint8_t bpps_triggered = bpps_l->value > RTD_BPPS && bpps_r->value > RTD_BPPS;
  uint8_t precharge_complete = recv_events_flags & EVENT_BIT(EVENT_PRECHARGE);

  HAL_GPIO_WritePin(RTD_OUTPUT_GPIO_Port, RTD_OUTPUT_Pin, GPIO_PIN_RESET);

  if (HAL_GPIO_ReadPin(RTD_INPUT_GPIO_Port, RTD_INPUT_Pin) == GPIO_PIN_RESET) {
    if (apps_triggered && !bpps_triggered) {
      control_state = CONTROL_CALIBRATE;
    } else if (!apps_triggered && bpps_triggered && precharge_complete) {
      control_state = CONTROL_RTD;
    }
  }
}

static inline void control_calibrate() {
  // Calibrate the APPS and Steering wheel sensors
  apps_l->cal.scale *= -MAX_TORQUE / apps_l->value;
  apps_r->cal.scale *= MAX_TORQUE / apps_r->value;

  if (HAL_GPIO_ReadPin(RTD_INPUT_GPIO_Port, RTD_INPUT_Pin) == GPIO_PIN_SET)
    control_state = CONTROL_STOPPED;
}

static inline void control_rtd() {
  // Ready to drive
  HAL_GPIO_WritePin(RTD_OUTPUT_GPIO_Port, RTD_OUTPUT_Pin, GPIO_PIN_SET);
  for (int i = 0; i < 1500; i++) {
    HAL_GPIO_TogglePin(BUZZER_OUTPUT_GPIO_Port, BUZZER_OUTPUT_Pin);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 1000);
  }
  control_state = CONTROL_RUNNING;
}

static inline void control_running() {
  /* NOTE: Disable the torque output if the APPS is less than 5% of the maximum
           torque output.
  */
  if (apps_l->value >= -MAX_TORQUE * 0.05f &&
      apps_r->value <= MAX_TORQUE * 0.05f) {
    inverter_R->torque = 0;
    inverter_L->torque = 0;
    return;
  }

  /* NOTE: If precharge pin goes low (HV is below 60V), disable the torque
           output.
  */
  if (recv_events_flags & EVENT_BIT(EVENT_PRECHARGE)) {
    inverter_R->torque = 0;
    inverter_L->torque = 0;
    return;
  }

  inverter_R->torque = inverter_L->torque =
      (-apps_l->value + apps_r->value) / 2;
}

void control_thread_entry(ULONG thread_input) {
  UINT status = TX_SUCCESS;

  // Wait for the filesystem and config to be loaded
  ULONG recv_events_flags = 0;
  status = tx_event_flags_get(
      &event_flags, EVENT_BIT(EVENT_FS_INIT) | EVENT_BIT(EVENT_CONFIG_LOADED),
      TX_AND, &recv_events_flags, TX_WAIT_FOREVER);

  CONTROL_DEBUG("control main loop started\n");

  apps_l = open_adc_instance(0);
  apps_r = open_adc_instance(1);
  bpps_l = open_adc_instance(2);
  bpps_r = open_adc_instance(3);

  inverter_R = open_inverter_instance(0);
  inverter_L = open_inverter_instance(1);

  while (1) {
    adc_convert(apps_l);
    adc_convert(apps_r);
    adc_convert(bpps_l);
    adc_convert(bpps_r);

    recv_events_flags = 0;
    tx_event_flags_get(&event_flags, EVENT_BIT(EVENT_PRECHARGE), TX_OR,
                       &recv_events_flags, TX_WAIT_FOREVER);

    switch (control_state) {
      case CONTROL_STOPPED:
        control_stopped();
        break;
      case CONTROL_CALIBRATE:
        control_calibrate();
        break;
      case CONTROL_RTD:
        control_rtd();
        break;
      case CONTROL_RUNNING:
        control_running();
        break;
      default:
        break;
    }

    if (control_state != CONTROL_RUNNING) {
      inverter_R->torque = 0;
      inverter_L->torque = 0;
    }

    inverter_send_torque(inverter_R);
    inverter_send_torque(inverter_L);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 1000);
  }

  tx_thread_terminate(tx_thread_identify());
}
