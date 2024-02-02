/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "main.h"
#include "adc.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include <string.h>
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  /**
   * I2C scanner
   */
  HAL_UART_Transmit(&huart2, (uint8_t *)"Scanning I2C bus:\r\n", 19, 100);
  HAL_StatusTypeDef result;
  uint8_t i;
  for (i = 1; i < 128; i++)
  {
    /*
     * the HAL wants a left aligned i2c address
     * &hi2c1 is the handle
     * (uint16_t)(i<<1) is the i2c address left aligned
     * retries 2
     * timeout 2
     */
    result = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i << 1), 10, 100);
    if (result != HAL_OK) // HAL_ERROR or HAL_BUSY or HAL_TIMEOUT
    {
      HAL_UART_Transmit(&huart2, (uint8_t *)".", 1, 100); // No ACK received at that address
    }
    if (result == HAL_OK)
    {
      char buffer[32];
      int size = snprintf(buffer, 32, "0x%X", i);
      HAL_UART_Transmit(&huart2, (uint8_t *)buffer, size, 100); // Received an ACK at that address
    }
  }
  HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 100);
  /*
   * Camera init
   */
  /**
   * SD test
   */
  printf("\r\n~ SD card demo by kiwih ~\r\n\r\n");

  HAL_Delay(1000); // a short delay is important to let the SD card settle

  // some variables for FatFs
  FATFS FatFs;  // Fatfs handle
  FIL fil;      // File handle
  FRESULT fres; // Result after operations

  // Open the file system
  fres = f_mount(&FatFs, "", 1); // 1=mount now
  if (fres != FR_OK)
  {
    printf("f_mount error (%i)\r\n", fres);
    while (1)
      ;
  }

  // Let's get some statistics from the SD card
  DWORD free_clusters, free_sectors, total_sectors;

  FATFS *getFreeFs;

  fres = f_getfree("", &free_clusters, &getFreeFs);
  if (fres != FR_OK)
  {
    printf("f_getfree error (%i)\r\n", fres);
    while (1)
      ;
  }

  // Formula comes from ChaN's documentation
  total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
  free_sectors = free_clusters * getFreeFs->csize;

  printf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);

  // Now let's try to open file "test.txt"
  fres = f_open(&fil, "test.txt", FA_READ);
  if (fres != FR_OK)
  {
    printf("f_open error (%i)\r\n",fres);
    while (1)
      ;
  }
  printf("I was able to open 'test.txt' for reading!\r\n");

  // Read 30 bytes from "test.txt" on the SD card
  BYTE readBuf[30];

  // We can either use f_read OR f_gets to get data out of files
  // f_gets is a wrapper on f_read that does some string formatting for us
  TCHAR *rres = f_gets((TCHAR *)readBuf, 30, &fil);
  if (rres != 0)
  {
    printf("Read string from 'test.txt' contents: %s\r\n", readBuf);
  }
  else
  {
    printf("f_gets error (%i)\r\n", fres);
  }

  // Be a tidy kiwi - don't forget to close your file!
  f_close(&fil);

  // Now let's try and write a file "write.txt"
  fres = f_open(&fil, "write.txt", FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
  if (fres == FR_OK)
  {
    printf("I was able to open 'write.txt' for writing\r\n");
  }
  else
  {
    printf("f_open error (%i)\r\n", fres);
  }

  // Copy in a string
  strncpy((char *)readBuf, "a new file is made!", 19);
  UINT bytesWrote;
  fres = f_write(&fil, readBuf, 19, &bytesWrote);
  if (fres == FR_OK)
  {
    printf("Wrote %i bytes to 'write.txt'!\r\n", bytesWrote);
  }
  else
  {
    printf("f_write error (%i)\r\n",fres);
  }

  // Be a tidy kiwi - don't forget to close your file!
  f_close(&fil);

  // We're done, so de-mount the drive
  f_mount(NULL, "", 0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint32_t tick_count = 0;
  while (1)
  {
    // HAL_Delay(300);
    if (tick_count % 10000 == 0)
      HAL_UART_Transmit(&huart2, (uint8_t *)"alive\r\n", 7, 100);

    tick_count++;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
