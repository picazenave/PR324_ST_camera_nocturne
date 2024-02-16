#include "usart.h"
#include "fatfs.h"
#include "stdio.h"
#include <string.h>
#include "main.h"

HAL_StatusTypeDef init_nb_iot()
{
    /**
     * AT * 3 to wake up
     * AT+CEREG? --> should be 1,1 (trying to attach is 1,2)
     * at#socketcreate=5,0,UDP,20001,1,1,1 --> create socket (+CME ERROR if already created) --> should hardware reset
     * AT#IPSENDUDP=5,0,90.50.33.140,20001,0,0,myValue:1234 --> wait for #IPRECV: 5,0 (send OK and echo)
    */
}