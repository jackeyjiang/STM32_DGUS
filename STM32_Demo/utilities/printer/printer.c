#include"bsp.h"

 uint8_t PrintBuf[10];
 uint8_t PrintIndex;

 /*******************************************************************************
 * 函数名称:Uart1_Card                                                                      
 * 描    述:串口通信                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
void Uart1_Card(uint8_t *p,uint8_t sizeData)
{
      uint8_t i;		   
	  for(i=0; i<sizeData; i++)
	  {
        USART_SendData(USART1, p[i]);//串口1发送一个字符
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//等待发送完成
 	  }
}								   	
										
 unsigned char PrintInitCmd[]={0x1b,0x40}; //打印机初始化
 unsigned char SetEntryPrintCHinese[]={0x1c,0x26};	  //进入汉子打印模式
 const unsigned char SetExitPrintCHinese[]={0x1c,0x2e};	  //退出打印模式
 unsigned char PrintMessage0[]    ={" 深圳菲莫斯智能智能餐饮有限公司"};	  //退出打印模式
 unsigned char PrintMessage1[]    ={"菜品     数量     单价     金额"};	  //退出打印模式
 unsigned char PrintMessagename0[]={"胡萝卜炒蛋   数量   单价   金额"};	  //退出打印模式
 unsigned char PrintMessagename1[]={"香菇滑鸡     数量   单价   金额"};	  //退出打印模式
 unsigned char PrintMessagename2[]={"脆皮烤鸭     数量   单价   金额"};	  //退出打印模式
 unsigned char PrintMessagename3[]={"红烧鱼块     数量   单价   金额"};	  //退出打印模式
 unsigned char PrintMessage2[]    ={"刷卡：             现金：       "};	  //退出打印模式
 unsigned char PrintMessage3[]    ={"应收：             找回：       "};	  //退出打印模式
 unsigned char PrintMessage4[]    ={"刷卡：             应收：       "};	  //退出打印模式
 unsigned char PrintMessage5[]    ={"     谢谢惠顾，请保留电脑小票   "};	  //退出打印模式
 unsigned char PrintMessage6[]    ={"          欢迎下次惠顾          "};	  //退出打印模式
 unsigned char PrintMessage7[]    ={"   时间：2013.08.12 12:30       "};	  //退出打印模式
 unsigned char PrintMessage8[]    ={"   电话：0755-XXXXXXXXX         "};	  //退出打印模式


 /*******************************************************************************
 * 函数名称:CheckPrintStatus                                                                     
 * 描    述:检查打印机的状态是否有纸张                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:unsigned char                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
uint8_t CheckPrintStatus(void)
{
  uint8_t Cmd[]={0x10,0x04,0x04};
  uint8_t CurrentPoint = 0;
  switch(CurrentPoint)
  {
    case 0:
	  {			
			PrintIndex = 0;
	    Uart1_Card(Cmd,sizeof(Cmd));
	    CurrentPoint = 1;
			delay_ms(100);
	  }break;	  
	  case 1: 
	  {
	    switch(PrintBuf[0])
		  {
			  case 0x00 :	break ;
			  case 0x02 :	break ;
			  case 0x0c :	break ;
			  case 0x10 :	break ;
			  case 0x60 :	return 1;     //break ;  //表示没有纸了
				default: break;
		  }
    }  
    return 0;//表示正常
  }
	return 1;
}
 /*******************************************************************************
 * 函数名称:COPY                                                                    
 * 描    述:提取时间和钱的数据用来作为打印的数据                                                                  
 *                                                                               
 * 输    入: a , *p0 , *p1                                                                   
 * 输    出:无                                                                     
 * 返    回:无                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
void COPY(Struct_TD  a,unsigned char *p0,unsigned char *p1)
{										
	p0[5]=UserAct.PayShould/100+'0';
	p0[6]=UserAct.PayShould%100/10+'0';
	p0[7]=UserAct.PayShould%100%10+'0';

	p0[13]=Print_Struct.P_paymoney /100+'0';
	p0[14]=Print_Struct.P_paymoney %100/10+'0';
	p0[15]=Print_Struct.P_paymoney %100%10+'0';

	p0[21]=Print_Struct.P_MoneyBack/100+'0';
	p0[22]=Print_Struct.P_MoneyBack%100/10+'0';
	p0[23]=Print_Struct.P_MoneyBack%100%10+'0';
			
	p1[5]  ='2';
	p1[6] = '0';
	p1[7] = a.Year     / 10+'0';
	p1[8] = a.Year     % 10+'0';
	p1[10] = a.Month    / 10+'0';
	p1[11] = a.Month    % 10+'0';
	p1[13] = a.Date     / 10+'0';
	p1[14] = a.Date     % 10+'0';
	p1[16] = a.Hours    / 10+'0';
	p1[17] = a.Hours    % 10+'0';
	p1[19] = a.Minutes  / 10+'0';
	p1[20] = a.Minutes  % 10+'0';

}

 /*******************************************************************************
 * 函数名称:CheckPrintStatus                                                                     
 * 描    述:检查打印机的状态是否有纸张                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:unsigned char                                                               
 * 修改日期:2013年8月28日 


 菜品       数量  单价  单总价
胡萝卜炒肉    1    15     15
香菇滑鸡      1    20     20
红烧鱼块      1    25     25
脆皮烤鸭      1    30     30
付款方式：银行卡支付/深圳通支付/现金支付
应收:90  已收:100  找回:10
时间:2014-04-16 23:19

 *******************************************************************************/


 P_stuction Print_Struct;
