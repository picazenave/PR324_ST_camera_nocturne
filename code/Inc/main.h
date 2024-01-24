/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4xx_hal_i2c.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define D0_Pin GPIO_PIN_0
#define D0_GPIO_Port GPIOC
#define D1_Pin GPIO_PIN_1
#define D1_GPIO_Port GPIOC
#define D2_Pin GPIO_PIN_2
#define D2_GPIO_Port GPIOC
#define D3_Pin GPIO_PIN_3
#define D3_GPIO_Port GPIOC
#define ADC1_IN1_LUM_Pin GPIO_PIN_1
#define ADC1_IN1_LUM_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define TOF_INT_Pin GPIO_PIN_4
#define TOF_INT_GPIO_Port GPIOA
#define D4_Pin GPIO_PIN_4
#define D4_GPIO_Port GPIOC
#define D5_Pin GPIO_PIN_5
#define D5_GPIO_Port GPIOC
#define TOF_PWR_EN_Pin GPIO_PIN_0
#define TOF_PWR_EN_GPIO_Port GPIOB
#define GPIO_PIR_Pin GPIO_PIN_1
#define GPIO_PIR_GPIO_Port GPIOB
#define CAM_PWDN_Pin GPIO_PIN_15
#define CAM_PWDN_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_6
#define D6_GPIO_Port GPIOC
#define D7_Pin GPIO_PIN_7
#define D7_GPIO_Port GPIOC
#define D8_Pin GPIO_PIN_8
#define D8_GPIO_Port GPIOC
#define D9_Pin GPIO_PIN_9
#define D9_GPIO_Port GPIOC
#define CAM_RST_Pin GPIO_PIN_8
#define CAM_RST_GPIO_Port GPIOA
#define XCLK_Pin GPIO_PIN_9
#define XCLK_GPIO_Port GPIOA
#define PCLK_Pin GPIO_PIN_10
#define PCLK_GPIO_Port GPIOA
#define HREF_Pin GPIO_PIN_11
#define HREF_GPIO_Port GPIOA
#define VSYNC_Pin GPIO_PIN_12
#define VSYNC_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define TOF_I2C1_RST_Pin GPIO_PIN_3
#define TOF_I2C1_RST_GPIO_Port GPIOB
#define TOF_LPn_C_Pin GPIO_PIN_4
#define TOF_LPn_C_GPIO_Port GPIOB
#define SD_CS_Pin GPIO_PIN_5
#define SD_CS_GPIO_Port GPIOB
#define CAMERA_CS_Pin GPIO_PIN_6
#define CAMERA_CS_GPIO_Port GPIOB
#define CAMERA_SHUTTER_Pin GPIO_PIN_7
#define CAMERA_SHUTTER_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
