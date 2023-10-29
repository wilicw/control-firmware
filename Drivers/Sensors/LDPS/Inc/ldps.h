/*
----------------------------------------------------------------------
File    : ldps.h
Purpose : Implementation of the LDPS and LDPS_BSP driver for the Lin-
          ear Displacement Sensor (LDPS).
Revision: $Rev: 2023.43$
----------------------------------------------------------------------
*/

#ifndef LDPS_H
#define LDPS_H

#include <stddef.h>
#include <stdint.h>

#ifdef DEBUG
#include "SEGGER_RTT.h"
#define LDPS_DEBUG(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#else
#define LDPS_DEBUG(...)
#endif

typedef struct {
  float scale;
  float offset;
} ldps_cal_t;

typedef struct {
  uint16_t dma_buffer;
} ldps_t;

void ldps_init(ldps_t *ldps, ldps_cal_t *cal, size_t n);
float ldps_read(ldps_t *ldps, ldps_cal_t *cal);

/* internal functions */
void ldps_bsp_init(ldps_t *ldps, size_t n);
void ldps_bsp_delay();

#endif
