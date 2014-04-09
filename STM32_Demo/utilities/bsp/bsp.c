
#include "bsp.h"
 /*******************************************************************************
 * 函数名称:WaitTimeInit                                                                     
 * 描    述:把WaitTime从新安排到新的值                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
 void  WaitTimeInit(uint8_t *Time)
 {
       	*Time = 60;
 }

 /*******************************************************************************
 * 函数名称:FindMeal                                                                     
 * 描    述:查找那个地方有餐                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
 uint16_t   Balance;
 static     uint8_t IsCard	;
 uint8_t    DisplayTimes	 ;
 uint8_t    Line = 0,Column = 0 ;
 uint8_t FindMeal(MealAttribute *DefineMeal) 
 {
     for(Line = 0; Line < FloorMealNum; Line++)
		{
			for(Column = 0; Column < 3; Column++)
			{
			  if(DefineMeal[UserAct.MealID - 1].Position[Line][Column] > 0)	
			  return 0;		
			}
			if(Column < 3)
			{
				return 0 ;
			}
		}
		return 1 ;
 }


  /*******************************************************************************
 * 函数名称:PrintTick                                                                     
 * 描    述:是否打印小票                                                                
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
			
void PrintTickFun(unsigned char *PrintTickFlag)
{      
	if(*PrintTickFlag == 0x01 )
	{
		 TIM_Cmd(TIM4, DISABLE);
	 	*PrintTickFlag = 0 ;
		/*打印小票的函数*/
		  SPRT(UserAct.MealID-1);
	    TIM_Cmd(TIM4, ENABLE);
	}	 
	if(*PrintTickFlag == 0x02 )
	{
	   TIM_Cmd(TIM4, DISABLE);
		 *PrintTickFlag = 0;
		 TIM_Cmd(TIM4, ENABLE);
	}
	
}	
  /*******************************************************************************
 * 函数名称:WaitPayMoney                                                                     
 * 描    述:等待接收用户投的钱                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
uint8_t CurrentPoint = 0 ; 
uint8_t UserPayMoney =0 ;
unsigned char  WaitPayMoney(void)
{
    uint8_t PicNumber;
	uint8_t temp = 0 ;
	uint8_t temp1 ;
	unsigned char times = 10 ;
	unsigned char Status = 0;
  switch(CurrentPoint)
	{
	case 0 :	//发送那个位置的餐
	     {	  
	       if(FindMeal(DefineMeal) == 0)
	       {
			     CurrentPoint = 1 ; 
		     }
		     else 
		       return Status_Error	;
 	     }
          break ;
	case 1 : 
	         /*显示付款方式，现金，银行卡，深圳通*/
			  if(WaitTime<56)
			  {
			    CurrentPoint = 3;
			    /*支付方式*/			 
			    UserAct.PayType = '1';/* 现金支付*/
			  }break;    		
	case 2 :  //设计接收什么样的钱
	       /*跳到选择付款方式界面*/
			    CurrentPoint = 3 ;
				  UserAct.PayType = '1' ;/* 现金支付*/
		   break;	         
	case 3 :  //读钱	  
	       	//UserPayMoney = ReadBills();			 
		      if(UserPayMoney !=0 )	   //表示收到了钱
		      {
			      UserAct.PayAlready  += UserPayMoney;
			      UserAct.PayForBills += UserPayMoney;	
			   	  UserPayMoney = 0 ;
			   	  WaitTimeInit(&WaitTime);
//				    DisplayBills(UserAct.PayForBills);
			      CurrentPoint = 5 ;
		      }
        break;					
	case 4 :   //读刷卡机的钱
	        if( temp != 0)  //刷卡
	        { 
			      IsCard =1 ;	  //表示的是刷卡啦。
			      UserAct.PayAlready  += temp;
			      UserAct.PayForCards += temp ;
			      temp = 0 ;      
//			      DisplayCards(UserAct.PayForCards);
        	  CurrentPoint = 5 ;
 	        }
		      else
		        CurrentPoint = 5;
				break;
	case 5 ://显示接收了多少硬币		    
		       CurrentPoint = 6 ;
	      break;
	case 6 :  
         /*禁止点击餐屏幕*/
	       if(UserAct.PayAlready +UserAct.PayForCards>=UserAct.PayShould)		//投的钱大于等于要付的钱
		     {     
		       CurrentPoint = 9;	                //测试	 
			     if(UserAct.PayAlready <UserAct.PayShould)
			     {
			       CurrentPoint = 3;
			       return Status_Error ;
			     } 
//		 	   CloseTIM3();
//         CloseMoneySystem();			 
	   	  }
	      else
	      { 	 /*判断是否收到了钱的标志位*/
//		      if(Rev_Money_Flag == 1)
//			    {
//			      Rev_Money_Flag =0 ;
//			      /*把钱放进钱箱*/
//		        Polls();/*保留最好一张钱*/
//			    }
		      CurrentPoint = 3; 
	      } 	
	     	  break;   				 
    case 7 :  /*银行卡支付*/
			  UserAct.PayType = '2' ;/* 现金支付*/
			  temp1 = 0;
			  //temp1 = GpbocDeduct(UserAct.PayShould *100);
			  //temp1 = GpbocDeduct(1);
			  if(temp1 == 1)
			  {
			    UserAct.PayForCards = UserAct.PayShould ;
			    UserAct.PayAlready += UserAct.PayForCards ;
			    CurrentPoint =6;
			  }
	          break;
	 case 8 :/*深圳通支付*/
	      UserAct.PayType = '3' ;/* 现金支付*/
			  temp1 = 0;
			  //temp1 = SztDeduct(UserAct.PayShould * 100);
			  //temp1 = SztDeduct(1);
			  if(temp1 == 1)
			  {
			    UserAct.PayForCards = UserAct.PayShould ;
			    UserAct.PayAlready += UserAct.PayForCards ;
			    CurrentPoint =6;
			  }
	          break;
	  case 9 :  //关闭所有的收银系统
			  UserAct.MoneyBack = UserAct.PayAlready - UserAct.PayShould;	
			  Print_Struct.P_ID        = UserAct.MealID ;
			  Print_Struct.P_paymoney  = UserAct.PayForBills +	UserAct.PayForCoins +UserAct.PayForCards ;
			  Print_Struct.P_PayShould = UserAct.PayShould ;
			  Print_Struct.P_MoneyBack =	UserAct.MoneyBack ;
	
			 WaitTime  = 0;
			 //DisplayWaitTime1(WaitTime);//等待时间
			 /*倒计时*/
			 TIM_Cmd(TIM4, ENABLE);
			 if(UserAct.MoneyBack > 0)
			 {
			   //OpenCoins();    //找币		
			 }
			 CurrentPoint = 0 ;
	       return  Status_OK;
	  default :break;
  }
  return  Status_Action;
}												




