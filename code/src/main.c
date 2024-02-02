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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vl53l5cx_api.h"

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

  // uint8_t 				status, loop, isAlive, isReady, i;
	// VL53L5CX_Configuration 	Dev;			/* Sensor configuration */
	// VL53L5CX_ResultsData 	Rsesults;		/* Results data from VL53L5CX */

  int status;
  volatile int IntCount;
  uint8_t p_data_ready;
  VL53L5CX_Configuration 	Dev;
  VL53L5CX_ResultsData 	Results;
  uint8_t resolution, isAlive;
  uint16_t idx;

  char buffer[100];  // Assurez-vous que la taille du tampon est suffisamment grande
  int len;

  // Dev.platform.address = VL53L5CX_DEFAULT_I2C_ADDRESS << 1;
  Dev.platform.address = VL53L5CX_DEFAULT_I2C_ADDRESS;

  /* Initialize the VL53L5CX sensor */
  HAL_GPIO_WritePin(TOF_PWR_EN_GPIO_Port, TOF_PWR_EN_Pin, GPIO_PIN_SET);
  // LPn pins set to High,
  HAL_GPIO_WritePin(TOF_LPn_C_GPIO_Port, TOF_LPn_C_Pin, GPIO_PIN_SET);
  // I2C reset pins set to Low,
  HAL_GPIO_WritePin(TOF_I2C1_RST_GPIO_Port, TOF_I2C1_RST_Pin, GPIO_PIN_RESET);

  // HAL_GPIO_WritePin(TOF_I2C1_RST_GPIO_Port, TOF_I2C1_RST_Pin, GPIO_PIN_RESET);
  // HAL_GPIO_WritePin(TOF_LPn_C_GPIO_Port, TOF_LPn_C_GPIO_Port, GPIO_PIN_RESET);
  // HAL_GPIO_WritePin(TOF_LPn_C_GPIO_Port, TOF_LPn_C_GPIO_Port, GPIO_PIN_SET);
  // vl53l5cx_set_i2c_address(&Dev, (0x52 << 1));
  // HAL_GPIO_WritePin(TOF_LPn_C_GPIO_Port, TOF_LPn_C_GPIO_Port, GPIO_PIN_SET);
  
  // HAL_GPIO_Init(TOF_I2C1_RST_GPIO_Port, TOF_I2C1_RST_Pin);
  // HAL_GPIO_Init(TOF_LPn_C_GPIO_Port, TOF_LPn_C_Pin);
  // HAL_GPIO_Init(TOF_INT_GPIO_Port, TOF_INT_Pin);

  // HAL_GPIO_Init(TOF_PWR_EN_GPIO_Port, TOF_PWR_EN_Pin);
  // HAL_GPIO_WritePin(TOF_PWR_EN_GPIO_Port, TOF_PWR_EN_Pin, GPIO_PIN_SET);

  char *text;

  while (1)
  {

    text = "Begin sequence\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);



    status = vl53l5cx_is_alive(&Dev, &isAlive);
    if(!isAlive || status)
    {
      text = "VL53L5CX not detected at requested address\n";
      HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);
    }

    // if(!isAlive)
    // {
    //   text = "VL53L5CX not detected at requested address (0x";
    //   HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);
    //   HAL_UART_Transmit(&huart2, (uint8_t *)Dev.platform.address, sizeof(Dev.platform.address), 100);
    //   text = ")\n";
    //   HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);
    //   // ///HAL_Delay(1000);
    //   // return 255;
    // }

    // text = "Sensor initializing, please wait few seconds\n";
    // HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);

    // status = vl53l5cx_init(&Dev);
    // status = vl53l5cx_set_ranging_frequency_hz(&Dev, 2);				// Set 2Hz ranging frequency
    // status = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_CONTINUOUS);  // Set mode continuous

    // text = "Ranging starts\n";
    // HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);

    /* (Mandatory) Init VL53L5CX sensor */
    status = vl53l5cx_init(&Dev);
    if(status)
    {
      text = "VL53L5CX ULD Loading failed\n";
      HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);
    }
    else
    {
      text = "VL53L5CX ULD ready ! Version : ";
      HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);
      HAL_UART_Transmit(&huart2, VL53L5CX_API_REVISION, strlen(VL53L5CX_API_REVISION), 100);
      text = "\n";
      HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);
    }
    
    

    text = "Ranging starts\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100);

    status = vl53l5cx_start_ranging(&Dev);

    status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
    status = vl53l5cx_get_resolution(&Dev, &resolution);
    // Affichez la résolution
    len = snprintf(buffer, sizeof(buffer), "Resolution: %d\n", resolution);
    if (len >= 0 && len < sizeof(buffer)) {
      HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
    }

		status = vl53l5cx_get_ranging_data(&Dev, &Results);
    for (int i = 0; i < resolution; i++) {
      /* Print per zone results */
      len = snprintf(buffer, sizeof(buffer), "Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
                    i,
                    Results.nb_target_detected[i],
                    Results.ambient_per_spad[i]);

      // Vérifiez que la chaîne a été correctement formatée
      if (len < 0 || len >= sizeof(buffer)) {
        // Gestion des erreurs si nécessaire
      } else {
        // Utilisez HAL_UART_Transmit pour transmettre le tampon via l'UART
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
      }

      /* Print per target results */
      if (Results.nb_target_detected[i] > 0) {
        len = snprintf(buffer, sizeof(buffer), "Target status : %3u, Distance : %4d mm\n",
                      Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * i],
                      Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i]);
      } else {
        len = snprintf(buffer, sizeof(buffer), "Target status : 255, Distance : No target\n");
      }

      // Vérifiez que la chaîne a été correctement formatée
      if (len < 0 || len >= sizeof(buffer)) {
        // Gestion des erreurs si nécessaire
      } else {
        // Utilisez HAL_UART_Transmit pour transmettre le tampon via l'UART
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
      }
    }



    // for(int i = 0; i < resolution;i++){
    //   /* Print per zone results */
    //   printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
    //       i,
    //       Results.nb_target_detected[i],
    //       Results.ambient_per_spad[i]);

    //   /* Print per target results */
    //   if(Results.nb_target_detected[i] > 0){
    //     printf("Target status : %3u, Distance : %4d mm\n",
    //         Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * i],
    //         Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i]);
    //   }else{
    //     printf("Target status : 255, Distance : No target\n");
    //   }
    // }


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