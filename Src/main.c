/*---------Includes---------*/
#include <string.h>
#include "main.h"
#include "stm32l4xx_hal.h"
#include "sx1280_app.h"
#include "hw.h"
#include "radio.h"
#include "sx1280.h"
#include "protocol.h"
#include "flash.h"
#include "adc.h"
#include "rtc.h"
#include "random.h"
#include "iwdg.h"

/*！！！！！！！！！！！！！！！！！！！！崎協吶！！！！！！！！！！！！！！！！！！！！*/
#define	start_up_time	16 //(距屁峙)峙埆弌??尼強埆岳
#define ALARM_TEMPERATURE 0x0AF8B //75 degree
#define MODE_LORA
#define RX_TIMEOUT_TICK_SIZE RADIO_TICK_SIZE_1000_US
//#define FLASH_OTP_ADDR	((uint32_t)0x1FFF7000)

/*！！！！！！！！！！！！！！！！！！！！32了！！！！！！！！！！！！！！！！！！！！*/
uint64_t sensor_id = 0 ;
uint64_t default_sensor_id=0;
uint32_t BURST_Count_down = 0;
uint32_t sleep_time = 0;
uint32_t sx1280_receive_time = 0; //壓宜柴扮侃尖仇圭序佩験峙弓受。
uint32_t RTC_SleepTime = 0;
volatile uint32_t MainRun_Time = 0;

/*！！！！！！！！！！！！！！！！！！！！16了！！！！！！！！！！！！！！！！！！！！*/
uint16_t Rssi = 0;      //佚催膿業
uint16_t temperatrue = 0;

/*！！！！！！！！！！！！！！！！！！！！8了！！！！！！！！！！！！！！！！！！！！*/
uint8_t Buffer[10] = {0x00};
uint8_t RspAckbuff[50] = {0x00};
uint8_t WriteBuffer[10], ReadBuffer[10];
uint8_t	Send_BURST_Count = 3; //嶷鹸窟僕BURST肝方協吶
uint8_t	Send_BURST_Flag = 0;
uint8_t Send_Frame_Type = 0;
uint8_t otp_sensor_id_buf[6]= {0};
uint8_t sensor_id_buf[6]= {0x1D,0x9C,0x08,0x2F,0xC0,0x3B};

/*！！！！！！！！！！！！！！！！！！！！凪麿！！！！！！！！！！！！！！！！！！！！*/
HAL_StatusTypeDef hi2c1_status;
PacketParams_t packetParams;
PacketStatus_t packetStatus;
float Shift_time = 0.0;
uint32_t Flash_Sensor_ID=0;
/*！！！！！！！！！！！！！！！！！！！！痕方！！！！！！！！！！！！！！！！！！！！*/
void Enter_ShutDownMode(void);
uint32_t Handle_RFSendflag(void);
extern uint16_t TxIrqMask ;

int main( void )
{
    RTC_SleepTime = WakeUp_TimeBasis;
    HwInit();
    Read_flash_Parameter();//響函歌方
    memcpy(&sensor_id_buf[2],&Flash_Sensor_ID,4);
    memcpy(&default_sensor_id,sensor_id_buf,6);
    MX_IWDG_Init();
    Read_flash_Parameter();//響函歌方
    SX1280_Init(Frequency_list[Frequency_point]);
    RTC_Config();
    Handle_WakeUpSource();
    __HAL_RCC_CLEAR_RESET_FLAGS();
    Battery_Voltage_Measure();
    Handle_RFSendflag();//侃尖窟僕屐窃侏
    HAL_IWDG_Refresh(&hiwdg);//心壇昂旅昂
    Enter_ShutDownMode();
}




