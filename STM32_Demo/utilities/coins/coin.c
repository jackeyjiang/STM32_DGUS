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
}
/*******************************************************************************
* Function Name  : OpenCoinMachine
* Description    : 打开投币机电源
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void OpenCoinMachine(void)		 
{
  GPIO_SetBits(GPIOE, GPIO_Pin_13);
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
}

/*******************************************************************************
* Function Name  : TIM5_Init
* Description    :定时器5初始化
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
 void TIM5_Init(void)//TIM_Period为16位的数	   //设置1秒钟中断一次
{ 
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	NVIC_InitTypeDef  NVIC_InitStructure; 
	 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//外设时钟使能  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;					
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级最高
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//子优先级
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = 19999;//计数器重装值
  	TIM_TimeBaseStructure.TIM_Prescaler = 4199;
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数方式
  	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM5, (4200-1), TIM_PSCReloadMode_Immediate);//设置16位时钟分频系数,立即重载模式
	  TIM_ClearFlag(TIM5, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
  	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);//计满中断
  	TIM_Cmd(TIM5, DISABLE);

}

    /*******************************************************************************
* Function Name  : OpenTIM5
* Description    : 打开定时器5
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void OpenTIM5(void)
{
   TIM_Cmd(TIM5, ENABLE);
}

    /*******************************************************************************
* Function Name  : CloseTIM5
* Description    : 关闭定时器5
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void CloseTIM5(void)
{
   TIM_Cmd(TIM5, DISABLE);
}
