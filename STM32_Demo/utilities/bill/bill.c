#include "stm32f4xx.h"										 
#include "bsp.h"

unsigned char CmdNum=0;

void InitBills(void)
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
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级为0
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//响应优先级为0
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道中断使能 
	  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : Uart4_Bill
* Description    : 串口4发送函数
* Input          : 数组以及大小
* Output         : void
* Return         : void
*******************************************************************************/
void Uart4Send(uint8_t *p,uint8_t sizeData)
{
  uint8_t i=0;		   
	for(i=0;i<sizeData;i++)
	{
	  printf("p[%d]=%x\r\n",i,p[i]);
    USART_SendData(UART4, p[i]);//串口1发送一个字符
		while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成
 	}
}

void Uart4_Bill(uint8_t *p,uint8_t sizeData)
{
  uint8_t i;  
	for(i=0; i<sizeData; i++)
	{									
    USART_SendData(UART4, p[i]);//串口1发送一个字符
		while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成
 	}
}

    /*******************************************************************************
* Function Name  : GetCrc16
* Description    : 进行CRC校验
* Input          : CRC
* Output         : 数组以及大小
* Return         : CRC
*******************************************************************************/
static unsigned int GetCrc16Fun(unsigned char *bufData,unsigned int sizeData)
 {
  	 unsigned int Crc ,i = 0;
	 unsigned char j = 0;
	 if(sizeData == 0)
	 return 1 ;

	 Crc = 0 ;
	 for(i=0;i<sizeData;i++)
	 {
	   Crc ^= bufData[i];
	   for(j=0;j<8;j++)
	   {
	    if(Crc&0x0001)
		{ 

		Crc >>=1 ;
		Crc ^=0x08408 ;

		}
		else 
		Crc >>=1 ;

	   }
	 } 
	 return Crc ; 
 }
 
 /*******************************************************************************
 * 函数名称:SetBills                                                                     
 * 描    述:设置接受10  20 5  的钱                                                                  
 *           设置接收到暂存器，在程序处理过程中选择进钱或退钱                                                                     
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void SetBills(void)
{
    unsigned char bufCmd[5] = {0x34, 0x00, 0x1e, 0x00, 0x00};
	uint8_t i;
	for(i = 0; i < 5; i++)
	{	
	    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成
	    USART_SendData(UART4, bufCmd[i]);//串口1发送一个字符	
	}
	CmdNum=1;
}
void DisableBills(void)
{
    unsigned char bufCmd[5] = {0x34, 0x00, 0x00, 0x00, 0x00};
	uint8_t i;
	for(i = 0; i < 5; i++)
	{	
	    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成
	    USART_SendData(UART4, bufCmd[i]);//串口1发送一个字符	
	}
    CmdNum=2;	
}

bool StringToHexGroup(unsigned char *OutHexBuffer, char *InStrBuffer, unsigned int strLength)
{
  unsigned int i, k=0;
  unsigned char HByte,LByte;
	if(InStrBuffer[strLength-1]!=0x0A)
		return false;
//   if(strLength%2 !=0) //判断末尾是否为LF,CR 无就return
//     return FALSE;
 
  for(i=0; i<strLength; i=i+3)
  {
    if(InStrBuffer[i]>='0' && InStrBuffer[i]<='9')
    {
      HByte=InStrBuffer[i]-'0';
    }
    else if(InStrBuffer[i]>='A' && InStrBuffer[i]<='F')
    {
      HByte=InStrBuffer[i]-'A' +10;
    }
    else
    {
      HByte=InStrBuffer[i];
      return false;
    }
    HByte=HByte <<4;
    HByte = HByte & 0xF0;
    if(InStrBuffer[i+1]>='0' && InStrBuffer[i+1]<='9')
    {
       LByte=InStrBuffer[i+1]-'0';
    }
    else if(InStrBuffer[i+1]>='A' && InStrBuffer[i+1]<='F')
    {
       LByte=InStrBuffer[i+1]-'A' +10;
    }
    else
    {
       LByte=InStrBuffer[i];
       return false;
    }
		if(InStrBuffer[i+2]==0x20)
		{			
			 OutHexBuffer[k++]=HByte |LByte;
		}
		if(InStrBuffer[i+2]==0x0D)
		{
			OutHexBuffer[k++]=HByte |LByte;
			return true ;
		}
		
 }
 return true;
}

