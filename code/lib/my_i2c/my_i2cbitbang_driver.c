#include "my_i2cbitbang.h"

I2CBB_DECLARE_STRUCTURE();

extern int8_t error;

void i2c_conf_hardware(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if (i2cbbConfigTable[0].GPIOclk_port == GPIOA || i2cbbConfigTable[0].GPIOdta_port == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	if (i2cbbConfigTable[0].GPIOclk_port == GPIOB || i2cbbConfigTable[0].GPIOdta_port == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	if (i2cbbConfigTable[0].GPIOclk_port == GPIOC || i2cbbConfigTable[0].GPIOdta_port == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	if (i2cbbConfigTable[0].GPIOclk_port == GPIOD || i2cbbConfigTable[0].GPIOdta_port == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();
	if (i2cbbConfigTable[0].GPIOclk_port == GPIOE || i2cbbConfigTable[0].GPIOdta_port == GPIOE)
		__HAL_RCC_GPIOE_CLK_ENABLE();
	// if (i2cbbConfigTable[0].GPIOclk_port == GPIOF || i2cbbConfigTable[0].GPIOdta_port == GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE();
	// if (i2cbbConfigTable[0].GPIOclk_port == GPIOG || i2cbbConfigTable[0].GPIOdta_port == GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE();
	// if (i2cbbConfigTable[0].GPIOclk_port == GPIOH || i2cbbConfigTable[0].GPIOdta_port == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();
	// if (i2cbbConfigTable[0].GPIOclk_port == GPIOI || i2cbbConfigTable[0].GPIOdta_port == GPIOI) __HAL_RCC_GPIOH_CLK_ENABLE();

	HAL_GPIO_WritePin(i2cbbConfigTable[0].GPIOdta_port,
					  i2cbbConfigTable[0].pinDta,
					  GPIO_PIN_SET);
	HAL_GPIO_WritePin(i2cbbConfigTable[0].GPIOclk_port,
					  i2cbbConfigTable[0].pinClk,
					  GPIO_PIN_SET);

	/*Configure GPIO clk */

	GPIO_InitStruct.Pin = i2cbbConfigTable[0].pinClk;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(i2cbbConfigTable[0].GPIOclk_port, &GPIO_InitStruct);

	/*Configure GPIO dta */
	GPIO_InitStruct.Pin = i2cbbConfigTable[0].pinDta;
	HAL_GPIO_Init(i2cbbConfigTable[0].GPIOdta_port, &GPIO_InitStruct);
}

void int8_ti2c_bus_init(void)
{
	error = I2CBB_ERROR_NONE;
	set_SDA();
	set_SCL();

	// TODO confirm that SDA and SCL are set, otherwise bus error
}

int8_t read_SCL(void) // Return current level of SCL line, 0 or 1
{
	if (HAL_GPIO_ReadPin(i2cbbConfigTable[0].GPIOclk_port,
						 i2cbbConfigTable[0].pinClk) == GPIO_PIN_SET)
		return 1;
	else
		return 0;
}
int8_t read_SDA(void) // Return current level of SDA line, 0 or 1
{

	if (HAL_GPIO_ReadPin(i2cbbConfigTable[0].GPIOdta_port,
						 i2cbbConfigTable[0].pinDta) == GPIO_PIN_SET)
		return 1;
	else
		return 0;
	return 0;
}

void set_SCL(void) // Do not drive SCL (set pin high-impedance)
{
	//	conf_SCL(I2CBB_INTPUT);
	HAL_GPIO_WritePin(i2cbbConfigTable[0].GPIOclk_port,
					  i2cbbConfigTable[0].pinClk,
					  GPIO_PIN_SET);
}

void clear_SCL(void) // Actively drive SCL signal low
{
	HAL_GPIO_WritePin(i2cbbConfigTable[0].GPIOclk_port,
					  i2cbbConfigTable[0].pinClk,
					  GPIO_PIN_RESET);
}

void set_SDA(void) // Do not drive SDA (set pin high-impedance)
{
	HAL_GPIO_WritePin(i2cbbConfigTable[0].GPIOdta_port,
					  i2cbbConfigTable[0].pinDta,
					  GPIO_PIN_SET);
}

void clear_SDA(void) // Actively drive SDA signal low
{
	HAL_GPIO_WritePin(i2cbbConfigTable[0].GPIOdta_port,
					  i2cbbConfigTable[0].pinDta,
					  GPIO_PIN_RESET);
}
