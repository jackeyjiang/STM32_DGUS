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
void Uart1_Card(const uint8_t *p,uint8_t sizeData)
{
    uint8_t i;		   
	  for(i=0; i<sizeData; i++)
	  {
        USART_SendData(USART1, p[i]);//串口1发送一个字符
		    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//等待发送完成
 	  }
}								   	
										


/*  样张
 菜品       数量  单价  单总价
胡萝卜炒肉    1    15     15
香菇滑鸡      1    20     20
红烧鱼块      1    25     25
脆皮烤鸭      1    30     30
付款方式：银行卡支付/深圳通支付/现金支付
应收:90  已收:100  找回:10
时间:2014-04-16 23:19
*/

const uint8_t PrintInitCmd[2]={0x1b,0x40};          //打印机初始化
const uint8_t SetEntryPrintCHinese[2]={0x1c,0x26};	//进入汉子打印模式
const uint8_t SetExitPrintCHinese[2]={0x1c,0x2e};	  //退出打印模式
const uint8_t SendStc[3]={0x1d,0x56,0x01};		      //切纸命令
const uint8_t huan3[3]={0x1b,0x64,0x06};            //换三行

unsigned char  p0[36]={0};
unsigned char  p1[28]={"时间:2013-12-15 12:30:00\r\n"};
unsigned char  p2[32]={"              1     20    20 \r\n"};
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
  uint8_t CurrentPoint = 0;
	uint8_t Cmd[3]={0x10,0x04,0x04};
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
void COPY(Struct_TD  a,unsigned char *p10,unsigned char *p11)
{
  char temp[10]={0};
  char length=0,length_totoal=0;
  length=sprintf(temp,"应付:%0d元 ",UserActMessageWriteToFlash.UserAct.PayShould);
  memcpy(p10,temp,length);
  length_totoal+=length;
  length=sprintf(temp,"已付:%0d元 ",Print_Struct.P_paymoney);
  memcpy(p10+length_totoal,temp,length);
  length_totoal+=length;
  length=sprintf(temp,"找回:%0d元\r\n",Print_Struct.P_MoneyBack);
  memcpy(p10+length_totoal,temp,length);
			
	p11[5]  ='2';
	p11[6] = '0';
	p11[7] = a.Year     / 10+'0';
	p11[8] = a.Year     % 10+'0';
	p11[10] = a.Month    / 10+'0';
	p11[11] = a.Month    % 10+'0';
	p11[13] = a.Date     / 10+'0';
	p11[14] = a.Date     % 10+'0';
	p11[16] = a.Hours    / 10+'0';
	p11[17] = a.Hours    % 10+'0';
	p11[19] = a.Minutes  / 10+'0';
	p11[20] = a.Minutes  % 10+'0';
	p11[22] = a.Senconds / 10+'0';
  p11[23] = a.Senconds % 10+'0';
}
 /*******************************************************************************
 * 函数名称:SearchPrintMealID
 * 描    述:查找ID，并赋值给p2, 赋值                                                             
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:unsigned char                                                               
 * 修改日期:2014年9月19日 
 *******************************************************************************/
void SearchPrintMealID(uint8_t MealID)
{
	/*如果有则赋值相关的餐品ID的数量*/
		switch(MealID)
		{
			case 0x00:break;
			case 0x01:
			{
				memcpy(p2,mealname_1st,10);
			}break;	
			case 0x02:
			{
				memcpy(p2,mealname_2nd,10);
			}break;	
			case 0x03:
			{
				memcpy(p2,mealname_3rd,14);
			}break;	
			case 0x04:
			{
				memcpy(p2,mealname_4th,10);
			}break;	
			case 0x05:
			{
				memcpy(p2,mealname_5th,10);
			}break;	
			case 0x06:
			{
				memcpy(p2,mealname_6th,12);
			}break;	    
			case 0x07:
			{
				memcpy(p2,mealname_7th,8);
			}break;	
			case 0x08:
			{
				memcpy(p2,mealname_8th,10);
			}break;	
			case 0x09:
			{
				memcpy(p2,mealname_9th,10);
			}break;
			case 0x0A:
			{
				memcpy(p2,mealname_10th,10);
			}break;	
			case 0x0B:
			{
				memcpy(p2,mealname_11th,10);
			}break;	
			case 0x0C:
			{
				memcpy(p2,mealname_12th,10);
			}break;	
      case 0x0D:
			{
				memcpy(p2,mealname_13th,10);
			}break;	
      case 0x0E:
			{
				memcpy(p2,mealname_14th,8);
			}break;	
      case 0x0F:
			{
				memcpy(p2,mealname_15th,8);
			}break;	
      case 0x10:
			{
				memcpy(p2,mealname_16th,10);
			}break;	
      case 0x11:
			{
				memcpy(p2,mealname_17th,10);
			}break;	
			default:break;			
		}
}
	

 /*******************************************************************************
 * 函数名称:SPRT                                                                     
 * 描    述:打印用户的小票                                                                
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:unsigned char                                                               
 * 修改日期:2014年9月19日 
 *******************************************************************************/

