#ifndef __PIR_LUM_H
#define __PIR_LUM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "stdint.h"

enum day_moment {JOUR, CREPUSCULE, NUIT};

struct lumionsite {
  int light_sensor;
  enum day_moment day_moment;
};

void seed_light(struct lumionsite *lum);
int seed_pir();
void bus_Scanning();
uint8_t onsemi_pir();
void debug_register();

#endif /* __MAIN_H */