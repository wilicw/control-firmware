/*
----------------------------------------------------------------------
File    : config.c
Purpose : Configuration file parser for ALL drivers and peripherals.
          Which only accepts JSON format.
Revision: $Rev: 2023.48$
----------------------------------------------------------------------
*/

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include "jsmn.h"

config_t config;

static inline float static_stof(const char *s, size_t n) {
  static char buf[32];
  strncpy(buf, s, n);
  buf[n] = '\0';
  return atof(buf);
}

int config_init(const char *s, size_t len) {
  /* Init config manually to prevent missing config in JSON files */

  /* Linear displacement sensors */
#if LDPS_ENABLE
  for (int i = 0; i < LDPS_N; i++) config.ldps_cal[i].scale = 1.0f;
#endif

  /* JSON parser */
  int json_r;
  jsmn_parser p;
  jsmntok_t t[16];
  jsmn_init(&p);

  json_r = jsmn_parse(&p, s, len, t, sizeof(t) / sizeof(t[0]));
  if (json_r < 0) {
    CONFIG_DEBUG("Failed to parse JSON: %d\n", json_r);
    return -1;
  } else {
    int i = 1;
    while (i < json_r) {
      if (0) {
        /* Never gets here */
      }
#if LDPS_ENABLE
      /* JSON parser for the linear displacement sensors
       * Example:
       * { "ldps": [1.0, 2.0, 3.5, -1.5] }
       */
      else if (strncmp(s + t[i].start, "ldps", t[i].end - t[i].start) == 0) {
        if (t[i + 1].type != JSMN_ARRAY) {
          CONFIG_DEBUG("Expected array\n");
          return -1;
        }

        size_t config_ldps_n = t[i + 1].size;

        if (config_ldps_n == LDPS_N) {
          for (int j = 0; j < config_ldps_n; j++) {
            jsmntok_t *g = &t[i + j + 2];
            float val = static_stof(s + g->start, g->end - g->start);
            config.ldps_cal[j].scale = val;
          }
        } else {
          CONFIG_DEBUG("LDPS parse error: Expected %d elements, gets %d\n",
                       LDPS_N, config_ldps_n);
        }

        i += t[i + 1].size + 1;
      }
#endif
      else {
        i++;
      }
    }
  }
  return 0;
}