P_stuction Print_Struct;
void  SPRT(void)
{
	char num_t[10]={0};
  RTC_TimeShow();//得到当前的时间
	COPY(TimeDate,p0,p1);
/*--------------打票机初始化----------------------/
  Uart1_Card(PrintInitCmd,sizeof(PrintInitCmd));
  Uart1_Card(SetEntryPrintCHinese,sizeof(SetEntryPrintCHinese));
/--------------打票机可以使用--------------------*/
  printf("菜品         数量  单价  金额\r\n");
	if(Print_Struct.P_Number1st>0)
  {
    memset(p2,0x20,34-4);
	  SearchPrintMealID(Print_Struct.P_Type1st);
		sprintf(num_t,"%1d",Print_Struct.P_Number1st);
		memcpy(p2+14,num_t,1);
		sprintf(num_t,"%2d",Print_Struct.P_Price1st*10/Discount);
		memcpy(p2+20,num_t,2);
    sprintf(num_t,"%3d",Print_Struct.P_Cost1st*10/Discount);
		memcpy(p2+25,num_t,3);
		printf("%s",p2);
  }
	if(Print_Struct.P_Number2nd>0)
	{
    memset(p2,0x20,34-4);
	  SearchPrintMealID(Print_Struct.P_Type2nd);
		sprintf(num_t,"%1d",Print_Struct.P_Number2nd);
		memcpy(p2+14,num_t,1);
		sprintf(num_t,"%2d",Print_Struct.P_Price2nd*10/Discount);
		memcpy(p2+20,num_t,2);
    sprintf(num_t,"%3d",Print_Struct.P_Cost2nd*10/Discount);
		memcpy(p2+25,num_t,3);
		printf("%s",p2);
	}
  if(Print_Struct.P_Number3rd>0)
	{
    memset(p2,0x20,34-4);
	  SearchPrintMealID(Print_Struct.P_Type3rd);
		sprintf(num_t,"%1d",Print_Struct.P_Number3rd);
		memcpy(p2+14,num_t,1);
		sprintf(num_t,"%2d",Print_Struct.P_Price3rd*10/Discount);
		memcpy(p2+20,num_t,2);
    sprintf(num_t,"%3d",Print_Struct.P_Cost3rd*10/Discount);
		memcpy(p2+25,num_t,3);
		printf("%s",p2);
	}
	if(Print_Struct.P_Number4th>0)
	{	
    memset(p2,0x20,34-4);    
	  SearchPrintMealID(Print_Struct.P_Type4th);
		sprintf(num_t,"%1d",Print_Struct.P_Number4th);
		memcpy(p2+14,num_t,1);
		sprintf(num_t,"%2d",Print_Struct.P_Price4th*10/Discount);
		memcpy(p2+20,num_t,2);
    sprintf(num_t,"%3d",Print_Struct.P_Cost4th*10/Discount);
		memcpy(p2+25,num_t,3);
		printf("%s",p2);
	}
  //printf("优惠信息:%d折\r\n",Discount);  
  printf("%s",p0);
	printf("%s",p1);
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
  printf("服务热线：400-0755-677\r\n");
	Uart1_Card(huan3,sizeof(huan3)); 
	Uart1_Card(huan3,sizeof(huan3)); 
	Uart1_Card(SendStc,sizeof(SendStc));//	切纸
	Uart1_Card(huan3,sizeof(huan3));  
  printf("深圳市速来食餐饮管理有限公司\r\n");
  printf("\r\n");//切纸后换行，以免纸进入缝隙中	
	printf("\r\n");//切纸后换行，以免纸进入缝隙中	
}

