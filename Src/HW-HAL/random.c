#include "random.h"
#include "stm32l4xx_hal.h"

RNG_HandleTypeDef RngHandle;
extern uint8_t	Time_random;
extern uint32_t  RTC_SleepTime;

RNG_HandleTypeDef RngHandle;

uint32_t Get_random(void)
{
    uint16_t random_data=0;
    RngHandle.Instance = RNG;
    if (HAL_RNG_Init(&RngHandle) != HAL_OK)
    {
    }
    else
    {
        random_data = HAL_RNG_GetRandomNumber(&RngHandle);
    }
    HAL_RNG_DeInit(&RngHandle);

    if(random_data%2==0)//����16λ�����/65535ռ�İٷֱ�*ʱ�䷶Χ*2����RTCʱ�䣬
    {
        random_data=((random_data*2.048/65536)*Time_random);
        RTC_SleepTime=(RTC_SleepTime+random_data)	;
    }
    else
    {

        random_data=((random_data*2.048/65536)*Time_random);
        RTC_SleepTime=(RTC_SleepTime-random_data)	;

    }

    return random_data;
}




