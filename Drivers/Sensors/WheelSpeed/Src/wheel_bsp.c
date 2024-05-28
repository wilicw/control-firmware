
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

void wheel_bsp_init(wheel_t *wheel) {
  HAL_TIM_IC_Start_IT(wheel->hw.handler, wheel->hw.channel);
}

void wheel_bsp_interrupt(wheel_t *w, void *arg) {
  TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)arg;
  TIM_HandleTypeDef *handler = w->hw.handler;
  if (htim->Instance == handler->Instance &&
      htim->Channel == (1 << (WHEEL_TIM_CHANNEL_INT(w->hw.channel) - 1))) {
    w->tick = HAL_TIM_ReadCapturedValue(htim, w->hw.channel);
    wheel_update(w);
    w->last_tick = w->tick;
  }
}

#endif
