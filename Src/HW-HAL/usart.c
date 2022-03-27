/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "stm32l4xx_hal_uart.h"
#include <string.h>
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */


volatile uint16_t USART1_RX_STA=0;       //����״̬���
uint16_t USART2_RX_STA=0;       //����״̬���
uint16_t USART3_RX_STA=0;       //����״̬���

uint8_t aRx1Buffer[RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
uint8_t USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

UART_HandleTypeDef UART1_Handler; //UART���
UART_HandleTypeDef UART2_Handler; //UART���
UART_HandleTypeDef UART3_Handler; //UART���
/**�����ض���fputc�ķ�ʽ�ʺ�С��ģʽ**/
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar( int ch )
#else
#define PUTCHAR_PROTOTYPE int fputc( int ch, FILE *f )
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit( &UART1_Handler, ( uint8_t * )&ch, 1, 0xFFFF );
    return ch;
}
#ifdef USE_FULL_ASSERT
void assert_failed( uint8_t* file, uint32_t line )
{
}
#endif
/**�����ض���fputc�ķ�ʽ�ʺ�С��ģʽ**/

void _Error_Handler(char *file, int line);

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}


void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

    if(uartHandle->Instance==USART1)
    {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
    else if(uartHandle->Instance==USART2)
    {
        /* USER CODE BEGIN USART2_MspDeInit 0 */

        /* USER CODE END USART2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART2_CLK_DISABLE();

        /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
        HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);

        /* USART2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspDeInit 1 */

        /* USER CODE END USART2_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */
UART_HandleTypeDef UART1_Handler; //UART���
//*************************************************
//����: void BSP_UART1_SendStr(uint8_t *Data , uint16_t Len)
//����: STM32�򴮿�2�����ַ���
//����: Len>0��sDataΪ���飬����Len�Ĵ�С�����ֽڣ�Len=0��sDataΪ�ַ����������ַ�����β��\0�����㳤�ȣ�
//����: ��
//**************************************************
void BSP_UART1_SendStr( uint8_t *sData, uint16_t Len)
{
    //uint16_t i;
    //__HAL_UART_CLEAR_FLAG(USART1, UART_FLAG_TC);
    if(Len>0)
    {
        HAL_UART_Transmit( &UART1_Handler, sData, Len, 0xFFFF );
    }
    else
    {
        HAL_UART_Transmit( &UART1_Handler,sData,strlen((char*)sData), 0xFFFF );
    }
}


void uart1_init(uint32_t bound)
{
    //UART ��ʼ������
    UART1_Handler.Instance=USART1;					    //USART1
    UART1_Handler.Init.BaudRate=bound;				    //������
    UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
    UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
    UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
    UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
    UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ

    UART1_Handler.Init.OverSampling            = UART_OVERSAMPLING_16;
    UART1_Handler.Init.OneBitSampling          = UART_ONE_BIT_SAMPLE_DISABLE;
    UART1_Handler.AdvancedInit.AdvFeatureInit  = UART_ADVFEATURE_NO_INIT;
    //HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
    if( HAL_UART_Init( &UART1_Handler ) != HAL_OK )
    {
        while( 1 ) {  }
        //Error_Handler( );
    }
    HAL_UART_Receive_IT(&UART1_Handler,(uint8_t *)aRx1Buffer,1);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
}


//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_Initure;
    if(huart->Instance==USART1)//����Ǵ���1�����д���1 MSP��ʼ��
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOBʱ��
        __HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��

        //TX
        GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;			//PA9
        GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
        GPIO_Initure.Pull=GPIO_PULLUP;//GPIO_NOPULL;//GPIO_PULLUP;			//����
        GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;//GPIO_SPEED_FAST;		//����
        GPIO_Initure.Alternate=GPIO_AF7_USART1;	//����ΪUSART1  USARTx_TX_AF
        HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PA9
        //RX
//				GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
//        GPIO_Initure.Mode=GPIO_MODE_INPUT;		//�����������
//        HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9
//


#if EN_USART1_RX
        __HAL_UART_ENABLE_IT(&UART1_Handler,UART_IT_RXNE);		//���������ж�
        //HAL_UART_Receive_IT(&UART1_Handler,(uint8_t *)aRx1Buffer,RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
        HAL_NVIC_EnableIRQ(USART1_IRQn);				//ʹ��USART1�ж�ͨ��
        HAL_NVIC_SetPriority(USART1_IRQn,3,2);			//��ռ���ȼ�3�������ȼ�3
#endif
    }

}

extern uint8_t SX1280_Buffer[];
////�����жϵĻص����������յ�0d,0a����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART1)//����Ǵ���1
    {

        //�򿪾�����Э�����
//			SX1280_Buffer[USART1_RX_STA]=aRx1Buffer[0];
//			 USART1_RX_STA++;

        if((USART1_RX_STA&0x8000)==0)//����δ���
        {
            if(USART1_RX_STA&0x4000)//���յ���0x0d
            {
                if(aRx1Buffer[0]!=0x0a)
                    USART1_RX_STA=0;//���մ���,���¿�ʼ
                else
                {
                    USART1_RX_STA|=0x8000;	//���������

                }
            }
            else //��û�յ�0X0D
            {
                if(aRx1Buffer[0]==0x0d)
                    USART1_RX_STA|=0x4000;
                else
                {
                    USART1_RX_BUF[USART1_RX_STA&0X3FFF]=aRx1Buffer[0] ;
                    USART1_RX_STA++;
                    if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����
                }
            }
        }
    }

}

//����1�жϷ������
void USART1_IRQHandler(void)
{
    uint32_t timeout=0;
    HAL_UART_IRQHandler(&UART1_Handler);	//����HAL���жϴ����ú���
    timeout=0;
    while (HAL_UART_GetState(&UART1_Handler)!=HAL_UART_STATE_READY)//�ȴ�����     == HAL_UART_STATE_BUSY_RX
    {
        timeout++;////��ʱ����
        if(timeout>HAL_MAX_DELAY) break;
    }
    timeout=0;
    while(HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)aRx1Buffer, RXBUFFERSIZE) != HAL_OK)//һ�δ������֮��(��������RXBUFFERSIZE���ȵ����ݣ��Ż����ص�����)�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
        if(timeout>HAL_MAX_DELAY) break;
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

void _Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {

    }
    /* USER CODE END Error_Handler_Debug */
}







