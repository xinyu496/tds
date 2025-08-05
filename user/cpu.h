#ifndef _CPU_H
#define _CPU_H

#include "stm32f10x.h"
#include "tim.h"
#include "rtc.h"
#include "include.h"
#include "gpio.h"
#include "uart.h"
#include "dac.h"
#include "adc.h"
#include "iic.h"



void beepOn_Off(u8 state);
void initCpu(void);


#endif




