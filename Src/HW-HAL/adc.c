#include "adc.h"
#include "protocol.h"
#include "string.h"

ADC_HandleTypeDef hadc1;
float a;
uint16_t b;
__IO uint16_t uhADCx_16 = 0, uhADCxRefValue = 0, uhADCxConvertedValue = 0;

uint16_t 	Water_Depth = 0;
uint8_t Burst_Status = 0;

void MX_ADC1_Init(void)
{

    ADC_ChannelConfTypeDef sConfig;

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    hadc1.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }


    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

}

void ADC1toChanel_Ref(void)
{
    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void ADC1toChanel_8(void)
{
    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void ADC1toChanel_16(void)
{
    ADC_ChannelConfTypeDef sConfig;

    sConfig.Channel = ADC_CHANNEL_16;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void Battery_Voltage_Measure(void)
{


    MX_ADC1_Init();

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }


    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }


    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        uhADCxRefValue = HAL_ADC_GetValue(&hadc1);
    }


    ADC1toChanel_16();

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }


    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }

    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        uhADCx_16 = HAL_ADC_GetValue(&hadc1);
    }
    if (uhADCx_16 > 2500)
    {
        Buffer[0]=0x02;
        Buffer[1]=0x00;
    }
    else
    {
        Buffer[0]=0x01;
        Buffer[1]=0x00;
    }

    if(uhADCx_16 < Alarm_threshold)
    {
        Buffer[0]=0x01;
        Buffer[1]=0x00;
        Burst_Status = HAL_RTCEx_BKUPRead(&RTCHandle, RTC_BKP_DR20);
        if(Burst_Status == 1) //֮ǰ״̬�Ǹ澯֡
        {
            Send_BURST_Flag = 0;
        }
        else
        {
            Send_BURST_Flag = 1;
            HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR20, 1);
        }
        if(Send_Frame_Type == 1)
        {
            Send_BURST_Flag = 1;
            Send_Frame_Type = 0;
        }
        if(Send_Frame_Type == 2)
        {
            Send_BURST_Flag = 1;
        }
    }
    else
    {
        HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR20, 0);
    }

    ADC1toChanel_8();

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        Error_Handler();
    }

    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        /* End Of Conversion flag not set on time */
        Error_Handler();
    }
    else
    {
        uhADCxConvertedValue = HAL_ADC_GetValue(&hadc1);
    }


    a = 1.34 * uhADCxConvertedValue / uhADCxRefValue*2;//��ѹ��λΪV
    memcpy(&Buffer[2],&a,4);//uhADCx_16
    Buffer[6]=uhADCx_16;
    Buffer[7]=uhADCx_16>>8;
}







