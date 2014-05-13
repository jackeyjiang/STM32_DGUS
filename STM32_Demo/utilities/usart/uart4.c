#include "stm32f4xx.h" 
#include "string.h"
#include "bill.h"
void Uart4_Configuration(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//外设时钟使能 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);//连接复用引脚  
  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化串口1的GPIO   
  	USART_InitStructure.USART_BaudRate = 9600;//波特率设置
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据模式
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
  	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件溢出控制
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//双工模式
	  USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);
  	USART_Init(UART4, &USART_InitStructure);
  	USART_Cmd(UART4, ENABLE);
  	USART_ClearFlag(UART4, USART_FLAG_TC);//清传送完成标志
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //嵌套优先级分组为1
	  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//嵌套通道为USART6_IRQn
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级为0
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//响应优先级为0
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道中断使能 
	  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
 * 函数名称:UART4_IRQHandler                                                                     
 * 描    述:中断程序 用于得到纸币机的数据                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
char         RX4Buffer[40]={0};	  /*串口4接受数据缓冲区 */
uint8_t 	   rx4DataLen =0 ;  /*串口4接受数据个数*/
uint8_t      rx4ack;		      /*接受数据应答*/
uint8_t      rx4Check ;		    /*接受数据校验位*/
unsigned char  BillDataBuffer[20]={0}; //作为转换完成的数组
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)//数据接收扫描
	{  
		USART_ClearITPendingBit(UART4,USART_FLAG_RXNE);//清接收标志
    RX4Buffer[rx4DataLen]=UART4->DR;
		if(RX4Buffer[rx4DataLen++]==0x0A) //当接收到LF时对数组进行锁止
		{
		   if(StringToHexGroup(BillDataBuffer, RX4Buffer, rx4DataLen)) //转换数组
		   {
		     rx4Check  = 1 ;  // 需要做标记
         rx4DataLen= 0 ;  //
		   }
		   else
		   {
          memset(RX4Buffer,0,rx4DataLen);				 
		      rx4DataLen= 0 ; 
		   }
		}
	}  
}

