#include <stdio.h>
#include <stdlib.h>
#include "oled.h"
#include "main.h"
#include "OLED_Font.h"
// OLED的显存
// 存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127
unsigned char OLED_GRAM[128][8];
#if OLED_MODE == 1
// 向SSD1106写入一个字节。
// dat:要写入的数据/命令
// cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(unsigned char dat, unsigned char cmd)
{
	DATAOUT(dat);
	if (cmd)
		OLED_DC_Set();
	else
		OLED_DC_Clr();
	OLED_CS_Clr();
	OLED_WR_Clr();
	OLED_WR_Set();
	OLED_CS_Set();
	OLED_DC_Set();
}
#else
// 向SSD1106写入一个字节。
// dat:要写入的数据/命令
// cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(unsigned char dat, unsigned char cmd)
{
	unsigned char i;
	if (cmd)
		OLED_DC_Set();
	else
		OLED_DC_Clr();
	OLED_CS_Clr();
	for (i = 0; i < 8; i++)
	{
		OLED_SCLK_Clr();
		if (dat & 0x80)
			OLED_SDIN_Set();
		else
			OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat <<= 1;
	}
	OLED_CS_Set();
	OLED_DC_Set();
}
#endif

/**
 * @description: 清除显存数组
 * @return {*}
 */
void OLED_GraphicsMemory_Clear(void)
{
	for (int i = 0; i < 128; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			OLED_GRAM[i][j] = 0;
		}
	}
}

// 设置oled显示坐标
// 参数x是列（0-63，也就是列地址最多为0x3F，SH1107最多可达0x7F）
// y是页（0-15，也就是页地址最多为0x0F）
// 坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);
}
// 开启OLED显示
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令  电荷泵使能
	OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON	开启电荷泵
	OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON	点亮屏幕
}
// 关闭OLED显示
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
	OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}
// 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
	unsigned char i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
		OLED_WR_Byte(0x02, OLED_CMD);	  // 设置显示位置—列低地址
		OLED_WR_Byte(0x10, OLED_CMD);	  // 设置显示位置—列高地址
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0, OLED_DATA);
	} // 更新显示
}

// 在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// size:选择字体 12/16/24
// 取模方式 逐列式
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char size1)
{
	unsigned char i, m, temp, size2, chr1;
	unsigned char y0 = y;
	size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2); // 得到字体一个字符对应点阵集所占的字节数
	chr1 = chr - ' ';										   // 计算偏移后的值
	for (i = 0; i < size2; i++)
	{
		if (size1 == 12)
		{
			temp = asc2_1206[chr1][i];
		} // 调用 1206 字体
		else if (size1 == 16)
		{
			temp = asc2_1608[chr1][i];
		} // 调用 1608 字体
		else if (size1 == 24)
		{
			temp = asc2_2412[chr1][i];
		} // 调用 2412 字体
		else
			return;
		for (m = 0; m < 8; m++) // 写入数据
		{
			if (temp & 0x80)
				OLED_DrawPoint(x, y);
			else
				OLED_ClearPoint(x, y);
			temp <<= 1;
			y++;
			if ((y - y0) == size1)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
} // m^n函数
unsigned long oled_pow(unsigned char m, unsigned char n)
{
	unsigned long result = 1;
	while (n--)
		result *= m;
	return result;
}

// 显示2个数字
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// mode:模式	0,填充模式;1,叠加模式
// num:数值(0~4294967295);
void OLED_ShowNum(unsigned char x, unsigned char y, unsigned long num, unsigned char len, unsigned char size)
{
	unsigned char t, temp;
	unsigned char enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size / 2) * t, y, ' ', size);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size);
	}
}
/**
 * @brief   显示一个字符号串
 * @param   x : uint8_t 起点列坐标
 * @param   y : uint8_t 起点行坐标
 * @param   chr : uint8_t *字符串指针，或者 "输入的内容"
 * @retval  无
 */
