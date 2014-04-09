#include "stm32f4xx.h"

void Uart5_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//外设时钟使能 
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//外设时钟使能 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
  	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);//连接复用引脚  
  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2  ;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化串口1的GPIO  
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化串口1的GPIO  
		 
  	USART_InitStructure.USART_BaudRate = 115200;//波特率设置
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据模式
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
  	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件溢出控制
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//双工模式
	  USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
  	USART_Init(UART5, &USART_InitStructure);	
    USART_ClearFlag(UART5, USART_FLAG_TC);//清传送完成标志
    USART_Cmd(UART5, ENABLE);
  	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //嵌套优先级分组为1
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//嵌套通道为USART6_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;//响应优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道中断使能 
	NVIC_Init(&NVIC_InitStructure);
}





 void Uart5Send(unsigned char *p,unsigned char lenght)
{
     uint8_t i=0;
	 for(i=0;i<lenght;i++)
	 {
	  
	    UART5->DR = (u8) p[i];       
    	while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
    	    
	}
}	

     /*******************************************************************************
 * 函数名称:UART5_IRQHandler                                                                     
 * 描    述:中断程序 用于与服务器之间的数据交换                                                                
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
unsigned char   F_RX1_Right ;
uint8_t  rx1Buf[512];
uint16_t  rx1BufIndex;
uint8_t  F_RX1_SYNC;	  //同步 tou 。。就是头头
uint8_t  F_RX1_VAILD;    //收到有效数据
long     rx1DataLen;  
long     CrcValue;
void UART5_IRQHandler(void)
{
	unsigned char res = 0;
	unsigned char j =0 ;
	#define DEF_RX1_Head   0x02
	#define DEF_RX1_End    0x03	
	if(USART_GetITStatus(UART5,USART_IT_RXNE)!=RESET)//数据接收扫描
	{	
	  USART_ClearITPendingBit(UART5,USART_FLAG_RXNE);//清接收标志
      res=UART5->DR;
	  if(!F_RX1_SYNC)
	  {		

		if(res ==DEF_RX1_Head )//判断是否是串口帧头
		{
			F_RX1_SYNC = 1;	 //接受同步标志
			rx1BufIndex = 0; //缓冲区个数清零
			F_RX1_VAILD = 1; //收到有效数据
			CrcValue = 0;
			F_RX1_Right = 0 ;
		}
		
  	 }
 	else 
  	{	
	   rx1Buf[rx1BufIndex]=res;
	   if(rx1BufIndex==0x04)
	    {
	        rx1DataLen   = rx1Buf[2] << 8 | rx1Buf[3] ; 	 /*接受数据长度*/	
	    }
	   if(F_RX1_VAILD == 0x01)/*接受的是数据*/
	    {
		    if((rx1BufIndex == rx1DataLen + 3)&&(rx1DataLen != 0x00)) /*判断接受的数据长度是否对应，并接受的数据不为0*/
		     {
			     F_RX1_VAILD = 0;  /*???标志一个接受的数据是正确的*/
			 }
		    /*校验数据*/
		    if(1){
	   		        CrcValue ^= res;
				    for(j=0;j<8;j++)
				    {
				        if(CrcValue&0x0001)
						    { 
						    	CrcValue >>=1 ;
								CrcValue ^=0x08408 ;
			
							}
						else 
								CrcValue >>=1 ;
				   }
		    	}	 
		}
		if(rx1BufIndex == rx1DataLen + 6) /*?数组接收完*/
		  {	 
		  	/*?判断结束，判断CRC是否正确*/	
		     if(rx1Buf[rx1DataLen +4] == DEF_RX1_End && (rx1Buf[rx1DataLen +5]<<8| rx1Buf[rx1DataLen +6]) == CrcValue)
			  {
			     F_RX1_SYNC = 0;
				 rx1BufIndex = 0;
				 F_RX1_VAILD = 1;
				 CrcValue = 0;
				 F_RX1_Right = 1 ;
				 return ;
			  }
		  }
		 rx1BufIndex++;
    }
	}  
	

}
