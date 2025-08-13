/*
 * @Description:
 * @Author: LHK
 * @Date: 2024-05-06 16:11:28
 * @LastEditors: LHK
 * @LastEditTime: 2025-05-24 22:14:49
 */
#include "delay.h"
#include "main.h"
#include "struct_typedef.h"
#include "tim.h"
// 延时nus
// nus为要延时的us数.
// nus:0~190887435(最大值即2^32/fac_us@fac_us=168)
static uint8_t fac_us = 72; // 这里主时钟为168M, 所以在1us内ticks会减168次

void delay_init(u8 SYSCLK)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK); // SysTick频率为HCLK
    fac_us = SYSCLK;
}
void delay_us(u32 uSeconds)
{
    HAL_TIM_Base_Start(&htim4);                     // 启动定时器4
    __HAL_TIM_SetCounter(&htim4, 0);                // 将定时器4的计数器清零
    while (uSeconds > __HAL_TIM_GetCounter(&htim4)) // 当计数器的值小于所需延时时间时，持续循环
    {
    }
    HAL_TIM_Base_Stop(&htim4); // 停止定时器4
}
// void delay_us(u32 nus)
// {
//     u32 ticks;
//     u32 told,tnow,tcnt=0;
//     u32 reload=SysTick->LOAD;	//装载值
//     ticks=nus*fac_us; //需要的节拍数
//     told=SysTick->VAL; //刚进入时的计数器值
//     while(1)
//     {
//         tnow=SysTick->VAL;
//         if(tnow!=told)
//         {
//             if(tnow<told)tcnt+=told-tnow;	//计数器递减
//             else tcnt+=reload-tnow+told;
//             told=tnow;
//             if(tcnt>=ticks)break;	//时间超过或等于延迟的时间时退出.
//         }
//     };
// }

// ms延时
void delay_ms(u32 nms)
{
    u32 i;
    for (i = 0; i < nms; i++)
        delay_us(1000);
}

// 设置延时的时间
void SetTime(tTimeDelay *TimeDelay, uint32_t TimeInter)
{
    TimeDelay->TMStart = HAL_GetTick();

    TimeDelay->TMInter = TimeInter;
}

// 比较延时的时间是否到达，达到则返回真，否则返回0
uint8_t CompareTime(tTimeDelay *TimeDelay)
{
    return ((HAL_GetTick() - TimeDelay->TMStart) >= TimeDelay->TMInter);
}
