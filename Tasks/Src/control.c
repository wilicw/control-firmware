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

#if INVERTER_ENABLE
extern inverter_t inverter;
#endif

void control_thread_entry(ULONG thread_input) {
  UINT status = TX_SUCCESS;

  // Wait for the filesystem to be loaded
  ULONG recv_events_flags = 0;
  status = tx_event_flags_get(&event_flags, EVENT_BIT(EVENT_FS_INIT), TX_AND,
                              &recv_events_flags, TX_WAIT_FOREVER);

  CONTROL_DEBUG("control main loop started\n");

  inverter.value[0] = 0;
  inverter.value[1] = 0;

  for (int i = 0; i < 100; i++) {
    inverter_set(&inverter);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 4);  // 4Hz
  }

  // Sleep for 0.25 seconds
  tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 4);

  // Operate at 1kHz
  for (;;) {
    // Apply 10Nm torque to each wheel
    inverter.value[0] = 10;
    inverter.value[1] = 10;
    inverter_set(&inverter);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 1000);
  }

  tx_thread_terminate(tx_thread_identify());
}
