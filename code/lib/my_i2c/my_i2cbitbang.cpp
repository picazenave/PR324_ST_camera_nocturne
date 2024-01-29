/*
 * i2cbitbang.cpp
 *
 *  Created on: 24 lis 2019
 *      Author: Tata
 *      based on: https://en.wikipedia.org/wiki/I%C2%B2C#Example_of_bit-banging_the_I.C2.B2C_Master_protocol
 */

#include "my_i2cbitbang.h"

// extern const i2cbbConfig_t i2cbbConfigTable[NUMBER_OF_I2CBB_INSTANCES];
uint8_t slaveAddress = 0x3C; // camera address
int8_t started = false;
uint32_t error = I2CBB_ERROR_NONE;
i2cMode_t i2cMode = MODE_I2C;					   // default is I2c Mode. There is also SCCB mode (e.g. for cam OV7670)
i2cbbSpeed_t speedSelect = SPEED_400k; // default speed 100kHz
uint32_t stretchTime_us = 1000;
ackMode_t ackMode = ACK_CHECK;
/*
 * ackMode parameter has two options:
 * 	ACK_CHECK: (default) ACK is check as normal i2c operation
 * 	ACK_IGNORE: to create SCCB communication (e.g. cam OV7670)
 */
void my_i2cbitbang_init()
{
	/*initalizing timing according to i2c standards
	 * https://www.analog.com/en/technical-articles/i2c-timing-definition-and-specification-guide-part-2.html#
	 */
	init_timings();
	i2c_conf_hardware();
}

void i2c_start_cond(void)
{
	if (started)
	{
		// if started, do a restart condition
		// set SDA to 1
		set_SDA();
		my_delay_hns(i2c_timings.susta);
		set_SCL();

		clock_stretching(stretchTime_us);
		// Repeated start setup time, minimum 4.7us
		my_delay_hns(i2c_timings.susta);
	}

	// TODO implement arbitration
	/*
	 conf_SDA(I2CBB_INPUT);
	 if (read_SDA() == 0)
	 {
	   arbitration_lost();
	 }
	 conf_SDA(I2CBB_OUTPUT);
   */

	// SCL is high, set SDA from 1 to 0.
	clear_SDA();
	my_delay_hns(i2c_timings.hdsta);
	clear_SCL();
	my_delay_hns(i2c_timings.low);
	started = true;
}

void i2c_stop_cond(void)
{
	// set SDA to 0
	clear_SDA();
	//  I2C_delay();
	my_delay_hns(i2c_timings.susto);

	set_SCL();

	clock_stretching(stretchTime_us);

	// Stop bit setup time, minimum 4us
	my_delay_hns(i2c_timings.susto);

	// SCL is high, set SDA from 0 to 1
	set_SDA();
	my_delay_hns(i2c_timings.buff);

	// TODO implement arbitration
	/*  if (read_SDA() == 0) {
		arbitration_lost();
	  }
	*/

	started = false;
}

// Write a bit to I2C bus
void i2c_write_bit(int8_t bit)
{

	if (bit)
	{
		set_SDA();
	}
	else
	{
		clear_SDA();
	}

	// SDA change propagation delay
	my_delay_hns(i2c_timings.sudat);

	// Set SCL high to indicate a new valid SDA value is available
	set_SCL();

	// Wait for SDA value to be read by slave, minimum of 4us for standard mode
	my_delay_hns(i2c_timings.high);

	clock_stretching(stretchTime_us);

	// TODO implement arbitration
	//  SCL is high, now data is valid
	//  If SDA is high, check that nobody else is driving SDA
	/*  conf_SDA(I2CBB_INPUT);
	  if (bit && (read_SDA() == 0)) {
		arbitration_lost();
	  }
	  conf_SDA(I2CBB_OUTPUT);
	*/
	// Clear the SCL to low in preparation for next change
	clear_SCL();
	my_delay_hns(i2c_timings.low);
}

