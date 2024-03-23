#include <stdlib.h>
#include <string.h>

#include "adc.h"

static inline float static_stof(const char* s, size_t n) {
  static char buf[32];
  strncpy(buf, s, n);
  buf[n] = '\0';
  return atof(buf);
}

int adc_config_hook(adc_t* instance, uint32_t id, const char* json,
                    jsmntok_t* t) {
  if (t[1].type != JSMN_ARRAY) {
    return -1;
  }

  jsmntok_t* g = &t[id + 2];
  float val = static_stof(json + g->start, g->end - g->start);
  instance->cal.scale = val;
  return 0;
}
