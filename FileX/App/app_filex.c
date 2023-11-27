/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_filex.c
 * @author  MCD Application Team
 * @brief   FileX applicative file
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
#include "app_filex.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
extern void Error_Handler(void);

TX_THREAD fx_app_thread;
uint32_t fx_sd_media_memory[FX_STM32_SD_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)];
FX_MEDIA sdio_disk;
FX_FILE fx_file;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void fx_app_thread_entry(ULONG thread_input);
/* USER CODE END PFP */

/**
 * @brief  Application FileX Initialization.
 * @param memory_ptr: memory pointer
 * @retval int
 */
UINT MX_FileX_Init(VOID *memory_ptr) {
  UINT ret = FX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;

  /* USER CODE BEGIN MX_FileX_MEM_POOL */
  VOID *pointer;
  static const ULONG fx_memory_size = 1024;
  ret = tx_byte_allocate(byte_pool, &pointer, fx_memory_size, TX_NO_WAIT);
  if (ret != TX_SUCCESS) {
    return ret;
  }
  /* USER CODE END MX_FileX_MEM_POOL */

  /* USER CODE BEGIN MX_FileX_Init */
  ret = tx_thread_create(&fx_app_thread, "fx_thread", fx_app_thread_entry, 0,
                         pointer, fx_memory_size, 1, 1, 1, TX_AUTO_START);
  if (ret != TX_SUCCESS) {
    return ret;
  }

  fx_system_initialize();
  /* USER CODE END MX_FileX_Init */
  return ret;
}

/* USER CODE BEGIN 1 */
void fx_app_thread_entry(ULONG thread_input) {
  UINT status = FX_SUCCESS;
  status =
      fx_media_open(&sdio_disk, "SDIO", fx_stm32_sd_driver, (VOID *)FX_NULL,
                    (VOID *)fx_sd_media_memory, sizeof(fx_sd_media_memory));

  if (status != FX_SUCCESS) {
    SEGGER_RTT_printf(0, "fx_media_open failed with status 0x%02X\n", status);
    Error_Handler();
  }

  SEGGER_RTT_printf(0, "fx_media_open success\n");

  status = fx_file_open(&sdio_disk, &fx_file, "config.txt", FX_OPEN_FOR_READ);
  if (status != FX_SUCCESS) {
    SEGGER_RTT_printf(0, "fx_file_open failed with status 0x%02X\n", status);
    Error_Handler();
  }

  SEGGER_RTT_printf(0, "fx_file_open success\n");

  CHAR buffer[128];
  ULONG bytes_read;
  status = fx_file_read(&fx_file, buffer, 128, &bytes_read);
  if (status != FX_SUCCESS) {
    SEGGER_RTT_printf(0, "fx_file_read failed with status 0x%02X\n", status);
    Error_Handler();
  }

  buffer[bytes_read] = '\0';
  SEGGER_RTT_printf(0, "fx_file_read success\n");
  SEGGER_RTT_printf(0, "Read %lu bytes\n", bytes_read);
  SEGGER_RTT_printf(0, "Content: %s\n", buffer);

  status = fx_file_close(&fx_file);
  if (status != FX_SUCCESS) {
    SEGGER_RTT_printf(0, "fx_file_close failed with status 0x%02X\n", status);
    Error_Handler();
  }

  SEGGER_RTT_printf(0, "fx_file_close success\n");

  while (1) {
    tx_thread_sleep(1000);
  }
}
/* USER CODE END 1 */