unsigned char  Record_buffer[1024] = {0} ;  //为什么定义局部变量没有用呢？
uint8_t WaitMeal(void)
{
   
	uint8_t  i ;
	static uint8_t  CurrentPointer = 0 ;
  static unsigned char Cmd[20]={0x05, 0x31, 0x30, 0x30, 0x31, 0x30 ,0x30, 0x36, 0x34, 0x30, 0x30, 0x30, 0x34, 0x4D, 0x31, 0x35, 0x43, 0x03, 0x0D ,0x0A};
	 /*判断是否打印小票*/ 			
  //PrintTickFun(&UserAct.PrintTick);
	switch(CurrentPointer)
	{
	  case 0 :  
	     /* 发送餐的数目减一*/
	      FloorMealMessageWriteToFlash.FloorMeal[Line].MealCount --; 
	      switch(Column)
				{
				  case 0 :   FloorMealMessageWriteToFlash.FloorMeal[Line].FCount -- ;	  break;
				  case 1 :   FloorMealMessageWriteToFlash.FloorMeal[Line].SCount -- ;	  break;
				  case 2 :   FloorMealMessageWriteToFlash.FloorMeal[Line].TCount -- ;	  break;
				  default:  break;
				}
				 DefineMeal[UserAct.MealID - 1].Position[Line][Column]--;
				 DefineMeal[UserAct.MealID - 1].MealCount--;
			   CurrentPointer = 1 ;
			 break ;
    case 1 :   /*发送行跟列。延时等待，等待机械手到制定位置*/
	      switch(Column)
				{
				  case 0 :   Column =3;	  break;
				  case 1 :   Column =2;	  break;
				  case 2 :   Column =1;	  break;
				  default :break;
				}
	      Cmd[16]=0x40 + Column ;
		 		Cmd[15]=0x30 + (Line+1) %10 ;
				Cmd[14]=0x30 + (Line+1) /10 ;
				for(i=0;i<20;i++)
				{
				  USART_SendData(USART6,Cmd[i] );//串口1发送一个字符
 	        while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
				  delay_ms(10);
				}
				CurrentPointer = 2 ;
				TIM_Cmd(TIM2, ENABLE);
				MealArr(UserAct.MealID);
		      /*发送取餐数据给服务器*/
		    memset(Record_buffer,0,1024);    
				Record_buffer[1012] = 'L' ;
			  Record_buffer[1013] = 'e' ; 
				Record_buffer[1014] = 'n' ;
				Record_buffer[1015] = 'g' ;  
				Record_buffer[1016] = 'h' ;
				Record_buffer[1017] = 't' ;
				Record_buffer[1018] = ':' ; 
				if(TakeMealsFun(Record_buffer) == 0x01) //表示发送失败
			  {
				  Record_buffer[1022] = 'N' ;
				  Record_buffer[1023] = 'O' ; /*表示发送失败，写入标志位，待下次发送*/  
			  }
				else 
			  {  
					Record_buffer[1021] = 'Y' ;
				  Record_buffer[1022] = 'e' ;
					Record_buffer[1023] = 's' ; /*表示发送失败，写入标志位，待下次发送*/  
        }
		  		WriteDatatoSD(Record_buffer);
		  break ;
	 case 2 :   	  
	       /*延时3秒，然后开开关，让机器出餐*/	             
//	      if (TimerFlag > 4 )
//			  {
//			    TIM_Cmd(TIM2, DISABLE); 
//			    TimerFlag = 0;				 
//          CurrentPointer = 3 ;
//				  GPIO_SetBits(GPIOC, GPIO_Pin_13);
//				  TIM_Cmd(TIM2, ENABLE);  	
//				}
			break;
	 case 3 :   /*断开出餐开关，*/		
//	      if (TimerFlag > 2 )
//			  {
//			    TIM_Cmd(TIM2, DISABLE);
//			    TimerFlag = 0;				  
//          CurrentPointer = 4 ;
//				  GPIO_ResetBits(GPIOC, GPIO_Pin_13);	
//				  TIM_Cmd(TIM4, DISABLE);
//				  DisplayWaitTime1(WaitTime);
//				  PlayMusic(VOICE_3);		//。3.请取餐
//				  TIM_Cmd(TIM2, ENABLE);  
//				}			 
			break;
	 case 4 :  	/*播放语音，请取餐*/
	           	
//	      if (TimerFlag > 2 )
//			  {
//			    Polls();/*保留最好一张钱*/
//			    TimerFlag = 0;
//				  TIM_Cmd(TIM2, DISABLE);  
//          CurrentPointer = 5 ;
//				  if(IsCard == 1)
//				  {
//					  IsCard = 0 ;   //清零 
//				  }
//				}   
			break;
	 case 5 :  /*显示金额*/	
	      WriteMeal();   /*保存当前的份数*/
				//PlayMusic(VOICE_4);	    //4.欢迎下次光临
				CurrentPointer = 6 ;			   
			break;
	 case 6 :	/*欢迎下次光临*/	 
	 	  	 
				CurrentPointer = 7;				
			break;
	 case 7 : 	/*回到主界面*/  							  
				UserAct.MoneyBack   = 0 ;
				UserAct.PayAlready  = 0;
		    UserAct.PayForBills = 0;
		    UserAct.PayForCoins = 0;
		    UserAct.PayForCards = 0;
//				TimerFlag = 0 ;
				CurrentPointer = 0 ;
				return 0 ;
	 case 8 :  break;		                
   default : break ;	 	
  }      
  return 1 ;
}   



  /*******************************************************************************
 * 函数名称:ClearingFuntion                                                                     
 * 描    述:结算命令 检验数据是否发送成功，加入没有发送成功，继续发送。全部发送完毕后结算。                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void ClearingFuntion(void)
{
	 
	RTC_TimeShow();//获得时间
	if(TimeDate.Hours == 5 || TimeDate.Hours == 19)
	{
	  // 结算命令 ；
	      ClearingFun();	  //后台结算命令
		 // BankFlashCard_Upload(); //数据上送	 不是退签

	}		 
			 
}

    /*******************************************************************************
 * 函数名称:TempHandler                                                                     
 * 描    述:温度处理函数                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
 void  TempHandler(void)
 {
 }
 /*
  PVD ---- 低电压检测                抢占优先级  0      亚优先级 0 		用于保护sd卡
  硬币机               外部中断5                 0                1
  纸币机               串口4                     0                2
  IC卡                 串口2                     0                3  
  屏幕                 串口中断3                 0                4
                       CAN通信                   0                5
  打印                 串口1                     0                6
  退币倒计时           定时器4                   1                0
  定时倒计时60s        定时器3                   1                1
  驱动电机             定时器2                   1                2
  驱动电机             外部中断4                 1                3
  定时采集温度采集     定时器7                   1                4
 */
  /*******************************************************************************
 * 函数名称:hardfawreInit                                                                     
 * 描    述:硬件初始化                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 

void hardfawreInit(void)
{
    uint8_t i, j, k;

    for(i = 0; i < 4; i++)
	{
	
		DefineMeal[i].MealPrice = 0;
		DefineMeal[i].MealCount = 0;
		for(j = 0; j < 15; j++)
		{
			for(k = 0; k < 3; k++)
			{
				DefineMeal[i].Position[j][k] = 0; /**/
			}
		}
		
	}
 	 UserAct.PayForCoins     = 0;
	 UserAct.PayForBills     = 0;
	 UserAct.PayForCards     = 0;
	 UserAct.PayAlready      = 0;
	 IsCard            = 0;
