/*
----------------------------------------------------------------------
File    : control.c
Purpose : Source file for the main control loop.
Revision: $Rev: 2024.11$
----------------------------------------------------------------------
*/

#include "control.h"

#include "adc.h"
#include "config.h"
#include "events.h"
#include "inverter.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "tx_port.h"

#define IS_PRECHARGED (recv_events_flags & EVENT_BIT(EVENT_PRECHARGE))
#define IS_RECORDED (recv_events_flags & EVENT_BIT(EVENT_LOGGING))

static const float TORQUE_FACTOR = 10.0f;
static const float MAX_TORQUE = 150 * TORQUE_FACTOR;
static const float REGEN_TORQUE = (REGEN_ENABLE ? -30 : 0) * TORQUE_FACTOR;
static const float BSE_MAX = 75.0f;
const static float MAX_PEDAL_POSITION = MAX_TORQUE - REGEN_TORQUE;

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

  static const float CALIBRATION_APPS = 0.1 * MAX_TORQUE;
  static const float RTD_BPPS = 50.0f;
  const uint8_t apps_triggered =
      apps_l->value < -CALIBRATION_APPS && apps_r->value > CALIBRATION_APPS;
  const uint8_t bpps_triggered =
      bpps_l->value > RTD_BPPS && bpps_r->value > RTD_BPPS;

  HAL_GPIO_WritePin(RTD_OUTPUT_GPIO_Port, RTD_OUTPUT_Pin, GPIO_PIN_RESET);

  if (HAL_GPIO_ReadPin(RTD_INPUT_GPIO_Port, RTD_INPUT_Pin) == GPIO_PIN_RESET) {
    if (apps_triggered && !bpps_triggered) {
      control_state = CONTROL_CALIBRATE;
    } else if (!apps_triggered && bpps_triggered) {
      control_state = CONTROL_RTD;
    }
  }
}

static inline void control_calibrate() {
  // Calibrate the APPS and Steering wheel sensors
  apps_l->cal.scale *= -MAX_PEDAL_POSITION / apps_l->value;
  apps_r->cal.scale *= MAX_PEDAL_POSITION / apps_r->value;

  if (HAL_GPIO_ReadPin(RTD_INPUT_GPIO_Port, RTD_INPUT_Pin) == GPIO_PIN_SET)
    control_state = CONTROL_STOPPED;
}

static inline void control_rtd() {
  // Ready to drive

  bpps_l->cal.scale *= BSE_MAX / bpps_l->value;
  bpps_r->cal.scale *= BSE_MAX / bpps_r->value;

  HAL_GPIO_WritePin(RTD_OUTPUT_GPIO_Port, RTD_OUTPUT_Pin, GPIO_PIN_SET);
  for (int i = 0; i < 1500; i++) {
    HAL_GPIO_TogglePin(BUZZER_OUTPUT_GPIO_Port, BUZZER_OUTPUT_Pin);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 1000);
  }
  control_state = CONTROL_RUNNING;
}

static inline void control_running() {
  const float __COMMAND_TORQUE =
      (-apps_l->value + apps_r->value) / 2 + REGEN_TORQUE;
  const float COMMAND_TORQUE =
      (__COMMAND_TORQUE > MAX_TORQUE
           ? MAX_TORQUE
           : (__COMMAND_TORQUE < REGEN_TORQUE ? REGEN_TORQUE
                                              : __COMMAND_TORQUE)) /
      TORQUE_FACTOR;
  const float COMMAND_BREAK = (bpps_l->value + bpps_r->value) / 2;

  /* WARN: BYPASS the rule while recoding */
  if (!IS_RECORDED) {
    /* NOTE:
     * T.4.3.3 (...skip) Any failure of the BSE or BSE wiring that persists
     * more than 100 msec must be detectable by the controller and treated like
     * an implausibility and power to the (IC) electronic throttle / (EV)
     * Motor(s) must be immediately stopped completely.
     * -
     * T.4.3.4 When an analogue signal is used, the BSE sensors will be
     * considered to have failed when they achieve an open circuit or short
     * circuit condition which generates a signal outside of the normal
     * operating range, for example <0.5 V or >4.5 V.
     */

    if (bpps_l->value < -5 || bpps_r->value < -5 ||
        bpps_l->value > BSE_MAX * 1.1f || bpps_r->value > BSE_MAX * 1.1f) {
      SEGGER_RTT_printf(0, "Fault T4.3.3 or T4.3.4\n");
      inverter_R->torque = inverter_L->torque = 0;
      return;
    }

    /* NOTE: EV.4.7.1 Must monitor for the two conditions:
     *        • The mechanical brakes are engaged EV.4.6, T.3.2.4
     *        • The APPS signals more than 25% Pedal Travel EV.4.5
     * -
     * EV.4.7.2 If the two conditions in EV.4.7.1 occur at the same time:
     *   a. Power to the Motor(s) must be immediately and completely shut down
     *   b. The Motor shut down must stay active until the APPS signals less
     * than 5% Pedal Travel, with or without brake operation */
    static uint8_t ev471_triggered = 0;
    if (!ev471_triggered && COMMAND_BREAK > 25 &&
        COMMAND_TORQUE >= MAX_TORQUE * 0.15f) {
      SEGGER_RTT_printf(0, "Fault EV4.7.2\n");
      inverter_R->torque = inverter_L->torque = 0;
      ev471_triggered = 1;
      return;
    }
    if (ev471_triggered && COMMAND_TORQUE <= MAX_TORQUE * 0.05f) {
      ev471_triggered = 0;
    }
  }

  /* NOTE: Disable the torque output if the APPS is less than 5% of the
     maximum torque output.
  */
  if (apps_l->value >= -MAX_TORQUE * 0.05f &&
      apps_r->value <= MAX_TORQUE * 0.05f) {
    inverter_R->torque = inverter_L->torque = 0;
    return;
  }

  inverter_R->torque = inverter_L->torque = COMMAND_TORQUE;
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

  control_state = CONTROL_STOPPED;

  if (HAL_GPIO_ReadPin(PRECHARGE_INPUT_GPIO_Port, PRECHARGE_INPUT_Pin) ==
      GPIO_PIN_SET)
    tx_event_flags_set(&event_flags, EVENT_BIT(EVENT_PRECHARGE), TX_OR);

  while (1) {
    adc_convert(apps_l);
    adc_convert(apps_r);
    adc_convert(bpps_l);
    adc_convert(bpps_r);

    recv_events_flags = 0;
    tx_event_flags_get(&event_flags,
                       EVENT_BIT(EVENT_PRECHARGE) | EVENT_BIT(EVENT_LOGGING),
                       TX_OR, &recv_events_flags, TX_NO_WAIT);

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
