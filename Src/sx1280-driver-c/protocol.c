#include "protocol.h"
#include "sx1280_app.h"
#include  "string.h"
#include "sx1280.h"
#include "usart.h"
#define  CollectStation_id 1
#define  FREQUENCY_LIST_LENTH 30
uint32_t const Frequency_list[FREQUENCY_LIST_LENTH]=
{   2399500000,2400500000,2401500000,2402500000,2403500000,2404500000,
    2405500000,2406500000,2407500000,2408500000,2409500000,
    2410500000,2411500000,2412500000,2413500000,2414500000,
    2415500000,2416500000,2417500000,2418500000,2419500000,
    2420500000,2421500000,2422500000,2423500000,2424500000
};
uint32_t DES_Offset_Delay=0;
uint32_t Message_cycle = 60000*5, ////300000//15000
         Offset_Delay = 0;   //��ʱû���õ�����ַ���ű���
uint16_t Ctrl_cycle= 25;
uint32_t Alarm_threshold=2100;//��ӦADC��ֵΪ2100
uint32_t Alarm_threshold_DOWN=0;
uint8_t Time_random=50;
uint8_t Frequency_point=25,
        REQ_Waiting_Reply=0,
        BURST_Waiting_Reply=0;

extern uint32_t sensor_id ;
extern uint64_t default_sensor_id;
extern uint8_t Send_Frame_type; //���ò������� ������Ҫͨ��֡����

static const unsigned char auchCRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;

