#ifndef INVERTER_H
#define INVERTER_H

#include <stddef.h>
#include <stdint.h>

#include "config.h"

typedef struct {
  uint16_t value[INVERTER_DRIVE_WHEEL];
} inverter_t;

void inverter_init(inverter_t *instance);
void inverter_set(inverter_t *instance);

void inverter_init_bsp();
void inverter_set_bsp(inverter_t *instance);
void inverter_bsp_interrupt(void *arg);

#endif
