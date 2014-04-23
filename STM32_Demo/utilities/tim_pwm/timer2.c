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
  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);

  /* GPIOC clock enable */
  
  /* --------------------------NVIC Configuration -------------------------------*/
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* ------------------- TIM1 Configuration:Output Compare Timing Mode ---------*/
    
   

  /*分频和周期计算公式：
  Prescaler = (TIMxCLK / TIMx counter clock) - 1;
  Period = (TIMx counter clock / TIM3 output clock) - 1 
  TIMx counter clock为你所需要的TXM的定时器时钟 
  */


  TIM_DeInit(TIM2);

  TIM_TimeBaseStructure.TIM_Period =10000-1  ;	
  TIM_TimeBaseStructure.TIM_ClockDivision = 1;
  TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;;					  //
//  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM2, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, DISABLE);
//   TIM_Cmd(TIM2, ENABLE);
  }



