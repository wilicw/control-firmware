/*
----------------------------------------------------------------------
File    : control.c
Purpose : Source file for the main control loop.
Revision: $Rev: 2024.11$
----------------------------------------------------------------------
*/

#include "control.h"

#include "config.h"
#include "events.h"
#include "inverter.h"

TX_THREAD control_thread;

extern TX_EVENT_FLAGS_GROUP event_flags;

void control_thread_entry(ULONG thread_input) {
  UINT status = TX_SUCCESS;

  // Wait for the filesystem to be loaded
  ULONG recv_events_flags = 0;
  status = tx_event_flags_get(&event_flags, EVENT_BIT(EVENT_FS_INIT), TX_AND,
                              &recv_events_flags, TX_WAIT_FOREVER);

  CONTROL_DEBUG("control main loop started\n");

  inverter_t *inverter_R = open_inverter_instance(0);
  inverter_t *inverter_L = open_inverter_instance(1);
  inverter_R->torque = 0;
  inverter_L->torque = 0;

  for (int i = 0; i < 100; i++) {
    inverter_send_torque(inverter_R);
    inverter_send_torque(inverter_L);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 4);  // 4Hz
  }

  // Sleep for 0.25 seconds
  tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 4);

  // Operate at 1kHz
  for (;;) {
    // Apply 10Nm torque to each wheel
    inverter_R->torque = 10;
    inverter_L->torque = 10;
    inverter_send_torque(inverter_R);
    inverter_send_torque(inverter_L);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 1000);
  }

  tx_thread_terminate(tx_thread_identify());
}
