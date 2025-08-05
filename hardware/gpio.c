#include "gpio.h"






void gpioInit(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_0;//5red 0 green
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB , GPIO_Pin_0);
	GPIO_SetBits(GPIOB , GPIO_Pin_5);
}




































