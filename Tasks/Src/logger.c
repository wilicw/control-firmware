/*
----------------------------------------------------------------------
File    : logger.c
Purpose : Source code for the data logger task. Data acquisition from
          multiple sensors will be done in this task.
Revision: $Rev: 2023.49$
----------------------------------------------------------------------
*/

#include "logger.h"

#include "config.h"
#include "fx_api.h"
#include "stddef.h"

TX_THREAD logger_thread;

// Logger file objects
extern FX_MEDIA sdio_disk;
FX_FILE logger_file;

#if LDPS_ENABLE
// LDPS instance objects
#include "ldps.h"
extern ldps_t ldps[LDPS_N];
#endif

// Configuration instance objects
extern config_t config;

void logger_thread_entry(ULONG thread_input) {
  UINT status = FX_SUCCESS;
  LOGGER_DEBUG("Logger thread started\n");

  fx_file_create(&sdio_disk, "fsae-2024.log");
  status = fx_file_open(&sdio_disk, &logger_file, "fsae-2024.log",
                        FX_OPEN_FOR_WRITE);
  if (status != FX_SUCCESS) {
    SEGGER_RTT_printf(0, "fx_file_open failed with status 0x%02X\n", status);
    tx_thread_suspend(tx_thread_identify());
  }

  while (1) {
    char buf[128];

#if LDPS_ENABLE
    sprintf(buf, "%d", tx_time_get());
    for (size_t i = 0; i < LDPS_N; i++) {
      int16_t v = ldps_read(&ldps[i], &config.ldps_cal[i]);
      sprintf(buf + strlen(buf), ",%d", v);
    }
    strcat(buf, "\n");
    fx_file_write(&logger_file, buf, strlen(buf));
#endif

    fx_media_flush(&sdio_disk);
    tx_thread_sleep(1);
  }
}
