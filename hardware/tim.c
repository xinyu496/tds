#include "tim.h"

//中断定时器10ms
static void timConfig_10ms(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_Period = 100 -1;	
	TIM_TimeBaseStructure.TIM_Prescaler =  7200 -1;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	

	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
	
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);// 清除计数器中断标志位
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);// 开启计数器中断

	TIM_Cmd(TIM6, ENABLE);
}

//蜂鸣器定时器配置
static void timConfigBeep(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_Period = 80 -1;	
	TIM_TimeBaseStructure.TIM_Prescaler =  9 -1;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/*--------------------输出比较结构体初始化-------------------*/		
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// 输出使能
	// 互补输出使能
//	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 7;// 占空比
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;// 输出通道电平极性配置
//	// 互补输出通道电平极性配置
//	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	// 输出通道空闲电平极性配置
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	// 互补输出通道空闲电平极性配置
//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

//	/*-------------------刹车和死区结构体初始化-------------------*/
//	// 有关刹车和死区结构体的成员具体可参考BDTR寄存器的描述
//	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
//  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
//  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
//  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
//	// 输出比较信号死区时间配置，具体如何计算可参考 BDTR:UTG[7:0]的描述
//	// 这里配置的死区时间为152ns
//  TIM_BDTRInitStructure.TIM_DeadTime = 11;
//  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
//	// 当BKIN引脚检测到高电平的时候，输出比较信号被禁止，就好像是刹车一样
//  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
//  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
//  TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStructure);
	
	// 使能计数器
	TIM_Cmd(TIM1, 		DISABLE);	
	// 主输出使能，当使用的是通用定时器时，这句不需要
	TIM_CtrlPWMOutputs(TIM1, DISABLE);
}

//蜂鸣器io配置
static void timGpioInitBeep(GPIOMode_TypeDef data) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = data;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

//  // 输出比较通道互补通道 GPIO 初始化
//	RCC_APB2PeriphClockCmd(ADVANCE_TIM_CH1N_GPIO_CLK, ENABLE);
//  GPIO_InitStructure.GPIO_Pin =  ADVANCE_TIM_CH1N_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(ADVANCE_TIM_CH1N_PORT, &GPIO_InitStructure);

//  // 输出比较通道刹车通道 GPIO 初始化
//	RCC_APB2PeriphClockCmd(ADVANCE_TIM_BKIN_GPIO_CLK, ENABLE);
//  GPIO_InitStructure.GPIO_Pin =  ADVANCE_TIM_BKIN_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(ADVANCE_TIM_BKIN_PORT, &GPIO_InitStructure);
	// BKIN引脚默认先输出低电平
//	GPIO_ResetBits(ADVANCE_TIM_BKIN_PORT,ADVANCE_TIM_BKIN_PIN);	
}


//蜂鸣器PWM
void timInit(void)
{
	timConfigBeep();//蜂鸣器定时器
	timGpioInitBeep(GPIO_Mode_Out_PP);//蜂鸣器IO
	
	timConfig_10ms();//10ms定时器
}