void Enter_ShutDownMode(void)
{
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    WRITE_REG( RTC->BKP31R, 0x1 );//芝村RTC篠贋匂??瀧貧序秘購字庁塀
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);
    RTC_SleepTime = ((RTC_SleepTime - MainRun_Time) * 2.048 - start_up_time); //5頁浩麻殻會尼強扮寂 2頁柴麻ms
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, RTC_SleepTime, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    HAL_PWREx_EnterSHUTDOWNMode(); //SHUTDOWN Mode two lines
}

uint32_t Handle_RFSendflag (void)
{
    uint8_t Return_Value = 0;
    if(Send_BURST_Flag == 1)
    {
        Radio.SetRfFrequency(Frequency_list[1]);//陣崙撞泣
        Send_BURST_Flag = 0;
        AppState = 0;
        Send_BURST_Count = 3;
        while(Send_BURST_Count)
        {
            Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
            SendtoStation_sx1280_frame(BURST, 19, WaterEntry_type, Buffer);
            sx1280_receive_time = 70;
            Send_BURST_Count--;
            while(sx1280_receive_time)
            {
                if( IrqState == true)
                {
                    SX1280ProcessIrqs();//膝宙光窃彜蓑??箭泌窟僕才俊辺??夸崔AppState葎斤哘延楚??隼朔宥狛和中旗鷹侃尖方象
                    IrqState = false;
                }
                Return_Value = Process_Appstate_0();
                if(Return_Value == APP_RX)
                {
                    Return_Value = Handle_receiveStation_sx1280_frame();
                    if(Return_Value == ACK) //俊鞭欺哘基屐鳩範
                    {
                        break; //柳竃階扮儉桟
                    }
                }
            }//宜柴扮 階扮
            if(Return_Value == ACK) //俊鞭欺哘基屐鳩範//
            {
                break; //柳竃嶷鹸肝方儉桟
            }
        }
    }

    if(Send_Frame_Type == 1)
    {
        Send_Frame_Type = 0; //圭宴尖盾
        SendtoStation_sx1280_frame(MESSAGE, 19, WaterEntry_type, Buffer);
        HAL_Delay(30);//窟僕距屁峙
        Get_random();
    }

    else if( Send_Frame_Type == 2)
    {
        Radio.SetRfFrequency(Frequency_list[1]);//俳算陣崙撞泣
        Send_Frame_Type = 0;
        AppState = 0;
        sx1280_receive_time = 80; //嗤扮昨俊鞭階屐 霞編匯屐52ms潤崩
        Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE);
        SendtoStation_sx1280_frame(REQ, 2, Temperature_type, Buffer);
        while(sx1280_receive_time)
        {
            if( IrqState == true)
            {
                SX1280ProcessIrqs();//膝宙光窃彜蓑??箭泌窟僕才俊辺??夸崔AppState葎斤哘延楚??隼朔宥狛和中旗鷹侃尖方象
                IrqState = false;
            }
            Return_Value = Process_Appstate_0();
            if(Return_Value == APP_RX)
            {
                Return_Value = Handle_receiveStation_sx1280_frame();
                if(Return_Value == RSP_END)
                {
                    Write_flash_Parameter();//繍俊鞭欺議塘崔歌方亟秘flash嶄
                    Shift_time = Offset_Delay % WakeUp_TimeBasis; //陶卞扮寂(弌噐匯倖蚕佰巓豚議扮寂)
                    RTC_SleepTime = (Shift_time + RTC_SleepTime - 23);
                    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR1, ((Offset_Delay % Message_cycle / WakeUp_TimeBasis))); //决扮匯倖匍暦巓豚5蛍嶝=30倖10s巓豚
                    Read_flash_Parameter();//響歌方議朕議頁鳩範塘崔頁倦屎鳩
                    RspAckbuff[0] = 0x01;
                    SendtoStation_sx1280_frame(ACK,1, RSP_END_ACK_type, RspAckbuff);
                    HAL_Delay(30);
                    break;//柳竃階扮
                }
            }
        }
    }
    Radio.SetStandby( STDBY_RC );
    SX1280_Enter_LowPower();
    return 0;
}



