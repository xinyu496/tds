/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "include.h"
#include "cpu.h"

//
#include "FreeRTOS.h"
#include "queue.h"

/* 用于统计运行时间 */
volatile uint32_t CPU_RunTime = 0UL;
static u8 time;

u16 p_CNT_1s;
extern __IO uint32_t ADC_ConvertedValue;

u8 p_rxbuff[512];
u8 p_txbuff[512];

extern QueueHandle_t modbusQueue ;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

///**
//  * @brief  This function handles SVCall exception.
//  * @param  None
//  * @retval None
//  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

///**
//  * @brief  This function handles PendSVC exception.
//  * @param  None
//  * @retval None
//  */
//void PendSV_Handler(void)
//{
//}





//10ms定时器中断
void  TIM6_IRQHandler(void)
{
	if ( TIM_GetITStatus( TIM6, TIM_IT_Update) != RESET ) 
	{	
		time++;
		if(time == 100)
		{
			time = 0;
			p_CNT_1s++;
			
			
		}
		TIM_ClearITPendingBit(TIM6 , TIM_FLAG_Update);  		 
	}		 	
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 
//systick中断服务函数
extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{	
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  /* INCLUDE_xTaskGetSchedulerState */  
        xPortSysTickHandler();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  /* INCLUDE_xTaskGetSchedulerState */
}

void ADC1_2_IRQHandler(void)
{	
	if (ADC_GetITStatus(ADC2,ADC_IT_EOC)==SET) 
	{
		// 读取ADC的转换值
//		ADC_ConvertedValue = ADC_GetConversionValue(ADC2);
	}
	ADC_ClearITPendingBit(ADC2,ADC_IT_EOC);
}

// 串口中断服务函数
void UART4_IRQHandler(void)
{
//  uint8_t ucTemp;
//	static u16 cnt = 0;
	uint16_t t , r;
	
	if(USART_GetITStatus(UART4,USART_IT_IDLE)!=RESET)
	{		
//		ucTemp = USART_ReceiveData(USART1);
//    p_rxbuff[cnt++] = ucTemp;
		DMA_Cmd(DMA2_Channel3,DISABLE);                         //关闭DMA传输

		t = DMA_GetCurrDataCounter(DMA2_Channel3);              //获取剩余的数据数量
		r = 512 - t;
		p_rxbuff[0] = (u8)(r >> 8);
		p_rxbuff[1] = (u8)r;
		
    xQueueSendToFrontFromISR(modbusQueue , p_rxbuff , 0);
		
		DMA_SetCurrDataCounter(DMA2_Channel3,512);    //重新设置传输的数据数量

		DMA_Cmd(DMA2_Channel3,ENABLE);                          //开启DMA传输
		
		USART_ReceiveData(UART4);                              //读取一次数据，不然会一直进中断
		USART_ClearFlag(UART4,USART_FLAG_IDLE);                //清除串口空闲中断标志位
	}	 
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
