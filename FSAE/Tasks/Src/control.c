/*
----------------------------------------------------------------------
File    : control.c
Purpose : Source file for the main control loop.
Revision: $Rev: 2024.11$
----------------------------------------------------------------------
*/

#include "control.h"

#include "SEGGER_RTT.h"
#include "adc.h"
#include "events.h"
#include "inverter.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "tx_port.h"
#include "utils.h"

#define IS_PRECHARGED (recv_events_flags & EVENT_BIT(EVENT_PRECHARGE))
#define IS_RECORDED (recv_events_flags & EVENT_BIT(EVENT_LOGGING))

static const float MAX_POWER_PRE_WHEEL = 1000;
static const float Ct = 0.95;
static const float MAX_TORQUE = 150;
static const float RTD_BPPS = 5 * 1000;  // 1MPa of average break pressure

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

static inline void control_stopped(void) {
  inverter_R->torque = 0;
  inverter_L->torque = 0;

  const uint8_t bpps_triggered =
      bpps_l->value > RTD_BPPS && bpps_r->value > RTD_BPPS;

  HAL_GPIO_WritePin(RTD_OUTPUT_GPIO_Port, RTD_OUTPUT_Pin, GPIO_PIN_RESET);

  if (HAL_GPIO_ReadPin(RTD_INPUT_GPIO_Port, RTD_INPUT_Pin) == GPIO_PIN_RESET &&
      bpps_triggered)
    control_state = CONTROL_RTD;
}

static inline void control_rtd(void) {
  // Ready to drive
  HAL_GPIO_WritePin(RTD_OUTPUT_GPIO_Port, RTD_OUTPUT_Pin, GPIO_PIN_SET);
  for (int i = 0; i < 1500; i++) {
    HAL_GPIO_TogglePin(BUZZER_OUTPUT_GPIO_Port, BUZZER_OUTPUT_Pin);
    HAL_Delay(1);
  }
  control_state = CONTROL_RUNNING;
}

static inline void control_running(void) {
  // FULL pedal position is 1000
  // Released pedal position is -50
  const float PEDAL_POSITION = (-apps_l->value + apps_r->value) / 2;
  const float AVERAGE_BREAK_PRESSURE = (bpps_l->value + bpps_r->value) / 2;

  SEGGER_RTT_printf(0, "Pedal: %4d\n", (int)PEDAL_POSITION);

  recv_events_flags = 0;
  tx_event_flags_get(&event_flags,
                     EVENT_BIT(EVENT_PRECHARGE) | EVENT_BIT(EVENT_LOGGING),
                     TX_OR, &recv_events_flags, TX_NO_WAIT);

  // if (!IS_PRECHARGED) {
  //   control_state = CONTROL_STOPPED;
  //   return;
  // }

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

    if (bpps_l->value > 3500 * bpps_l->cal.scale + bpps_l->cal.offset ||
        bpps_r->value > 3500 * bpps_r->cal.scale + bpps_r->cal.offset) {
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
    if (!ev471_triggered && AVERAGE_BREAK_PRESSURE > RTD_BPPS * 2) {
      SEGGER_RTT_printf(0, "Fault EV4.7.2\n");
      inverter_R->torque = inverter_L->torque = 0;
      ev471_triggered = 1;
      return;
    }
    if (ev471_triggered && PEDAL_POSITION <= 50) {
      ev471_triggered = 0;
    } else if (ev471_triggered) {
      inverter_R->torque = inverter_L->torque = 0;
      return;
    }
  }

  /* NOTE: Disable the inverter when either of the APPS sensors disconnected.
   */
  if (apps_l->value <= -1500 || apps_r->value >= 1500 || apps_l->value >= 200 ||
      apps_r->value <= -200) {
    inverter_R->torque = inverter_L->torque = 0;
    return;
  }

  if (PEDAL_POSITION < 60) {
    inverter_R->torque = inverter_L->torque = 0;
    return;
  }

  float ALLOW_TORQUE_R = _MIN_(MAX_TORQUE, MAX_POWER_PRE_WHEEL * 9.54929 * Ct /
                                               (_ABS_(inverter_R->speed) + 1));
  float ALLOW_TORQUE_L = _MIN_(MAX_TORQUE, MAX_POWER_PRE_WHEEL * 9.54929 * Ct /
                                               (_ABS_(inverter_L->speed) + 1));

  inverter_R->torque = PEDAL_POSITION * ALLOW_TORQUE_R / 1000;
  inverter_L->torque = PEDAL_POSITION * ALLOW_TORQUE_L / 1000;
}

void control_main(ULONG input) {
  adc_convert(apps_l);
  adc_convert(apps_r);
  adc_convert(bpps_l);
  adc_convert(bpps_r);

  switch (control_state) {
    case CONTROL_STOPPED:
      control_stopped();
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

  if (control_state <= CONTROL_RTD) inverter_R->torque = inverter_L->torque = 0;

  inverter_send_torque(inverter_R);
  inverter_send_torque(inverter_L);
}

void control_thread_entry(ULONG thread_input) {
  // Wait for the filesystem and config to be loaded
  ULONG recv_events_flags = 0;
  tx_event_flags_get(&event_flags, EVENT_BIT(EVENT_FS_INIT), TX_AND,
                     &recv_events_flags, TX_WAIT_FOREVER);

  CONTROL_DEBUG("control main loop started\n");

  apps_l = open_adc_instance(0);
  apps_r = open_adc_instance(1);
  bpps_l = open_adc_instance(2);
  bpps_r = open_adc_instance(3);

  adc_return_to_zero(apps_l);
  adc_return_to_zero(apps_r);

  inverter_R = open_inverter_instance(0);
  inverter_L = open_inverter_instance(1);

  control_state = CONTROL_STOPPED;

  if (HAL_GPIO_ReadPin(PRECHARGE_INPUT_GPIO_Port, PRECHARGE_INPUT_Pin) ==
      GPIO_PIN_SET)
    tx_event_flags_set(&event_flags, EVENT_BIT(EVENT_PRECHARGE), TX_OR);

  TX_TIMER control_timer;
  const ULONG delay = TX_TIMER_TICKS_PER_SECOND / 200;
  tx_timer_create(&control_timer, "C Timer", control_main, 0, delay, delay,
                  TX_AUTO_ACTIVATE);
  SEGGER_RTT_printf(0, "control thread started\n");
  tx_thread_terminate(tx_thread_identify());
}
