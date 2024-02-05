#include "config.h"
#include "wheel.h"

#ifdef USE_HAL_DRIVER

#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#endif

#define US_PRE_TICK 1UL

#define WHEEL_TIM_CHANNEL_INT(CHANNEL)   \
  ((CHANNEL) == TIM_CHANNEL_1            \
       ? 1                               \
       : ((CHANNEL) == TIM_CHANNEL_2 ? 2 \
                                     : ((CHANNEL) == TIM_CHANNEL_3 ? 3 : 4)))

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

void wheel_bsp_init(wheel_t *wheel) {
  wheel[0].hw.handler = &htim3;
  wheel[0].hw.channel = TIM_CHANNEL_2;
  wheel[1].hw.handler = &htim3;
  wheel[1].hw.channel = TIM_CHANNEL_4;
  wheel[2].hw.handler = &htim4;
  wheel[2].hw.channel = TIM_CHANNEL_2;
  wheel[3].hw.handler = &htim4;
  wheel[3].hw.channel = TIM_CHANNEL_4;

  for (uint8_t i = 0; i < WHEEL_N; i++)
    HAL_TIM_IC_Start_IT(wheel[i].hw.handler, wheel[i].hw.channel);
}

void wheel_bsp_interrupt(void *arg) {
  extern wheel_t wheel[WHEEL_N];
  TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)arg;
  for (uint8_t i = 0; i < WHEEL_N; i++) {
    wheel_t *w = &wheel[i];
    TIM_HandleTypeDef *handler = w->hw.handler;
    if (htim->Instance == handler->Instance &&
        htim->Channel == (1 << (WHEEL_TIM_CHANNEL_INT(w->hw.channel) - 1))) {
      w->tick = HAL_TIM_ReadCapturedValue(htim, w->hw.channel);
      wheel_update(w, US_PRE_TICK);
      w->last_tick = w->tick;
    }
  }
}

#endif
