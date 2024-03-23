#include "inverter.h"

#define MAX_INSTANCES 4
static inverter_t instance_list[MAX_INSTANCES];

inverter_t *open_inverter_instance(uint32_t id) {
  if (id >= MAX_INSTANCES) return NULL;
  return &instance_list[id];
}

void inverter_init(inverter_t *instance) {
  instance->torque = 0;
  inverter_bsp_set_hw_id(instance);
  inverter_bsp_set_direction(instance);
}

void inverter_send_torque(inverter_t *instance) {
  inverter_bsp_send_torque(instance);
}
