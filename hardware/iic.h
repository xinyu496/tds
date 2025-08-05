#ifndef _IIC_H
#define _IIC_H

#include "stm32f10x.h"
#include "uart.h"

/*********************
模拟IIC

********************/




/*********************
硬件IIC

********************/
/* EEPROM Addresses defines */
#define EEPROM_Block0_ADDRESS 0xA0   /* E2 = 0 */
//#define EEPROM_Block1_ADDRESS 0xA2 /* E2 = 0 */
//#define EEPROM_Block2_ADDRESS 0xA4 /* E2 = 0 */
//#define EEPROM_Block3_ADDRESS 0xA6 /* E2 = 0 */

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


















void iicInit(void);
void iic1Write(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);
uint32_t iic1ByteWrite(u8* pBuffer, u8 WriteAddr);
uint32_t iic1PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);
uint32_t iic1Read(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
void iic1Wait(void);





































































#endif





