#include "stm32f10x.h"
#include "cpu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>   // ���ڻ�ȡʱ������
#include "queue.h"

DateTime p_clockDisplay;

 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED1������ */
static TaskHandle_t LED1_Task_Handle = NULL;


static TaskHandle_t CPU_Task_Handle = NULL;



static TaskHandle_t readAdTask_Handle = NULL;
static TaskHandle_t modbusReceiveTask_Handle = NULL;

extern u8 p_rxbuff[512];
extern u8 p_txbuff[512];

float regisiter[128];

QueueHandle_t modbusQueue =NULL;

#define  modbusQueueLen    1   /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define  modbusQueueSize   512   /* ������ÿ����Ϣ��С���ֽڣ� */

/**********************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LED1_Task(void* parameter)
{	
    while (1)
    {
				GPIO_ResetBits(GPIOB , GPIO_Pin_5);  
					
        vTaskDelay(500);   /* ��ʱ500��tick */		 		
        printf("LED1_Task Running,LED1_OFF\r\n");
			
        GPIO_SetBits(GPIOB , GPIO_Pin_5);
		
        vTaskDelay(500);   /* ��ʱ500��tick */
        printf("LED1_Task Running,LED1_ON\r\n");
    }
}





static void CPU_Task(void* parameter)
{	
  uint8_t CPU_RunInfo[400];		//������������ʱ����Ϣ
	
  while (1)
  {
    memset(CPU_RunInfo,0,400);				//��Ϣ����������
    
    vTaskList((char *)&CPU_RunInfo);  //��ȡ��������ʱ����Ϣ
    
    printf("---------------------------------------------\r\n");
		
    printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    
    memset(CPU_RunInfo,0,400);				//��Ϣ����������
    
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    
    printf("������       ���м���         ������\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
		
    vTaskDelay(1000);   /* ��ʱ500��tick */		
  }
}


//��ȡadc����  �����
static void readAdTask(void* parameter)
{	
	u16 temperture,adcvalue;
	float voltage ;
	
	while(1)
	{
		adcvalue = readAdc2Chennl();
		voltage = (float)adcvalue / 4096 *3.3 ;
		
		printf("dac value is %fV\n",voltage);//��ӡdacֵ
		
		temperture = readAverageTempertureValue();
			
		printf("temp value is %3d ��\n",temperture);//��ӡ�¶�ֵ
		
		vTaskDelay(1000);   /* ��ʱ500��tick */		
	}
}

//modbusͨ������ ���� 
static void modbusReceiveTask(void* parameter)
{	
	BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */
  uint8_t r_queue[512];	/* ����һ��������Ϣ�ı��� */

  while (1)
  {
		
		
		
		
    xReturn = xQueueReceive( modbusQueue,    /* ��Ϣ���еľ�� */
                             &r_queue,      /* ���͵���Ϣ���� */
                             portMAX_DELAY); /* �ȴ�ʱ�� һֱ�� */
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
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
	
	  /* ����modbusQueue */
  modbusQueue = xQueueCreate((UBaseType_t ) modbusQueueLen,          /* ��Ϣ���еĳ��� */
                            (UBaseType_t )  modbusQueueSize);     /* ��Ϣ�Ĵ�С */
  if(NULL != modbusQueue)
    printf("����Test_Queue��Ϣ���гɹ�!\r\n");
  
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED1_Task, /* ������ں��� */
                        (const char*    )"LED1_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED1_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����LED1_Task����ɹ�!\r\n");
  

	

	
	/* ����CPU_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )CPU_Task, /* ������ں��� */
                        (const char*    )"CPU_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )4,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&CPU_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����CPU_Task����ɹ�!\r\n");
	

	
	/* ����readAdTask���� */
  xReturn = xTaskCreate((TaskFunction_t )readAdTask, /* ������ں��� */
                        (const char*    )"readAdTask",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )4,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&readAdTask_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����readAdTask����ɹ�!\r\n");
	
	/* ����modbusTask���� */
  xReturn = xTaskCreate((TaskFunction_t )modbusReceiveTask, /* ������ں��� */
                        (const char*    )"modbusReceiveTask",/* �������� */
                        (uint16_t       )2048,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )4,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&modbusReceiveTask_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����modbusReceiveTask����ɹ�!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



u8 data[10],data1[10],flag;


int main(void)
{
	double xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
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
	
	 /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
												
	  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1; 											
	

	while (1) {
			
	}
}



