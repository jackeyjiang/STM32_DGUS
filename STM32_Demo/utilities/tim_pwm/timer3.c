#include "stm32f4xx.h"
#include "timer3.h"
#include "bsp.h"
 /***********************************************
 用于定时时间。
 *************************************************/
//通用定时器3初始化,对84MHz（TIM3时钟源为168MHz/2）进行TIM_scale分频后作为计数时钟
void TIM3_Init(void)//TIM_Period为16位的数	   //设置1秒钟中断一次
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	NVIC_InitTypeDef  NVIC_InitStructure; 
	 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//外设时钟使能  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;					
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;//抢占优先级最高
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//子优先级
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = 19999;//计数器重装值
  	TIM_TimeBaseStructure.TIM_Prescaler = 4199;
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数方式
  	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM3, (4200-1), TIM_PSCReloadMode_Immediate);//设置16位时钟分频系数,立即重载模式
	  TIM_ClearFlag(TIM3, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
  	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//计满中断
  	TIM_Cmd(TIM3, DISABLE);
}

void OpenTIM3(void)
{
	 WaitTimeInit(&WaitTime);
   TIM_Cmd(TIM3, ENABLE);
}

void CloseTIM3(void)
{
	 WaitTimeInit(&WaitTime);
   TIM_Cmd(TIM3, DISABLE);
}


