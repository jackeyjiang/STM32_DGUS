#include "stm32f4xx.h"										 
#include "bsp.h"

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
}
 /*******************************************************************************
 * 函数名称:DisableBills                                                                     
 * 描    述:设置不接受钱                                                               
 *           设置接收到暂存器，在程序处理过程中选择进钱或退钱                                                                     
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void DisableBills(void)
{
  unsigned char bufCmd[5] = {0x34, 0x00, 0x00, 0x00, 0x00};
	uint8_t i;
	for(i = 0; i < 5; i++)
	{	
	    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成
	    USART_SendData(UART4, bufCmd[i]);//串口1发送一个字符	
	}
}

/*******************************************************************************/
#define Normal 1 //纸币机初始化成功	30 06 09
#define Abnormal 2  //纸币机处于关闭状态 30 09
#define Erro 3     //纸币机返回的是0xff
#define BillInEscrow 4 //钱在暂存器中

#define		BillStacked  1
#define		EscrowRequest  2
#define		BillReturned  3
#define		DisabledBillRejected  4	

char BillStatus= 0; //纸币机状态
char BillRunStatus;	  //纸币机运行状态
uint8_t  ReadBill(void)
{ 
  uint8_t	BillValue=0;
  if(BillDataBuffer[0]==0x30) //money form cash macthin 
	{
	  if(BillDataBuffer[1]&0x80)	//cash was valuable
		{
			switch(BillDataBuffer[1]&0x70)    //cash matchine status
			{
				case 0x00: BillStatus =BillStacked;break;
				case 0x10: BillStatus =EscrowRequest;break;
				case 0x20: BillStatus =BillReturned;break;
				case 0x40: BillStatus =DisabledBillRejected;break;
				default:break ;
				//return BillValue; //
			}
			if(BillStatus ==BillStacked) //正常接受纸币
			{
			  switch(BillDataBuffer[1]&0x0F)
			  {
				//case 0x00: BillValue = 1;break; /*不收1元*/
				  case 0x01: BillValue = 5;break;
				  case 0x02: BillValue = 10;break;
				  case 0x03: BillValue = 20;break;
				  default:break ;
			  }
				memset(BillDataBuffer,0,sizeof(BillDataBuffer));
			  return BillValue;
		  }
			else if(BillStatus ==EscrowRequest)
			{
 			   switch(BillDataBuffer[1]&0x0F)
				{
				   case 0x00: BillValue = 1;break;
				   case 0x01: BillValue = 5;break;
				   case 0x02: BillValue = 10;break;
				   case 0x03: BillValue = 20;break;
				   default:break ;
				}
			/*在暂存器中的钱币*/
			BillRunStatus =BillInEscrow;            
      }
			else if(BillStatus ==DisabledBillRejected)
			{
				AbnormalHandle(billset_erro);
			}
	  }
	  else if(BillDataBuffer[1]==0x06)
	  {			
		   if(BillDataBuffer[2]==0x09) 
       BillRunStatus = Normal;
	  }
    else if(BillDataBuffer[1]==0x09)
	  {
		  BillRunStatus = Abnormal;
	  }
		return 0 ;
	}
	else if(BillDataBuffer[0]==0xFF)
	{ 
		return nack_flag;
  }
  else if(BillDataBuffer[0]==0x00)
	{
		return ack_flag;
	}
   return 0 ;	
}


   /*******************************************************************************
 * 函数名称:Polls                                                                     
 * 描    述:把钱放进钱箱                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日  
 *******************************************************************************/

void  Polls(void)
{
//      char i;
//      char Polls_Cmd[2]={0x34,0x01};	
// 	 for(i = 0; i < 2; i++)
// 	 {
// 	    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成
//         USART_SendData(UART4, Polls_Cmd[i]);//串口1发送一个字符
// 	 }
}

    /*******************************************************************************
 * 函数名称:Polls                                                                     
 * 描    述:把钱退回给用户                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日  
 *******************************************************************************/

void  BackPolls(void)
{
//      char i;
//      char Polls_Cmd[2]={0x34,0x00};	
// 	 for(i = 0; i < 2; i++)
// 	 {
// 	    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成
//         USART_SendData(UART4, Polls_Cmd[i]);//串口1发送一个字符
// 	 }
}
  /*******************************************************************************
 * 函数名称:ReadBills                                                                     
 * 描    述:设置接受10  20 5  的钱                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日  
 *******************************************************************************/
unsigned char  Rev_Money_Flag ; 
uint8_t  ReadBills(void)
{
  uint8_t temp = 0;
 	temp=ReadBill();
	memset(BillDataBuffer,0,sizeof(BillDataBuffer));
/*如果能锁存最好，因为是公共数据，但是如果只在中断进行改变，不影响数
  据的完整性，中断会打断程序的运行，不会同时操作 */	                          
	switch(temp)
	{
	   case 0:
	           return 0 ;
	   		 
	   case 1  :	  //1元
	   case 5  : 	  //5元
	   case 10 :      //10元
	   case 20 :      //20元		
	              //Polls(); //进钱箱
		  Rev_Money_Flag = 1 ;
	    return(temp);
  	  default : break;
	}
   return 0 ;
}

  /*******************************************************************************
 * 函数名称:StringToHexGroup                                                                    
 * 描    述:将字符数组转换为hex数组,功能有所修改                                                                  
 *                                                                               
 * 输    入:                                                                   
 * 输    出:bool                                                                      
 * 返    回:void                                                               
 * 修改日期:2013年8月28日  
 *******************************************************************************/
bool StringToHexGroup(unsigned char *OutHexBuffer, char *InStrBuffer, unsigned int strLength)
{
  unsigned int i, k=0;
  unsigned char HByte,LByte;
	if(InStrBuffer[strLength-1]!=0x0A)
		return false;
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

