#include "inverter.h"

void inverter_init(inverter_t *instance) {
  for (int i = 0; i < 2; i++) instance->value[i] = 0;
  inverter_bsp_init();
}

void inverter_set(inverter_t *instance) { inverter_bsp_set(instance); }
