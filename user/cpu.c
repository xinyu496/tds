#include "cpu.h"
#include "rtc.h"

//�жϳ�ʼ��
static void interruptInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	// �����ж���Ϊ0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn ;	
	// ���������ȼ�Ϊ 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// ������ռ���ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//ʱ�ӳ�ʼ��
static void periphClockInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//����������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);//10ms��ʱ������ʱ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//������Io����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);// LED Io����ʱ��
}




//���Ʒ���������
void beepOn_Off(u8 state)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(state == ENABLE)
	{
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		// ʹ�ܼ�����
		TIM_Cmd(TIM1, 		ENABLE);	
		// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
		TIM_CtrlPWMOutputs(TIM1, ENABLE);
	}
	else if(state == DISABLE)
	{
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		// ʹ�ܼ�����
		TIM_Cmd(TIM1, 		DISABLE);	
		// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
		TIM_CtrlPWMOutputs(TIM1, DISABLE);
	}
}




















//�����ʼ��
void initCpu(void)
{
	periphClockInit();//ʱ�ӳ�ʼ��
	interruptInit();//�жϳ�ʼ��
	
	timInit();//��ʱ����ʼ��
	RTC_Init();
	gpioInit();
	USART_Config();
	dacInit();
	ADCx_Init();
	iicInit();
}











