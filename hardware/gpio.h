#ifndef _GPIO_H
#define _GPIO_H

#include "stm32f10x.h"


#define LED_R_ON GPIO_SetBits(GPIOB , GPIO_Pin_5);
#define LED_R_OFF GPIO_ResetBits(GPIOB , GPIO_Pin_5);



#define LED_G_ON GPIO_SetBits(GPIOB , GPIO_Pin_0);
#define LED_G_OFF GPIO_ResetBits(GPIOB , GPIO_Pin_0);





void gpioInit(void);

































#endif





