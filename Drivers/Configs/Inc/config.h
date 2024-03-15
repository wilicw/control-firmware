/*
----------------------------------------------------------------------
File    : config.h
Purpose : Implementation and functionilay definitation for ALL driver-
          -s and peripherals.
Revision: $Rev: 2023.48$
----------------------------------------------------------------------
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#ifdef DEBUG
#include "SEGGER_RTT.h"
#define CONFIG_DEBUG(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#else
#define CONFIG_DEBUG(...)
#endif

#define CONFIG_FILENAME "config.json"

// Config for ALL peripherals

/* Linear Displacement Sensor configs */
#define LDPS_ENABLE 1

#if LDPS_ENABLE
#include "ldps.h"

#define LDPS_N 4
#endif

/* IMU configs */
#define IMU_ENABLE 1

#if IMU_ENABLE
// Define IMU interface type
#define IMU_CAN
// #define IMU_SPI

// Define IMU type
#define IMU_MTI630
// #define IMU_ADIS16467
#endif

#define WHEEL_ENABLE 1
#if WHEEL_ENABLE
#include "wheel.h"
#define WHEEL_N 4
#endif

/* Logger configs */
#define LOGGER_ENABLE 1

#if LOGGER_ENABLE
// Logger interface type (multiple can be defined)
#define LOGGER_USB_SERIAL
#define LOGGER_SD
#endif

/* Inverter configs */
#define INVERTER_ENABLE 1

#if INVERTER_ENABLE
// Drive wheel number
#define INVERTER_DRIVE_WHEEL 2
// Inverter type
#define INVERTER_PM100DZ
#endif

typedef struct {
#if LDPS_ENABLE
  // Linear displacement sensors (LDPS)
  ldps_cal_t ldps_cal[LDPS_N];
#endif
} config_t;

extern config_t config;

int config_init(const char *config_json, size_t len);

#endif
