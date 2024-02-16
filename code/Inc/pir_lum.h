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

typedef enum 
{
  JOUR, 
  CREPUSCULE, 
  NUIT
} day_moment_t;

typedef struct
{
  int light_sensor;
  day_moment_t day_moment;
} Luminosite_t;

int seed_light(Luminosite_t *lum);
int is_movement();
void bus_Scanning();
uint8_t onsemi_pir();
void debug_register();

#endif /* __MAIN_H */