void OLED_ShowString(unsigned char x, unsigned char y, unsigned char *chr, unsigned char size)
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], size);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

unsigned int OLED_Pow(unsigned char m, unsigned char n)
{
	unsigned int result = 1;
	while (n--)
	{
		result *= m;
	}
	return result;
}
// 显示汉字
// x,y:起点坐标
// num:汉字对应的序号
// 取模方式 列行式

void OLED_ShowChinese(unsigned char x, unsigned char y, unsigned char num, unsigned char size1)
{
	unsigned char i, m, n = 0, temp, chr1;
	unsigned char x0 = x, y0 = y;
	unsigned char size3 = size1 / 8;
	while (size3--)
	{
		chr1 = num * size1 / 8 + n;
		n++;
		for (i = 0; i < size1; i++)
		{
			if (size1 == 16)
			{
				temp = Hzk1[chr1][i];
			} // 调用 16*16 字体
			else if (size1 == 24)
			{
				temp = Hzk2[chr1][i];
			} // 调用 24*24 字体
			else if (size1 == 32)
			{
				temp = Hzk3[chr1][i];
			} // 调用 32*32 字体
			else if (size1 == 64)
			{
				temp = Hzk4[chr1][i];
			} // 调用 64*64 字体
			else
				return;
			for (m = 0; m < 8; m++)
			{
				if (temp & 0x01)
					OLED_DrawPoint(x, y);
				else
					OLED_ClearPoint(x, y);
				temp >>= 1;
				y++;
			}
			x++;
			if ((x - x0) == size1)
			{
				x = x0;
				y0 = y0 + 8;
			}
			y = y0;
		}
	}
}

/**********************************************
 * @brief  显示图片 (后期加上参数检验)
 * @param  x0 : 列地址-起(0~127)
 * @param  x1 : 列地址-终(0~127)
 * @param  y0 : 页地址-起(0~7)
 * @param  y1 : 页地址-终(0~7)
 * @param  BMP: 存储图片的数组
 * @retval None
 **********************************************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		OLED_Set_Pos(x0, y);
		for (x = x0; x < x1; x++)
		{
			OLED_WR_Byte(BMP[j++], OLED_DATA);
		}
	}
}
// 反显函数
void OLED_ColorTurn(unsigned char i)
{
	if (i == 0)
	{
		OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示
	}
	if (i == 1)
	{
		OLED_WR_Byte(0xA7, OLED_CMD); // 反色显示
	}
}
// 屏幕旋转 180 度
void OLED_DisplayTurn(unsigned char i)
{
	if (i == 0)
	{
		OLED_WR_Byte(0xC8, OLED_CMD); // 正常显示
		OLED_WR_Byte(0xA1, OLED_CMD);
	}
	if (i == 1)
	{
		OLED_WR_Byte(0xC0, OLED_CMD); // 反转显示
		OLED_WR_Byte(0xA0, OLED_CMD);
	}
}
// 更新显存到 OLED
void OLED_Refresh(void)
{
	unsigned char i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置行起始地址
		OLED_WR_Byte(0x00, OLED_CMD);	  // 设置低列起始地址
		OLED_WR_Byte(0x10, OLED_CMD);	  // 设置高列起始地址
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
	}
}
// 画点
// x:0~127
// y:0~63
void OLED_DrawPoint(unsigned char x, unsigned char y)
{
	unsigned char i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	OLED_GRAM[x][i] |= n;
}
// 清除一个点
// x:0~127
// y:0~63
void OLED_ClearPoint(unsigned char x, unsigned char y)
{
	unsigned char i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
	OLED_GRAM[x][i] |= n;
	OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
}
// 画线
// x:0~128
// y:0~64
void OLED_DrawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
	unsigned char i, k, k1, k2;
	if ((x2 > 128) || (y2 > 64) || (x1 > x2) || (y1 > y2))
		return;
	if (x1 == x2) // 画竖线
	{
		for (i = 0; i < (y2 - y1); i++)
		{
			OLED_DrawPoint(x1, y1 + i);
		}
	}
	else if (y1 == y2) // 画横线
	{
		for (i = 0; i < (x2 - x1); i++)
		{
			OLED_DrawPoint(x1 + i, y1);
		}
	}
	else // 画斜线
	{
		k1 = y2 - y1;
		k2 = x2 - x1;
		k = k1 * 10 / k2;
		for (i = 0; i < (x2 - x1); i++)
		{
			OLED_DrawPoint(x1 + i, y1 + i * k / 10);
		}
	}
}
// x,y:圆心坐标
// r:圆的半径
void OLED_DrawCircle(unsigned char x, unsigned char y, unsigned char r)
{
	int a, b, num;
	a = 0;
	b = r;
	while (2 * b * b >= r * r)
	{
		OLED_DrawPoint(x + a, y - b);
		OLED_DrawPoint(x - a, y - b);
		OLED_DrawPoint(x - a, y + b);
		OLED_DrawPoint(x + a, y + b);
		OLED_DrawPoint(x + b, y + a);
		OLED_DrawPoint(x + b, y - a);
		OLED_DrawPoint(x - b, y - a);
		OLED_DrawPoint(x - b, y + a);
		a++;
		num = (a * a + b * b) - r * r; // 计算画的点离圆心的距离
		if (num > 0)
		{
			b--;
			a--;
		}
	}
}

/**
 * @description: 显示整数，支持负数
 * @param {unsigned char} x	起点x坐标
 * @param {unsigned char} y 起点y坐标
 * @param {signed int} num	数字
 * @param {unsigned char} len	数字的位数
 * @param {unsigned char} size1	字体大小12/16/24
 * @return {*}
 */
