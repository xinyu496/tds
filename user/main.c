#include "stm32f10x.h"
#include "cpu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // 用于获取时间种子
#include "queue.h"

DateTime p_clockDisplay;

 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED1任务句柄 */
static TaskHandle_t LED1_Task_Handle = NULL;


static TaskHandle_t CPU_Task_Handle = NULL;



static TaskHandle_t readAdTask_Handle = NULL;
static TaskHandle_t modbusReceiveTask_Handle = NULL;

extern u8 p_rxbuff[512];
extern u8 p_txbuff[512];

float regisiter[128];

QueueHandle_t modbusQueue =NULL;

#define  modbusQueueLen    1   /* 队列的长度，最大可包含多少个消息 */
#define  modbusQueueSize   512   /* 队列中每个消息大小（字节） */

/**********************************************************************
  * @ 函数名  ： LED_Task
  * @ 功能说明： LED_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void LED1_Task(void* parameter)
{	
    while (1)
    {
				GPIO_ResetBits(GPIOB , GPIO_Pin_5);  
					
        vTaskDelay(500);   /* 延时500个tick */		 		
        printf("LED1_Task Running,LED1_OFF\r\n");
			
        GPIO_SetBits(GPIOB , GPIO_Pin_5);
		
        vTaskDelay(500);   /* 延时500个tick */
        printf("LED1_Task Running,LED1_ON\r\n");
    }
}





static void CPU_Task(void* parameter)
{	
  uint8_t CPU_RunInfo[400];		//保存任务运行时间信息
	
  while (1)
  {
    memset(CPU_RunInfo,0,400);				//信息缓冲区清零
    
    vTaskList((char *)&CPU_RunInfo);  //获取任务运行时间信息
    
    printf("---------------------------------------------\r\n");
		
    printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    
    memset(CPU_RunInfo,0,400);				//信息缓冲区清零
    
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    
    printf("任务名       运行计数         利用率\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
		
    vTaskDelay(1000);   /* 延时500个tick */		
  }
}


//读取adc任务  秒更新
static void readAdTask(void* parameter)
{	
	u16 temperture,adcvalue;
	float voltage ;
	
	while(1)
	{
		adcvalue = readAdc2Chennl();
		voltage = (float)adcvalue / 4096 *3.3 ;
		
		printf("dac value is %fV\n",voltage);//打印dac值
		
		temperture = readAverageTempertureValue();
			
		printf("temp value is %3d ℃\n",temperture);//打印温度值
		
		vTaskDelay(1000);   /* 延时500个tick */		
	}
}

//modbus通信任务 接收 
static void modbusReceiveTask(void* parameter)
{	
	BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdTRUE */
  uint8_t r_queue[512];	/* 定义一个接收消息的变量 */

  while (1)
  {
		
		
		
		
    xReturn = xQueueReceive( modbusQueue,    /* 消息队列的句柄 */
                             &r_queue,      /* 发送的消息内容 */
                             portMAX_DELAY); /* 等待时间 一直等 */
    if(pdTRUE == xReturn)
		{
			if(r_queue[3] == 0x03)
			{
				if((u16)(r_queue[4] + 5) == (u16)(r_queue[0] << 8 | r_queue[1]) )
				{
					
					switch(r_queue[2])
					{
						case 1:
								
							break;
						
						default:
							break;
					}
				}
			}
//			else if(r_queue[3] == 0x10)
//			{
//				if((u16)(r_queue[0] << 8 | r_queue[1]) == 8)
//				{
//					data = 1;
//				}
//			}
		}
    else
    {
		
		}
  }
}



/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区
	
	  /* 创建modbusQueue */
  modbusQueue = xQueueCreate((UBaseType_t ) modbusQueueLen,          /* 消息队列的长度 */
                            (UBaseType_t )  modbusQueueSize);     /* 消息的大小 */
  if(NULL != modbusQueue)
    printf("创建Test_Queue消息队列成功!\r\n");
  
  /* 创建LED_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )LED1_Task, /* 任务入口函数 */
                        (const char*    )"LED1_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&LED1_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建LED1_Task任务成功!\r\n");
  

	

	
	/* 创建CPU_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )CPU_Task, /* 任务入口函数 */
                        (const char*    )"CPU_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )4,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&CPU_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建CPU_Task任务成功!\r\n");
	

	
	/* 创建readAdTask任务 */
  xReturn = xTaskCreate((TaskFunction_t )readAdTask, /* 任务入口函数 */
                        (const char*    )"readAdTask",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )4,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&readAdTask_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建readAdTask任务成功!\r\n");
	
	/* 创建modbusTask任务 */
  xReturn = xTaskCreate((TaskFunction_t )modbusReceiveTask, /* 任务入口函数 */
                        (const char*    )"modbusReceiveTask",/* 任务名字 */
                        (uint16_t       )2048,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )4,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&modbusReceiveTask_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建modbusReceiveTask任务成功!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}



u8 data[10],data1[10],flag;


int main(void)
{
	double xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
	initCpu();
	
	iic1Read(data , 0 , 10);
	
	while(1)
	{
		if(flag == 1)
		{
			iic1Write(data1 , 0, 10);
			flag = 0;
		}
	}
	
	 /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
												
	  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1; 											
	

	while (1) {
			
	}
}



