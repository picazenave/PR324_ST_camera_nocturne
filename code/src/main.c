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
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include <string.h>
#include "camera.h"
#include "detection_zone.h"
#include "app_tof.h"
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
volatile uint8_t uart2_tx_done = 0;
volatile uint8_t uart1_rx_done = 0;
volatile uint16_t ext_it = 0;

struct img_struct_t img_struct = {.img_buffer = {0}, .img_len = 0};
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
 // MX_I2C1_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize the VL53L5CX sensor */
  HAL_GPIO_WritePin(TOF_PWR_EN_GPIO_Port, TOF_PWR_EN_Pin, GPIO_PIN_SET);
  // LPn pins set to High,
  HAL_GPIO_WritePin(TOF_LPn_C_GPIO_Port, TOF_LPn_C_Pin, GPIO_PIN_SET);
  // I2C reset pins set to Low,
  HAL_GPIO_WritePin(TOF_I2C1_RST_GPIO_Port, TOF_I2C1_RST_Pin, GPIO_PIN_RESET);
  MX_VL53L5CX_ToF_Init();
  
#if 0
  HAL_StatusTypeDef status = HAL_ERROR;

  status = camera_init(0); // 0 not default config
  CHECK_HAL_STATUS_OR_PRINT(status);
  status = sd_init();
  CHECK_HAL_STATUS_OR_PRINT(status);
  /**
   * Test jpg saving
   */
  printf("\r\n=====Test jpg saving=====\r\n\r\n");
  printf(" get_camera_jpg  \r\n");
  status = get_camera_jpg(&img_struct);
  CHECK_HAL_STATUS_OR_PRINT(status);
  printf(" save_picture_sd  \r\n");
  status = save_picture_sd(&img_struct);
  CHECK_HAL_STATUS_OR_PRINT(status);
  printf(" send_jpg_uart2  \r\n");
  status = send_jpg_uart2(&img_struct);
  CHECK_HAL_STATUS_OR_PRINT(status);
  printf("\r\n\r\n DONE  \r\n");
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  DetectionZone_t detect={.acquisition=0,.capture=0,.initialization=0};
  uint32_t tick_count = 0;
  while (1)
  {
    // HAL_Delay(300);
    if (tick_count % 1000 == 0)
      HAL_UART_Transmit(&huart2, (uint8_t *)"alive\r\n", 7, 100);

    // HAL_Delay(10);
    tick_count++;
    int status = MX_VL53L5CX_ToF_Process(&detect);
    printf("******************************\r\n");
    switch (status)
    {
    case INITIALISATION:
      printf("ToF status : Initialisation\r\n");
      break;
    case ACQUISITION:
      printf("ToF status : Search an animal\r\n");
      break;
    case ANIMAL:
      printf("ToF status : Following an animal\r\n");
      break;
    case CAPTURE:
      printf("ToF status : Capture the animal\r\n");
      break;
    default:
      printf("ToF status : Error\r\n");
      break;
    }
    printf("******************************\r\n");
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  ext_it=GPIO_Pin;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    uart1_rx_done = 1;
  }
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2)
  {
    uart2_tx_done = 1;
  }
}

void i2c_scanner()
{
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
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  printf("\r\n\r\n Error HANDLER Error HANDLER Error HANDLER Error HANDLER \r\n\r\n");
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
