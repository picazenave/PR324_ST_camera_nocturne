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
#include "pir_lum.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "stdint.h"

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
  /* USER CODE BEGIN 2 */
  

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint8_t tof_matrix[8 * 8] = {0};
  uint32_t tick_count = 0;
  uint8_t uart_in = 0;

  ///////////////////////////////////initialisation des capteurs///////////////////////////////////////////////
  int pir_movment;
  char str2[15];

  uint8_t pir2;
  char str3[15];
  /////////////////////////Configuration des registre PIR detection ONSEMI PIR-GEVB////////////////////////////
  uint8_t config_register_data[1] = {0xFE}; // Met IO0_0 (xLED_EN) en sortie, les autres en entrée  //dans registre 6 0b00000110 //pour FE 0b11111110
  HAL_I2C_Mem_Write(&hi2c1, 0x24, 0x06, 1, &config_register_data, 1, HAL_MAX_DELAY);
  HAL_Delay(100);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  while (1)
  {
    /////////////////////////////////////////PIR detection 101020353 Seed/////////////////////////////////////////
    pir_movment = seed_pir();
    //affichage   
    sprintf(str2, "detection flag : %d\n", pir_movment);
		HAL_UART_Transmit_IT(&huart2,(uint8_t*)str2,strlen(str2));
    HAL_Delay(1000);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////PIR detection ONSEMI PIR-GEVB/////////////////////////////////// 
    /*pir2 = onsemi_pir();
    //affichage 
    if (pir2 == 0)
    {
      sprintf(str3, "PIR2 : 0, %d\n", pir2);
		  HAL_UART_Transmit_IT(&huart2,(uint8_t*)str3,strlen(str3));
      HAL_Delay(100);
    }
    else
    {
      sprintf(str3, "PIR2 : 1, %d\n", pir2);
		  HAL_UART_Transmit_IT(&huart2,(uint8_t*)str3,strlen(str3));
      HAL_Delay(100);
    }*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////Light detection 101020132 Seed///////////////////////////////////////
    //init
    struct lumionsite *light_detect = malloc(sizeof(struct lumionsite));
    seed_light(light_detect);
    /*char str8[15];
    sprintf(str8, "valeur 222222222: %d\n", light_detect->light_sensor);
		HAL_UART_Transmit_IT(&huart2,(uint8_t*)str8,strlen(str8));
    HAL_Delay(1000);*/
    free(light_detect);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    

    // HAL_Delay(300);
    if (tick_count % 50 == 0)
      HAL_UART_Transmit(&huart2, "alive\r\n", 7, 100);

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
