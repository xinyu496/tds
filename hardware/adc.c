#include "adc.h"


static uint32_t ADC_ConvertedValue;


/**
  * @brief  ADC GPIO ��ʼ��
  * @param  ��
  * @retval ��
  */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// �� ADC IO�˿�ʱ��
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA, ENABLE );
	
	// ���� ADC IO ����ģʽ
	// ����Ϊģ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// ��ʼ�� ADC IO
	GPIO_Init(GPIOA, &GPIO_InitStructure);				
}

/**
  * @brief  ����ADC����ģʽ  ˫ADCͬ������ u32 ������16λ �ӻ���16λ
  * @param  ��
  * @retval ��
  */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	// ��DMAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// ��ADCʱ��
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_ADC2, ENABLE );
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_ADC1, ENABLE );
	
  /* ------------------DMAģʽ����---------------- */	
	// ��λDMA������
	DMA_DeInit(DMA1_Channel1);	
	// ���� DMA ��ʼ���ṹ��
	// �����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&( ADC1->DR ));	
	// �洢����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&ADC_ConvertedValue);	
	// ����Դ��������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	
	// ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С
	DMA_InitStructure.DMA_BufferSize = 1;	
	// ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// �洢����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 	
	// �������ݴ�С
	DMA_InitStructure.DMA_PeripheralDataSize = 
	                                  DMA_PeripheralDataSize_Word;	
	// �ڴ����ݴ�С�����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;	
	// ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	
	// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
	// ��ʼ��DMA
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	
	// ʹ�� DMA ͨ��
	DMA_Cmd(DMA1_Channel1 , ENABLE);
	
	/* ----------------ADCx_1 ģʽ����--------------------- */
	// ˫ADC�Ĺ���ͬ��
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;	
	// ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 
	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// �����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	// ת��ͨ������
	ADC_InitStructure.ADC_NbrOfChannel = 1;			
	// ��ʼ��ADC
	ADC_Init(ADC1, &ADC_InitStructure);	
	// ����ADCʱ�ӣΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 	
	// ����ADC ͨ����ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, 
	                         ADC_SampleTime_239Cycles5);	
													 
													 //ʹ���¶ȴ��������ڲ��ο���ѹ   
  ADC_TempSensorVrefintCmd(ENABLE);     
	
	// ʹ��ADC DMA ����
	ADC_DMACmd(ADC1, ENABLE);
	
	
		/* ----------------ADCx_2 ģʽ����--------------------- */
	// ˫ADC�Ĺ���ͬ��
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;	
	// ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 
	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// �����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = 
	                           ADC_ExternalTrigConv_None;
	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	// ת��ͨ������
	ADC_InitStructure.ADC_NbrOfChannel = 1;			
	// ��ʼ��ADC
	ADC_Init(ADC2, &ADC_InitStructure);	
	// ����ADCʱ��ΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 	
	// ����ADC ͨ����ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADC2, ADC_Channel_6, 1, 
	                         ADC_SampleTime_239Cycles5);
	/* ʹ��ADCx_2���ⲿ����ת�� */
  ADC_ExternalTrigConvCmd(ADC2, ENABLE);
	
	/* ----------------ADCx_1 У׼--------------------- */
	// ����ADC ������ʼת��
	ADC_Cmd(ADC1, ENABLE);	
	// ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(ADC1);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADC1));	
	// ADC��ʼУ׼
	ADC_StartCalibration(ADC1);
	// �ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADC1));
	
  /* ----------------ADCx_2 У׼--------------------- */
		// ����ADC ������ʼת��
	ADC_Cmd(ADC2, ENABLE);	
	// ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(ADC2);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADC2));	
	// ADC��ʼУ׼
	ADC_StartCalibration(ADC2);
	// �ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADC2));

	// ����û�в����ⲿ����������ʹ���������ADCת�� 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

//static void ADC_NVIC_Config(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;


//  // �����ж����ȼ�
//  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//}


/**
  * @brief  ADC��ʼ��
  * @param  ��
  * @retval ��
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
//	ADC_NVIC_Config();
}

//��ȡ��Ƭ���¶�ԭʼֵ
static u16 readTempertureValue(void)
{
	u16 data;
	
	data = (0x06ee - (u16)(ADC_ConvertedValue)) / 0x05 + 25;
	
	return data;
}

//ȡ�¶Ⱦ�ֵ
u16 readAverageTempertureValue(void)
{
	static u16 cnt = 0 , cnt1 = 0 , ii  ;
	static float  average[averageNum] , sum;
	u16 temperture;
	
	temperture = readTempertureValue();
	
	sum = 0;
	average[cnt] = (float)temperture / 100;
	
	cnt++;
	if(cnt == averageNum)
	{
		cnt1 = averageNum;
		cnt = 0;
	}
	else if(cnt1 != averageNum)
	{
		cnt1 = cnt;
	}
	for(ii = 0; ii < cnt1 ; ii++)
	{
		sum = average[ii] + sum;
	}
	temperture = (u16)(sum / cnt1 * 100) ;
	
	return temperture;
}

//��ȡADC2ͨ��6ԭʼֵ
u16 readAdc2Chennl(void)
{
	u16 data;
	
	data = (u16)(ADC_ConvertedValue>>16);
	
	return data;
}
	





