// -----------------------------------------------------------------------------
// DESCRIPTION: RTU CRCУ��ĵ�λ�ֽڱ�
// -----------------------------------------------------------------------------
static const unsigned char auchCRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
//У�麯����
unsigned short RTU_CRC( unsigned char *puchMsg, unsigned short usDataLen )
{
    unsigned char uchCRCHi;                // high byte of CRC initialized
    unsigned char uchCRCLo;                // low byte of CRC initialized
    unsigned uIndex;                       // will index into CRC lookup table

    uchCRCHi = 0xFF;
    uchCRCLo = 0xFF;

    while ( usDataLen-- )
    {
        // calculate the CRC
        uIndex = uchCRCHi ^ (unsigned char)( *puchMsg++ );

        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
        uchCRCLo = auchCRCLo[uIndex];
    }
    return ( uchCRCHi << 8 | uchCRCLo );
}
uint8_t SendtoStation_sx1280_frame(uint8_t send_MHDR_type, uint8_t send_Length,uint8_t Data_Type, uint8_t *data)
{
    struct MHDR  send_MHDR;
    unsigned short CRC16_buf=0;
    uint8_t return_send_value=0;
    uint8_t PydNum=0;//����������
    bool CC_Ind=0;//ͨ������ָʾ
    bool FragInd=0;//�����Ƿ�ְ� 0�����ְ�
    bool Type_Ident=0;//�����������ͱ�ʶ ���ֶ���ʵ�����壬Ĭ��ȡ0
    uint8_t PacketType=0;//������
    uint16_t SensorType=0;//����������
    uint8_t LengthFlag=0;//���ݳ����ֶ�ָʾλ
    uint8_t Ctrl_Type=0;//���Ʊ������ͣ�REQ��ʹ��
    uint16_t Frame_Struct=0;//֡�ṹ SensorType<<2+LengthFlag

    send_MHDR.MType=send_MHDR_type;
    send_MHDR.Length=send_Length;//Length������MACPayload���ֵ��ֽڳ��ȣ��˲��ֳ���Ϊ5������
    memcpy( send_MHDR.Sensor_ID,sensor_id_buf,6);
    send_MHDR.Key_Value=0x00;//����ָʾ�ֶ�
    SndBuffer[0]=((send_MHDR.MType<<4)+(CC_Ind<<3)+(send_MHDR.Key_Value&0x07));//MHDR ��һ���ֽ�  ֡��ʽ
    SndBuffer[1]=send_MHDR.Length;//MHDR �ڶ����ֽ�  ֡����
    memcpy( &SndBuffer[2],send_MHDR.Sensor_ID,6);  //MHDR ��5���ֽ�    sensor ID

    //MACPayload��������
    switch(send_MHDR.MType)
    {
    case MESSAGE : //ҵ��֡��Ϣ
        if(Data_Type==WaterEntry_type) //ˮ����������
        {
            PacketType=0x00;//�������� 0��ʾMessage
            PydNum=3;
            SndBuffer[8]=((PydNum<<4)+(FragInd<<3)+(PacketType&0x07));//SndBuffer[8]=0x10;
//�¶����ݷ���
            SensorType=0xA2;//���������� :ˮ������;
            LengthFlag=0x01;// �������ݳ��Ȳ����ڣ�����Ĭ�����ֽ�;
            Frame_Struct=((SensorType<<2)+(LengthFlag&0x03));
            SndBuffer[9]=Frame_Struct;
            SndBuffer[10]=Frame_Struct>>8;
            SndBuffer[11]=0x02;
            memcpy( &SndBuffer[12],data,2);
//�������ݷ���
            SensorType=0x03;//���������� :ʣ�����;
            LengthFlag=0x00;//�������ݳ��Ȳ����ڣ�����Ĭ�����ֽ�;
            Frame_Struct=((SensorType<<2)+(LengthFlag&0x03));
            SndBuffer[14]=Frame_Struct;
            SndBuffer[15]=Frame_Struct>>8;
            memcpy( &SndBuffer[16],&data[2],4);

            SensorType=15666;//���������� :ʣ�����;
            LengthFlag=0x01;//�������ݳ��Ȳ����ڣ�����Ĭ�����ֽ�;
            Frame_Struct=((SensorType<<2)+(LengthFlag&0x03));
            SndBuffer[20]=Frame_Struct;
            SndBuffer[21]=Frame_Struct>>8;
            SndBuffer[22]=2;
            memcpy( &SndBuffer[23],&data[6],2);
            CRC16_buf=RTU_CRC(&SndBuffer[2],send_Length+6-2);//8��ʾ֡ͷ��ID�������ֽ�(-2��ʾ��ȥCRC�����ֽ�)
            SndBuffer[8+send_Length-2]=CRC16_buf;
            SndBuffer[8+send_Length-2+1]=CRC16_buf>>8;

        }
        return_send_value=MESSAGE;
        break;
    case REQ :
        SndBuffer[8]=0x01;
        SndBuffer[9]=0x00;//����
        return_send_value=REQ;
        break;
    case BURST :
        if(Data_Type==WaterEntry_type) //ˮ����������
        {
            PacketType=0x02;//�������� 0��ʾMessage
            PydNum=3;
            SndBuffer[8]=((PydNum<<4)+(FragInd<<3)+(PacketType&0x07));//SndBuffer[8]=0x10;
//�¶����ݷ���
            SensorType=0xA2;//���������� :�¶�����;
            LengthFlag=0x01;// �������ݳ��Ȳ����ڣ�����Ĭ�����ֽ�;
            Frame_Struct=((SensorType<<2)+(LengthFlag&0x03));
            SndBuffer[9]=Frame_Struct;
            SndBuffer[10]=Frame_Struct>>8;
            SndBuffer[11]=0x02;
            memcpy( &SndBuffer[12],data,2);
//�������ݷ���
            SensorType=0x03;//���������� :ʣ�����;
            LengthFlag=0x00;//�������ݳ��Ȳ����ڣ�����Ĭ�����ֽ�;
            Frame_Struct=((SensorType<<2)+(LengthFlag&0x03));
            SndBuffer[14]=Frame_Struct;
            SndBuffer[15]=Frame_Struct>>8;
            memcpy( &SndBuffer[16],&data[2],4);

            SensorType=15666;//���������� :ʣ�����;
            LengthFlag=0x01;//�������ݳ��Ȳ����ڣ�����Ĭ�����ֽ�;
            Frame_Struct=((SensorType<<2)+(LengthFlag&0x03));
            SndBuffer[20]=Frame_Struct;
            SndBuffer[21]=Frame_Struct>>8;
            SndBuffer[22]=2;
            memcpy( &SndBuffer[23],&data[6],2);
            CRC16_buf=RTU_CRC(&SndBuffer[2],send_Length+6-2);//8��ʾ֡ͷ��ID�������ֽ�(-2��ʾ��ȥCRC�����ֽ�)
            SndBuffer[8+send_Length-2]=CRC16_buf;
            SndBuffer[8+send_Length-2+1]=CRC16_buf>>8;
        }
        return_send_value=BURST;
        break;
    case ACK :
        if(Data_Type==RSP_END_ACK_type) //
        {
            memcpy( &SndBuffer[8], data, 20);
        }
        return_send_value=ACK;
        break;
    default:
        break;
    }//switch(send_MHDR.MType)

//CRC16У��

//�ۼӺ�У��
    SndBuffer[8+send_Length-2+2] = 0;
    for (uint8_t i=0; i<8+send_Length-2+2; i++)
    {
        SndBuffer[8+send_Length-2+2]+= SndBuffer[i];
    }
    SX1280_SendData(SndBuffer,8+send_Length-2+2+1);
    return return_send_value ;
}