void OLED_ShowSignum(unsigned char x, unsigned char y, signed int num, unsigned char len, unsigned char size1)
{
	unsigned char t, temp;
	if (num < 0)
	{
		num = ~num + 1;
		OLED_ShowChar(x, y, '-', size1);
		for (t = 1; t < len; t++)
		{
			temp = (num / OLED_Pow(10, len - t - 1)) % 10;
			if (temp == 0)
			{
				OLED_ShowChar(x + (size1 / 2) * t, y, '0', size1);
			}
			else
			{
				OLED_ShowChar(x + (size1 / 2) * t, y, temp + '0', size1);
			}
		}
	}
	else
	{
		OLED_ShowChar(x, y, ' ', size1);
		for (t = 0; t < len; t++)
		{
			temp = (num / OLED_Pow(10, len - t - 1)) % 10;
			if (temp == 0)
			{
				OLED_ShowChar(x + (size1 / 2) * t, y, '0', size1);
			}
			else
			{
				OLED_ShowChar(x + (size1 / 2) * t, y, temp + '0', size1);
			}
		}
	}
}
// 配置写入数据的起始位置
void OLED_WR_BP(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD); // 设置行起始地址
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f), OLED_CMD);
}
// x0,y0：起点坐标
// x1,y1：终点坐标
// BMP[]：要写入的图片数组
void OLED_ShowPicture(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x = 0, y = 0;
	if (y % 8 == 0)
		y = 0;
	else
		y += 1;
	for (y = y0; y < y1; y++)
	{
		OLED_WR_BP(x0, y);
		for (x = x0; x < x1; x++)
		{
			OLED_WR_Byte(BMP[j], OLED_DATA);
			j++;
		}
	}
}

