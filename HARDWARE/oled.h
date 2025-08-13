/*
 * @Descripttion:
 * @Author: LHK
 * @Date: 2023-01-17 13:36:32
 * @LastEditors: LHK
 * @LastEditTime: 2025-05-24 22:00:30
 */

#ifndef __OLED_H
#define __OLED_H

//========移植的时候，根据所选芯片引入相应的头文件==========
#include "main.h"

// OLED模式设置
// 0:4线串行模式
// 1:并行8080模式
/* #define OLED_MODE 0
#define SIZE 16
#define XLevelL 0x00
#define XLevelH 0x10
#define Max_Column 128
#define Max_Row 64
#define Brightness 0xFF
#define X_WIDTH 128
#define Y_WIDTH 64
 */
//-----------------OLED端口定义----------------

#define OLED_CS_GPIO GPIOA
#define OLED_CS_GPIO_PIN GPIO_PIN_4
#define OLED_RST_GPIO GPIOB
#define OLED_RST_GPIO_PIN GPIO_PIN_6
#define OLED_DC_GPIO GPIOC
#define OLED_DC_GPIO_PIN GPIO_PIN_7
#define OLED_WR_GPIO GPIOG
#define OLED_WR_GPIO_PIN GPIO_PIN_14
#define OLED_RD_GPIO GPIOG
#define OLED_RD_GPIO_PIN GPIO_PIN_13

// PC0~7,作为数据线
#define DATAOUT(x) GPIO_Write(GPIOC, x); // 输出

// 使用4线串行接口时使用
#define OLED_SCLK_GPIO GPIOA
#define OLED_SCLK_GPIO_PIN GPIO_PIN_5
#define OLED_SDIN_GPIO GPIOA
#define OLED_SDIN_GPIO_PIN GPIO_PIN_7

// CS
#define OLED_CS_Clr() HAL_GPIO_WritePin(OLED_CS_GPIO, OLED_CS_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_CS_Set() HAL_GPIO_WritePin(OLED_CS_GPIO, OLED_CS_GPIO_PIN, GPIO_PIN_SET)

// RES
#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RST_GPIO, OLED_RST_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RST_GPIO, OLED_RST_GPIO_PIN, GPIO_PIN_SET)

// DC
#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO, OLED_DC_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO, OLED_DC_GPIO_PIN, GPIO_PIN_SET)

// SCLK,D0
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCLK_GPIO, OLED_SCLK_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCLK_GPIO, OLED_SCLK_GPIO_PIN, GPIO_PIN_SET)

// SDIN,D1
#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDIN_GPIO, OLED_SDIN_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDIN_GPIO, OLED_SDIN_GPIO_PIN, GPIO_PIN_SET)

// WR
#define OLED_WR_Clr() HAL_GPIO_WritePin(OLED_WR_GPIO, OLED_WR_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_WR_Set() HAL_GPIO_WritePin(OLED_WR_GPIO, OLED_WR_GPIO_PIN, GPIO_PIN_SET)

// RD
#define OLED_RD_Clr() HAL_GPIO_WritePin(OLED_RD_GPIO, OLED_RD_GPIO_PIN, GPIO_PIN_RESET)
#define OLED_RD_Set() HAL_GPIO_WritePin(OLED_RD_GPIO, OLED_RD_GPIO_PIN, GPIO_PIN_SET)

#define OLED_CMD 0  // 写命令
#define OLED_DATA 1 // 写数据

// OLED控制用函数
void OLED_WR_Byte(unsigned char dat, unsigned char cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char size);
unsigned long oled_pow(unsigned char m, unsigned char n);
void OLED_ShowNum(unsigned char x, unsigned char y, unsigned long num, unsigned char len, unsigned char size);
void OLED_ShowString(unsigned char x, unsigned char y, unsigned char *p, unsigned char size);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowChinese(unsigned char x, unsigned char y, unsigned char num, unsigned char size1);
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);
void OLED_ClearPoint(unsigned char x, unsigned char y);
void OLED_ColorTurn(unsigned char i);
void OLED_DisplayTurn(unsigned char i);
void OLED_Refresh(void);
void OLED_DrawPoint(unsigned char x, unsigned char y);
void OLED_DrawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void OLED_DrawCircle(unsigned char x, unsigned char y, unsigned char r);
void OLED_ScrollDisplay(unsigned char num, unsigned char space);
void OLED_WR_BP(unsigned char x, unsigned char y);
void OLED_ShowPicture(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);
void OLED_ShowSignum(unsigned char x, unsigned char y, signed int num, unsigned char len, unsigned char size1);
void OLED_GraphicsMemory_Clear(void);

#endif
