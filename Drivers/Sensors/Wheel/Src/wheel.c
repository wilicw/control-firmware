#include "wheel.h"

#include "utils.h"

#define WHEEL_HOLE_PRE_REV 18

void wheel_init(wheel_t *wheel, size_t n) {
  if (!wheel) return;
  if (!n) return;
  WHEEL_DEBUG("wheel_init: %d\n", n);
  while (n--) {
    wheel[n].rpm = 0;
    wheel[n].last_tick = 0;
    wheel[n].tick = 0;
  }

  wheel_bsp_init(wheel);
}

void wheel_update(wheel_t *wheel, uint32_t us_per_tick) {
  uint32_t delta_t = INTERVAL_16U(wheel->tick, wheel->last_tick);
  wheel->rpm = 6000000.0 / us_per_tick / WHEEL_HOLE_PRE_REV / delta_t;
}
