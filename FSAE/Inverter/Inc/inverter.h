#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum {
  INVERTER_PM100,
} inverter_type_t;

typedef struct {
  int16_t torque;
  int16_t torque_feedback;
  int16_t speed;
  uint32_t timestamp;
  uint16_t voltage;
  uint16_t current;
  uint8_t direction;
  inverter_type_t type;
  uint32_t hw_id;
  char priv_pool[256];
} inverter_t;

inverter_t *open_inverter_instance(uint32_t id);
void inverter_init(inverter_t *instance);
void inverter_send_torque(inverter_t *instance);

void inverter_bsp_init(inverter_t *instance);
void inverter_bsp_send_torque(inverter_t *instance);
void inverter_bsp_interrupt(inverter_t *instance, void *arg1, void *arg2);
void inverter_bsp_write_parameter(inverter_t *instance, uint16_t address,
                                  uint16_t value);
void inverter_bsp_read_parameter(inverter_t *instance, uint16_t address);
