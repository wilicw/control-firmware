/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_threadx.c
 * @author  MCD Application Team
 * @brief   ThreadX applicative file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "config.h"
#include "control.h"
#include "init.h"
#include "logger.h"
#include "tx_api.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
TX_EVENT_FLAGS_GROUP event_flags;

extern void Error_Handler(void);

extern TX_THREAD logger_thread;
extern TX_THREAD init_thread;
extern TX_THREAD control_thread;

/* Default ThreadX application
 * It will suspend forever after ThreadX initialization
 * */
TX_THREAD app_thread;
void app_thread_entry(ULONG thread_input) {
  while (1)
    if (tx_thread_suspend(&app_thread) != TX_SUCCESS)
      SEGGER_RTT_printf(0, "tx_thread_suspend failed\n");
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
 * @brief  Application ThreadX Initialization.
 * @param memory_ptr: memory pointer
 * @retval int
 */
UINT App_ThreadX_Init(VOID *memory_ptr) {
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_Init */
  static const ULONG app_memory_size = 256;

  VOID *pointer;
  ret = tx_byte_allocate(byte_pool, &pointer, app_memory_size, TX_NO_WAIT);
  if (ret != TX_SUCCESS) {
    SEGGER_RTT_printf(0, "tx_byte_allocate failed\n");
    Error_Handler();
  }

  ret =
      tx_thread_create(&app_thread, "app_thread", app_thread_entry, 0, pointer,
                       app_memory_size, TX_MAX_PRIORITIES - 1,
                       TX_MAX_PRIORITIES - 1, TX_NO_TIME_SLICE, TX_AUTO_START);

  /* System Initialization Thread
   * - Priority: 1
   * - Stack size: 1024 bytes
   * - Auto start: NO
   *   Dependencies: FileX
   * - Description: This thread defined in init.h and implement its
   *   functionality at init.c. It can only be started after FileX
   *   initialization.
   */
  VOID *init_pointer;
  int init_priority = 1;
  tx_byte_allocate(byte_pool, &init_pointer, 1024, TX_NO_WAIT);
  ret = tx_thread_create(&init_thread, "init_thread", init_thread_entry, 0,
                         init_pointer, 1024, init_priority, init_priority,
                         TX_NO_TIME_SLICE, TX_AUTO_START);

  /* Data Logger Thread
   * - Priority: 5
   * - Stack size: 256 bytes
   * - Auto start: NO
   *   Dependencies: FileX
   * - Description: This thread defined in logger.h and implement its
   *   functionality at logger.c. It can only be started after FileX
   *   initialization. Only enabled if LOGGER_ENABLE is defined.
   */

#if LOGGER_ENABLE
  VOID *logger_pointer;
  int logger_priority = 5;
  tx_byte_allocate(byte_pool, &logger_pointer, app_memory_size, TX_NO_WAIT);
  ret = tx_thread_create(&logger_thread, "logger_thread", logger_thread_entry,
                         0, logger_pointer, app_memory_size, logger_priority,
                         logger_priority, TX_NO_TIME_SLICE, TX_AUTO_START);
#endif

  /* Control Thread
   * - Priority: 4
   * - Stack size: 256 bytes
   * - Auto start: NO
   *   Dependencies: FileX
   * - Description: This thread defined in control.h and implement its
   *   functionality at control.c. It can only be started after FileX
   *   initialization. Only enabled if CONTROL_ENABLE is defined.
   */
  VOID *control_pointer;
  int control_priority = 4;
  tx_byte_allocate(byte_pool, &control_pointer, 2048, TX_NO_WAIT);
  ret =
      tx_thread_create(&control_thread, "control_thread", control_thread_entry,
                       0, control_pointer, app_memory_size, control_priority,
                       control_priority, TX_NO_TIME_SLICE, TX_AUTO_START);

  /* USER CODE END App_ThreadX_Init */

  return ret;
}

/**
 * @brief  MX_ThreadX_Init
 * @param  None
 * @retval None
 */
void MX_ThreadX_Init(void) {
  /* USER CODE BEGIN  Before_Kernel_Start */
  /* Event Flags Group Initialization */
  UINT ret = tx_event_flags_create(&event_flags, "event_flags");
  if (ret != TX_SUCCESS) {
    SEGGER_RTT_printf(0, "tx_event_flags_create failed\n");
    Error_Handler();
  }
  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
