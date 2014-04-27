#include "bsp.h"
#include "stm32f4xx.h"
/*******************************************************************************
 说明：此文件为投币机程序模块
*******************************************************************************/

/*******************************************************************************
* Function Name  : InitCoins
* Description    :硬币机初始化（投币）;包括电源控制口
* Input          : void
* Output         : void						 
* Return         : void
*******************************************************************************/
void InitCoins(void)
{									 
    GPIO_InitTypeDef GPIO_InitStructure;//GPIO初始化结构体
	  NVIC_InitTypeDef NVIC_InitStructure;//中断向量初始化结构体
	  EXTI_InitTypeDef EXTI_InitStructure;//外部中断初始化结构体
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//外设时钟使能
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	//脉冲输入脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = 	GPIO_PuPd_UP ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	//投币机电源控制脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = 	GPIO_PuPd_DOWN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource12); //中断线
    EXTI_InitStructure.EXTI_Line=EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode =EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    EXTI_ClearITPendingBit(EXTI_Line12);
}
/*******************************************************************************
* Function Name  : OpenCoinMachine
* Description    : 打开投币机电源
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
extern unsigned int   CoinsCount;
void OpenCoinMachine(void)		 
{
  GPIO_SetBits(GPIOE, GPIO_Pin_13);
  CoinsCount=0;
}

/*******************************************************************************
* Function Name  : OpenCoinMachine
* Description    : 关闭投币机电源
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void CloseCoinMachine(void)	
{
  GPIO_ResetBits(GPIOE, GPIO_Pin_13);
  CoinsCount=0;
}