void  SPRT(void)
{
  unsigned char   SendStc[3]={0x1d,0x56,0x01};		//切纸命令
	unsigned char  huan3[]={0x1b,0x64,0x06};
	unsigned char  p0[]={"应收:201已收:   找回:   \r\n"};
	unsigned char  p1[]={"时间:2013-12-15-12:30\r\n"};
	unsigned char  p2[]={"胡萝卜炒蛋1\t15\t015\r\n"};
	unsigned char  p3[]={"香菇滑鸡1\t20\t020\r\n"};
	unsigned char  p4[]={"脆皮烤鸭1\t25\t025\r\n"};
	unsigned char  p5[]={"红烧鱼块1\t30\t030\r\n"};

  printf("\r\n");//打印回车换行
  printf("菜品\t数量\t单价\t金额\r\n"); 	
  RTC_TimeShow();//得到当前的时间
	COPY(TimeDate,p0,p1);
  if(Print_Struct.P_Number1st>0)
  {	
		p2[10]= Print_Struct.P_Number1st%10 +'0'; 
		p2[15]=UserAct.MealCost_1st/100+'0';
		p2[16]=UserAct.MealCost_1st%100/10+'0';
		p2[17]=UserAct.MealCost_1st%100%10+'0';
		printf("%s",p2);
  }
	if(Print_Struct.P_Number2nd>0)
	{
		p3[8]= Print_Struct.P_Number2nd%10 +'0'; 
		p3[13]=UserAct.MealCost_2nd/100+'0';
		p3[14]=UserAct.MealCost_2nd%100/10+'0';
		p3[15]=UserAct.MealCost_2nd%100%10+'0';
		printf("%s",p3);
	}
  if(Print_Struct.P_Number3rd>0)
	{
		p4[8]= Print_Struct.P_Number3rd%10 +'0'; 
		p4[13]=UserAct.MealCost_3rd/100+'0';
		p4[14]=UserAct.MealCost_3rd%100/10+'0';
		p4[15]=UserAct.MealCost_3rd%100%10+'0';
		printf("%s",p4);
	}
	if(Print_Struct.P_Number4th>0)
	{		
		p5[8]= Print_Struct.P_Number4th%10 +'0'; 
		p5[13]=UserAct.MealCost_4th/100+'0';
		p5[14]=UserAct.MealCost_4th%100/10+'0';
		p5[15]=UserAct.MealCost_4th%100%10+'0';
		printf("%s",p5);
	} 
  printf("%s",p0);
	printf("%s",p1);
	if(UserAct.PayType == '2' )
	{
		 printf("支付方式：银行卡支付\r\n");
	}
  if(UserAct.PayType == '1')
	{
		 printf("支付方式：现金支付\r\n");
	}
	if(UserAct.PayType == '3')
	{
		printf("支付方式：深圳通支付\r\n");
	}
	Uart1_Card(huan3,sizeof(huan3)); 
	Uart1_Card(SendStc,sizeof(SendStc));//	切纸
}