// Read a bit from I2C bus
int8_t i2c_read_bit(void)
{
	int8_t bit;

	// Let the slave drive data
	set_SDA();

	// Wait for SDA value to be written by slave, minimum of 4us for standard mode
	my_delay_hns(i2c_timings.dvdat);

	// Set SCL high to indicate a new valid SDA value is available
	set_SCL();
	clock_stretching(stretchTime_us);

	// Wait for SDA value to be written by slave, minimum of 4us for standard mode
	my_delay_hns(i2c_timings.high);

	// SCL is high, read out bit
	bit = read_SDA();

	// Set SCL low in preparation for next operation
	clear_SCL();

	my_delay_hns(i2c_timings.low);

	return bit;
}

// Write a byte to I2C bus. Return 0 if ack by the slave.
int8_t i2c_write_byte(unsigned char byte)
{
	unsigned bit;
	int8_t nack;

	for (bit = 0; bit < 8; ++bit)
	{
		i2c_write_bit((byte & 0x80) != 0);
		byte <<= 1;
	}

	nack = i2c_read_bit();

	set_SDA();

	return nack;
}

// Read a byte from I2C bus
unsigned char i2c_read_byte(int8_t nack)
{
	unsigned char byte = 0;
	unsigned char bit;

	for (bit = 0; bit < 8; ++bit)
	{
		byte = (byte << 1) | i2c_read_bit();
	}

	i2c_write_bit(nack);
	set_SDA();

	return byte;
}

/*
 * timings are in hns units (0.1 um units)
 * e.g. 47 means 4.7 us
 */
#define MARGIN_100 -10
#define MARGIN_400 -6
#define MARGIN_50 10
#define MARGIN_10 0

#if (MARGIN_400 < -6)
#error "In I2Cbitbang class: MARGIN_100 need to be larger than -6"
#endif
#if (MARGIN_100 < -36)
#error "In I2Cbitbang class: MARGIN_100 need to be larger than 36"
#endif

void init_timings(void)
{
	switch (speedSelect)
	{
	case SPEED_400k:
		i2c_timings.hdsta = 6 + MARGIN_400;
		i2c_timings.susto = 6 + MARGIN_400;
		i2c_timings.susta = 6 + MARGIN_400;
		i2c_timings.sudat = 1;
		i2c_timings.dvdat = 9 + MARGIN_400;
		i2c_timings.dvack = 9 + MARGIN_400;
		i2c_timings.high = 6 + MARGIN_400;
		i2c_timings.low = 13 + MARGIN_400;
		i2c_timings.buff = 13 + MARGIN_400;
		break;
	case SPEED_10k:
		i2c_timings.hdsta = 400 + MARGIN_10;
		i2c_timings.susto = 400 + MARGIN_10;
		i2c_timings.susta = 470 + MARGIN_10;
		i2c_timings.sudat = 30;
		i2c_timings.dvdat = 360 + MARGIN_10;
		i2c_timings.dvack = 360 + MARGIN_10;
		i2c_timings.high = 400 + MARGIN_10;
		i2c_timings.low = 470 + MARGIN_10;
		i2c_timings.buff = 470 + MARGIN_10;
		break;
	case SPEED_50k:
		i2c_timings.hdsta = 80 + MARGIN_50;
		i2c_timings.susto = 80 + MARGIN_50;
		i2c_timings.susta = 94 + MARGIN_50;
		i2c_timings.sudat = 6;
		i2c_timings.dvdat = 72 + MARGIN_50;
		i2c_timings.dvack = 72 + MARGIN_50;
		i2c_timings.high = 80 + MARGIN_50;
		i2c_timings.low = 94 + MARGIN_50;
		i2c_timings.buff = 94 + MARGIN_50;
		break;
	case SPEED_100k:
	default:
		i2c_timings.hdsta = 40 + MARGIN_100;
		i2c_timings.susto = 40 + MARGIN_100;
		i2c_timings.susta = 47 + MARGIN_100;
		i2c_timings.sudat = 3;
		i2c_timings.dvdat = 36 + MARGIN_100;
		i2c_timings.dvack = 36 + MARGIN_100;
		i2c_timings.high = 40 + MARGIN_100;
		i2c_timings.low = 47 + MARGIN_100;
		i2c_timings.buff = 47 + MARGIN_100;
		break;
	}
}

