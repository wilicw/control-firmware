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

#include "jsmn.h"

#define CONFIG_FILENAME "config.json"
#define ADC_ENABLE 0
#define IMU_ENABLE 1
#define WHEEL_ENABLE 1
#define LOGGER_ENABLE 1
#define INVERTER_ENABLE 1

// Config for ALL peripherals

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

enum {
  CONFIG_SUCCESS,
  CONFIG_ERR_PARSE,
  CONFIG_ERR_LEN,
};

typedef int (*config_hook_t)(void* instance, void* data, const char* json,
                             jsmntok_t* tok);

int config_load(const char* json, size_t len, void* instance, void* data,
                config_hook_t hook, const char* target);
