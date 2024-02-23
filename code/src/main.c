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
#include "app_tof.h"
#include "pir_lum.h"
#include "tracking.h"
//#include "nb_iot.h"

#define TOF_REPLAY
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
volatile uint8_t uart2_tx_done = 1;
volatile uint8_t uart1_rx_done = 0;
volatile uint16_t ext_it = 0;

RANGING_SENSOR_Result_t Result;
struct target_t target_struct = {.target_distance_to_center = 255, .target_index = 0};
struct img_struct_t img_struct = {.img_buffer = {0}, .img_len = 0};

#define CAMERA_CATPURE_THRESHOLD 4    // N*66ms averaging
#define PIR_CAPTURE_INTERVAL 500      // ms
#define LUM_CAPTURE_INTERVAL 30000    // ms
#define CAMERA_CAPTURE_INTERVAL 30000 // ms
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
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_StatusTypeDef status = HAL_ERROR;
  status = init_nb_iot();
  CHECK_HAL_STATUS_OR_PRINT(status);
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
  // printf(" send_jpg_uart2  \r\n");
  // status = send_jpg_uart2(&img_struct, 1);
  // CHECK_HAL_STATUS_OR_PRINT(status);
  printf("\r\n\r\n DONE  \r\n");

  tracking_init_tof();
  tracking_init_background(&Result);

  // TODO check no problem no init
  //  for (uint8_t i = 0; i < 64; i++)
  //    possible_target_index[i] = 255;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  /**
   * Task priority : TOF -> Capture camera (& save) -> PIR & Light sensor
   */

  Luminosite_t luminosite = {.light_sensor = 0, .day_moment = JOUR};
  uint32_t tick_count = 0;
  uint32_t last_time = HAL_GetTick();
  uint32_t last_time_PIR = HAL_GetTick();
  uint32_t last_time_Lum = HAL_GetTick();
  uint32_t last_time_camera = HAL_GetTick();
  uint8_t camera_should_capture = 0; // used for averaging data
  uint8_t PIR_set = 0;
  uint8_t last_PIR_set = 0;
  float last_distance_to_center = 255.f;
  uint8_t last_target_status = NO_TARGET;
  while (1)
  {
    // HAL_Delay(300);
    // if (tick_count % 100 == 0)
    //   printf("camera_should_capture=%d\r\n", camera_should_capture);

    /**
     * TOF task every 66ms
     */
    if (HAL_GetTick() - last_time_camera > CAMERA_CAPTURE_INTERVAL)
    {
      last_time_camera = HAL_GetTick();
      // status = get_camera_jpg(&img_struct);
      // CHECK_HAL_STATUS_OR_PRINT(status);
      // // while (!uart2_tx_done)
      // //   ;
      // // uart2_tx_done = 0;
      // status = send_jpg_uart2(&img_struct, 1);
      // CHECK_HAL_STATUS_OR_PRINT(status);
    }

    if (ext_it != 0)
    {
      ext_it = 0;
      status = tracking_get_target(&target_struct, &Result);
      CHECK_HAL_STATUS_OR_PRINT(status);
      // printf("tick=%ld||counter=%ld||ms=%ld||is_tracking=%d||distance_center=%f||target_index=%d||target_distance=%ld\r\n",
      //        HAL_GetTick(), tick_count, HAL_GetTick() - last_time, target_struct.target_status == TRACKING,
      //        target_struct.target_distance_to_center, target_struct.target_index,
      //        target_struct.target_status == TRACKING ? Result.ZoneResult[target_struct.target_index].Distance[0] : 255);
      last_time = HAL_GetTick();

      // send data
      // if(!uart2_tx_done)
      // printf("DMA TX WASNT DONE uart2_tx_done=%d\r\n",uart2_tx_done);
      // while (!uart2_tx_done)
      //   ;
      // uart2_tx_done = 0;
      tracking_send_tof_uart2(&target_struct, &Result, 1);
      // status = get_camera_jpg(&img_struct);
      // CHECK_HAL_STATUS_OR_PRINT(status);
      // // while (!uart2_tx_done)
      // //   ;
      // // uart2_tx_done = 0;
      // status = send_jpg_uart2(&img_struct, 1);
      // CHECK_HAL_STATUS_OR_PRINT(status);

      // decision to capture or not
      // only capture with pir to avoid reflections on tof
      if (PIR_set == 1 || last_PIR_set == 1)
      {
        if (last_target_status == NO_TARGET && target_struct.target_status == TRACKING)
        {
          camera_should_capture = CAMERA_CATPURE_THRESHOLD;
          printf("Acquired target, should capture\r\n");
          printf("== BEGIN TRACKING ==\r\n");
        }
        last_target_status = target_struct.target_status;
      }
      // PIR will loose tracking if not much movement
      if (last_target_status == TRACKING && target_struct.target_status == TRACKING)
      {
        if (last_distance_to_center < target_struct.target_distance_to_center)
        {
          camera_should_capture++;
          printf("Moving away should capture=%d\r\n", camera_should_capture);
        }else if (last_distance_to_center > target_struct.target_distance_to_center)
        {
          if(camera_should_capture>0 && camera_should_capture != CAMERA_CATPURE_THRESHOLD)
          camera_should_capture--;
          printf("Moving away should capture=%d\r\n", camera_should_capture);
        }
        last_distance_to_center = target_struct.target_distance_to_center;
        last_target_status = target_struct.target_status;
      }
    }

    if (camera_should_capture >= CAMERA_CATPURE_THRESHOLD)
    {
      /**
       * Capture and Write jpeg on SD
       */
      // FIXME separate Capture and write on two tasks with DMA
      printf("CAPTURE TASK : should capture=%d", camera_should_capture);
      camera_should_capture = 0;
      status = get_camera_jpg(&img_struct);
      CHECK_HAL_STATUS_OR_PRINT(status);
      printf(" save_picture_sd  \r\n");
      status = save_picture_sd(&img_struct);
      CHECK_HAL_STATUS_OR_PRINT(status);
      nb_iot_send_msg((uint8_t *)"NB_IOT capture done", 19);
    }

    if (HAL_GetTick() - last_time_PIR > PIR_CAPTURE_INTERVAL)
    {
      last_time_PIR = HAL_GetTick();
      last_PIR_set = PIR_set;

      if (is_movement())
      {
        PIR_set = 1;
      }
      else
      {
        PIR_set = 0;
      }
      if (last_PIR_set != PIR_set)
      {
        printf("PIR changed PIR_set=%d", PIR_set);
      }
    }

    if (HAL_GetTick() - last_time_Lum > LUM_CAPTURE_INTERVAL)
    {
      last_time_Lum = HAL_GetTick();
      if (seed_light(&luminosite) == JOUR)
      {
        printf("Il fait jour\r\n");
      }
      else
      {
        printf("Il fait PAS jour\r\n");
      }
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    tick_count++;
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
