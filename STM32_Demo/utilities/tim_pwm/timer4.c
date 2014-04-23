#include "stm32f4xx.h"
#include "timer4.h"

void TIM4_Init(void)//TIM_Period为16位的数	   //设置1秒钟中断一次
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef  NVIC_InitStructure; 
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//外设时钟使能  
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;					
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;//抢占优先级最高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);					
	TIM_TimeBaseStructure.TIM_Period = 19999;//计数器重装值
	TIM_TimeBaseStructure.TIM_Prescaler = 4199;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数方式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_PrescalerConfig(TIM4, (4200-1), TIM_PSCReloadMode_Immediate);//设置16位时钟分频系数,立即重载模式
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//计满中断
	TIM_Cmd(TIM4, DISABLE);
}

void OpenTIM4(void)
{
  TIM_Cmd(TIM4, ENABLE);	
}

void CloseTIM4(void)
{
  TIM_Cmd(TIM4, DISABLE);
}

