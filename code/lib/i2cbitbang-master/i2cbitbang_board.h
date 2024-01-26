/*
 * i2cbitbang_board.h
 *
 *  Created on: 25 lis 2019
 *      Author: Tata
 */

#ifndef INC_I2CBITBANG_BOARD_H_
#define INC_I2CBITBANG_BOARD_H_

#include "stm32f4xx.h"

#define I2CBB_SCCB   0
#define I2CBB_OLED	 1
#define I2CBB_LSM	 2

#define NUMBER_OF_I2CBB_INSTANCES 1

/*
 * BE CAREFUL!
 * It supports only pins which are GPIO as non-alternate function
 *
 */

#define I2CBB_DECLARE_STRUCTURE() 									\
	const i2cbbConfig_t i2cbbConfigTable[NUMBER_OF_I2CBB_INSTANCES] = 	 \
	{ 																\
		{GPIOC, GPIO_PIN_4,	 		/*SCL 0*/								\
		 GPIOC,	GPIO_PIN_5}, 		/*SDA 0*/								\
	}

#endif /* INC_I2CBITBANG_BOARD_H_ */
