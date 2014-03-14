#include "led.h"
#include "stm32f4xx.h"

//LED初始化
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;//GPIO初始化结构体
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD, ENABLE);//外设时钟使能
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//外设时钟使能
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//对应GPIO引脚
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//状态为输出
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//IO速度为100MHZ
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_14 ;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13|GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);	 
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//对应GPIO引脚
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//状态为输出
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//IO速度为100MHZ
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//对应GPIO引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//状态为输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//IO速度为100MHZ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO

 //   GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void LEDOn(Led_Def Led)//LED亮
{
  	LED_PORT[Led]->BSRRL=LED_PIN[Led];
}

void LEDOff(Led_Def Led)//LED灭
{
  	LED_PORT[Led]->BSRRH=LED_PIN[Led];  
}

void LEDTog(Led_Def Led)//LED状态翻转
{
  	LED_PORT[Led]->ODR^=LED_PIN[Led];
}

