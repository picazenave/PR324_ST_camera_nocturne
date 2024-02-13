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
struct img_struct_t
{
    uint8_t img_buffer[65535];
    uint16_t img_len;
};
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define CHECK_HAL_STATUS_OR_PRINT(status) \
  if (status != HAL_OK)                   \
    printf("lg:%d file:%s KO:0x%.2X \r\n", __LINE__,__FILE__, status);
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
HAL_StatusTypeDef camera_init(uint8_t default_config);
void i2c_scanner();
HAL_StatusTypeDef save_picture_sd(struct img_struct_t *img_struct);
HAL_StatusTypeDef sd_init();
HAL_StatusTypeDef get_camera_jpg(struct img_struct_t *img_struct);
HAL_StatusTypeDef send_jpg_uart2(struct img_struct_t *img_struct);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define ADC1_IN1_LUM_Pin GPIO_PIN_1
#define ADC1_IN1_LUM_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define TOF_PWR_EN_Pin GPIO_PIN_0
#define TOF_PWR_EN_GPIO_Port GPIOB
#define GPIO_PIR_Pin GPIO_PIN_1
#define GPIO_PIR_GPIO_Port GPIOB
#define CAMERA_RST_N_Pin GPIO_PIN_7
#define CAMERA_RST_N_GPIO_Port GPIOC
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

/* USER CODE BEGIN Private defines */
#define SD_SPI_HANDLE hspi1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
