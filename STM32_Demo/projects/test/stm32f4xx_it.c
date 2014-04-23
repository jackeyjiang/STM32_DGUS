/**
  ******************************************************************************
  * @file    ADC/ADC3_DMA/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "bsp.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_ADC3_DMA
  * @{
  */  

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern int ntime;
void SysTick_Handler(void)
{
	ntime--;	
}


void RCC_IRQHandler(void)
{
  if(RCC_GetITStatus(RCC_IT_HSERDY) != RESET)
  { 
    /* Clear HSERDY interrupt pending bit */
    RCC_ClearITPendingBit(RCC_IT_HSERDY);

    /* Check if the HSE clock is still available */
    if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)
    { 
      /* Enable PLL: once the PLL is ready the PLLRDY interrupt is generated */ 
      RCC_PLLCmd(ENABLE);     
    }
  }

  if(RCC_GetITStatus(RCC_IT_PLLRDY) != RESET)
  { 
    /* Clear PLLRDY interrupt pending bit */
    RCC_ClearITPendingBit(RCC_IT_PLLRDY);

    /* Check if the PLL is still locked */
    if (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET)
    { 
      /* Select PLL as system clock source */
      RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    }
  }
}
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/


/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
   /*******************************************************************************
 * 函数名称:TIM2_IRQHandler                                                                     
 * 描    述:用于定时                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 

unsigned char TimerFlag = 0 ;
void TIM2_IRQHandler(void)
{	    
  if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
	   TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
	   TimerFlag ++ ;
  }
}

   /*******************************************************************************
 * 函数名称:EXTI15_10_IRQHandler                                                                     
 * 描    述:投币机中断线接收                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
unsigned int   CoinsCount=0;
unsigned int   CoinsOutFlat=1;
extern unsigned char NewCoinsCnt;
void EXTI15_10_IRQHandler(void)
{
  unsigned char PinStatus;
  if (EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
	  PinStatus=GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_12);
		if(PinStatus == 0)
		{
		  CoinsCount++;
		  if(CoinsCount == 2)
			{
				CoinsCount = 0;
				UserAct.PayForCoins++;
				UserAct.PayAlready++;
				CoinsTotoalMessageWriteToFlash.CoinTotoal--;
			}
		}
    EXTI_ClearITPendingBit(EXTI_Line12);
	}
	
	if( EXTI_GetITStatus(EXTI_Line10) != RESET)	
	{
		NewCoinsCnt++; //新的硬币机接收个数
		Coins_cnt++; 
		CoinsTotoalMessageWriteToFlash.CoinTotoal++;
	  EXTI_ClearITPendingBit(EXTI_Line10);	
	}
	WriteCoins();
}

  /*******************************************************************************
 * 函数名称:TIM3_IRQHandler                                                                     
 * 描    述:中断程序  用于倒计时60秒。时间到返回主界面                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{	 
	  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if(WaitTime > 0)
		{
			WaitTime--;
		}  
		else 
		{
			CloseTIM3();
			ClearUserBuffer();//清空用户数据
	  }	     
  }
}

  /*******************************************************************************
 * 函数名称:TIM4_IRQHandler                                                                     
 * 描    述:打印倒计时5S                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void TIM4_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{	 
	  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(WaitTime > 0)
		{
			WaitTime--;
		}  
		else 
		{
			CloseTIM4();
			PageChange(Mealout_interface);//超时退出进入餐品数量选择界面
	  }	     
  }
}
 /*******************************************************************************
 * 函数名称:TIM5_IRQHandler                                                                     
 * 描    述:中断程序  ,用于与机械手通迅计时                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:                                                                   
 *******************************************************************************/
//  uint8_t LinkTime; 
void TIM5_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)
	{	 
	  TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		LinkTime++;   
  }
}

   /*******************************************************************************
 * 函数名称:TIM7_IRQHandler                                                                     
 * 描    述:购物车倒计时                                                                
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void TIM7_IRQHandler(void)
{			
  if(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
	{	 
	  TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
		if(WaitTime > 0)
		{
			WaitTime--;
		}  
		else 
		{
			CloseTIM7();
	  }	     
  }
}
   /*******************************************************************************
 * 函数名称:PVD_IRQHandler                                                                    
 * 描    述:掉电检测                                                        
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
extern  FIL fsrc;
void PVD_IRQHandler(void) 
{
  EXTI_ClearITPendingBit(EXTI_Line16); 
  if(PWR_GetFlagStatus(PWR_FLAG_PVDO)) //
 {
     PWR_ClearFlag(PWR_FLAG_PVDO);
     f_close(&fsrc);	   //低电压检测    
 }	

}



/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
