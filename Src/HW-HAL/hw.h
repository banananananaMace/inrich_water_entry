#ifndef __HW_H__
#define __HW_H__

#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
#include "hw-spi.h"
#include "hw-uart.h"
#include "hw-gpio.h"
#include "i2c.h"

#include "sx1280.h"
#include "sx1280-hal.h"


#include "boards.h"

//#define USE_DMA

void HwInit( void );

void HwSetLowPower( void );


void SystemClock_Config( void );
void SystemClock_Config_24M(void);
void SystemClock_Config_1M(void);
void HAL_Delay( uint32_t Delay );



void Error_Handler( void );

#endif // __HW_H__


#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 


/******* PC12��1  *************************/
#define	IIC_SCL_SET	 GPIOB->BSRR=1<<6
/******* PC11��1  *************************/
#define	IIC_SDA_SET	 GPIOB->BSRR=1<<7
/******* PC12��0  *************************/
#define	IIC_SCL_CLR	 GPIOB->BRR=1<<6
/******* PC11��0  *************************/
#define	IIC_SDA_CLR	 GPIOB->BRR=1<<7
/******* SDA����  *************************/
#define READ_SDA   PBin(7)
/******* SDA��� *************************/
#define IIC_SDA    PBout(7)

///*******  SDA���뷽�� *************************/
//#define SDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
///*******  SDA������� *************************/
//#define SDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}



/******* IIC��IO�ڳ�ʼ�� *************************/
void IIC_Init(void);
/******* ����IIC��ʼ�ź�  *************************/
void IIC_Start(void);
/******* ����IICֹͣ�ź�  *************************/
void IIC_Stop(void);
/******* IIC����һ���ֽ�  *************************/
void IIC_Send_Byte(uint8_t txd);
/******* IIC��ȡһ���ֽ�  *************************/
uint8_t IIC_Read_Byte(void);
/******* IIC�ȴ�ACK�ź�  *************************/
void IIC_Wait_Ack(void);
/******* IIC����ACK�ź�  *************************/
void IIC_Ack(void);
/******* IIC������ACK�ź�  *************************/
void IIC_NAck(void);


void HwDeInit( void );//zcr

