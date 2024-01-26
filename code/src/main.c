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
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "ov5640_regs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define cam_address (uint8_t)(0x3C << 1)
#define OV5640_CHIPID_HIGH 0x300a
#define OV5640_CHIPID_LOW 0x300b
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
#include "stm32f4xx_hal_i2c.h"
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

HAL_StatusTypeDef I2C_RX_16reg_8data(uint16_t regID, uint8_t *regDat)
{
  uint8_t temp[2] = {regID >> 8, regID & 0x00FF};
  uint8_t rxdata = 0;
  // HAL_I2C_Master_Transmit(&hi2c1, cam_address, temp, 2, 500);
  // HAL_I2C_Master_Receive(&hi2c1, cam_address, &rxdata, 1, 500);

  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Master_Transmit(&hi2c1, cam_address, &temp[0], 1, 100);
  if (ret == HAL_OK)
    ret = HAL_I2C_Master_Transmit(&hi2c1, cam_address, &temp[1], 1, 100);
  if (ret == HAL_OK)
    ret = HAL_I2C_Master_Receive(&hi2c1, cam_address, &rxdata, 1, 100);

  // HAL_I2C_Mem_Read(&hi2c1, cam_address, (uint16_t)regID, I2C_MEMADD_SIZE_16BIT, &rxdata, 1, 500);

  char buffer[64];
  int size = snprintf(buffer, 64, "I2C RXdata=0x%X || reg=0x%X || ret=%X\r\n", rxdata, regID, ret);
  HAL_UART_Transmit(&huart2, (uint8_t *)buffer, size, 100);
  return ret;
}

HAL_StatusTypeDef I2C_TX_16reg_8data(uint16_t regID, uint8_t regDat)
{
  uint8_t temp[3] = {regID >> 8, regID & 0x00FF, regDat & 0x00FF};
  return HAL_I2C_Master_Transmit(&hi2c1, cam_address, temp, 3, 100);
}

HAL_StatusTypeDef I2C_TX_multiple_16reg_8data(const struct sensor_reg reglist[])
{
  int err = 0;
  const struct sensor_reg *next = reglist;
  uint16_t reg_addr = next->reg;
  uint8_t reg_val = next->val;

  while ((reg_addr != 0xffff) | (reg_val != 0xff))
  {

    reg_addr = next->reg;
    reg_val = next->val;
    err = I2C_TX_16reg_8data(reg_addr, reg_val);
    next++;
  }
  return err;
}
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
  // HAL_GPIO_WritePin(CAMERA_SHUTTER_GPIO_Port, CAMERA_SHUTTER_Pin, GPIO_PIN_RESET);
  // HAL_GPIO_WritePin(CAM_PWDN_GPIO_Port, CAM_PWDN_Pin, GPIO_PIN_SET);
  // HAL_Delay(300);
  // HAL_GPIO_WritePin(CAMERA_SHUTTER_GPIO_Port, CAMERA_SHUTTER_Pin, GPIO_PIN_SET);
  // HAL_GPIO_WritePin(CAM_PWDN_GPIO_Port, CAM_PWDN_Pin, GPIO_PIN_RESET);
  // HAL_Delay(300); // wait for camera startup TODO remove

  // read FW_STATUS bits
  hi2c1.Instance->CR2=0;
  uint8_t rxdata = 0;
  HAL_StatusTypeDef res = I2C_RX_16reg_8data(0x3029, &rxdata);
  char buffer[32];
  int size = snprintf(buffer, 32, "FW_STATUS=0x%X || res=%d\r\n", rxdata, res);
  HAL_UART_Transmit(&huart2, (uint8_t *)buffer, size, 100);

  // download firmware
  
  // I2C_TX_16reg_8data((uint16_t)0x3103, (uint8_t)0x11);
  // I2C_TX_16reg_8data((uint16_t)0x3008, (uint8_t)0x82);
  // HAL_Delay(100);
  // I2C_TX_multiple_16reg_8data(OV5640YUV_Sensor_Dvp_Init);
  // HAL_Delay(500);
  // I2C_TX_multiple_16reg_8data(OV5640_JPEG_QSXGA);
  // I2C_TX_multiple_16reg_8data(OV5640_QSXGA2QVGA);
  // I2C_TX_16reg_8data((uint16_t)0x4407, (uint8_t)0x0C);

  // check FW_STATUS bits for FW OK
  rxdata = 0;
  res = I2C_RX_16reg_8data(0x3029, &rxdata);
  size = snprintf(buffer, 32, "FW_STATUS=0x%X || res=%d\r\n", rxdata, res);
  HAL_UART_Transmit(&huart2, (uint8_t *)buffer, size, 100);

  HAL_UART_Transmit(&huart2, (uint8_t *)"alive\r\n", 7, 100);
  while (1)
    ;
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
