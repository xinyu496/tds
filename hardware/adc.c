#include "adc.h"


static uint32_t ADC_ConvertedValue;


/**
  * @brief  ADC GPIO 初始化
  * @param  无
  * @retval 无
  */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 打开 ADC IO端口时钟
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA, ENABLE );
	
	// 配置 ADC IO 引脚模式
	// 必须为模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化 ADC IO
	GPIO_Init(GPIOA, &GPIO_InitStructure);				
}

/**
  * @brief  配置ADC工作模式  双ADC同步规则 u32 主机低16位 从机高16位
  * @param  无
  * @retval 无
  */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	// 打开DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// 打开ADC时钟
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_ADC2, ENABLE );
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_ADC1, ENABLE );
	
  /* ------------------DMA模式配置---------------- */	
	// 复位DMA控制器
	DMA_DeInit(DMA1_Channel1);	
	// 配置 DMA 初始化结构体
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&( ADC1->DR ));	
	// 存储器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&ADC_ConvertedValue);	
	// 数据源来自外设
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	
	// 缓冲区大小，应该等于数据目的地的大小
	DMA_InitStructure.DMA_BufferSize = 1;	
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 存储器地址递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 	
	// 外设数据大小
	DMA_InitStructure.DMA_PeripheralDataSize = 
	                                  DMA_PeripheralDataSize_Word;	
	// 内存数据大小，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;	
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	
	// 禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
	// 初始化DMA
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	
	// 使能 DMA 通道
	DMA_Cmd(DMA1_Channel1 , ENABLE);
	
	/* ----------------ADCx_1 模式配置--------------------- */
	// 双ADC的规则同步
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;	
	// 扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 
	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	// 转换通道个数
	ADC_InitStructure.ADC_NbrOfChannel = 1;			
	// 初始化ADC
	ADC_Init(ADC1, &ADC_InitStructure);	
	// 配置ADC时钟Ｎ狿CLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 	
	// 配置ADC 通道的转换顺序和采样时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, 
	                         ADC_SampleTime_239Cycles5);	
													 
													 //使能温度传感器和内部参考电压   
  ADC_TempSensorVrefintCmd(ENABLE);     
	
	// 使能ADC DMA 请求
	ADC_DMACmd(ADC1, ENABLE);
	
	
		/* ----------------ADCx_2 模式配置--------------------- */
	// 双ADC的规则同步
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;	
	// 扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 
	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = 
	                           ADC_ExternalTrigConv_None;
	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	// 转换通道个数
	ADC_InitStructure.ADC_NbrOfChannel = 1;			
	// 初始化ADC
	ADC_Init(ADC2, &ADC_InitStructure);	
	// 配置ADC时钟为PCLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 	
	// 配置ADC 通道的转换顺序和采样时间
	ADC_RegularChannelConfig(ADC2, ADC_Channel_6, 1, 
	                         ADC_SampleTime_239Cycles5);
	/* 使能ADCx_2的外部触发转换 */
  ADC_ExternalTrigConvCmd(ADC2, ENABLE);
	
	/* ----------------ADCx_1 校准--------------------- */
	// 开启ADC ，并开始转换
	ADC_Cmd(ADC1, ENABLE);	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADC1);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC1));	
	// ADC开始校准
	ADC_StartCalibration(ADC1);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADC1));
	
  /* ----------------ADCx_2 校准--------------------- */
		// 开启ADC ，并开始转换
	ADC_Cmd(ADC2, ENABLE);	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADC2);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC2));	
	// ADC开始校准
	ADC_StartCalibration(ADC2);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADC2));

	// 由于没有采用外部触发，所以使用软件触发ADC转换 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

//static void ADC_NVIC_Config(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;


//  // 配置中断优先级
//  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//}


/**
  * @brief  ADC初始化
  * @param  无
  * @retval 无
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
//	ADC_NVIC_Config();
}

//读取单片机温度原始值
static u16 readTempertureValue(void)
{
	u16 data;
	
	data = (0x06ee - (u16)(ADC_ConvertedValue)) / 0x05 + 25;
	
	return data;
}

//取温度均值
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

//读取ADC2通道6原始值
u16 readAdc2Chennl(void)
{
	u16 data;
	
	data = (u16)(ADC_ConvertedValue>>16);
	
	return data;
}
	





































