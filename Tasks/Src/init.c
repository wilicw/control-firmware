/*
----------------------------------------------------------------------
File    : init.c
Purpose : Source file for the system initialization.
Revision: $Rev: 2023.49$
----------------------------------------------------------------------
*/

#include "init.h"

#include "config.h"
#include "events.h"
#include "fx_api.h"
#include "imu.h"
#include "inverter.h"
#include "ldps.h"
#include "wheel.h"

TX_THREAD init_thread;

extern TX_EVENT_FLAGS_GROUP event_flags;

extern config_t config;

/* System Peripherals and Drivers Instances */
// Instance of the linear displacement sensors
#if LDPS_ENABLE
ldps_t ldps[LDPS_N];
#endif

#if IMU_ENABLE
// Instance of the IMU
imu_t imu;
#endif

// Instance of the wheel sensors
#if WHEEL_ENABLE
wheel_t wheel[WHEEL_N];
#endif

#if INVERTER_ENABLE
inverter_t inverter;
#endif

// Config json file objects
extern FX_MEDIA sdio_disk;
FX_FILE config_file;

void init_thread_entry(ULONG thread_input) {
  UINT status = FX_SUCCESS;

  // Wait for the filesystem to be loaded
  ULONG recv_events_flags = 0;
  status = tx_event_flags_get(&event_flags, EVENT_BIT(EVENT_FS_INIT), TX_AND,
                              &recv_events_flags, TX_WAIT_FOREVER);

  INIT_DEBUG("Init started\n");

  char buf[512];
  ULONG len;

  status =
      fx_file_open(&sdio_disk, &config_file, CONFIG_FILENAME, FX_OPEN_FOR_READ);
  if (status != FX_SUCCESS) {
    INIT_DEBUG("Failed to open config file\n");
    tx_thread_terminate(tx_thread_identify());
  }

  status = fx_file_read(&config_file, buf, sizeof(buf), &len);
  if (status != FX_SUCCESS) {
    INIT_DEBUG("Failed to read config file\n");
    tx_thread_terminate(tx_thread_identify());
  }

  status = fx_file_close(&config_file);
  if (status != FX_SUCCESS) {
    INIT_DEBUG("Failed to close config file\n");
    tx_thread_terminate(tx_thread_identify());
  }

  config_init(buf, len);

#if LDPS_ENABLE
  ldps_init(&ldps[0], &config.ldps_cal[0], LDPS_N);
#endif

#if IMU_ENABLE
  imu_init(&imu);
#endif

#if WHEEL_ENABLE
  wheel_init(wheel, WHEEL_N);
#endif

#if INVERTER_ENABLE
  inverter_init(&inverter);
#endif

  tx_event_flags_set(&event_flags, EVENT_BIT(EVENT_CONFIG_LOADED), TX_OR);

  INIT_DEBUG("Init finished\n");

  tx_thread_terminate(tx_thread_identify());
}
