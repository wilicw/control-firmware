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

typedef struct {
  void *handler;
  uint8_t channel;
} wheel_hw_t;

typedef struct {
  wheel_hw_t hw;
  float rpm;
  uint16_t last_tick;
  uint16_t tick;
  uint8_t ticks_pre_rev;
} wheel_t;

wheel_t *open_wheel_instance(uint32_t id);
void wheel_init(wheel_t *wheel);
void wheel_update(wheel_t *wheel);

void wheel_bsp_init(wheel_t *wheel);
void wheel_bsp_interrupt(wheel_t *wheel, void *handler);

#endif
