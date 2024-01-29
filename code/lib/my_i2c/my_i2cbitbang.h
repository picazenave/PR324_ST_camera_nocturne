/*
 * i2cbitbang.h
 *
 *  Created on: 24 lis 2019
 *      Author: Tata
 */

#ifndef SRC_I2CBITBANG_H_
#define SRC_I2CBITBANG_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "i2cbitbang_board.h"
#include "my_delay.h"

#define i2cbbconfigASSERT(x) \
    if ((x) == 0)            \
    {                        \
        for (;;)             \
            ;                \
    }
#define START_YES 1
#define START_NO 0
#define STOP_YES 1
#define STOP_NO 0
#define ACK 0
#define NACK 1

#define I2CBB_ERROR_NONE 0x0UL
#define I2CBB_ERROR_I2CBB 0x1UL
#define I2CBB_ERROR_NACK 0x2UL
#define I2CBB_ERROR_STRETCH_TOUT 0x4UL
#define I2CBB_ERROR_ARBITRATION 0x8UL

typedef struct
{
    GPIO_TypeDef *GPIOclk_port;
    uint32_t pinClk;
    GPIO_TypeDef *GPIOdta_port;
    uint32_t pinDta;
} i2cbbConfig_t;

typedef enum
{
    SPEED_100k = 0,
    SPEED_400k = 1,
    SPEED_50k = 2,
    SPEED_10k = 3
} i2cbbSpeed_t;

typedef enum
{
    ACK_CHECK = 0,
    ACK_IGNORE = 1 // for SCCB interface
} ackMode_t;

typedef enum
{
    MODE_I2C = 0,
    MODE_SCCB = 1 // for SCCB interface
} i2cMode_t;

// uint32_t instance;
// uint8_t slaveAddress;
// i2cbbSpeed_t speedSelect;
// ackMode_t ackMode;
// i2cMode_t i2cMode;
// uint32_t error;
// uint32_t stretchTime_us;
// int8_t started; // global data
void i2c_conf_hardware(void);
int8_t read_SCL(void);  // Return current level of SCL line, 0 or 1
int8_t read_SDA(void);  // Return current level of SDA line, 0 or 1
void set_SCL(void);   // Do not drive SCL (set pin high-impedance)
void clear_SCL(void); // Actively drive SCL signal low
void set_SDA(void);   // Do not drive SDA (set pin high-impedance)
void clear_SDA(void); // Actively drive SDA signal low
struct i2c_timings_t  /*timing according to i2c standards*/
{
    uint16_t hdsta; // hold time for start condition /the minimum time the DTA should be low before SCL goes low/
    uint16_t susta; // set-up time for repeated START condition
    uint16_t susto; // setup time for stop condition
    uint16_t sudat; // the minimum amount of time required for SDA to have reached a stable level before an SCL transition takes place
    uint16_t dvdat; // data validity time
    uint16_t dvack; // acknoqledge validity time
    uint16_t high;  // high period of SCL
    uint16_t low;   // low period of SCL /IMPORTANT !!!must be > dvdat and > sudat/
    uint16_t buff;  // buffer time /the bus free time between stop and start conditions/
} i2c_timings;
void i2c_bus_init(void);
void i2c_start_cond(void);
void i2c_stop_cond(void);
void i2c_write_bit(int8_t bit);
int8_t i2c_read_bit(void);
int8_t i2c_write_byte(unsigned char byte);
void init_timings(void); // initialize timing structure t according to i2c standard
unsigned char i2c_read_byte(int8_t nack);
void arbitration_lost(void);
void clock_stretching(uint32_t t_us);
void my_i2cbitbang_init();
void setSpeed(i2cbbSpeed_t fHz);
ackMode_t getAckMode(void);
i2cMode_t getI2CMode(void);
void setI2CMode(i2cMode_t i2cm);
void setAckMode(ackMode_t ack);
i2cbbSpeed_t getSpeed(void);
uint32_t getInstance(void);
void writeDatareg8(uint8_t reg, uint8_t *pData, uint16_t size);
void readDatareg8(uint8_t reg, uint8_t *pData, uint16_t size);
void writeDatareg16(uint16_t reg, uint8_t *pData, uint16_t size);
void readDatareg16(uint16_t reg, uint8_t *pData, uint16_t size);
uint32_t getError(void);

#endif /* SRC_I2CBITBANG_H_ */
