/*
 * @Description: 
 * @Author: LHK
 * @Date: 2024-05-06 16:11:28
 * @LastEditors: LHK
 * @LastEditTime: 2025-01-27 19:54:31
 */
#ifndef _DELAY_H
#define _DELAY_H
#include <struct_typedef.h>	  
#include <stdint.h>

typedef struct{
	uint32_t TMStart;
	uint32_t TMInter;
	
}tTimeDelay;


void SetTime(tTimeDelay *TimeType,uint32_t TimeInter);

uint8_t  CompareTime(tTimeDelay *TimeType);



void delay_init(u8 SYSCLK);
void delay_ms(U32 nms);
void delay_us(U32 nus);
#endif

