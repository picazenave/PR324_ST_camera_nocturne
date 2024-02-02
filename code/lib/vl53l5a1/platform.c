/**
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */


#include "platform.h"


uint8_t RdByte(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAddress,
		uint8_t *p_value)
{
	// uint8_t status = 255;
	
	/* Need to be implemented by customer. This function returns 0 if OK */

	uint8_t status = RdMulti(p_platform, RegisterAddress, p_value, 1);
  	return status;
}

uint8_t WrByte(
		VL53L5CX_Platform *p_platform,
		uint16_t RegisterAddress,
		uint8_t value)
{
	// uint8_t status = 255;

	/* Need to be implemented by customer. This function returns 0 if OK */
	
	uint8_t status = WrMulti(p_platform, RegisterAddress, &value, 1);
	return status;
}


uint8_t WrMulti(
  VL53L5CX_Platform *p_platform,
  uint16_t RegisterAddress,
  uint8_t *p_values,
  uint32_t size)
{
  uint32_t i = 0;
  uint8_t buffer[2];

  while (i < size) {
    size_t current_write_size = (size - i > DEFAULT_I2C_BUFFER_LEN ? DEFAULT_I2C_BUFFER_LEN : size - i);

    buffer[0] = (uint8_t)((RegisterAddress + i) >> 8);
    buffer[1] = (uint8_t)((RegisterAddress + i) & 0xFF);

    if (HAL_I2C_Mem_Write(p_platform->dev_i2c, p_platform->address, buffer[0], I2C_MEMADD_SIZE_16BIT, p_values + i, current_write_size, HAL_MAX_DELAY) != HAL_OK) {
      return 1;
    }

    i += current_write_size;
  }

  return 0;
}

uint8_t RdMulti(
  VL53L5CX_Platform *p_platform,
  uint16_t RegisterAddress,
  uint8_t *p_values,
  uint32_t size)
{
  int status = 0;
  uint8_t buffer[2];

  do {
    buffer[0] = (uint8_t)(RegisterAddress >> 8);
    buffer[1] = (uint8_t)(RegisterAddress & 0xFF);

    if (HAL_I2C_Mem_Read(p_platform->dev_i2c, p_platform->address, buffer[0], I2C_MEMADD_SIZE_16BIT, p_values, size, HAL_MAX_DELAY) != HAL_OK) {
      status = 1;
    } else {
      status = 0;
    }

  } while (status != 0);

  return status;
}

uint8_t Reset_Sensor(
		VL53L5CX_Platform *p_platform)
{
	uint8_t status = 0;
	
	/* (Optional) Need to be implemented by customer. This function returns 0 if OK */
	
	/* Set pin LPN to LOW */
	/* Set pin AVDD to LOW */
	/* Set pin VDDIO  to LOW */
	WaitMs(p_platform, 100);

	/* Set pin LPN of to HIGH */
	/* Set pin AVDD of to HIGH */
	/* Set pin VDDIO of  to HIGH */
	WaitMs(p_platform, 100);

	return status;
}

void SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size)
{
	uint32_t i, tmp;
	
	/* Example of possible implementation using <string.h> */
	for(i = 0; i < size; i = i + 4) 
	{
		tmp = (
		  buffer[i]<<24)
		|(buffer[i+1]<<16)
		|(buffer[i+2]<<8)
		|(buffer[i+3]);
		
		memcpy(&(buffer[i]), &tmp, 4);
	}
}	

uint8_t WaitMs(
		VL53L5CX_Platform *p_platform,
		uint32_t TimeMs)
{
	uint8_t status = 255;

	/* Need to be implemented by customer. This function returns 0 if OK */
	(void)p_platform;
	HAL_Delay(TimeMs);
	
	return status;
}







// uint8_t WrMulti(
// 		VL53L5CX_Platform *p_platform,
// 		uint16_t RegisterAddress,
// 		uint8_t *p_values,
// 		uint32_t size)
// {
// 	uint8_t status = 255;
	
// 	/* Need to be implemented by customer. This function returns 0 if OK */
// 	uint32_t i = 0;
// 	uint8_t buffer[2];

// 	while (i < size) {
// 	// If still more than DEFAULT_I2C_BUFFER_LEN bytes to go, DEFAULT_I2C_BUFFER_LEN,
// 	// else the remaining number of bytes
// 	size_t current_write_size = (size - i > DEFAULT_I2C_BUFFER_LEN ? DEFAULT_I2C_BUFFER_LEN : size - i);

// 	buffer[0] = (uint8_t)((RegisterAddress + i) >> 8);
//     buffer[1] = (uint8_t)((RegisterAddress + i) & 0xFF);

// 	status = (uint8_t)HAL_I2C_Master_Transmit(p_platform->dev_i2c, p_platform->address, buffer, current_write_size, I2C_TIMEOUT_BUSY_FLAG);

// 	if (status == HAL_ERROR)
// 		return status;
// 	else
// 	{
// 		i += current_write_size;
// 	}
	
//   }

// 	return 0;
// }


// uint8_t RdMulti(
// 		VL53L5CX_Platform *p_platform,
// 		uint16_t RegisterAddress,
// 		uint8_t *p_values,
// 		uint32_t size)
// {
// 	uint8_t status = 255;
	
// 	/* Need to be implemented by customer. This function returns 0 if OK */
	
// 	uint8_t buffer[2];

// 	buffer[0] = (uint8_t)(RegisterAddress >> 8);
// 	buffer[1] = (uint8_t)(RegisterAddress & 0xFF);

// 	if (HAL_I2C_Master_Transmit(p_platform->dev_i2c, (uint16_t)(p_platform->address << 1), buffer, 2, HAL_MAX_DELAY) != HAL_OK) {
// 		return 1;
// 	}

// 	if (HAL_I2C_Master_Receive(p_platform->dev_i2c, (uint16_t)(p_platform->address << 1), p_values, size, HAL_MAX_DELAY) != HAL_OK) {
// 		return 1;
// 	}

// 	return 0;
// }

