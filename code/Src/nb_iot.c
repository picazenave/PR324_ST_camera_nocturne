#include "usart.h"
#include "fatfs.h"
#include "stdio.h"
#include <string.h>
#include "main.h"
#include "nb_iot.h"

#define TEMP_LEN 200

// init nb iot, will reset if needed (will wait 5sec before init)
HAL_StatusTypeDef init_nb_iot()
{
    /**
     * AT * 3 to wake up
     * AT+CEREG? --> should be 1,1 (trying to attach is 1,2)
     * at#socketcreate=5,0,UDP,20001,1,1,1 --> create socket (+CME ERROR if already created) --> should hardware reset
     * AT#IPSENDUDP=5,0,90.50.33.140,20001,0,0,Coucou bg --> wait for #IPRECV: 5,0 (send OK and echo)
     */
    // FIXME gotos
    uint8_t temp[TEMP_LEN] = {0};
    uint16_t msg_len = 0;
    uint8_t r = 0;
start:

    HAL_Delay(5000);
    wait_for_AT_ok();

    HAL_Delay(300);
    while (1)
    {
        HAL_UART_Transmit(&huart6, (uint8_t *)"AT+CEREG?\r", 10, 100);
        HAL_UART_Receive(&huart6, temp, TEMP_LEN, 1000);
        msg_len = estimate_AT_msg_size(temp, TEMP_LEN);
        printf("msg_len=%d\r\n", msg_len);
        r = compare_AT_with_expected(CEREG_expected_answer, temp, sizeof(CEREG_expected_answer) - 1, msg_len);
        if (r == 0)
            break;
        // fill end of msg with 0 for printing
        for (uint16_t i = msg_len; i < TEMP_LEN; i++)
            temp[i] = 0;
        printf("CEREG answer not CEREG expected pos=%d\r\n==========%s\r\n=========\r\n", r, temp);
        HAL_Delay(100);
    }

    HAL_Delay(300);
    while (1)
    {
        HAL_UART_Transmit(&huart6, (uint8_t *)"at#socketcreate=5,0,UDP,20001,1,1,1\r", 36, 100);
        HAL_UART_Receive(&huart6, temp, TEMP_LEN, 1000);
        msg_len = estimate_AT_msg_size(temp, TEMP_LEN);
        printf("msg_len=%d\r\n", msg_len);
        r = compare_AT_with_expected(socketcreate_expected_answer, temp, sizeof(socketcreate_expected_answer) - 1, msg_len);
        if (r == 0)
            break;

        // check for socket error
        r = compare_AT_with_expected(socketcreate_error_answer, temp, sizeof(socketcreate_error_answer) - 1, msg_len);
        if (r == 0)
        {
            // socket already created
            for (uint16_t i = msg_len; i < TEMP_LEN; i++)
                temp[i] = 0;
            printf("socketcreate error(NO PROBLEM) pos=%d\r\n==========%s\r\n=========\r\n", r, temp);
            break;
        }
        else
        {
            // need to reset and start again, module is unknow state
            printf("socketcreate real error pos=%d\r\n==========%s\r\n=========\r\n", r, temp);
            // reset_nb_iot();
            HAL_Delay(1000);
            goto start;
        }

        // fill end of msg with 0 for printing
        for (uint16_t i = msg_len; i < TEMP_LEN; i++)
            temp[i] = 0;
        printf("socketcreate answer not socketcreate expected pos=%d\r\n==========%s\r\n=========\r\n", r, temp);
        HAL_Delay(100);
    }
    // send notification
    nb_iot_send_msg((uint8_t *)"NB_IOT init done", 16);

    // send notification
    nb_iot_send_msg((uint8_t *)"NB_IOT init done", 16);

    printf("NB IOT Init DONE\r\n");

    return HAL_OK;
}

void reset_nb_iot()
{
    printf("resetting nb iot\r\n");
    HAL_GPIO_WritePin(MODEM_RSTn_GPIO_Port, MODEM_RSTn_Pin, GPIO_PIN_RESET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(MODEM_RSTn_GPIO_Port, MODEM_RSTn_Pin, GPIO_PIN_SET);
    HAL_Delay(300);
}

HAL_StatusTypeDef nb_iot_send_msg(uint8_t msg[], uint8_t len)
{
    wait_for_AT_ok();
    for (uint16_t i = len; i < TEMP_LEN; i++)
        msg[i] = 0;
    _send_msg(msg);
    return HAL_OK;
}

HAL_StatusTypeDef _send_msg(uint8_t msg[])
{
    uint8_t temp[TEMP_LEN] = {0};
    uint16_t r = snprintf(temp, TEMP_LEN, "AT#IPSENDUDP=5,0,90.50.33.140,20001,0,0,%s\r", msg);
    HAL_UART_Transmit(&huart6, temp, r, 100);
    HAL_UART_Receive(&huart6, temp, TEMP_LEN, 5000);
    uint16_t msg_len = estimate_AT_msg_size(temp, TEMP_LEN);
    printf("msg_len=%d\r\n", msg_len);
    // fill end of msg with 0 for printing
    for (uint16_t i = msg_len; i < TEMP_LEN; i++)
        temp[i] = 0;
    printf("answer from send notif=\r\n==========%s\r\n=========\r\n", temp);
}

void wait_for_AT_ok()
{
    printf("WAIT FOR AT OK\r\n");
    uint8_t temp[TEMP_LEN] = {0};
    uint16_t msg_len = 0;
    uint8_t r = 0;
    while (1)
    {
        HAL_UART_Transmit(&huart6, (uint8_t *)"AT\r", 3, 100);
        HAL_UART_Receive(&huart6, temp, TEMP_LEN, 1000);
        msg_len = estimate_AT_msg_size(temp, TEMP_LEN);
        printf("msg_len=%d\r\n", msg_len);
        r = compare_AT_with_expected(AT_expected_answer, temp, sizeof(AT_expected_answer) - 1, msg_len);
        if (r == 0)
            break;

        // fill end of msg with 0 for printing
        for (uint16_t i = msg_len; i < TEMP_LEN; i++)
            temp[i] = 0;
        printf("AT answer not AT expected pos=%d\r\n==========%s\r\n=========\r\n", r, temp);
        HAL_Delay(100);
    }
}

uint16_t estimate_AT_msg_size(uint8_t *msg, uint16_t len)
{
    uint16_t local_end = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        if (i + 1 < len)
        {
            if (msg[i] == '\r' && msg[i + 1] == '\n')
            {
                local_end = i + 1 + 1;
                // msg[local_end]='\0';
            }
        }
    }
    return local_end;
}

// 0 if OK, position of KO otherwise
uint8_t compare_AT_with_expected(uint8_t *expected_msg, uint8_t *msg, uint16_t expected_msg_len, uint16_t msg_len)
{
    if (expected_msg_len > msg_len)
        return 255;

    // remove CR LF for strstr
    char temp[TEMP_LEN] = {0};
    for (uint16_t i = 0; i < TEMP_LEN - 2; i++)
    {
        temp[i] = expected_msg[i + 2];
    }
    temp[expected_msg_len - 2] = 0;

    if (strstr(msg, temp) != NULL)
        return 0;

    for (uint16_t i = 0; i < expected_msg_len; i++)
    {
        if (expected_msg[i] != msg[i])
        {
            return i;
        }
    }
    return 0;
}