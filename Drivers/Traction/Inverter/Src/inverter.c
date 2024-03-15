#include "inverter.h"

void inverter_init(inverter_t *instance) {
  for (int i = 0; i < 2; i++) instance->value[i] = 0;
  inverter_init_bsp();
}

void inverter_set(inverter_t *instance) { inverter_set_bsp(instance); }
