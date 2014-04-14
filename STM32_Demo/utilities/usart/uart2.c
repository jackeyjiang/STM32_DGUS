#include "stm32f4xx.h"
#include "szt_gpboc.h"
/*初始化串口2*/
void Uart2_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  
  /* Enable USART clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  /* Connect USART pins to AF7 */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  
  /* NVIC configuration */
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable USART */
  USART_Cmd(USART2, ENABLE);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_ClearFlag(USART2, USART_FLAG_TC);     // 清标志
}

  /*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : 深圳通、银联卡中断接收数据
* Input          : 无
* Output         : 无
* Return         : void
*修改时间：2014.2.14
*修改人  ： yao
*******************************************************************************/

  extern uint8_t PrintBuf2[640];
  extern uint32_t PrintIndex2;
  extern uint8_t  AckOver;
  extern uint8_t  DataOver;
  extern uint8_t  Usart2Flag;
void USART2_IRQHandler(void)
{
  	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)//数据接收扫描
	{
		USART_ClearITPendingBit(USART2,USART_FLAG_RXNE);//清接收标志
		PrintBuf2[PrintIndex2]	= USART_ReceiveData(USART2)  ;
		PrintIndex2++;
		//USART_SendData(USART2, USART_ReceiveData(USART2));//串口2发送一个字符
 	    //while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完成
		if( PrintIndex2 < 6)
		{
		  AckOver = RxNo;
		  Usart2Flag = AckFlag;
		}
		else if( PrintIndex2 == 6)
		{
		  AckOver = RxOk;
		}
		else
		{
		  Usart2Flag = DataFlag;
		}
		
		if( PrintIndex2 >= 11)
		{
		  if( PrintIndex2 >= (( PrintBuf2[9] <<8 ) + PrintBuf2[10] + 13 ) )
	 	  	{
	 	  	  DataOver = RxOk;
	 	  	}
		  else
		  {
		    DataOver = RxNo;
		  }
		} 
	}
}
