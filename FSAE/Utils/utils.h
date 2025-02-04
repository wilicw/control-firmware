#ifndef UTILS_H
#define UTILS_H

#define INTERVAL_32U(current_time, last_time) \
  (((0xFFFFFFFF - (last_time)) + (current_time)) + 1)

#define INTERVAL_16U(current_time, last_time) \
  (((0xFFFF - (last_time)) + (current_time)) + 1)

#define _ABS_(x) ((x) < 0 ? -(x) : (x))

#define _MIN_(a, b) ((a) < (b) ? (a) : (b))

#endif
