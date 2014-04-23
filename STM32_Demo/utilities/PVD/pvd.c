#include "stm32f4xx.h"
#include "PVD.h"

void PVD_Configuration(void) 
{ 	
  EXTI_InitTypeDef EXTI_InitStructure; 
  NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
//	EXTI_DeInit(); 											为什么我把这个不去掉了。就进不了外部中断了呢？。。
	EXTI_StructInit(&EXTI_InitStructure); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line16; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 	  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
	EXTI_Init(&EXTI_InitStructure);      // Configure EXTI Line16 to generate an interrupt 
	
	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  // Configure one bit for preemption priority 
 	NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);          // Enable the PVD Interrupt 
	EXTI_ClearITPendingBit(EXTI_Line16 );
	PWR_PVDLevelConfig(PWR_PVDLevel_5); 
  PWR_PVDCmd(ENABLE);
	
} 

/********************************
 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
中的初始化值，根据你的需要进行修改，具体细节如下：
    EXTI_Trigger_Rising --- 表示电压从高电压下降到低于设定的电压阀值产生中断；
    EXTI_Trigger_Falling --- 表示电压从低电压上升到高于设定的电压阀值产生中断；
    EXTI_Trigger_Rising_Falling --- 表示电压从高电压下降到低于设定的电压阀值、或从低电压上升到高于设定的电压阀值产生中断。
 

*********************************/