//	 LCD_En_Temp();

   SystemInit();
//	 delay_ms(30000);
//	 LED_Init();
//     InitBills();               //纸币机初始化 
// 	 Uart1_Configuration();	    //串口1初始化
	 Uart3_Configuration();	    // 串口屏初始化
//	 Uart2_Configuration();	    //深圳通、银联卡串口
//	 Uart5_Configuration();		//网络串口初始化
//	 Uart6_Configuration();
//	 //InitCards();			    //IC卡初始化
//	 TIM2_Init();		        //电机
//	 TIM3_Init();		        //用于定时，倒计时
//	 TIM4_Init();		        //待定
//	 TIM5_Init();		        //倒计时退币
//	 TIM7_Init();				//用于定时采集温度
//	 InitCoins();		        //硬币初始化
//	 InitCoinsHopper() ;		 //退币器始化
////	 InitCoinMachine();		    //硬币机初始化
////	 CAN_InitConfig();	        //can初始化
// 	 PictrueDisplay(0);        //显示LOGO
//	 InitVoice()  ;             //语音初始化
	 MyRTC_Init();              //RTC初始化
	 SPI_FLASH_Init();          //Flash初始化
//	 LED_Init();
  // SPI_FLASH_SectorErase(SPI_FLASH_Sector0);
	 ReadMeal();
	 for(i=0;i<90;i++)
	 {
	   if(FloorMealMessageWriteToFlash.FlashBuffer[i] == 0xff)
	   FloorMealMessageWriteToFlash.FlashBuffer[i]    = 0 ;
	 }
	 WriteMeal();
	 StatisticsTotal();   	
// PictrueDisplay(HOME_PAGE);	       //显示菜单页面
// DS18B20_DEMO();
// TemperatureDisplay(Temperature);  //显示温度	  
// DisplayMealCount();			   //显示餐的数目 
 //delay_ms(30000);
	 
}														 

