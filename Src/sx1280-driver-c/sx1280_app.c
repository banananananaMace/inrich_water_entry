#include "main.h"
#include "sx1280_app.h"
#include "hw.h"
#include "radio.h"
#include "sx1280.h"
#include "sx1280_app.h"
//#include "SP1808.h"
//#include "spi.h"
//#include "delay.h"
//#include "usart.h"
//#include "led.h"
#include "protocol.h"
//#include "hw-gpio.h"
#include "usart.h"

#define BUFFER_SIZE                                 20
extern uint8_t AppState;
//brief Define the possible message type for this application/
//const uint8_t PingMsg[] = "PING:Txtimeout!";
const char PongMsg0[] = "SX_0:Timers!";
const char PongMsg1[] = "SX_1:Timer1s!";
const char PongMsg2[] = "SX_2:Timer22s!";
char    PongMsg3[] = "SX_3:Timer!";

uint8_t RecBufferSize = REC_BUFFER_SIZE;
uint8_t RecBuffer[REC_BUFFER_SIZE];

uint8_t SndBufferSize = SND_BUFFER_SIZE;
uint8_t SndBuffer[SND_BUFFER_SIZE];

uint8_t Send_Frame_type = MESSAGE; //���ò������� ������Ҫͨ��֡����
//uint8_t SndBuffer[40];

//brief Mask of IRQs to listen to in rx mode/
uint16_t RxIrqMask = IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT|IRQ_CRC_ERROR;
//\brief Mask of IRQs to listen to in tx mode/
uint16_t TxIrqMask = IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT;

uint16_t mainsteptime;//= {0,1050,2170,3230}; //ȫ�����ʱ������systick�м�һ(1ms)��
uint32_t lorasteptime[2] = {400, 0}; //ȫ�����ʱ������systick�м�һ(1ms)��
TickTime_t TTtime_RX = {RX_TIMEOUT_TICK_SIZE, RX_TIMEOUT_SINGLEMODE}; //SIZE*VALUE=
TickTime_t TTtime_TX = {TX_TIMEOUT_TICK_SIZE, TX_TIMEOUT_VALUE}; //SIZE*VALUE=


uint16_t SX1280_ID = 0;
uint16_t irqRegs;
static RadioCallbacks_t* RadioCallbacks;
/*******************************************************
*Function Name 	:SX1280_Param_Init
*Description  	: sx1280��ʼ��
*Input					: sxcase��[0,3]��ʾ�ڼ���sx1280��ԭ��ƹ���4��
*Output					:
*******************************************************/

void OnTxDone( void );

void OnRxDone( void );

void OnTxTimeout( void );

void OnRxTimeout( void );

void OnRxError( IrqErrorCode_t );

void SX1280ProcessTXIrqs( void );

RadioCallbacks_t Callbacks =
{
    &OnTxDone,        // txDone
    &OnRxDone,        // rxDone
    NULL,             // syncWordDone
    NULL,             // headerDone
    &OnTxTimeout,     // txTimeout
    &OnRxTimeout,     // rxTimeout
    &OnRxError,       // rxError
    NULL,             // rangingDone
    NULL,             // cadDone
};
void SX1280_Init(uint32_t RF_FREQUENCY )
{
    PacketParams_t packetParams;
    ModulationParams_t modulationParams;
    SX1280Init(&Callbacks);//zcr V2.0
    Radio.SetRegulatorMode( USE_DCDC ); // Can also be set in LDO mode but consume more power


    modulationParams.PacketType = PACKET_TYPE_LORA;
    modulationParams.Params.LoRa.SpreadingFactor = LORA_SF8;
    modulationParams.Params.LoRa.Bandwidth = LORA_BW_0800;
    modulationParams.Params.LoRa.CodingRate = LORA_CR_4_5;

    packetParams.PacketType = PACKET_TYPE_LORA;
    packetParams.Params.LoRa.PreambleLength = 6;
    packetParams.Params.LoRa.HeaderType = LORA_PACKET_VARIABLE_LENGTH;
    packetParams.Params.LoRa.PayloadLength = BUFFER_SIZE;
    packetParams.Params.LoRa.CrcMode = LORA_CRC_ON;
    packetParams.Params.LoRa.InvertIQ = LORA_IQ_NORMAL;

    Radio.SetStandby( STDBY_RC );
    Radio.SetPacketType( modulationParams.PacketType );
    Radio.SetRfFrequency( RF_FREQUENCY );
    Radio.SetBufferBaseAddresses( 0x00, 0x00 );


    Radio.SetModulationParams( &modulationParams );
    Radio.WriteRegister( 0x925, 0x32 );

    Radio.SetPacketParams( &packetParams );
    Radio.SetTxParams( TX_OUTPUT_POWER, RADIO_RAMP_02_US );
}



