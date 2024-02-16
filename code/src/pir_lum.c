#include "pir_lum.h"
#include <string.h>

int seed_light(Luminosite_t *lum)
{
  // char str[50]; 
    
  HAL_ADC_Start(&hadc1); //deplacer
 
  //Get value
  lum->light_sensor = (int)HAL_ADC_GetValue(&hadc1);
  // char str6[15]; 
  // sprintf(str6, "Lumionsité : %d\n", lum->light_sensor);
	// HAL_UART_Transmit_IT(&huart2,(uint8_t*)str6,strlen(str6));
  // HAL_Delay(100);
  if (lum->light_sensor<700)
  {
    lum->day_moment = NUIT;
    return NUIT;
    // sprintf(str, "C'est la nuit.\n"); 
		// HAL_UART_Transmit_IT(&huart2,(uint8_t*)str,strlen(str));
    // HAL_Delay(1000);
  }
  else if((lum->light_sensor > 700) &&(lum->light_sensor < 1700))
  {
    lum->day_moment = CREPUSCULE;
    return CREPUSCULE;
    // sprintf(str, "C'est le crépuscule.\n"); 
		// HAL_UART_Transmit_IT(&huart2,(uint8_t*)str,strlen(str));
    // HAL_Delay(100);
  }
  else if ((lum->light_sensor > 1700))
  {
    lum->day_moment = JOUR;
    return JOUR;
    // sprintf(str, "C'est le jour.\n"); 
		// HAL_UART_Transmit_IT(&huart2,(uint8_t*)str,strlen(str));
    // HAL_Delay(100);
  }
  return -1;
}

int is_movement()
{
  int movment_detected = 0;
  if(HAL_GPIO_ReadPin(GPIO_PIR_GPIO_Port, GPIO_PIR_Pin) == 1)
      {
        movment_detected = 1;
      }
  else
  {
    movment_detected = 0;
  }
  return movment_detected;
}

// void bus_Scanning()
// {
//   uint8_t Buffer[25] = {0};
//   uint8_t Space[] = " - ";
//   uint8_t StartMSG[] = "Starting I2C Scanning: \r\n";
//   uint8_t EndMSG[] = "Done! \r\n\r\n";

//   /*-[ I2C Bus Scanning ]-*/
//   HAL_UART_Transmit(&huart2, StartMSG, sizeof(StartMSG), 10000);
//   uint8_t i = 0, ret;
//   for(i=1; i<128; i++)
//   {
//       ret = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 3, 5);
//       if (ret != HAL_OK) 
//       {
//         HAL_UART_Transmit(&huart2, Space, sizeof(Space), 10000);
//       }
//       else if(ret == HAL_OK)
//       {
//         sprintf(Buffer, "0x%X", i);
//         HAL_UART_Transmit(&huart2, Buffer, sizeof(Buffer), 10000);
//       }
//   }
//   HAL_UART_Transmit(&huart2, EndMSG, sizeof(EndMSG), 10000);
//   /*--[ Scanning Done ]--*/
// }

uint8_t onsemi_pir()
{
  // Lecture de la valeur du registre à partir du PCA9655E
  uint8_t Value;
  HAL_I2C_Master_Receive(&hi2c1, 0x24, &Value, 1, HAL_MAX_DELAY);
  uint8_t result = (Value & 0x04);
  return result;
}

// void debug_register()
// {
//   //Degug memwrite
//   uint8_t registerAddress = 0x02; // Adresse du registre que tu veux lire (par exemple, IODIR)
//   uint8_t registerValue;
//   HAL_Delay(100);
//   HAL_I2C_Mem_Read(&hi2c1, 0x24, registerAddress, 1, &registerValue, 1, HAL_MAX_DELAY);
//   HAL_Delay(100);
//   char str4[15]; 
//   // sprintf(str4, "Valeur du registre 0x%02X : 0x%02X\n", registerAddress, registerValue);
// 	HAL_UART_Transmit_IT(&huart2,(uint8_t*)str4,strlen(str4));
//   HAL_Delay(100);
// }