ackMode_t getAckMode(void)
{
	return ackMode;
}

void setAckMode(ackMode_t ack)
{
	ackMode = ack;
}

void writeDatareg8(uint8_t reg, uint8_t *pData, uint16_t size)
{
	int8_t nack;
	uint8_t *ptr = pData;

	i2c_bus_init();

	i2c_start_cond();

	/*send slave addres*/
	nack = i2c_write_byte(slaveAddress & 0xFE);

	if ((ackMode == ACK_CHECK) && nack)
		error |= I2CBB_ERROR_NACK;

	/*send register addres*/
	nack = i2c_write_byte(reg);

	if ((ackMode == ACK_CHECK) && nack)
		error |= I2CBB_ERROR_NACK;

	/*send data addres*/
	while (size)
	{
		nack = i2c_write_byte(*ptr++);
		if (nack)
			break;
		size--;
	}

	i2c_stop_cond();

	if ((ackMode == ACK_CHECK) && nack)
		error |= I2CBB_ERROR_NACK;
}


void readDatareg8(uint8_t reg, uint8_t *pData, uint16_t size)
{
	int8_t nack;
	uint8_t *ptr;

	ptr = pData;

	i2c_bus_init();

	i2c_start_cond();
	nack = i2c_write_byte(slaveAddress & 0xFE);
	if ((ackMode == ACK_CHECK) && nack)
	{
		error |= I2CBB_ERROR_NACK;
	}

	nack = i2c_write_byte(reg);
	if ((ackMode == ACK_CHECK) && nack)
	{
		error |= I2CBB_ERROR_NACK;
	}

	if (i2cMode == MODE_SCCB)
	{
		i2c_stop_cond();
	}

	i2c_start_cond();

	nack = i2c_write_byte(slaveAddress | 0x01);
	if ((ackMode == ACK_CHECK) && nack)
	{
		error |= I2CBB_ERROR_NACK;
	}
	while (size)
	{
		*ptr++ = i2c_read_byte((size > 1) ? ACK : NACK);
		size--;
	}

	i2c_stop_cond();
}

void readDatareg16(uint16_t reg, uint8_t *pData, uint16_t size)
{
	int8_t nack;
	uint8_t *ptr;

	ptr = pData;

	i2c_bus_init();

	i2c_start_cond();
	nack = i2c_write_byte(slaveAddress & 0xFE);
	if ((ackMode == ACK_CHECK) && nack)
	{
		error |= I2CBB_ERROR_NACK;
	}

	nack = i2c_write_byte((uint8_t)(reg>>8));
	if ((ackMode == ACK_CHECK) && nack)
	{
		error |= I2CBB_ERROR_NACK;
	}

	if (i2cMode == MODE_SCCB)
	{
		i2c_stop_cond();
	}

	nack = i2c_write_byte((uint8_t)(reg&0xFF));
	if ((ackMode == ACK_CHECK) && nack)
	{
		error |= I2CBB_ERROR_NACK;
	}

	if (i2cMode == MODE_SCCB)
	{
		i2c_stop_cond();
	}

	i2c_start_cond();

	nack = i2c_write_byte(slaveAddress | 0x01);
	if ((ackMode == ACK_CHECK) && nack)
	{
		error |= I2CBB_ERROR_NACK;
	}
	while (size)
	{
		*ptr++ = i2c_read_byte((size > 1) ? ACK : NACK);
		size--;
	}

	i2c_stop_cond();
}

i2cMode_t getI2CMode(void)
{
	return i2cMode;
}

void setI2CMode(i2cMode_t i2cm)
{
	i2cMode = i2cm;
}

uint32_t getError(void)
{
	return error;
}

void setSpeed(i2cbbSpeed_t fHz)
{
	speedSelect = fHz;
	init_timings();
}

void clock_stretching(uint32_t t_us)
{
	while ((read_SCL() == 0) && (t_us > 0))
	{
		my_delay_us(1);
		t_us--;
		if (t_us == 0)
			error |= I2CBB_ERROR_STRETCH_TOUT;
	}
}