// num 显示汉字的个数
// space 每一遍显示的间隔
void OLED_ScrollDisplay(unsigned char num, unsigned char space)
{
	unsigned char i, n, t = 0, m = 0, r;
	while (1)
	{
		if (m == 0)
		{
			OLED_ShowChinese(128, 24, t, 16); // 写 入 一 个 汉 字 保 存 在 OLED_GRAM[][] 数组中
			t++;
		}
		if (t == num)
		{
			for (r = 0; r < 16 * space; r++) // 显示间隔
			{
				for (i = 0; i < 144; i++)
				{
					for (n = 0; n < 8; n++)
					{
						OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
					}
				}
				OLED_Refresh();
			}
			t = 0;
		}
		m++;
		if (m == 16)
		{
			m = 0;
		}
		for (i = 0; i < 144; i++) // 实现左移
		{
			for (n = 0; n < 8; n++)
			{
				OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

// 初始化SH1106
void OLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOB_CLK_ENABLE();

	// CS
	GPIO_InitStruct.Pin = OLED_CS_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED_CS_GPIO, &GPIO_InitStruct);

	// DC
	GPIO_InitStruct.Pin = OLED_DC_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED_DC_GPIO, &GPIO_InitStruct);

	// SCLK
	GPIO_InitStruct.Pin = OLED_SCLK_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED_SCLK_GPIO, &GPIO_InitStruct);

	// SDIN
	GPIO_InitStruct.Pin = OLED_SDIN_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED_SDIN_GPIO, &GPIO_InitStruct);

	HAL_GPIO_WritePin(OLED_CS_GPIO, OLED_CS_GPIO_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(OLED_DC_GPIO, OLED_DC_GPIO_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(OLED_SCLK_GPIO, OLED_SCLK_GPIO_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(OLED_SDIN_GPIO, OLED_SDIN_GPIO_PIN, GPIO_PIN_SET);
#if OLED_MODE == 1

	GPIO_InitStruct.GPIO_Pin = 0xFF; // PC0~7 OUT推挽输出
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, 0xFF); // PC0~7输出高

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; // PG13,14,15 OUT推挽输出
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOG, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15); // PG13,14,15 OUT  输出高

#else
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1; // PC0,1 OUT推挽输出
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET); // PC0,1 OUT  输出高

	GPIO_InitStruct.Pin = OLED_RST_GPIO_PIN; //  OUT推挽输出	  RST
	HAL_GPIO_Init(OLED_RST_GPIO, &GPIO_InitStruct);
	HAL_GPIO_WritePin(OLED_RST_GPIO, OLED_RST_GPIO_PIN, GPIO_PIN_SET); //  OUT  输出高

#endif
	OLED_RST_Set();
	HAL_Delay(100);
	OLED_RST_Clr();
	HAL_Delay(100);
	OLED_RST_Set();

	OLED_WR_Byte(0xAE, OLED_CMD); //--turn off oled panel
	OLED_WR_Byte(0x02, OLED_CMD); //---set low column address
	OLED_WR_Byte(0x10, OLED_CMD); //---set high column address
	OLED_WR_Byte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81, OLED_CMD); //--set contrast control register
	OLED_WR_Byte(0xCF, OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8, OLED_CMD); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6, OLED_CMD); //--set normal display
	OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f, OLED_CMD); //--1/64 duty
	OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00, OLED_CMD); //-not offset
	OLED_WR_Byte(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9, OLED_CMD); //--set pre-charge period
	OLED_WR_Byte(0xF1, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA, OLED_CMD); //--set com pins hardware configuration
	OLED_WR_Byte(0x12, OLED_CMD);
	OLED_WR_Byte(0xDB, OLED_CMD); //--set vcomh
	OLED_WR_Byte(0x40, OLED_CMD); // Set VCOM Deselect Level
	OLED_WR_Byte(0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02, OLED_CMD); //
	OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
	OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable
	OLED_WR_Byte(0xA4, OLED_CMD); // Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6, OLED_CMD); // Disable Inverse Display On (0xa6/a7)
	OLED_WR_Byte(0xAF, OLED_CMD); //--turn on oled panel

	OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
	OLED_Clear();
	OLED_Set_Pos(0, 0);
}
