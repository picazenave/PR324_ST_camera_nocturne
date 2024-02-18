#ifndef NB_IOT_H
#define NB_IOT_H

#include "main.h"

uint8_t AT_expected_answer[] = "\r\nOK\r\n";
uint8_t CEREG_expected_answer[] = "\r\n+CEREG: 1,1\r\nOK\r\n";
uint8_t socketcreate_expected_answer[] = "\r\n#SOCKETCREATE: 0\r\nOK\r\n";
uint8_t socketcreate_error_answer[] = "\r\n+CME ERROR: 2150\r\n";
uint16_t estimate_AT_msg_size(uint8_t *msg, uint16_t len);
uint8_t compare_AT_with_expected(uint8_t *expected_msg, uint8_t *msg, uint16_t expected_msg_len, uint16_t msg_len);
void reset_nb_iot();
HAL_StatusTypeDef _send_msg(uint8_t msg[]);
HAL_StatusTypeDef nb_iot_send_msg(uint8_t msg[], uint8_t len);
void wait_for_AT_ok();



#endif
