#ifndef __DS18B20_H
#define __DS18B20_H 

#include "stm32f4xx.h"
  


extern short temp;

#define DS18B20_PIN                         GPIO_Pin_3
#define DS18B20_GPIO_PORT                   GPIOA
#define DS18B20_GPIO_CLK                    RCC_AHB1Periph_GPIOA  

#define DS18B20_DATA_IN  GPIO_ReadInputDataBit(DS18B20_GPIO_PORT, DS18B20_PIN)
#define DS18B20_OUT_HIGH GPIO_SetBits(DS18B20_GPIO_PORT, DS18B20_PIN);
#define DS18B20_OUT_LOW  GPIO_ResetBits(DS18B20_GPIO_PORT, DS18B20_PIN);

uint8_t DS18B20_DEMO(void) ; 	
u8 DS18B20_Init(void);//初始化DS18B20
short DS18B20_Get_Temp(void);//获取温度
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);//读出一个字节
u8 DS18B20_Read_Bit(void);//读出一个位
u8 DS18B20_Check(void);//检测是否存在DS18B20
void DS18B20_Reset(void);       //复位DS18B20    
u8 DS18B20_Get_Serial(u8 *serial);
u8 GetCRC(u8 *str, u32 length);
#endif















