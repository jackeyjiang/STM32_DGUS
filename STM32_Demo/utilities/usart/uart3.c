#include "bsp.h"
 
void Uart3_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//外设时钟使能 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);//连接复用引脚  
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化串口1的GPIO  
	 
  	USART_InitStructure.USART_BaudRate = 115200;//波特率设置
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据模式
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
  	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件溢出控制
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//双工模式
		USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
  	USART_Init(USART3, &USART_InitStructure);	
    USART_ClearFlag(USART3, USART_FLAG_TC);//清传送完成标志
  	USART_Cmd(USART3, ENABLE);
  	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //嵌套优先级分组为1
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//嵌套通道为USART3_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//响应优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道中断使能 
	NVIC_Init(&NVIC_InitStructure);
}

void Uart3_Send(const uint8_t *p,uint8_t length)
{
   uint8_t i=0;
	 USART_ClearFlag(USART3,USART_FLAG_TC);
	 for(i=0;i<length;i++)
	 {	
		  //while((USART3->SR&0X40)==0);//循环发送,直到发送完毕    
	    USART_SendData(USART3, (u8) p[i]);   
    	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    	    
	 }
}
void Uart3_Sent(const char *p,uint8_t length)
{
   uint8_t i=0;
	 USART_ClearFlag(USART3,USART_FLAG_TC);
	 for(i=0;i<length;i++)
	 {	  
	    USART3->DR = (u8) p[i];       
    	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
    	    
	 }
}
  /*******************************************************************************
 * 函数名称:_LCD_Disable_RxInit                                                                     
 * 描    述:关闭LCD接受中断                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void _LCD_Disable_RxInit(void)
{
  	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
}
  /*******************************************************************************
 * 函数名称:_LCD_Enable_RxInit                                                                     
 * 描    述:打开LCD接受中断                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void _LCD_Enable_RxInit(void)
{
  	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
}



#define RX3BUF_SIZE 300				 /*接收FIFO的最大容量*/  
unsigned char RX3_BUFF[RX3BUF_SIZE];	 /*接收FIFO缓冲区数组*/
unsigned char UART_InpLen =0;        /*接收FIFO内待读取的字节数*/
unsigned int  RX_IndexR =0;          /*接受FIFO的读指针*/
unsigned int  RX_IndexW =0;          /*接受FIFO的读指针*/
 /*******************************************************************************
 * 函数名称: USART3_GetChar()
 * 描    述: 从串口读取一个字节数据（从缓冲队列读取1字节已接受的数据）
 *
 * 输    入: *chr：读取数据所存放的地址指针
 * 输    出: 返回1表示读取成功，返回0表示读取失败
 * 返    回: char
 * 修改日期: 2014年3月13日   
*******************************************************************************/
char USART3_GetChar(unsigned char *Chr)
{
	if(UART_InpLen==0)return(0);    //如果FIFO内无数据，返回0
	//_DINT();											//涉及FIFO操作时不允许中断，以免指针错乱
	UART_InpLen--;                  //待读取数据字节数减1
	*Chr =RX3_BUFF[RX_IndexR];			//待读取指针读取一个字节作为返回值
	if(++RX_IndexR>=RX3BUF_SIZE)    //读取指针递增，且判断是否下标越界
	{
		RX_IndexR =0;                 //如果越界则写指针归0(循环队列)
	}
	//_EINT();                      //FIFO操作完毕，恢复中断允许
	return(1); 											//返回成功标志	
}
 /*******************************************************************************
 * 函数名称: USART3_GetCharInRxBuf()
 * 描    述: 获取FIFO内已接受的数据字节数
 *
 * 输    入: 无
 * 输    出: 无
 * 返    回: FIFO内数据的字节数
 * 修改日期: 2014年3月13日 
*******************************************************************************/
unsigned int UART3_GetCharsInRxBuf(void)
{
	return(UART_InpLen);           //返回FIFO内数据的字节数
}

 /*******************************************************************************
 * 函数名称: UART0_ClrRxBuf()
 * 描    述: 清除接收FIFO区
 *
 * 输    入: 无
 * 输    出: 无
 * 返    回: 无
 * 修改日期: 2014年3月13日 
*******************************************************************************/
void UART3_ClrRxBuf(void)
{
	//_DINT();
	memset(RX3_BUFF,0,sizeof(RX3_BUFF));
	//_EINT();

}



 /*******************************************************************************
 * 函数名称: USART3_IRQHandler
 * 描    述: 中断触发将数据压入缓存
 *           简单的数据帧接受
 * 输    入: 无
 * 输    出: 无
 * 返    回: 无
 * 修改日期: 2014年3月13日 
*******************************************************************************/
void USART3_IRQHandler(void)
{	
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//数据接收扫描
	{
  		
	UART_InpLen++;
	RX3_BUFF[RX_IndexW]=USART_ReceiveData(USART3);
	if(++RX_IndexW >=RX3BUF_SIZE)
	{
		RX_IndexW =0;
	}
	}
}
