
#include "wheel.h"

#include "utils.h"

#define MAX_INSTANCE 4

static wheel_t wheel_instance[MAX_INSTANCE];

wheel_t *open_wheel_instance(uint32_t id) {
  if (id >= MAX_INSTANCE) return NULL;
  return &wheel_instance[id];
}

void wheel_init(wheel_t *wheel) {
  if (!wheel) return;
  wheel_bsp_init(wheel);
}

void wheel_update(wheel_t *wheel) {
  uint32_t delta_t = INTERVAL_16U(wheel->tick, wheel->last_tick);
  wheel->rpm = 6000000.0 / wheel->ticks_pre_rev / delta_t;
}
