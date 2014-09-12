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
	p0[5]=UserActMessageWriteToFlash.UserAct.PayShould/100+'0';
	p0[6]=UserActMessageWriteToFlash.UserAct.PayShould%100/10+'0';
	p0[7]=UserActMessageWriteToFlash.UserAct.PayShould%100%10+'0';

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
	p1[22] = a.Senconds / 10+'0';
  p1[23] = a.Senconds % 10+'0';
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
	unsigned char  p1[]={"时间:2013-12-15-12:30:00\r\n"};
	unsigned char  p2[]={"秘制猪手饭  1\t016\t016\r\n"};
	unsigned char  p3[]={"潮氏卤鸡腿饭1\t016\t016\r\n"};
	unsigned char  p4[]={"特色稻香肉饭1\t016\t016\r\n"};
	unsigned char  p5[]={"黑椒猪扒饭  1\t016\t016\r\n"};
  unsigned char  p6[]={"蒲烧鲷鱼饭  1\t018\t018\r\n"};
  unsigned char  p7[]={"蒲烧秋刀鱼饭1\t018\t018\r\n"};
  unsigned char  p8[]={"咖喱鸡扒饭  1\t016\t016\r\n"};
  unsigned char  p9[]={"梅菜扣肉饭  1\t016\t016\r\n"};
  //printf("@\r\n");//打印回车换行
  //Uart1_Card(huan3,sizeof(huan3)); 
  printf("菜品\t   数量\t单价\t金额\r\n"); 	
  RTC_TimeShow();//得到当前的时间
	COPY(TimeDate,p0,p1);
  if(Print_Struct.P_Number1st>0)
  {	
		p2[12]= Print_Struct.P_Number1st%10 +'0'; 
		p2[14]= (price_1st*10/Discount)/100+'0';
		p2[15]= (price_1st*10/Discount)%100/10+'0';
		p2[16]= (price_1st*10/Discount)%100%10+'0';
		p2[18]= (UserActMessageWriteToFlash.UserAct.MealCost_1st*10/Discount)/100+'0';
		p2[19]= (UserActMessageWriteToFlash.UserAct.MealCost_1st*10/Discount)%100/10+'0';
		p2[20]= (UserActMessageWriteToFlash.UserAct.MealCost_1st*10/Discount)%100%10+'0';  
		printf("%s",p2);
  }
	if(Print_Struct.P_Number2nd>0)
	{
		p3[12]= Print_Struct.P_Number2nd%10 +'0'; 
		p3[14]= (price_2nd*10/Discount)/100+'0';
		p3[15]= (price_2nd*10/Discount)%100/10+'0';
		p3[16]= (price_2nd*10/Discount)%100%10+'0';
		p3[18]= (UserActMessageWriteToFlash.UserAct.MealCost_2nd*10/Discount)/100+'0';
		p3[19]= (UserActMessageWriteToFlash.UserAct.MealCost_2nd*10/Discount)%100/10+'0';
		p3[20]= (UserActMessageWriteToFlash.UserAct.MealCost_2nd*10/Discount)%100%10+'0';
		printf("%s",p3);
	}
  if(Print_Struct.P_Number3rd>0)
	{
		p4[12]= Print_Struct.P_Number3rd%10 +'0'; 
		p4[14]= (price_3rd*10/Discount)/100+'0';
		p4[15]= (price_3rd*10/Discount)%100/10+'0';
		p4[16]= (price_3rd*10/Discount)%100%10+'0';
		p4[18]= (UserActMessageWriteToFlash.UserAct.MealCost_3rd*10/Discount)/100+'0';
		p4[19]= (UserActMessageWriteToFlash.UserAct.MealCost_3rd*10/Discount)%100/10+'0';
		p4[20]= (UserActMessageWriteToFlash.UserAct.MealCost_3rd*10/Discount)%100%10+'0';
		printf("%s",p4);
	}
	if(Print_Struct.P_Number4th>0)
	{		
		p5[12]= Print_Struct.P_Number4th%10 +'0'; 
		p5[14]= (price_4th*10/Discount)/100+'0';
		p5[15]= (price_4th*10/Discount)%100/10+'0';
		p5[16]= (price_4th*10/Discount)%100%10+'0';
		p5[18]= (UserActMessageWriteToFlash.UserAct.MealCost_4th*10/Discount)/100+'0';
		p5[19]= (UserActMessageWriteToFlash.UserAct.MealCost_4th*10/Discount)%100/10+'0';
		p5[20]= (UserActMessageWriteToFlash.UserAct.MealCost_4th*10/Discount)%100%10+'0';
		printf("%s",p5);
	} 
	if(Print_Struct.P_Number5th>0)
	{		
		p6[12]= Print_Struct.P_Number5th%10 +'0'; 
		p6[14]= (price_5th*10/Discount)/100+'0';
		p6[15]= (price_5th*10/Discount)%100/10+'0';
		p6[16]= (price_5th*10/Discount)%100%10+'0';
    p6[18]= (UserActMessageWriteToFlash.UserAct.MealCost_5th*10/Discount)/100+'0';
		p6[19]= (UserActMessageWriteToFlash.UserAct.MealCost_5th*10/Discount)%100/10+'0';
		p6[20]= (UserActMessageWriteToFlash.UserAct.MealCost_5th*10/Discount)%100%10+'0';
		printf("%s",p6);
	} 
	if(Print_Struct.P_Number6th>0)
	{		
		p7[12]= Print_Struct.P_Number6th%10 +'0'; 
		p7[14]= (price_6th*10/Discount)/100+'0';
		p7[15]= (price_6th*10/Discount)%100/10+'0';
		p7[16]= (price_6th*10/Discount)%100%10+'0';
    p7[18]= (UserActMessageWriteToFlash.UserAct.MealCost_6th*10/Discount)/100+'0';
		p7[19]= (UserActMessageWriteToFlash.UserAct.MealCost_6th*10/Discount)%100/10+'0';
		p7[20]= (UserActMessageWriteToFlash.UserAct.MealCost_6th*10/Discount)%100%10+'0';
		printf("%s",p7);
	}
	if(Print_Struct.P_Number7th>0)
	{		
		p8[12]= Print_Struct.P_Number7th%10 +'0'; 
		p8[14]= (price_7th*10/Discount)/100+'0';
		p8[15]= (price_7th*10/Discount)%100/10+'0';
		p8[16]= (price_7th*10/Discount)%100%10+'0';
    p8[18]= (UserActMessageWriteToFlash.UserAct.MealCost_7th*10/Discount)/100+'0';
		p8[19]= (UserActMessageWriteToFlash.UserAct.MealCost_7th*10/Discount)%100/10+'0';
		p8[20]= (UserActMessageWriteToFlash.UserAct.MealCost_7th*10/Discount)%100%10+'0';
		printf("%s",p8);
	}
	if(Print_Struct.P_Number8th>0)
	{		
		p9[12]= Print_Struct.P_Number8th%10 +'0'; 
		p9[14]= (price_8th*10/Discount)/100+'0';
		p9[15]= (price_8th*10/Discount)%100/10+'0';
		p9[16]= (price_8th*10/Discount)%100%10+'0';
    p9[18]= (UserActMessageWriteToFlash.UserAct.MealCost_8th*10/Discount)/100+'0';
		p9[19]= (UserActMessageWriteToFlash.UserAct.MealCost_8th*10/Discount)%100/10+'0';
		p9[20]= (UserActMessageWriteToFlash.UserAct.MealCost_8th*10/Discount)%100%10+'0';
		printf("%s",p9);
	}  
  printf("%s",p0);
	printf("%s",p1);
  printf("折扣信息：五折\r\n");
	if(UserActMessageWriteToFlash.UserAct.PayType == '2' )
	{
		 printf("支付方式：银行卡支付\r\n");
	}
  if(UserActMessageWriteToFlash.UserAct.PayType == '1')
	{
		 printf("支付方式：现金支付\r\n");
	}
	if(UserActMessageWriteToFlash.UserAct.PayType == '3')
	{
		printf("支付方式：深圳通支付\r\n");
	}
  printf("服务热线：400-0755-677");
	Uart1_Card(huan3,sizeof(huan3)); 
	Uart1_Card(huan3,sizeof(huan3)); 
	Uart1_Card(SendStc,sizeof(SendStc));//	切纸
	Uart1_Card(huan3,sizeof(huan3));  
  printf("深圳市速来食餐饮管理有限公司\r\n");
  printf("\r\n");//切纸后换行，以免纸进入缝隙中	
	printf("\r\n");//切纸后换行，以免纸进入缝隙中	
}
