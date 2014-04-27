#include "bsp.h"
#include "stm32f4xx.h"

/*定义退币机的端口，用于打开电源，关闭退币机使其工作*/	  	 
#define COIN_PIN                         GPIO_Pin_0
#define COIN_GPIO_PORT                   GPIOA
#define COIN_GPIO_CLK                    RCC_AHB1Periph_GPIOA  
/*定义退币机中断端口信号口*/
#define COIN_INT_PIN                         GPIO_Pin_1
#define COIN_GPIO_INT_PORT                   GPIOB
#define COIN_GPIO_INT_CLK                    RCC_AHB1Periph_GPIOB

uint32_t Coins_cnt =0; //一键退币计数
uint32_t Coins_totoal=0; //硬币总数统计

/*
PE11为出币控制信号
P10为吃币点数输出
PE9为空币信号输出
PE8为计币器输出（暂不用），
*/
void InitMiniGPIO(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   EXTI_InitTypeDef EXTI_InitStructure;//外部中断初始化结构体
   
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE ,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOE,&GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource10);
	  EXTI_InitStructure.EXTI_Line=EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode =EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		EXTI_ClearITPendingBit(EXTI_Line10);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOE,&GPIO_InitStructure);
 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOE,&GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : OpenCoins
* Description    : 开打退币机
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void OpenCoins(void)	
{
  GPIO_SetBits(COIN_GPIO_PORT, COIN_PIN);
}
	
/*******************************************************************************
* Function Name  : CloseCoins
* Description    : 关闭退币机
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void CloseCoins(void) 
{										   
  GPIO_ResetBits(COIN_GPIO_PORT, COIN_PIN); 
}

 /*******************************************************************************
* Function Name  : SendOutN_Coin
* Description    : 退币
* Input          : 要退的币数
* Output         : 未退的币数，修改
* Return         : 
*******************************************************************************/
uint8_t ErrorType;
uint8_t SendOutN_Coin(int num)
{
  int i;
  ErrorType = 0;
  for(i=0;i<num;i++)
  {
    if( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9) != 0)
	  {
		  GPIO_ResetBits(GPIOE,GPIO_Pin_11);
		  delay_ms(15);
		  GPIO_SetBits(GPIOE,GPIO_Pin_11);
		  delay_ms(15);
		  GPIO_ResetBits(GPIOE,GPIO_Pin_11);
		  delay_ms(50);
			
	  }
	  else
	  {
	    ErrorType = 1;
	    return num-i;
	  }
  }
  return 0;
}

 /*******************************************************************************
* Function Name  : refund_button
* Description    : 全部退币
* Input          : 要退的币数
* Output         : 未退的币数，修改
* Return         : 
*******************************************************************************/
void RefundButton(void)
{
	uint16_t i;
	for(i=0;i<20;i++)
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_11);
		delay_ms(15);
		GPIO_SetBits(GPIOE,GPIO_Pin_11);
		delay_ms(15);
		GPIO_ResetBits(GPIOE,GPIO_Pin_11);
		delay_ms(50);
	}	
}
 /*******************************************************************************
* Function Name  : StopRefond
* Description    : 全部退币
* Input          : 要退的币数
* Output         : 未退的币数，修改
* Return         : 
*******************************************************************************/
void StopRefond(void)
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_11);
}
