#include "stdint.h"
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
 bool FindMeal(MealAttribute *DefineMeal) 
 {
    for(Line = 0; Line < FloorMealNum; Line++)  //查找层
		{
			for(Column = 0; Column < 3; Column++)     //查找列
			{
			  if(DefineMeal[UserAct.MealID - 1].Position[Line][Column] > 0)	
			    return true;		                           //当该位置有餐的话返回0
			}
		}
		return false ;
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
		 SPRT();
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

uint8_t CurrentPoint = 1 ; 
uint8_t UserPayMoney =0 ;
unsigned char NewCoinsCnt=0;
unsigned char OldCoinsCnt=0;
unsigned char  WaitPayMoney(void)
{
//	uint8_t temp = 0;
	uint8_t temp1;
	VariableChage(wait_payfor,WaitTime);
  switch(CurrentPoint)
	{
	  case 1 : 
	  {
	    /*显示付款方式，现金，银行卡，深圳通*/
			if(WaitTime<56)
			{
			  CurrentPoint = 3;
			  /*支付方式*/			 
			  UserAct.PayType = '1';/* 现金支付*/
			}
		}break;    		
	  case 2:  //由屏幕控制跳转
	  {
	    /*跳到选择付款方式界面*/
			CurrentPoint = 3 ;
		  UserAct.PayType = '1' ;/* 现金支付*/
	  }break;	  
		         
	  case 3 :  //读钱
		{			
	    UserPayMoney = ReadBills();			 
		  if(UserPayMoney !=0 )	   //表示收到了钱
		  {
			  UserAct.PayAlready  += UserPayMoney;
			  UserAct.PayForBills += UserPayMoney;	
				VariableChage(payment_bill,UserAct.PayForBills);
			  UserPayMoney = 0 ;
		  }
			CurrentPoint = 5 ;
		}break;    					
	  case 5 ://显示接收了多少硬币	
	  {
		  VariableChage(payment_coin,UserAct.PayForCoins);	
		  VariableChage(payment_card,UserAct.PayForCards); 
			CurrentPoint = 6;
		}break;		    
	  case 6 : //统计钱数
    {
	    if(UserAct.PayAlready +UserAct.PayForCards>=UserAct.PayShould)		//投的钱大于等于要付的钱
		  {     
		    CurrentPoint = 9;	             
	   	}
	    else
	    { 
		    CurrentPoint = 3; 
	    } 	
		}break;   				 
    case 7 :  /*银行卡支付由屏幕控制*/
		{
			WaitTimeInit(&WaitTime);
			PageChange(Cardbalence_interface);
			UserAct.PayType = '2' ;/* 银行卡支付*/
			temp1 = 0;
			temp1 = GpbocDeduct(UserAct.PayShould-UserAct.PayAlready); //*100;
			//temp1 = GpbocDeduct(1);
			if(temp1 == 1)
			{
				UserAct.PayForCards = UserAct.PayShould - UserAct.PayAlready;
			  //UserAct.PayForCards = UserAct.PayShould ;
			  UserAct.PayAlready += UserAct.PayForCards ;
				UART3_ClrRxBuf();
			  CurrentPoint =6;
			}
			else
			{
				WaitTimeInit(&WaitTime);
				PageChange(Acount_interface+2);
				CurrentPoint = 1;
			  /*支付方式*/			 
			  UserAct.PayType = 0x00;/* 现金支付*/			
        UART3_ClrRxBuf();
			}
	  }break;
	  case 8 :/*深圳通支付由屏幕控制*/
	  {
			WaitTimeInit(&WaitTime);
			PageChange(Cardbalence_interface);
	    UserAct.PayType = '3' ;/* 深圳通支付*/
			temp1 = 0;
			temp1 = SztDeduct(UserAct.PayShould - UserAct.PayAlready); //*100;
			if(temp1 == 1)
			{
				UserAct.PayForCards = UserAct.PayShould - UserAct.PayAlready;
			  //UserAct.PayForCards = UserAct.PayShould ;
			  UserAct.PayAlready += UserAct.PayForCards ;
				UART3_ClrRxBuf();
			  CurrentPoint =6;
			}
			else
			{
				WaitTimeInit(&WaitTime);
				PageChange(Acount_interface+2);
				CurrentPoint = 1;
			  /*支付方式*/			 
			  UserAct.PayType = 0x00;/* 现金支付*/			
        UART3_ClrRxBuf();
			}
		}break;
	  case 9 :  //付款成功关闭所有的收银系统
		{
			UserAct.MoneyBack = UserAct.PayAlready - UserAct.PayShould;	
			UserAct.Meal_totoal= UserAct.MealCnt_4th+UserAct.MealCnt_3rd+UserAct.MealCnt_2nd+UserAct.MealCnt_1st;
      OldCoinsCnt= UserAct.MoneyBack ; //在这里程序只执行一次
			VariableChage(mealout_totle,UserAct.Meal_totoal);	
      NewCoinsCnt= 0; 
			CloseTIM7();
			CloseTIM3();
      //将需要打印的数据赋值到相关结构体	
     	Print_Struct.P_Number1st = UserAct.MealCnt_1st;
			Print_Struct.P_Number2nd = UserAct.MealCnt_2nd;
			Print_Struct.P_Number3rd = UserAct.MealCnt_3rd;
			Print_Struct.P_Number4th = UserAct.MealCnt_4th;
			Print_Struct.P_Cost1st   = UserAct.MealCost_1st;
			Print_Struct.P_Cost2nd   = UserAct.MealCost_2nd;
			Print_Struct.P_Cost3rd   = UserAct.MealCost_3rd;
			Print_Struct.P_Cost4th   = UserAct.MealCost_4th;
			Print_Struct.P_paymoney  = UserAct.PayForBills +	UserAct.PayForCoins +UserAct.PayForCards ;
			Print_Struct.P_PayShould = UserAct.PayShould ;
			Print_Struct.P_MoneyBack = UserAct.MoneyBack ;
			CurrentPoint = 0 ;
	       return  Status_OK;
		}
	  default :break;
  }
	if(WaitTime==0) 
	{
    WaitTimeInit(&WaitTime);
		PageChange(Menu_interface);//超时退出用户餐品数量选择界面
		if(!CloseCashSystem()) printf("cash system is erro6\r\n");  //关闭现金接受
		CurrentPoint = 0 ;
		UserAct.MoneyBack= UserAct.PayAlready; //超时将收到的钱以硬币的形式返还
		ClearUserBuffer();//清空用户数据
		if(UserAct.MoneyBack>0)
		Current= hpper_out;
	}
  return  Status_Action;
}												


uint8_t  CurrentPointer = 0 ;
uint8_t WaitMeal(void)
{
  //static unsigned char Cmd[20]={0x05, 0x31, 0x30, 0x30, 0x31, 0x30 ,0x30, 0x36, 0x34, 0x30, 0x30, 0x30, 0x34, 0x4D, 0x31, 0x35, 0x43, 0x03, 0x0D ,0x0A};
	uint8_t temp;
	switch(CurrentPointer)
	{
		case 0 : /*查找用户所选餐品的位置*/
		{
			//赋值当前UserAct.MealID
	    if(UserAct.MealCnt_1st>0)
			{
				UserAct.MealID = 0x01; 
				goto loop3;
			}				
      else if(UserAct.MealCnt_2nd>0)
			{
				UserAct.MealID = 0x02;
				goto loop3;
			}				
      else if(UserAct.MealCnt_3rd>0)
			{
				 UserAct.MealID = 0x03;	
					goto loop3;
			}
      else if(UserAct.MealCnt_4th>0)
			{
				 UserAct.MealID = 0x04;	
					goto loop3;
			}
			else
			{	
         //如果餐品全部出完，退出到主界面		
         printf("takeafter_meal\r\n");		
         PlayMusic(VOICE_12);				
				 return takeafter_meal;
			}
loop3:	if(FindMeal(DefineMeal)) /*查找餐品ID的位置*/
			   CurrentPointer= 1;
		}//break;
	  case 1 : /* 发送餐的数目减一*/
		{			 
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
			WriteMeal();
			StatisticsTotal();
			DispLeftMeal(); 
			CurrentPointer = 2 ;
		}//break ;   
    case 2 : /*发送行和列的位置，等待响应*/
 		{
        //根据[Line][Column]的值发送坐标 等待ACK	
			printf("发送行和列的位置，等待响应\r\n");
 			temp =0;
 			temp = OrderSendCoord(Line,Column);
 			
 			if( temp ==1)//发送成功
 			{
        LinkTime =0;
				machinerec.rerelative =0;   //不是待机位置
 				CurrentPointer = 3 ;
 			}
 			else				//发送失败
 			{
 				printf("send coord error\r\n");
        AbnormalHandle(SendUR6Erro);
 			}
 	  }//break;  
 		case 3 :    /*发送取餐命令*/
 		{
			printf("发送取餐命令\r\n");
 			//查询机械手是否准备好，如果准备好发送取餐命令
 			//如果超时则 返回错误，
 		  //如果没有餐品 CurrentPointer=0;  else CurrentPointer=3
 			while(1)  //查询机械手是否准备好，
      {
        if(LinkTime > 10)    //超时
        {
          LinkTime =0;
          printf("move to coord timeout!\r\n");
          AbnormalHandle(SendUR6Erro);
          break;
        }       
 			  manageusart6data();  //若机械手有数据，处理机械手返回数据
        if(machinerec.regoal ==1)   //到达取餐点
        {
           break;
        }
      }
      if(machinerec.regoal ==1)   //到达取餐点
      {
        machinerec.regoal =0 ;
        temp =0;
        temp = OrderGetMeal();   //发送取餐命令
        LinkTime =0;
        CurrentPointer=4;  				
//        if(temp ==1)       // 取餐命令发送成功
//        {
//          LinkTime =0;
//          CurrentPointer=4;  
//        }
//        else          //发送失败     
//        {
//          printf(" send getmeal order error\r\n");
//          AbnormalHandle(SendUR6Erro);
//        }
      }
 		}//break;
 	  case 4 :  	/*播放语音，请取餐*/
 		{
			//CurrentPointer=5; 
      PlayMusic(VOICE_9);			
      printf("播放语音，请取餐");			
       //如果餐品到达取餐口播放语音
 			//如果餐品取出则 跳出子程序进行数据上传  
 			while(1)
       {
         if(LinkTime >120)   //从发出取餐命令后到餐已到达出餐口，最多等待一分钟
         {
           printf("from send getmeal order to sell door timeout!\r\n");
           AbnormalHandle(SendUR6Erro);
           break;
         }
 			   manageusart6data();   //若机械手有数据，处理机械手返回数据
         if(machinerec.retodoor == 1)   //到达出餐口
         {
					 machinerec.retodoor = 0;
					 //播放请取餐语音
					  PlayMusic(VOICE_9);
           break;
         }
         if(machinerec.reenablegetmeal ==1)  //取餐5秒了还未取到餐
         {
           printf("取餐5秒了还未取到餐\r\n");
					 
           AbnormalHandle(GetMealError);
           break;
         }
       }
//       if( machinerec.retodoor == 1) //到达出餐口
//       {
//         machinerec.retodoor = 0;
//         
//       }
       LinkTime =0;
       while(1)//等待客户取走餐之后，才跳到Case 5
       {
         if( LinkTime >180) //餐在出餐口未被取走，最多等待3分，超过了报错
         {
           printf("餐未超过了三分钟还未被取走\r\n");
           AbnormalHandle(SendUR6Erro);
           break;
         }
 			 manageusart6data();   //若机械手有数据，处理机械手返回数据
 		   if( machinerec.remealaway == 1) //餐已被取走
       {
					 printf("餐已被取走\r\n");
					 //machinerec.remealaway = 0;
					 LinkTime =0;
					 machinerec.remealaway = 0;
					 CurrentPointer=5;
           break;
       }
       if( machinerec.remealnoaway == 1)  //餐在取餐口过了20秒还未被取走
       {
					printf("餐在取餐口过了20秒还未被取走\r\n");
				  PlayMusic(VOICE_10);
           //machinerec.remealnoaway = 0;
					 LinkTime =0;
					 machinerec.remealnoaway = 0;
					 CurrentPointer=5;
					break;
           //语音提示“请取走出餐口的餐 "
        } 
      }
//      if( machinerec.remealaway == 1) //餐已被取走
//      {
//         LinkTime =0;
//         machinerec.remealaway = 0;
//         CurrentPointer=5;
//      }
//			if( machinerec.remealnoaway == 1) //餐在取餐口过了20秒还未被取走，暂时用此判断
//      {
//         LinkTime =0;
//         machinerec.remealnoaway = 0;
//         CurrentPointer=5;
//      }
 	  }//break;			    
    case 5:     /*对用户数据进行减一*/  //?? 如果需要进行错误退币，需要修该返回值所在范围
		{
			delay_ms(1000);
      UserAct.Meal_takeout++;//取餐数据加
      VariableChage(mealout_already,UserAct.Meal_takeout);	//UI显示
			CurrentPointer= 0;
			if(UserAct.MealID == 0x01)
			{
			  UserAct.MealCnt_1st--;
				if(UserAct.MealCnt_1st==0)
				{
					printf("tookkind_meal\r\n");
					return tookkind_meal;
				}
			}
      else if(UserAct.MealID == 0x02)
			{
				UserAct.MealCnt_2nd--;
				if(UserAct.MealCnt_2nd==0)
				{
					printf("tookkind_meal\r\n");
					return tookkind_meal;
				}					
			}
      else if(UserAct.MealID == 0x03)
			{
				UserAct.MealCnt_3rd--;
				if(UserAct.MealCnt_3rd==0)
				{
					printf("tookkind_meal\r\n");
					return tookkind_meal;
				}			
			}				
      else if(UserAct.MealID == 0x04)
			{
				UserAct.MealCnt_4th--; 	
				if(UserAct.MealCnt_4th==0)
				{
					printf("tookkind_meal\r\n");
					return tookkind_meal;
				}		
			}
			printf("tookone_meal\r\n");
			return tookone_meal;
		}
		default:break;
	}
}   

  /*******************************************************************************
 * 函数名称:CloseCashSystem                                                                     
 * 描    述:关闭现金接受                                                        
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
#define bill_time  600  //500ms不会出错，100ms纸币机反应不过来
bool CloseCashSystem(void)
{
	uint8_t cnt_t=253,money=0,temp=0;
  CloseCoinMachine();			    //关闭投币机	
	delay_ms(bill_time);
	memset(BillDataBuffer,0xFE,sizeof(BillDataBuffer));
	DisableBills();             //设置并关闭纸币机
	do
	{
		cnt_t--;
		delay_ms(1);
		if((RX4Buffer[0]=='0')&&(RX4Buffer[1]=='0'))
		{
			return true;
		}
		else if((RX4Buffer[0]=='F')&&(RX4Buffer[1]=='F'))
		{
			return false;  			
		}
	}while(cnt_t);			 
	 return false; 
}

bool OpenCashSystem(void)
{
	uint8_t cnt_t=254,temp;	
	OpenCoinMachine();    //打开投币机	
	delay_ms(bill_time);        //需要控制
	memset(BillDataBuffer,0xFE,sizeof(BillDataBuffer));
	SetBills();           //设置并打开纸币机
	do
	{
		cnt_t--;
		delay_ms(1);
		if((RX4Buffer[0]=='0')&&(RX4Buffer[1]=='0'))
		{
			return true;
		}
		else if((RX4Buffer[0]=='F')&&(RX4Buffer[1]=='F'))
		{
			return false;  			
		}
	}while(cnt_t);	
	return false; 
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
 * 函数名称:AcountCopy                                                                   
 * 描    述:异常处理                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void AcountCopy(void)
{
	UserAct.MealCnt_1st_t= UserAct.MealCnt_1st;
	UserAct.MealCnt_2nd_t= UserAct.MealCnt_2nd;
	UserAct.MealCnt_3rd_t= UserAct.MealCnt_3rd;
	UserAct.MealCnt_4th_t= UserAct.MealCnt_4th;

}

  /*******************************************************************************
 * 函数名称:PowerupAbnormalHandle                                                                    
 * 描    述:开机异常处理，只显示错误标识，更根据密码显示用户操作记录                                                                
 *            首先需要判断是否断电，再判断UserAct.paybacke是否大于0，显示用户选择的
              餐品数，未出的餐品数，以及钱数，                                                                  
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月9日                                                                    
 *******************************************************************************/ 
void PowerupAbnormalHandle(int32_t erro_record)
{
	uint16_t i=0;
	for(i=0;i<32;i++)
	{
		if(erro_record&(1<<i))
		AbnormalHandle(i);	
	}
}

  /*******************************************************************************
 * 函数名称:AbnormalHandle                                                                    
 * 描    述:异常处理                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
uint16_t erro_flag=0;
void AbnormalHandle(uint16_t erro)
{	
	erro_flag = erro;
	erro_record |= (1<<erro);
	switch(erro)
	{
		case outage_erro:      //断电
			{
				if(UserAct.MoneyBack>0)
				{
					PageChange(Err_interface);
					DisplayAbnormal("E070");					
				}
				else 
				{
					//erro_record &= ~(1<<erro);
					erro_flag= 0;
					break;;
				}
			}break;
		case sdcard_erro:     //SD卡存储异常
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);				
				DisplayAbnormal("E000");
			}break;
		case billset_erro:    //纸币机异常
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);				
				DisplayAbnormal("E010");
			}break;
		case coinset_erro:      //投币机
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);				
				DisplayAbnormal("E020");
			}break;
		case coinhooperset_erro:    //退币机
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);				
				DisplayAbnormal("E030");
			}break;
		case coinhooperset_empty:   //找零用光
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E032");
				//UserAct.MoneyBack
			}break;
		case printer_erro:      //打印机异常
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E040");
			}break;
		case cardread_erro:     //读卡器异常
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E050");
			}break;
		case network_erro:     //网络异常
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E060");
			}break;
		case X_timeout:        //x轴传感器超时
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E101");
				PayBackUserMoney();
			}break;
		case X_leftlimit:      //马达左动作极限输出
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E102");
				PayBackUserMoney();
			}break;
		case X_rightlimit:     //马达右动作极限输出
			{
				DisplayAbnormal("E103");
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				PayBackUserMoney();
			}break;
		case mealtake_timeout: //取餐口传感器超时
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E201");
			  PayBackUserMoney();
			}break;
		case Y_timeout:        //y轴传感器超时
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E301");
				PayBackUserMoney();
			}break;
		case link_timeout:     //链接超时
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E401");
				PayBackUserMoney();
			}break;
		case Z_timeout:        //z轴传感器超时
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E501");
				PayBackUserMoney();
			}break;
		case Z_uplimit:        //z轴马达上动作超出
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E502");
				PayBackUserMoney();
			}break;
		case Z_downlimit:      //z马达下动作超出
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E503");
				PayBackUserMoney();
			}break;
		case solenoid_timeout: //电磁阀超时  ???有时有异常
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E601");
				PayBackUserMoney();
			}break;
		case Eeprom_erro:      //eeprom 异常
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
				DisplayAbnormal("E711");
				PayBackUserMoney();
			}break;
		case SendUR6Erro:      //发送数据异常或超时
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
        DisplayAbnormal("E801");
				PayBackUserMoney();
      }break;
    case GetMealError:     //机械手5秒取不到餐
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
        DisplayAbnormal("E802");
				PayBackUserMoney();
      }break;
    case MealNoAway:       //餐在出餐口20秒还未被取走
			{
	      PlayMusic(VOICE_11);	
	      PageChange(Err_interface);
        DisplayAbnormal("E803");
      }break;
		default:break;
	}
		while(1)
		{
			DealSeriAceptData();
			if(erro_flag==0)
			{
				erro_record=0;
				RTC_WriteBackupRegister(RTC_BKP_DR13, erro_record);
				break;
			}
		}
}
  /*******************************************************************************
 * 函数名称:PayBackUserMoney                                                                    
 * 描    述:出餐出错退币                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年4月23日    
 *******************************************************************************/ 
uint32_t UserMoneyBack=0;
void PayBackUserMoney(void)
{
	UserMoneyBack =UserAct.MealCnt_1st *price_1st+UserAct.MealCnt_2nd *price_2nd+UserAct.MealCnt_3rd *price_3rd+UserAct.MealCnt_4th*price_4th; //计算单总价
  if(CoinsTotoalMessageWriteToFlash.CoinTotoal>=UserMoneyBack)
	{
		SendOutN_Coin(UserMoneyBack);
		ClearUserBuffer();
	}
	else
	{
		PlayMusic(VOICE_11);
	}
}

  /*******************************************************************************
 * 函数名称:SaveUserData                                                                    
 * 描    述:掉电保存                                                               
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年4月23日    
 *******************************************************************************/ 
void SaveUserData(void)
{
	RTC_WriteBackupRegister(RTC_BKP_DR13, erro_record);
	RTC_WriteBackupRegister(RTC_BKP_DR4,  UserAct.MealCnt_1st);
	RTC_WriteBackupRegister(RTC_BKP_DR5,  UserAct.MealCnt_2nd);
	RTC_WriteBackupRegister(RTC_BKP_DR6,  UserAct.MealCnt_3rd);
	RTC_WriteBackupRegister(RTC_BKP_DR7,  UserAct.MealCnt_4th);
	RTC_WriteBackupRegister(RTC_BKP_DR8,  UserAct.Meal_totoal);
	RTC_WriteBackupRegister(RTC_BKP_DR9,  UserAct.Meal_takeout);
	RTC_WriteBackupRegister(RTC_BKP_DR10, UserAct.PayShould);	
	RTC_WriteBackupRegister(RTC_BKP_DR11, UserAct.PayAlready);
	RTC_WriteBackupRegister(RTC_BKP_DR12, UserAct.MoneyBack);
	RTC_WriteBackupRegister(RTC_BKP_DR14,  UserAct.MealCnt_1st_t);
	RTC_WriteBackupRegister(RTC_BKP_DR15,  UserAct.MealCnt_2nd_t);
	RTC_WriteBackupRegister(RTC_BKP_DR16,  UserAct.MealCnt_3rd_t);
	RTC_WriteBackupRegister(RTC_BKP_DR17,  UserAct.MealCnt_4th_t);	
}

  /*******************************************************************************
 * 函数名称:ReadUserData                                                                    
 * 描    述:掉电保存                                                               
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年4月23日    
 *******************************************************************************/ 
void ReadUserData(void)
{
	UserAct.MealCnt_1st= RTC_ReadBackupRegister(RTC_BKP_DR4);
	UserAct.MealCnt_2nd= RTC_ReadBackupRegister(RTC_BKP_DR5);
	UserAct.MealCnt_3rd= RTC_ReadBackupRegister(RTC_BKP_DR6);
	UserAct.MealCnt_4th= RTC_ReadBackupRegister(RTC_BKP_DR7);
	UserAct.Meal_totoal= RTC_ReadBackupRegister(RTC_BKP_DR8);
	UserAct.Meal_takeout=RTC_ReadBackupRegister(RTC_BKP_DR9);
	UserAct.PayShould  = RTC_ReadBackupRegister(RTC_BKP_DR10);	
	UserAct.PayAlready = RTC_ReadBackupRegister(RTC_BKP_DR11);
	UserAct.MoneyBack  = RTC_ReadBackupRegister(RTC_BKP_DR12);
	erro_record        = RTC_ReadBackupRegister(RTC_BKP_DR13);
	UserAct.MealCnt_1st_t= RTC_ReadBackupRegister(RTC_BKP_DR14);
	UserAct.MealCnt_2nd_t= RTC_ReadBackupRegister(RTC_BKP_DR15);
	UserAct.MealCnt_3rd_t= RTC_ReadBackupRegister(RTC_BKP_DR16);
	UserAct.MealCnt_4th_t= RTC_ReadBackupRegister(RTC_BKP_DR17);	
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
  Uart6_Configuration();
//初始化存放位置数据结构体
  for(i = 0; i < 4; i++)
	{
		DefineMeal[i].MealPrice = 0;
		DefineMeal[i].MealCount = 0;
		for(j = 0; j < 15; j++)
		{
			for(k = 0; k < 3; k++)
			{
				DefineMeal[i].Position[j][k] = 0; 
			}
		}	
	}
 	 UserAct.PayForCoins     = 0;
	 UserAct.PayForBills     = 0;
	 UserAct.PayForCards     = 0;
	 UserAct.PayAlready      = 0;
   SystemInit();
//	 delay_ms(30000); //上电等待路由器启动
	 PVD_Configuration();        //掉电检测初始化
	 Uart4_Configuration();     //纸币机串口初始化 
	 Uart1_Configuration();	    //打印机串口初始化
	 Uart3_Configuration();	    // 串口屏初始化
	 Uart2_Configuration();	    //深圳通、银联卡串口
	 Uart5_Configuration();		//网络串口初始化
	 Uart6_Configuration();
	 //TIM2_Init();		        //电机
	 TIM3_Init();		        //餐品数量选择倒计时
	 TIM4_Init();		        //打印倒计时5S
	 TIM5_Init();		        //机械手倒计时
	 TIM7_Init();				    //购物车界面倒计时
   InitCoins();		        //投币机初始化
   InitMiniGPIO() ;		   //退币器始化
	 PageChange(Logo_interface); //显示logo
	 PageChange(Logo_interface); //重复一次就可以成功
	 InitVoice();             //语音初始化
	 MyRTC_Init();              //RTC初始化
	 SPI_FLASH_Init();          //Flash初始化
	 //SPI_FLASH_Init();          //重复初始化才行
   SPI_FLASH_BufferRead(FloorMealMessageWriteToFlash.FlashBuffer, SPI_FLASH_Sector0 , FloorMealNum*6);//读取各层的餐品
	 //WriteCoins();
   ReadCoins();//读取有多少硬币	 
	 //VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal);//显示机内硬币数
	 for(i=0;i<90;i++)
	 {
	   if(FloorMealMessageWriteToFlash.FlashBuffer[i] == 0xff)
	   FloorMealMessageWriteToFlash.FlashBuffer[i]    = 0 ;
	 }
	 WriteMeal();  //写入餐品数据
	 StatisticsTotal(); //后面的程序需要使用  	
	 ReadUserData();  //需要进行数据处理，判断
	 if(erro_record!=0) //当有错误记录，需要进行处理
	 {
		 PowerupAbnormalHandle(erro_record);
	 }
	 else
	 {
		 ClearUserBuffer(); //清除之前读取的数据
	 }
	 
}														 




