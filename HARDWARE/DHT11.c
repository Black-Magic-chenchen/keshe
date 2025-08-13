
#include "main.h"
#include "DHT11.h"
#include "delay.h"

void DHT11_IO_IN(void) // 温湿度模块输入函数
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    DHT11_GPIO_PIN_CLK;

    /*Configure GPIO pins : PAPin PAPin */
    GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT11_GPIO, &GPIO_InitStruct);
}

void DHT11_IO_OUT(void) // 温湿度模块输出函数
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    DHT11_GPIO_PIN_CLK;

    /*Configure GPIO pins : PAPin PAPin */
    GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_GPIO, &GPIO_InitStruct);
}

// 复位DHT11
void DHT11_Rst(void)
{
    DHT11_IO_OUT();      // SET OUTPUT
    DHT11_DQ_Low;        // DQ=0
    delay_us(20 * 1000); // 拉低至少18ms
    DHT11_DQ_High;       // DQ=1
    delay_us(30);        // 主机拉高20~40us
}

// 等待DHT11的回应
// 返回1:未检测到DHT11的存在
// 返回0:存在
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;                                                         // 定义临时变量
    DHT11_IO_IN();                                                             // SET INPUT
    while ((HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_GPIO_PIN) == 1) && retry < 100) // DHT11会拉低40~80us
    {
        retry++;
        delay_us(1);
    };
    if (retry >= 100)
        return 1;
    else
        retry = 0;
    while ((HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_GPIO_PIN) == 0) && retry < 100) // DHT11拉低后会再次拉高40~80us
    {
        retry++;
        delay_us(1);
    };
    if (retry >= 100)
        return 1;
    return 0;
}
// 从DHT11读取一个位
// 返回值：1/0
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    while ((HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_GPIO_PIN) == 1) && retry < 100) // 等待变为低电平
    {
        retry++;
        delay_us(1);
    }
    retry = 0;
    while ((HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_GPIO_PIN) == 0) && retry < 100) // 等待变高电平
    {
        retry++;
        delay_us(1);
    }
    delay_us(40); // 等待40us
    if (HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_GPIO_PIN) == 1)
        return 1;
    else
        return 0;
}
// 从DHT11读取一个字节
// 返回值：读到的数据
uint8_t DHT11_Read_Byte(void)
{
    uint8_t i, dat;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

// 从DHT11读取一次数据
// temp:温度值(范围:0~50°)
// humi:湿度值(范围:20%~90%)
// 返回值：0,正常;1,读取失败
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    DHT11_Rst();
    if (DHT11_Check() == 0)
    {
        for (i = 0; i < 5; i++) // 读取40位数据
        {
            buf[i] = DHT11_Read_Byte();
        }
        if (((buf[0] + buf[1] + buf[2] + buf[3]) & 0xFF) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
        return 1;
    return 0;
}
// 初始化DHT11的IO口 DQ 同时检测DHT11的存在
// 返回1:不存在
// 返回0:存在
void DHT11_Init(void)
{
    DHT11_IO_OUT(); // 设置为输出模式
    DHT11_DQ_High;  // DQ=1
    DHT11_Rst();   // 复位DHT11
    DHT11_Check(); // 等待DHT11的回应
}
