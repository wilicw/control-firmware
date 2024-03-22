/*
----------------------------------------------------------------------
File    : config.h
Purpose : Implementation and functionilay definitation for ALL driver-
          -s and peripherals.
Revision: $Rev: 2024.12$
----------------------------------------------------------------------
*/

#pragma once

#include <stddef.h>

#ifdef DEBUG
#include "SEGGER_RTT.h"
#define CONFIG_DEBUG(...)                                           \
  {                                                                 \
    SEGGER_RTT_printf("%s:%s <%s>:", __FILE__, __LINE__, __func__); \
    SEGGER_RTT_printf(0, __VA_ARGS__);                              \
  }
#else
#define CONFIG_DEBUG(...)
#endif

#define CONFIG_FILENAME "config.json"
#define LDPS_ENABLE 1
#define IMU_ENABLE 1
#define WHEEL_ENABLE 1
#define LOGGER_ENABLE 1
#define INVERTER_ENABLE 1

// Config for ALL peripherals

/* Linear Displacement Sensor configs */
#if LDPS_ENABLE
#include "ldps.h"
#define LDPS_N 4
#endif

/* IMU configs */
#if IMU_ENABLE
// Define IMU interface type
#define IMU_CAN
// #define IMU_SPI

// Define IMU type
#define IMU_MTI630
// #define IMU_ADIS16467
#endif

#if WHEEL_ENABLE
#define WHEEL_N 4
#endif

/* Logger configs */
#if LOGGER_ENABLE
// Logger interface type (multiple can be defined)
#define LOGGER_USB_SERIAL
#define LOGGER_SD
#endif

/* Inverter configs */
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

enum {
  CONFIG_SUCCESS,
  CONFIG_ERR_PARSE,
  CONFIG_ERR_LEN,
};

config_t* open_config_instance(uint32_t id);
int config_load(config_t* instance, const char* json, size_t len);
