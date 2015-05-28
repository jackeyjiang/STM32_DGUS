#include "stm32f4xx.h"
#include "protocol.h"
#include "uart6.h"
#include "string.h"

uint8_t Temperature=0;

void Uart6_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	
	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
	
	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART */
	USART_Cmd(USART6, ENABLE);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	USART_ClearFlag(USART6, USART_FLAG_TC);     // 清标志
}

 /*******************************************************************************
* Function Name	: UART6_IRQHandler
* Description	: 与机械手通迅接收数据
* Input			: 无
* Output 		: 无
* Return 		: void
*修改时间：2014.2.14
*修改人	： yao
*******************************************************************************/
uint8_t Usart6Buff[6];
uint8_t Usart6Index;

uint8_t Usart6DataFlag =0;  //此标志作为是否有数据接收，0无，1有数据，平时清0
uint8_t TemperSign ;
uint8_t Temperature;

 void USART6_IRQHandler(void)
 {
	if(USART_GetITStatus(USART6,USART_IT_RXNE)!=RESET)//数据接收扫描
	{
		USART_ClearITPendingBit(USART6,USART_FLAG_RXNE);//清接收标志
		Usart6Buff[Usart6Index]	= USART_ReceiveData(USART6)  ;
		Usart6Index++;
		if (Usart6Buff[0] ==0x06)  //ack
		{
			Usart6Buff[0] =0;
			Usart6Index =0;
				machinerec.reack =1;
				machinerec.renack = 0;
			}
			else if( Usart6Buff[0] ==0x15)   //nack
			{
				Usart6Buff[0] =0;
				Usart6Index =0;
				machinerec.reack =0;
				machinerec.renack = 1;
			}
			else
			{
				;
			}
			if(( Usart6Buff[Usart6Index-2] == 0x0d) && ( Usart6Buff[Usart6Index-1] == 0x0a)&& (Usart6Index >=2))  //后两个数为回车换行时
			{
// 				Usart6Buff[0] =0;
// 				Usart6Index =0;
				Usart6DataFlag =1;
				if(Usart6Index ==6)
				{
					SendAck();
				}
				else
				{
					SendNack();
				}
			}
		if( Usart6Index >6)
		{
			Usart6Index =0;
		}	
	} 
 }
