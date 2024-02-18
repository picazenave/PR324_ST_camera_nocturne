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

#define TOF_REPLAY
#ifdef TOF_REPLAY
#include "auto_gen.h"
#endif
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

static RANGING_SENSOR_Result_t Result;
#ifndef TOF_REPLAY
struct img_struct_t img_struct = {.img_buffer = {0}, .img_len = 0};
#endif
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
  printf("TOF init DONE\r\n");
  /**
   * Configure TOF
   */
  static RANGING_SENSOR_Capabilities_t Cap;
  static RANGING_SENSOR_ProfileConfig_t Profile;
  uint32_t Id;
  int32_t status = 0;

  CUSTOM_RANGING_SENSOR_ReadID(CUSTOM_VL53L5CX, &Id);
  CUSTOM_RANGING_SENSOR_GetCapabilities(CUSTOM_VL53L5CX, &Cap);

  // Profile.RangingProfile = RS_PROFILE_4x4_CONTINUOUS;
  Profile.RangingProfile = RS_PROFILE_8x8_CONTINUOUS;
  Profile.TimingBudget = 30; // TIMING_BUDGET;
  Profile.Frequency = 15;    // RANGING_FREQUENCY; /* Ranging frequency Hz (shall be consistent with TimingBudget value) */
  Profile.EnableAmbient = 0; /* Enable: 1, Disable: 0 */
  Profile.EnableSignal = 0;  /* Enable: 1, Disable: 0 */

  /* set the profile if different from default one */
  CUSTOM_RANGING_SENSOR_ConfigProfile(CUSTOM_VL53L5CX, &Profile);

  status = CUSTOM_RANGING_SENSOR_Start(CUSTOM_VL53L5CX, VL53L5CX_MODE_BLOCKING_CONTINUOUS);
  printf("TOF_ranging started\r\n");
#ifndef TOF_REPLAY
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
  // DetectionZone_t detect = {.acquisition = 0, .capture = 0, .initialization = 0};
  uint32_t tick_count = 0;

  uint8_t possible_target_index[64];
  uint8_t possible_target_i = 0;
  for (uint8_t i = 0; i < 64; i++)
    possible_target_index[i] = 255;

  /**
   * Init background
   */
  printf("Waiting for background to stabilize\r\n");
  uint16_t background[64];
  for (uint8_t i = 0; i < 5; i++) // 1.2sec
  {
    while (ext_it == 0)
      ;
    ext_it = 0;
    status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L5CX, &Result);
  }
  printf("DONE\r\n");

  for (uint8_t i = 0; i < 64; i++)
  {
    if ((Result.ZoneResult[i].Status[0] == 5 || Result.ZoneResult[i].Status[0] == 9) && (Result.ZoneResult[i].NumberOfTargets > 0))
      background[i] = Result.ZoneResult[i].Distance[0];
    else
      background[i] = 2550;

    printf("[%d]=%d", i, background[i]);
  }

  printf("Background init DONE\r\n");

  tick_count = 0;
  uint16_t threshold = 200;
  uint8_t is_tracking = 0;
  uint32_t last_tick = HAL_GetTick();

  while (1)
  {
    while (ext_it == 0)
      ; // wait for int to go low
    ext_it = 0;
    status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L5CX, &Result);
    // HAL_Delay(300);
    // if (tick_count % 1000 == 0)
    //   HAL_UART_Transmit(&huart2, (uint8_t *)"alive\r\n", 7, 100);
    if (status == BSP_ERROR_NONE)
    {
      int16_t temp = 0;
      possible_target_i = 0;
      uint8_t target_index = 0;
      is_tracking=0;
      for (uint8_t i = 0; i < 64; i++)
      {
        // ignore corners bc bigleux
        if (i != 0 && i != 1 && i != 8 && i != 6 && i != 7 && i != 15 && i != 48 && i != 56 && i != 57 && i != 55 && i != 62 && i != 63)
        {
          //if temp is <0 then we have something behind the 2550 value wich is mostly noise or lucky sensing
          temp = (int32_t)((int32_t)background[i] - (int32_t)Result.ZoneResult[i].Distance[0]);
          if ((temp > threshold) && (Result.ZoneResult[i].Status[0] == 5 || Result.ZoneResult[i].Status[0] == 9) && (Result.ZoneResult[i].NumberOfTargets > 0))
          {
            //printf("[%d]=%d",i,temp);
            possible_target_index[possible_target_i] = i;
            possible_target_i++;
            is_tracking = 1;
          }
        }
      }

      uint16_t x = 0, y = 0;
      uint16_t distance_to_center = 255;
      if (is_tracking == 1)
      {
        // find center of mass in x,y
        for (uint8_t i = 0; i < possible_target_i; i++)
        {
          x += possible_target_index[i] - (possible_target_index[i] / 8) * 8;
          y += (possible_target_index[i] / 8);
        }
        x = x / possible_target_i;
        y = y / possible_target_i;
        // convert back to index
        distance_to_center = distance_matrix[x + y * 8];
        target_index = x + y * 8;
      }
      tick_count++;

      // transmit fps
      printf("tick=%ld||counter=%ld||ms=%ld||is_tracking=%d||distance=%d||target_index=%d||target_distance=%ld\r\n", HAL_GetTick(), tick_count, HAL_GetTick() - last_tick, is_tracking, distance_to_center, target_index, Result.ZoneResult[target_index].Distance[0]);
      last_tick = HAL_GetTick();
    }
    else
      printf("error BSP TOF\r\n");

#ifndef TOF_REPLAY
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
#endif
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
  ext_it = GPIO_Pin;
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
