/*
 * @Description  :
 * @Author       : WYT
 * @Date         : 2019-07-09 23:03:06
 * @LastEditTime: 2025-05-24 22:26:13
 * @LastEditors: LHK
 */
#ifndef __DHT11_H
#define __DHT11_H

#define DHT11_GPIO GPIOA
#define DHT11_GPIO_PIN GPIO_PIN_6
#define DHT11_GPIO_PIN_CLK __HAL_RCC_GPIOA_CLK_ENABLE()

#define DHT11_DQ_High HAL_GPIO_WritePin(DHT11_GPIO, DHT11_GPIO_PIN, GPIO_PIN_SET)
#define DHT11_DQ_Low HAL_GPIO_WritePin(DHT11_GPIO, DHT11_GPIO_PIN, GPIO_PIN_RESET)

void delay(unsigned int uSeconds);
void DHT11_IO_OUT(void);
void DHT11_IO_IN(void);
void DHT11_Init(void);
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Check(void);
void DHT11_Rst(void);

#endif
