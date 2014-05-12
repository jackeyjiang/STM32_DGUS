 #include "stm32f4xx.h"
#include "timer2.h"



/*******************************************************************************
 * 函数名称:TIM2_Configuration                                                                    
 * 描    述:TIM2初始化函数   5ms中断一次                                                                
 *                                                                               
 * 输    入:设置频率                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年4月20日                                                                    
 *******************************************************************************/


void TIM2_Init(void)				   //用于采集ADC用的定时器
{
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	NVIC_InitTypeDef  NVIC_InitStructure; 
	 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//外设时钟使能  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;					
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级最高
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//子优先级
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = 10000-1;//计数器重装值
  	TIM_TimeBaseStructure.TIM_Prescaler = 4199;
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数方式
  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM2, (4200-1), TIM_PSCReloadMode_Immediate);//设置16位时钟分频系数,立即重载模式
	  TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
  	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//计满中断
  	TIM_Cmd(TIM2, DISABLE);
}

void OpenTIM2(void)
{
   TIM_Cmd(TIM2, ENABLE);
}

void CloseTIM2(void)
{
   TIM_Cmd(TIM2, DISABLE);
}