uint8_t Receive_BURST_ACK_flag=0;
extern uint8_t RspAckbuff[50];
uint8_t Handle_receiveStation_sx1280_frame(void)
{
    struct MHDR  Receive_MHDR;
    uint8_t Receive_Len=0;
    uint8_t sumtest=0;
    uint8_t compare_result=0;
    uint8_t return_receive_value=0;
    uint8_t Receive_Sensor_ID_Buf[6]= {0};
    uint8_t data_type=0;
    uint8_t Receive_CC_Ind=0;
    uint16_t CRC_Test=0;
    uint16_t Receive_CRC;
    uint64_t Receive_Sensor_ID=0;

    CRC_Test=RTU_CRC(&RecBuffer[2],RecBuffer[1]+4);
    Receive_CC_Ind = ((RecBuffer[0]&0x08)>>3);
    Receive_MHDR.MType = RecBuffer[0]>>4;  //MType ǰ3���ֽ�
    Receive_MHDR.Length = RecBuffer[1];
    memcpy( &Receive_Sensor_ID,&RecBuffer[2],6);  //���յ���۽ڵ��·��Ĵ�����ID
    Receive_MHDR.Key_Value = 0;//Ĭ��
    if(Receive_Sensor_ID == default_sensor_id)//���ܵ���ȷ����ID
    {
        for(uint8_t i=0; i<(RecBuffer[1]+6+2); i++)
        {
            sumtest += RecBuffer[i];//У��ͼ���
        }
        if(sumtest==RecBuffer[2+6+(RecBuffer[1])])//У�����ȷ
        {
            switch(Receive_MHDR.MType)
            {
            case MESSAGE :
                return_receive_value=MESSAGE;
                break;
            case REQ ://���ܵ�REQ֡
                return_receive_value=REQ;
                break;
            case RSP :
                Receive_Len=8;
                if(Receive_CC_Ind == 0)//�ж��Ƿ���ͨ�Ų���
                {
                    while( Receive_Len < (Receive_MHDR.Length+8) )
                    {
                        data_type=RecBuffer[Receive_Len];
                        switch(data_type)//��������
                        {
                        case Message_cycle_type :
                            Message_cycle=(RecBuffer[Receive_Len+4]+(RecBuffer[Receive_Len+3]<<8)+(RecBuffer[Receive_Len+2]<<16)+(RecBuffer[Receive_Len+1]<<24));
                            Receive_Len=Receive_Len+5;
                            break;
                        case Ctrl_cycle_type :
                            Ctrl_cycle = RecBuffer[Receive_Len+2]+(RecBuffer[Receive_Len+1]<<8);
                            Receive_Len=Receive_Len+3;
                            break;
                        case Delay_type :
                            Offset_Delay =(RecBuffer[Receive_Len+4]+(RecBuffer[Receive_Len+3]<<8)+(RecBuffer[Receive_Len+2]<<16)+(RecBuffer[Receive_Len+1]<<24));
                            Receive_Len=Receive_Len+5;
                            break;
                        case Time_random_type://�������Ŷ�ʱ��
                            Time_random=(5*RecBuffer[Receive_Len+1]);
                            Receive_Len=Receive_Len+2;
                            break;
                        case Frequency_point_type://Ƶ��
                            Frequency_point=RecBuffer[Receive_Len+1];
                            Receive_Len=Receive_Len+2;
                            break;
                        case ALL_RSP_END_need_type :
                            Message_cycle=(RecBuffer[12]+(RecBuffer[11]<<8)+(RecBuffer[10]<<16)+(RecBuffer[9]<<24));
                            Ctrl_cycle=(RecBuffer[14]+(RecBuffer[13]<<8));
                            Offset_Delay=(RecBuffer[18]+(RecBuffer[17]<<8)+(RecBuffer[16]<<16)+(RecBuffer[15]<<24));
                            Time_random=(RecBuffer[19]*5);
                            Receive_Len=(Receive_MHDR.Length+8);
                            break;
                        default:
                            Receive_Len=(Receive_MHDR.Length+8);
                            break;
                        }
                    }// while( Receive_Len < (Receive_MHDR.Length+8) )
                    if(Message_cycle > 30 * 60 * 1000) //MESSAGE�������30���ӷ�һ�Σ�
                    {
                        Message_cycle = (30 * 60 * 1000);
                    }
                    if(Message_cycle < 30 * 1000) //MESSAGE����  30�뷢һ�Σ�
                    {
                        Message_cycle = (30 * 1000);
                    }
                    if(Ctrl_cycle < 12)
                    {
                        Ctrl_cycle = 12;
                    }
                    if(Frequency_point > 25)
                    {
                        Frequency_point = 25;
                    }
                }
                return_receive_value=RSP;
                break;
            case RSP_END : //���ܵ�Station������RSP_END������ȷ��֡
                Receive_Len=8;
                if(Receive_CC_Ind == 0)//�ж��Ƿ���ͨ�Ų���
                {
                    while( Receive_Len < (Receive_MHDR.Length+8) )
                    {
                        data_type=RecBuffer[Receive_Len];
                        switch(data_type)//��������
                        {
                        case Message_cycle_type :
                            Message_cycle=(RecBuffer[Receive_Len+4]+(RecBuffer[Receive_Len+3]<<8)+(RecBuffer[Receive_Len+2]<<16)+(RecBuffer[Receive_Len+1]<<24));
                            Receive_Len=Receive_Len+5;
                            break;
                        case Ctrl_cycle_type :
                            Ctrl_cycle = RecBuffer[Receive_Len+2]+(RecBuffer[Receive_Len+1]<<8);
                            Receive_Len=Receive_Len+3;
                            break;
                        case Delay_type :
                            Offset_Delay =(RecBuffer[Receive_Len+4]+(RecBuffer[Receive_Len+3]<<8)+(RecBuffer[Receive_Len+2]<<16)+(RecBuffer[Receive_Len+1]<<24));
                            Receive_Len=Receive_Len+5;
                            break;
                        case Time_random_type://�������Ŷ�ʱ��
                            Time_random=(5*RecBuffer[Receive_Len+1]);
                            Receive_Len=Receive_Len+2;
                            break;
                        case Frequency_point_type://Ƶ��
                            Frequency_point=RecBuffer[Receive_Len+1];
                            Receive_Len=Receive_Len+2;
                            break;
                        case ALL_RSP_END_need_type :
                            Message_cycle=(RecBuffer[12]+(RecBuffer[11]<<8)+(RecBuffer[10]<<16)+(RecBuffer[9]<<24));
                            Ctrl_cycle=(RecBuffer[14]+(RecBuffer[13]<<8));
                            Offset_Delay=(RecBuffer[18]+(RecBuffer[17]<<8)+(RecBuffer[16]<<16)+(RecBuffer[15]<<24));
                            Time_random=(RecBuffer[19]*5);
                            Receive_Len=(Receive_MHDR.Length+8);
                            break;
                        default:
                            Receive_Len=(Receive_MHDR.Length+8);
                            break;
                        }
                    }// while( Receive_Len < (Receive_MHDR.Length+8) )
                    if(Message_cycle > 30 * 60 * 1000) //MESSAGE�������30���ӷ�һ�Σ�
                    {
                        Message_cycle = (30 * 60 * 1000);
                    }
                    if(Message_cycle < 30 * 1000) //MESSAGE����  30�뷢һ�Σ�
                    {
                        Message_cycle = (30 * 1000);
                    }
                    if(Ctrl_cycle < 12)
                    {
                        Ctrl_cycle = 12;
                    }
                    if(Frequency_point > 25)
                    {
                        Frequency_point = 25;
                    }
                }
                return_receive_value=RSP_END;
                break;
            case BURST :
                return_receive_value=BURST;
                break;
            case ACK ://���ܵ� ACK
                if(data_type==BURST_ACK_type )//���ܵ��澯֡Ӧ��
                {
                    Receive_BURST_ACK_flag=1;
                }
                return_receive_value=ACK;
                break;
            case RFU_1 :
                return_receive_value=RFU_1;
                break;
            case RFU_2 :
                return_receive_value=RFU_1;
                break;
            default:
                break;
            }
        }//У�����ȷ
    }//ID��ȷ
    return return_receive_value ;
}









