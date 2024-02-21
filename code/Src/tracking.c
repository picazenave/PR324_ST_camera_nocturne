#include "tracking.h"
#include "app_tof.h"
#include "auto_gen.h"
#include "stdio.h"
#include "usart.h"

extern volatile uint16_t ext_it;

uint16_t background[64];
uint8_t possible_target_index[64];
uint8_t possible_target_i = 0;
uint16_t threshold = 200;
uint8_t is_tracking = 0;

uint8_t tof_header[5] = {0xAA, 0x55, 0x32, 0x01, 0x21};
extern volatile uint8_t uart2_tx_done;

HAL_StatusTypeDef tracking_init_tof()
{
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
    if (status == BSP_ERROR_NONE)
        return HAL_OK;
    else
        return HAL_ERROR;
}

HAL_StatusTypeDef tracking_init_background(RANGING_SENSOR_Result_t *Result)
{
    HAL_StatusTypeDef status = HAL_ERROR;
    printf("Waiting for background to stabilize\r\n");
    for (uint8_t i = 0; i < 5; i++) // 1.2sec
    {
        while (ext_it == 0)
            ;
        ext_it = 0;
        status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L5CX, Result);
    }
    printf("DONE\r\n Background values :\r\n");
    for (uint8_t i = 0; i < 64; i++)
    {
        if ((Result->ZoneResult[i].Status[0] == 5 || Result->ZoneResult[i].Status[0] == 9) && (Result->ZoneResult[i].NumberOfTargets > 0))
            background[i] = Result->ZoneResult[i].Distance[0];
        else
            background[i] = 2550;

        printf("|[%d]=%d|", i, background[i]);
    }

    printf("Background init DONE\r\n");
    if (status == BSP_ERROR_NONE)
        return HAL_OK;
    else
        return HAL_ERROR;
}

HAL_StatusTypeDef tracking_get_target(struct target_t *target_struct, RANGING_SENSOR_Result_t *Result)
{
    HAL_StatusTypeDef status = CUSTOM_RANGING_SENSOR_GetDistance(CUSTOM_VL53L5CX, Result);
    // printf("TOF[0]=%ld\r\n",(int32_t)Result->ZoneResult[0].Distance[0]);
    if (status == BSP_ERROR_NONE)
    {
        int16_t temp = 0;
        possible_target_i = 0;
        is_tracking = 0;
        for (uint8_t i = 0; i < 64; i++)
        {
            // ignore corners bc bigleux
            // FIXME c'est degeu ca
            if (i != 0 && i != 1 && i != 8 && i != 6 && i != 7 && i != 15 && i != 48 && i != 56 && i != 57 && i != 55 && i != 62 && i != 63)
            {
                // if temp is <0 then we have something behind the 2550 value wich is mostly noise or lucky sensing
                temp = (int32_t)((int32_t)background[i] - (int32_t)Result->ZoneResult[i].Distance[0]);
                if ((temp > threshold) && (Result->ZoneResult[i].Status[0] == 5 || Result->ZoneResult[i].Status[0] == 9) && (Result->ZoneResult[i].NumberOfTargets > 0))
                {
                    // printf("|[%d]=%d|",i,temp);
                    possible_target_index[possible_target_i] = i;
                    possible_target_i++;
                    is_tracking = 1;
                }
            }
        }
        uint16_t x = 0, y = 0;
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
            target_struct->target_distance_to_center = distance_matrix[x + y * 8];
            target_struct->target_index = x + y * 8;
            target_struct->x = x;
            target_struct->y = y;
            target_struct->target_status = TRACKING;
        }
        else
        {
            target_struct->target_distance_to_center = 255;
            target_struct->target_index = 255;
            target_struct->x = 255;
            target_struct->y = 255;
            target_struct->target_status = NO_TARGET;
        }

        return HAL_OK;
    }
    else
    {
        printf("error BSP TOF\r\n");
        printf("error BSP TOF\r\n");
        printf("error BSP TOF\r\n");
        printf("error BSP TOF\r\n");
        printf("error BSP TOF\r\n");
        return HAL_ERROR;
    }
}

HAL_StatusTypeDef tracking_send_tof_uart2(struct target_t *target_struct, RANGING_SENSOR_Result_t *Result, uint8_t blocking)
{
    uint16_t uart2_timeout = 1000;
    uint8_t to_transmit[64 * 3 + 3] = {0};

    for (uint8_t i = 0; i < 64; i++)
    {
        if ((Result->ZoneResult[i].Status[0] == 5 || Result->ZoneResult[i].Status[0] == 9) && (Result->ZoneResult[i].NumberOfTargets > 0))
        {
            to_transmit[i] = Result->ZoneResult[i].Distance[0] / 10;
        }
        else
            to_transmit[i] = 255;
    }
    for (uint8_t i = 64; i < 64 * 2; i++)
    {
        to_transmit[i] = Result->ZoneResult[i].Status[0];
    }
    for (uint8_t i = 64 * 2; i < 64 * 3; i++)
    {
        to_transmit[i] = Result->ZoneResult[i].NumberOfTargets;
    }
    to_transmit[64 * 3] = target_struct->x;
    to_transmit[64 * 3 + 1] = target_struct->y;
    to_transmit[64 * 3 + 2] = target_struct->target_status == TRACKING;

    HAL_StatusTypeDef status;
    status = HAL_UART_Transmit(&huart2, tof_header, sizeof(tof_header), uart2_timeout);
    CHECK_HAL_STATUS_OR_PRINT(status);
    status = HAL_UART_Transmit_DMA(&huart2, to_transmit, sizeof(to_transmit));
    CHECK_HAL_STATUS_OR_PRINT(status);

    if (blocking)
    {
        uart2_tx_done = 0;
        while (!uart2_tx_done)
            ;
        //uart2_tx_done = 0;
    }
    return status;
}