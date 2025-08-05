#include "tim.h"

//�ж϶�ʱ��10ms
static void timConfig_10ms(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_Period = 100 -1;	
	TIM_TimeBaseStructure.TIM_Prescaler =  7200 -1;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	

	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
	
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);// ����������жϱ�־λ
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);// �����������ж�

	TIM_Cmd(TIM6, ENABLE);
}

//��������ʱ������
static void timConfigBeep(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_Period = 80 -1;	
	TIM_TimeBaseStructure.TIM_Prescaler =  9 -1;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWMģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;// ���ʹ��
	// �������ʹ��
//	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 7;// ռ�ձ�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;// ���ͨ����ƽ��������
//	// �������ͨ����ƽ��������
//	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	// ���ͨ�����е�ƽ��������
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	// �������ͨ�����е�ƽ��������
//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

//	/*-------------------ɲ���������ṹ���ʼ��-------------------*/
//	// �й�ɲ���������ṹ��ĳ�Ա����ɲο�BDTR�Ĵ���������
//	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
//  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
//  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
//  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
//	// ����Ƚ��ź�����ʱ�����ã�������μ���ɲο� BDTR:UTG[7:0]������
//	// �������õ�����ʱ��Ϊ152ns
//  TIM_BDTRInitStructure.TIM_DeadTime = 11;
//  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
//	// ��BKIN���ż�⵽�ߵ�ƽ��ʱ������Ƚ��źű���ֹ���ͺ�����ɲ��һ��
//  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
//  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
//  TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStructure);
	
	// ʹ�ܼ�����
	TIM_Cmd(TIM1, 		DISABLE);	
	// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
	TIM_CtrlPWMOutputs(TIM1, DISABLE);
}

//������io����
static void timGpioInitBeep(GPIOMode_TypeDef data) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = data;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

//  // ����Ƚ�ͨ������ͨ�� GPIO ��ʼ��
//	RCC_APB2PeriphClockCmd(ADVANCE_TIM_CH1N_GPIO_CLK, ENABLE);
//  GPIO_InitStructure.GPIO_Pin =  ADVANCE_TIM_CH1N_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(ADVANCE_TIM_CH1N_PORT, &GPIO_InitStructure);

//  // ����Ƚ�ͨ��ɲ��ͨ�� GPIO ��ʼ��
//	RCC_APB2PeriphClockCmd(ADVANCE_TIM_BKIN_GPIO_CLK, ENABLE);
//  GPIO_InitStructure.GPIO_Pin =  ADVANCE_TIM_BKIN_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(ADVANCE_TIM_BKIN_PORT, &GPIO_InitStructure);
	// BKIN����Ĭ��������͵�ƽ
//	GPIO_ResetBits(ADVANCE_TIM_BKIN_PORT,ADVANCE_TIM_BKIN_PIN);	
}


//������PWM
void timInit(void)
{
	timConfigBeep();//��������ʱ��
	timGpioInitBeep(GPIO_Mode_Out_PP);//������IO
	
	timConfig_10ms();//10ms��ʱ��
}



