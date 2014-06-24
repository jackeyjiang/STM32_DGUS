#include "vioce.h"
#include "stm32f4xx.h"
#include "delay.h"

#define SPI_CS_H    GPIO_SetBits(GPIOD, GPIO_Pin_7)
#define SPI_CS_L    GPIO_ResetBits(GPIOD, GPIO_Pin_7)
#define SPI_SCK_H   GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define SPI_SCK_L   GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#define SPI_MOSI_H	GPIO_SetBits(GPIOB, GPIO_Pin_6) 
#define SPI_MOSI_L	GPIO_ResetBits(GPIOB, GPIO_Pin_6)  
#define SPI_MISO_R	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)  
/*******************************************************************************
* Function Name  : VoiceReadWriteByte
* Description    : wt588d
* Input          : uint8_t TxData
* Output         : uint8_t
* Return         : uint8_t
*******************************************************************************/
//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
void VoiceReadWriteByte(uint8_t TxData)
{
  uint8_t i; 
	SPI_CS_L;
	delay_ms(2); /*片选信号保持低电平2ms*/
    for (i = 0; i < 8; i++)
    {
      SPI_SCK_L;
      if (TxData&0x80)      
      {
          SPI_MOSI_H;
      }
      else
      {
          SPI_MOSI_L;
      }
      TxData <<= 1; 
	  delay_us(300);
	  SPI_SCK_H; 
	  delay_us(300);
	 }
	 SPI_CS_H;  
 } 

void InitVoice(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;//GPIO初始化结构体
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD, ENABLE);//外设时钟使能
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_8;//对应GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//状态为输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO速度为100MHZ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//对应GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//状态为输入
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO速度为100MHZ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//状态为输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//IO速度为100MHZ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_6);
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
	GPIO_SetBits(GPIOD, GPIO_Pin_7);
}
void PlayMusic(char Data)
{
   unsigned char SendDataLoop;
   GPIO_ResetBits(GPIOD,GPIO_Pin_7);
   delay_ms(5);
   for(SendDataLoop=0;SendDataLoop<8;SendDataLoop++)
   {
     GPIO_ResetBits(GPIOB,GPIO_Pin_8);
     if((Data&0x01)==1)
     {
	     GPIO_SetBits(GPIOB,GPIO_Pin_6);
     }
     else
     {
	     GPIO_ResetBits(GPIOB,GPIO_Pin_6);
     }
		 Data>>=1;
		 delay_us(300);
		 GPIO_SetBits(GPIOB,GPIO_Pin_8);
		 delay_us(300);
   }
   GPIO_SetBits(GPIOD,GPIO_Pin_7);
}