/*******************************************************
*Function Name 	:Process_Appstate_0
*Description  	: ��һ��sx1280����״̬������AppState[0]�����з��ദ��
*Input					:
*Output					:
*******************************************************/


uint8_t Process_Appstate_0(void)
{
    uint8_t return_value = 0;
    //uint8_t appi=0;
    switch( AppState )
    {
    case APP_RX: //��ȷ���յ�����
        if(Radio.GetPayload( RecBuffer, &RecBufferSize, REC_BUFFER_SIZE)) //ͨ������SX1280HalReadBuffer�����ѽ������ݷ���Buffer,�������ݳ��ȷ���BufferSize�����ܳ���BUFFER_SIZE
        {
        }
        else
        {
        }
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        Radio.SetRx(TTtime_RX);//����Ϊ����ģʽ		
        return_value = AppState;
        AppState = APP_LOWPOWER;
        break;
    case APP_TX: // ���ͳɹ�
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        Radio.SetRx(TTtime_RX);//����Ϊ����ģʽ
        AppState = APP_LOWPOWER;
        return_value = APP_TX;
        break;
    case APP_RX_TIMEOUT:  //���ճ�ʱ
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        Radio.SetRx(TTtime_RX);//����Ϊ����ģʽ
        AppState = APP_LOWPOWER;
        return_value = APP_RX_TIMEOUT;
        break;
    case APP_RX_ERROR:
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        Radio.SetRx(TTtime_RX);//����Ϊ����ģʽ
        AppState = APP_LOWPOWER;
        return_value = APP_RX_ERROR;
        break;
    case APP_TX_TIMEOUT:  //���ͳ�ʱ
        Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        Radio.SetRx(TTtime_RX);//����Ϊ����ģʽ
        AppState = APP_LOWPOWER;
        return_value = APP_TX_TIMEOUT;
        break;

    case APP_LOWPOWER:				
        break;
    default:
        // Set low power
        break;

    }
    return return_value;
}





void SX1280ProcessTXIrqs( void )
{
//   RadioPacketTypes_t packetType = PACKET_TYPE_NONE;


//    packetType = SX1280GetPacketType( );
    irqRegs = SX1280GetIrqStatus( );
    SX1280ClearIrqStatus( IRQ_RADIO_ALL );


//       case PACKET_TYPE_LORA:
    if( ( irqRegs & IRQ_TX_DONE ) == IRQ_TX_DONE )
    {
        if( ( RadioCallbacks != NULL ) && ( RadioCallbacks->txDone != NULL ) )
        {
            RadioCallbacks->txDone( );
        }
    }
    if( ( irqRegs & IRQ_RX_TX_TIMEOUT ) == IRQ_RX_TX_TIMEOUT )
    {
        if( ( RadioCallbacks != NULL ) && ( RadioCallbacks->txTimeout != NULL ) )
        {
            RadioCallbacks->txTimeout( );
        }
    }

}






void SX1280_Enter_LowPower( void )
{
    SleepParams_t sleepParams;

    sleepParams.DataBufferRetention = 0;
    sleepParams.DataRamRetention = 0;
    sleepParams.InstructionRamRetention = 0;

    Radio.SetSleep( sleepParams );

    /*Configure GPIO pins :RADIO_BUSY_PIN  As ANALOG*/
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin =  RADIO_BUSY_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

}

void SX1280_Exit_LowPower( void )
{
    /*Configure GPIO pins :RADIO_BUSY_PIN  As INPUT*/
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin =  RADIO_BUSY_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    GpioWrite( RADIO_NSS_PORT, RADIO_NSS_PIN, 0 );
    HAL_Delay( 2 );
    GpioWrite( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );

    Radio.SetStandby( STDBY_RC );

}


uint8_t SX1280_SendData( uint8_t *SendBuf, uint8_t send_length)
{
    Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);//���˽��ܲ���
    SX1280HalWriteRegister( REG_LR_PAYLOADLENGTH, send_length );
    //  Radio.SendPayload( SendBuf, send_length, ( TickTime_t ) {RX_TIMEOUT_TICK_SIZE, TX_TIMEOUT_VALUE} );
    Radio.SendPayload( SendBuf, send_length, TTtime_TX );
    //  HAL_Delay( 5 );//�����ʱ����Ҳ���ܲ�������   ʱ����˷��Ͳ���ȥ�����˽��ܲ���
    return 0;
}




void OnTxDone( void )
{
    AppState = APP_TX;
}

void OnRxDone( void )
{
    AppState = APP_RX;
}

void OnTxTimeout( void )
{
    AppState = APP_TX_TIMEOUT;
}

void OnRxTimeout( void )
{
    AppState = APP_RX_TIMEOUT;
}

void OnRxError( IrqErrorCode_t errorCode )
{
    AppState = APP_RX_ERROR;
}

void OnRangingDone( IrqRangingCode_t val )
{
}

void OnCadDone( bool channelActivityDetected )
{
}








