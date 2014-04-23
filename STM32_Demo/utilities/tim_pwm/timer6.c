#include "stm32f4xx.h"
#include "timer6.h" 
  
  /***********************************************
 用于定时时间。
 *************************************************/
//通用定时器3初始化,对84MHz（TIM3时钟源为168MHz/2）进行TIM_scale分频后作为计数时钟
void TIM7_Init(void)//TIM_Period为16位的数	   //设置1秒钟中断一次
{
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	NVIC_InitTypeDef  NVIC_InitStructure; 
	 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//外设时钟使能  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;					
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级最高
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//子优先级
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = 19999;//计数器重装值
  	TIM_TimeBaseStructure.TIM_Prescaler = 4199;
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数方式
  	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM7, (4200-1), TIM_PSCReloadMode_Immediate);//设置16位时钟分频系数,立即重载模式
	  TIM_ClearFlag(TIM7, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
  	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);//计满中断
  	TIM_Cmd(TIM7, DISABLE);
}

void OpenTIM7(void)
{
   TIM_Cmd(TIM7, ENABLE);
}

void CloseTIM7(void)
{
   TIM_Cmd(TIM7, DISABLE);
}
