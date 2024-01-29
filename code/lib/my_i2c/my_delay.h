#ifndef MY_DELAY_H_
#define MY_DELAY_H_

#include "stm32f4xx_hal.h"

void my_delay_init();

void my_delay_hns(uint32_t hns);
void my_delay_us(uint32_t us);
void my_delay_ms(uint32_t ms);

#endif /* DELAY_H_ */
