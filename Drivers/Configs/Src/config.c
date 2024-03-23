/*
----------------------------------------------------------------------
File    : config.c
Purpose : Configuration file parser for ALL drivers and peripherals.
          Which only accepts JSON format.
Revision: $Rev: 2024.12$
----------------------------------------------------------------------
*/

#include "config.h"

#include <stdlib.h>
#include <string.h>

int config_load(const char* json, size_t len, void* instance, void* data,
                config_hook_t hook, const char* target) {
  /* Init config manually to prevent missing config in JSON files */

  /* JSON parser */
  int json_r;
  jsmn_parser p;
  jsmntok_t t[16];
  jsmn_init(&p);

  json_r = jsmn_parse(&p, json, len, t, sizeof(t) / sizeof(t[0]));
  if (json_r < 0) {
    return CONFIG_ERR_LEN;
  } else {
    int i = 1;
    while (i < json_r) {
      if (strncmp(json + t[i].start, target, t[i].end - t[i].start) == 0)
        return hook(instance, data, json, &t[i]);
      else
        i++;
    }
  }
  return CONFIG_SUCCESS;
}
