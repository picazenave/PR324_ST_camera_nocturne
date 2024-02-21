#ifndef TRACKING_H
#define TRACKING_H

#include "app_tof.h"
#include "main.h"


HAL_StatusTypeDef tracking_get_target(struct target_t *target_struct, RANGING_SENSOR_Result_t *Result);
HAL_StatusTypeDef tracking_init_background(RANGING_SENSOR_Result_t *Result);
HAL_StatusTypeDef tracking_init_tof();
HAL_StatusTypeDef tracking_send_tof_uart2(struct target_t *target_struct, RANGING_SENSOR_Result_t *Result,uint8_t blocking);


#endif