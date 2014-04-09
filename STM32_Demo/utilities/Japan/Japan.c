#include "stm32f4xx.h"
#include "bsp.h"
#include "japan.h"
uint8_t      RX4Buffer[40];	  /*串口4接受数据缓冲区 */
uint8_t 	   rx4DataLen =0 ;    /*串口4接受数据个数*/
uint8_t      rx4ack;		  /*接受数据应答*/
uint8_t      rx4Check ;		  /*接受数据校验位*/


unsigned char  Rev_Money_Flag ; 
 /*******************************************************************************
 * 函数名称:mem_set_0                                                                      
 * 描    述:将缓冲数据清零                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
static void mem_set_0(unsigned char *dest, const long s_len)
{
    unsigned char j0;

    for(j0 = 0; j0 < s_len; j0++)  dest[j0] = 0;
}
 /*******************************************************************************
 * 函数名称:Uart4_Card                                                                      
 * 描    述:串口通信                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
void Uart4SendData(uint8_t *p,uint8_t sizeData)
{
      uint8_t i;		   
	  for(i=0; i<sizeData; i++)
	  {
	    printf("p[%d]=%x\r\n",i, p[i]);
      USART_SendData(UART4, p[i]);//串口1发送一个字符
		  while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成
 	  }
}
 /*******************************************************************************
 * 函数名称:SetJapanBills                                                                     
 * 描    述:设置接受10  20 5  的钱                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void SetJapanBills(void)
{
    unsigned char bufCmd[6] = {0x34, 0x00, 0x1f, 0x00, 0x00};
	uint8_t i;
	for(i = 0; i < 5; i++)
	{
	    USART_SendData(UART4, bufCmd[i]);//串口1发送一个字符
		while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成
	}
}
 /*******************************************************************************
 * 函数名称:ack                                                                     
 * 描    述:校验从机是否发送了ACK                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
unsigned char  ACKFun(void)
{
   unsigned char   WaitOut = 0xff ;
   do
   {	
      WaitOut --;
	 if(WaitOut == 0x00) 
	  break;
	  delay_us(1);
   } while(!rx4ack);
   if(WaitOut == 0x00)
   return 1;
   rx4ack = 0 ;
   return 0 ;
}

unsigned char  Check(void)
{
   unsigned char   WaitOut = 0xff ;
   do
   {   WaitOut --;
	 if(WaitOut == 0x00) 
	  break;
	  delay_ms(1);
   } while(!rx4Check);
   if(WaitOut == 0x00)
   return 1;
   rx4Check = 0 ;
   return 0 ;
}
 /*******************************************************************************
 * 函数名称:Polls                                                                     
 * 描    述:发送命令让从机收钱                                                                
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void  PollFun(void)
{
   
	USART_SendData(UART4, 0x01);//串口1发送一个字符
	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//等待发送完成 
}
void   BackBill(void)
{
 
}
  /*******************************************************************************
 * 函数名称:ReadBills                                                                     
 * 描    述:设置接受10  20 5  的钱                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日  
 00 20 A0 03 90 09 5C    //接受1元
 00 20 A0 03 91 09 5D 	 //接受5元
 00 20 A0 03 92 09 5E    //接受10元
 00 20 A0 03 93 09 5F    //接受20元  

 00 20 A0 02 09 CB        //系统忙不可以 进入投币
 00 20 A0 02 0A CC        //没有钱进钱箱。
 00 20 A0 02 0B CD        //钱没有被投入进去                                                           
 *******************************************************************************/
#define Normal 1 //纸币机初始化成功	30 06 09
#define Abnormal 2  //纸币机处于关闭状态 30 09
#define Erro 3     //纸币机返回的是0xff
#define BillInEscrow 4 //钱在暂存器中

#define		BillStacked  1
#define		EscrowRequest  2
#define		BillReturned  3
#define		DisabledBillRejected  4	

char BillStatus=0; //纸币机状态
char BillRunStatus;	  //纸币机运行状态
extern unsigned char BillDataBuffer[20];
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
			if(BillStatus ==EscrowRequest)
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
		if(CmdNum==1)SetBills(); 
		if(CmdNum==2)DisableBills();
    delay_ms(100);	
  }
	 memset(BillDataBuffer,0,sizeof(BillDataBuffer));
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
uint8_t  ReadBills(void)
{
    uint8_t CurrentPoint = 0;
	uint8_t CoinsCnt=0;/*硬币计数*/
 	CurrentPoint=ReadBill();
/*如果能锁存最好，因为是公共数据，但是如果只在中断进行改变，不影响数
  据的完整性，中断会打断程序的运行，不会同时操作 */	                          
	//CoinsCnt=UserAct.PayForCoins; /*将硬币数据提出*/
	//UserAct.PayForCoins=0;       /*清零硬币数*/
	switch(CurrentPoint)
	{
	   case 0:
	           return 0 ;
	   		 
	   case 1  :	  //1元
	   case 5  : 	  //5元
	   case 10 :      //10元
	   case 20 :      //20元		//进钱箱
	              //Polls();
				  Rev_Money_Flag = 1 ;
	              return(CurrentPoint);
  	   default : break;
	}
   return 0 ;
}





