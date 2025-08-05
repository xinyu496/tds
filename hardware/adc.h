#ifndef _ADC_H
#define _ADC_H

#include "stm32f10x.h"


#define averageNum 100

//ADC1_BASE地址：0x4000 0000+0x2400 即ADC1的基地址，而他的规则数据寄存器的偏移地址是：0x4c
#define ADC1_DR_Address    ((u32)ADC1_BASE+0x4c)


void ADCx_Init(void);


//u16 readTempertureValue(void);//读取单片机温度原始值



u16 readAdc2Chennl(void);//读取ADC2通道6原始值


u16 readAverageTempertureValue(void);//取温度均值






































#endif





