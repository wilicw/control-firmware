/*
----------------------------------------------------------------------
File    : wheel.h
Purpose : Implementation of the bosch 2-wire wheel speed sensors.
Revision: $Rev: 2024.05$
----------------------------------------------------------------------
*/

#ifndef WHEEL_H
#define WHEEL_H

#include <stddef.h>
#include <stdint.h>

#ifdef DEBUG
#include "SEGGER_RTT.h"
#define WHEEL_DEBUG(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#else
#define WHEEL_DEBUG(...)
#endif

typedef struct {
  void *handler;
  uint8_t channel;
} wheel_hw_t;

typedef struct {
  wheel_hw_t hw;
  float rpm;
  uint16_t last_tick;
  uint16_t tick;
} wheel_t;

void wheel_init(wheel_t *wheel, size_t n);
void wheel_update(wheel_t *wheel, uint32_t us_per_tick);

void wheel_bsp_init(wheel_t *wheel);
void wheel_bsp_interrupt(void *handler);

#endif
