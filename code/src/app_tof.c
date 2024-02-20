/**
 ******************************************************************************
 * @file          : app_tof.c
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

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_tof.h"
#include "main.h"
#include <stdio.h>

#include "custom_ranging_sensor.h"
// #include "stm32f4xx_nucleo.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#ifndef USE_BARE_DRIVER
  static RANGING_SENSOR_Capabilities_t Cap;
#endif
  static RANGING_SENSOR_ProfileConfig_t Profile;
  static RANGING_SENSOR_Result_t Result;

  static int32_t status = 0;
  volatile uint8_t ToF_EventDetected = 0;

  extern volatile uint16_t ext_it;

  /* Private function prototypes -----------------------------------------------*/

  void MX_VL53L5CX_ToF_Init(void)
  {
    status = CUSTOM_RANGING_SENSOR_Init(CUSTOM_VL53L5CX);

    if (status != BSP_ERROR_NONE)
    {
      printf("CUSTOM_RANGING_SENSOR_Init failed, status=0x%.2lX,%ld\n", status, status);
      while (1)
        ;
    }
  }

  int MX_VL53L5CX_ToF_Process()
  {
    uint32_t Id;

    CUSTOM_RANGING_SENSOR_ReadID(CUSTOM_VL53L5CX, &Id);
    CUSTOM_RANGING_SENSOR_GetCapabilities(CUSTOM_VL53L5CX, &Cap);

    // Profile.RangingProfile = RS_PROFILE_4x4_CONTINUOUS;
    Profile.RangingProfile = RS_PROFILE_8x8_CONTINUOUS;
    Profile.TimingBudget = 50;  //TIMING_BUDGET;
    Profile.Frequency = 15;    // RANGING_FREQUENCY; /* Ranging frequency Hz (shall be consistent with TimingBudget value) */
    Profile.EnableAmbient = 0; /* Enable: 1, Disable: 0 */
    Profile.EnableSignal = 0;  /* Enable: 1, Disable: 0 */

    /* set the profile if different from default one */
    CUSTOM_RANGING_SENSOR_ConfigProfile(CUSTOM_VL53L5CX, &Profile);


    status = CUSTOM_RANGING_SENSOR_Start(CUSTOM_VL53L5CX, VL53L5CX_MODE_BLOCKING_CONTINUOUS);

    while (1)
    {

      while (ext_it == 0)
        ; // wait for int to go low
      ext_it = 0;
      /* polling mode */
      status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L5CX, &Result);

      if (status == BSP_ERROR_NONE)
      {

        // uint8_t zones_per_line = ((Profile.RangingProfile == RS_PROFILE_8x8_AUTONOMOUS) ||
        //                           (Profile.RangingProfile == RS_PROFILE_8x8_CONTINUOUS)) ? 8 : 4;

        // return check(detect, &Result, zones_per_line);
        printf("%ld,",HAL_GetTick());
        for (uint8_t i = 0; i < 64; i++)
          printf("%ld,", (uint32_t)Result.ZoneResult[i].Distance[0]);
        for (uint8_t i = 0; i < 64; i++)
          printf("%ld,", (uint32_t)Result.ZoneResult[i].NumberOfTargets);

        for (uint8_t i = 0; i < 63; i++) // print status
          printf("%ld,", (uint32_t)Result.ZoneResult[i].Status[0]);
        printf("%ld\n\r", (uint32_t)Result.ZoneResult[63].Status[0]);
      }
      else
      {
        printf("ranging status error status=%ld\r\n",status);
        return -1;
      }

      // HAL_Delay(POLLING_PERIOD);
    }
  }

#ifdef __cplusplus
}
#endif
