#ifndef _ADC_H
#define _ADC_H

#include "stm32f10x.h"


#define averageNum 100

//ADC1_BASE��ַ��0x4000 0000+0x2400 ��ADC1�Ļ���ַ�������Ĺ������ݼĴ�����ƫ�Ƶ�ַ�ǣ�0x4c
#define ADC1_DR_Address    ((u32)ADC1_BASE+0x4c)


void ADCx_Init(void);


//u16 readTempertureValue(void);//��ȡ��Ƭ���¶�ԭʼֵ



u16 readAdc2Chennl(void);//��ȡADC2ͨ��6ԭʼֵ


u16 readAverageTempertureValue(void);//ȡ�¶Ⱦ�ֵ






































#endif





