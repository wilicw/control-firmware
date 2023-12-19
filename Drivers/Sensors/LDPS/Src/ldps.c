/*
----------------------------------------------------------------------
File    : ldps.c
Purpose : User dirver functions for the linear displacement sernsor.
Revision: $Rev: 2023.43$
----------------------------------------------------------------------
*/

#include "ldps.h"

void ldps_init(ldps_t *ldps, ldps_cal_t *cal, size_t n) {
  if (!ldps) return;
  if (!cal) return;
  if (!n) return;

  LDPS_DEBUG("ldps_init: %d\n", n);
  ldps_bsp_init(ldps, n);

  ldps_bsp_delay();

  while (n--) cal[n].offset = ldps[n].dma_buffer;
}

void ldps_deinit() {}

float ldps_read(ldps_t *ldps, ldps_cal_t *cal) {
  return (float)(ldps->dma_buffer) * cal->scale - cal->offset;
}
