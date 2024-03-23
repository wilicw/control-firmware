#ifndef UTILS_H
#define UTILS_H

#define INTERVAL_32U(current_time, last_time) \
  (((0xFFFFFFFF - (last_time)) + (current_time)) + 1)

#define INTERVAL_16U(current_time, last_time) \
  (((0xFFFF - (last_time)) + (current_time)) + 1)

#endif
