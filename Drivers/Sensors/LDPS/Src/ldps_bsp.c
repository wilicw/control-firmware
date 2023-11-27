/*
----------------------------------------------------------------------
File    : ldps_bsp.c
Purpose : BSP(Board Support Package) for the linear displacement
          sernsor. Curently only implemented for STM32F407xx via HAL.
Revision: $Rev: 2023.43$
----------------------------------------------------------------------
*/

#include "ldps.h"

#ifdef USE_HAL_DRIVER

#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#endif

extern ADC_HandleTypeDef hadc1;

void ldps_bsp_init(ldps_t *ldps, size_t n) {
  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ldps, n) != HAL_OK) {
    LDPS_DEBUG("HAL_ADC_Start_DMA failed\n");
    while (1)
      ;
  }
}

void ldps_bsp_delay() { HAL_Delay(10); }

#endif
