#include "iic.h"

/* AT24C01/02ÿҳ��8���ֽ� */
#define I2C_PageSize           8

uint16_t EEPROM_ADDRESS;
static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;   

/*********************
ģ��IIC

********************/

/*********************
Ӳ��IIC

********************/

/**
  * @brief  Basic management of the timeout situation.
  * @param  errorCode��������룬����������λ���ĸ����ڳ���.
  * @retval ����0����ʾIIC��ȡʧ��.
  */
static  uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* Block communication and all processes */
  printf("I2C �ȴ���ʱ!errorCode = %d",errorCode);
  
  return 0;
}

/**
  * @brief  I2C ����(EEPROM)��ʼ��
  * @param  ��
  * @retval ��
  */
void iicInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	I2C_InitTypeDef  I2C_InitStructure; 

	/* ʹ���� I2C �йص�ʱ�� */
	RCC_APB1PeriphClockCmd ( RCC_APB1Periph_I2C1, ENABLE );
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOB, ENABLE );
	
    
  /* I2C_SCL��I2C_SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // ��©���
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // ��©���
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	/* I2C ���� */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	
	/* �ߵ�ƽ�����ȶ����͵�ƽ���ݱ仯 SCL ʱ���ߵ�ռ�ձ� */
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	
  I2C_InitStructure.I2C_OwnAddress1 =0x0a; 
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	
	/* I2C��Ѱַģʽ */
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	/* ͨ������ */
  I2C_InitStructure.I2C_ClockSpeed = 400000;
  
	/* I2C ��ʼ�� */
  I2C_Init(I2C1, &I2C_InitStructure);
  
	/* ʹ�� I2C */
  I2C_Cmd(I2C1, ENABLE);   


/* ����ͷ�ļ�i2c_ee.h�еĶ�����ѡ��EEPROMҪд��ĵ�ַ */
#ifdef EEPROM_Block0_ADDRESS
  /* ѡ�� EEPROM Block0 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
#endif

#ifdef EEPROM_Block1_ADDRESS  
	/* ѡ�� EEPROM Block1 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
#endif

#ifdef EEPROM_Block2_ADDRESS  
	/* ѡ�� EEPROM Block2 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
#endif

#ifdef EEPROM_Block3_ADDRESS  
	/* ѡ�� EEPROM Block3 ��д�� */
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
#endif
}

/**
  * @brief   ���������е�����д��I2C EEPROM��
  * @param   
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:д��ַ
  *     @arg NumByteToWrite:д���ֽ���
  * @retval  ��
  */
void iic1Write(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      iic1PageWrite(pBuffer, WriteAddr, NumOfSingle);
      iic1Wait();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        iic1PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
				iic1Wait();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        iic1PageWrite(pBuffer, WriteAddr, NumOfSingle);
        iic1Wait();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      iic1PageWrite(pBuffer, WriteAddr, NumOfSingle);
      iic1Wait();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        iic1PageWrite(pBuffer, WriteAddr, count);
        iic1Wait();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        iic1PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        iic1Wait();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        iic1PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        iic1Wait();
      }
    }
  }  
}
/**
  * @brief   дһ���ֽڵ�I2C EEPROM��
  * @param   
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:д��ַ 
  * @retval  ��
  */
uint32_t iic1ByteWrite(u8* pBuffer, u8 WriteAddr) 
{
  /* Send STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  I2CTimeout = I2CT_FLAG_TIMEOUT;  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(0);
  } 
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(1);
  }  
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, WriteAddr);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(2);
  } 
  
  /* Send the byte to be written */
  I2C_SendData(I2C1, *pBuffer); 
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
  } 
  
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
  
  return 1;
}


/**
  * @brief   ��EEPROM��һ��дѭ���п���д����ֽڣ���һ��д����ֽ���
  *          ���ܳ���EEPROMҳ�Ĵ�С��AT24C02ÿҳ��8���ֽ�
  * @param   
  *		@arg pBuffer:������ָ��
  *		@arg WriteAddr:д��ַ
  *     @arg NumByteToWrite:д���ֽ���
  * @retval  ��
  */
uint32_t iic1PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
  I2CTimeout = I2CT_LONG_TIMEOUT;

  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))   
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(4);
  } 
  
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))  
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(5);
  } 
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(6);
  } 
  
  /* Send the EEPROM's internal address to write to */    
  I2C_SendData(I2C1, WriteAddr);  

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(7);
  } 

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(I2C1, *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
      if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(8);
    } 
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
  
  return 1;
}

/**
  * @brief  Wait for EEPROM Standby state 
  * @param  ��
  * @retval ��
  */
void iic1Wait(void)      
{
  vu16 SR1_Tmp = 0;

  do
  {
    /* Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* Read I2C1 SR1 register */
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  }while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));
  
  /* Clear AF flag */
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    /* STOP condition */    
  I2C_GenerateSTOP(I2C1, ENABLE); 
}


/**
  * @brief   ��EEPROM�����ȡһ������ 
  * @param   
  *		@arg pBuffer:��Ŵ�EEPROM��ȡ�����ݵĻ�����ָ��
  *		@arg WriteAddr:�������ݵ�EEPROM�ĵ�ַ
  *     @arg NumByteToWrite:Ҫ��EEPROM��ȡ���ֽ���
  * @retval  ��
  */
uint32_t iic1Read(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{  
  
  I2CTimeout = I2CT_LONG_TIMEOUT;
  
  //*((u8 *)0x4001080c) |=0x80; 
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
   }
  
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
   }
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(11);
   }
    
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, ReadAddr);  

   
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(12);
   }
    
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(I2C1, ENABLE);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(13);
   }
    
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Receiver);
  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(14);
   }
  
  /* While there is data to be read */
  while(NumByteToRead)  
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C1, ENABLE);
    }

    /* Test on EV7 and clear it */    
    I2CTimeout = I2CT_LONG_TIMEOUT;
    
		while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
		{
			if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
		} 
    {      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(I2C1);

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;        
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  
    return 1;
}

































