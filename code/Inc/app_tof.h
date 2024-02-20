/**
  ******************************************************************************
  * @file          : app_tof.h
  * @author        : IMG SW Application Team
  * @brief         : This file provides code for the configuration
  *                  of the STMicroelectronics.X-CUBE-TOF1.3.4.0 instances.
  ******************************************************************************
  *
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_TOF_H
#define __APP_TOF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "custom_ranging_sensor.h"


/* Exported defines ----------------------------------------------------------*/
/* #define USE_BARE_DRIVER */
#define TIMING_BUDGET     (30U) /* 5 ms < TimingBudget < 100 ms */
#define RANGING_FREQUENCY (5U) /* Ranging frequency Hz (shall be consistent with TimingBudget value) */
#define POLLING_PERIOD    (1000U/RANGING_FREQUENCY) /* refresh rate for polling mode (milliseconds) */
#define NB_SEUIL_ZONE     (4U) /* Nombre de zone à dépasser pour affirmer une présence */


/* Exported functions --------------------------------------------------------*/
void MX_VL53L5CX_ToF_Init(void);
int MX_VL53L5CX_ToF_Process();

#ifdef __cplusplus
}
#endif

#endif /* __APP_TOF_H */
