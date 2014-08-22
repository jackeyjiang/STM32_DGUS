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
 uint8_t    DisplayTimes	 ;
 uint8_t    Line = 0,Column = 0 ;
 bool FindMeal(MealAttribute *DefineMeal) 
 {
    for(Line = 0; Line < FloorMealNum; Line++)  //查找层
		{
			for(Column = 0; Column < 3; Column++)     //查找列
			{
			  if(DefineMeal[UserActMessageWriteToFlash.UserAct.MealID - 1].Position[Line][Column] > 0)	
			    return true;		                           //当该位置有餐的话返回0
			}
		}
		return false ;
 }

  /*******************************************************************************
 * 函数名称:MoveToFisrtMeal();                                                                    
 * 描    述:找到用户选择的第一份餐品，并且移动到相应位置                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年6月25日                                                                    
 *******************************************************************************/ 
void MoveToFisrtMeal(void)
{
	if(UserActMessageWriteToFlash.UserAct.MealCnt_1st>0)
	{
	  UserActMessageWriteToFlash.UserAct.MealID = 0x01; 
	}				
	else if(UserActMessageWriteToFlash.UserAct.MealCnt_2nd>0)
  {
    UserActMessageWriteToFlash.UserAct.MealID = 0x02;
	}				
	else if(UserActMessageWriteToFlash.UserAct.MealCnt_3rd>0)
	{
		UserActMessageWriteToFlash.UserAct.MealID = 0x03;	
	}
	else if(UserActMessageWriteToFlash.UserAct.MealCnt_4th>0)
	{
		UserActMessageWriteToFlash.UserAct.MealID = 0x04;	
	}
  if(FindMeal(DefineMeal)) /*查找餐品ID的位置*/
  {
		OrderSendCoord(Line,Column);
	}   
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
			
void PrintTickFun(uint32_t *PrintTickFlag)
{      
	if(*PrintTickFlag == 0x00000001 )
	{
		 TIM_Cmd(TIM4, DISABLE);
	 	*PrintTickFlag = 0x00000000;
		/*打印小票的函数*/
		 SPRT();
	   TIM_Cmd(TIM4, ENABLE);
	}	 
	if(*PrintTickFlag == 0x00000002 )
	{
	   TIM_Cmd(TIM4, DISABLE);
		 *PrintTickFlag = 0x00000000;
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
	uint8_t reduce_money_flag = 0;
	uint32_t temp1=0,temp2=0;
	VariableChage(wait_payfor,WaitTime);
  switch(CurrentPoint)
	{
	  case 1 : 
	  {
	    /*显示付款方式，现金，银行卡，深圳通*/
			if(WaitTime<53)
			{
			  CurrentPoint = 3;
			  /*支付方式*/			 
			  //UserActMessageWriteToFlash.UserAct.PayType = 0x31;/* 现金支付*/
        //取消自动
        //PlayMusic(VOICE_3);
				//if(!OpenCashSystem()){OpenCashSystem();};// printf("cash system is erro2");  //关闭现金接受
			}
		}break;    		
	  case 2:  //由屏幕控制跳转
	  {
	    /*跳到选择付款方式界面*/
			CurrentPoint = 3 ;
		  UserActMessageWriteToFlash.UserAct.PayType = 0x31 ;/* 现金支付*/
	  }break;	  
		         
	  case 3 :  //读钱
		{			
	    UserPayMoney = ReadBills();			 
		  if(UserPayMoney !=0 )	   //表示收到了钱
		  {
			  UserActMessageWriteToFlash.UserAct.PayAlready  += UserPayMoney;
			  UserActMessageWriteToFlash.UserAct.PayForBills += UserPayMoney;	
				VariableChage(payment_bill,UserActMessageWriteToFlash.UserAct.PayForBills);
			  UserPayMoney = 0 ;
		  }
			CurrentPoint = 5 ;
		}break;    					
	  case 5 ://显示接收了多少硬币	
	  {
		  VariableChage(payment_coin,UserActMessageWriteToFlash.UserAct.PayForCoins);	
		  VariableChage(payment_card,UserActMessageWriteToFlash.UserAct.PayForCards); 
			CurrentPoint = 6;
		}break;		    
	  case 6 : //统计钱数
    {
	    if(UserActMessageWriteToFlash.UserAct.PayAlready +UserActMessageWriteToFlash.UserAct.PayForCards>=UserActMessageWriteToFlash.UserAct.PayShould)		//投的钱大于等于要付的钱
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
			UserActMessageWriteToFlash.UserAct.PayType = 0x32 ;/* 银行卡支付*/
			reduce_money_flag = GpbocDeduct(UserActMessageWriteToFlash.UserAct.PayShould-UserActMessageWriteToFlash.UserAct.PayAlready); //*100;
			if(reduce_money_flag == 1)
			{
				UserActMessageWriteToFlash.UserAct.PayForCards = UserActMessageWriteToFlash.UserAct.PayShould - UserActMessageWriteToFlash.UserAct.PayAlready;
			  UserActMessageWriteToFlash.UserAct.PayAlready += UserActMessageWriteToFlash.UserAct.PayForCards ;
				UART3_ClrRxBuf();
			  CurrentPoint =6;
			}
			else
			{
				WaitTimeInit(&WaitTime);
				PageChange(Acount_interface+2);
				CurrentPoint = 0;
			  /*支付方式*/			 
			  UserActMessageWriteToFlash.UserAct.PayType = 0x00;//清空支付方式			
        UART3_ClrRxBuf();
			}
	  }break;
	  case 8 :/*深圳通支付由屏幕控制*/
	  {
			WaitTimeInit(&WaitTime);
			PageChange(Cardbalence_interface);
	    UserActMessageWriteToFlash.UserAct.PayType = 0x33 ;/* 深圳通支付*/
			reduce_money_flag = SztDeduct(UserActMessageWriteToFlash.UserAct.PayShould - UserActMessageWriteToFlash.UserAct.PayAlready); //*100;
			if(reduce_money_flag == 1)
			{
				UserActMessageWriteToFlash.UserAct.PayForCards = UserActMessageWriteToFlash.UserAct.PayShould - UserActMessageWriteToFlash.UserAct.PayAlready;
			  UserActMessageWriteToFlash.UserAct.PayAlready += UserActMessageWriteToFlash.UserAct.PayForCards ;
				UART3_ClrRxBuf();
			  CurrentPoint =6;
			}
			else
			{
				WaitTimeInit(&WaitTime);
				PageChange(Acount_interface+2);
				CurrentPoint = 0;
			  /*支付方式*/			 
			  UserActMessageWriteToFlash.UserAct.PayType = 0x00;//清空支付方式
        UART3_ClrRxBuf();
			}
		}break;
	  case 9 :  //付款成功关闭所有的收银系统
		{
			UserActMessageWriteToFlash.UserAct.MoneyBack = UserActMessageWriteToFlash.UserAct.PayAlready - UserActMessageWriteToFlash.UserAct.PayShould;	
			UserActMessageWriteToFlash.UserAct.Meal_totoal = UserActMessageWriteToFlash.UserAct.MealCnt_8th+UserActMessageWriteToFlash.UserAct.MealCnt_7th+ UserActMessageWriteToFlash.UserAct.MealCnt_6th+UserActMessageWriteToFlash.UserAct.MealCnt_5th
                                                      +UserActMessageWriteToFlash.UserAct.MealCnt_4th+UserActMessageWriteToFlash.UserAct.MealCnt_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_2nd+UserActMessageWriteToFlash.UserAct.MealCnt_1st;
      UserActMessageWriteToFlash.UserAct.MoneyBackShould = UserActMessageWriteToFlash.UserAct.MoneyBack; //记录付钱之后第一次应该退的币	
      VariableChage(mealout_totle,UserActMessageWriteToFlash.UserAct.Meal_totoal);	
      //将需要打印的数据赋值到相关结构体	
     	Print_Struct.P_Number1st = UserActMessageWriteToFlash.UserAct.MealCnt_1st;
			Print_Struct.P_Number2nd = UserActMessageWriteToFlash.UserAct.MealCnt_2nd;
			Print_Struct.P_Number3rd = UserActMessageWriteToFlash.UserAct.MealCnt_3rd;
			Print_Struct.P_Number4th = UserActMessageWriteToFlash.UserAct.MealCnt_4th;
			Print_Struct.P_Number5th = UserActMessageWriteToFlash.UserAct.MealCnt_5th;
      Print_Struct.P_Number6th = UserActMessageWriteToFlash.UserAct.MealCnt_6th;
			Print_Struct.P_Number7th = UserActMessageWriteToFlash.UserAct.MealCnt_7th;
      Print_Struct.P_Number8th = UserActMessageWriteToFlash.UserAct.MealCnt_8th;
			Print_Struct.P_Cost1st   = UserActMessageWriteToFlash.UserAct.MealCost_1st;
			Print_Struct.P_Cost2nd   = UserActMessageWriteToFlash.UserAct.MealCost_2nd;
			Print_Struct.P_Cost3rd   = UserActMessageWriteToFlash.UserAct.MealCost_3rd;
			Print_Struct.P_Cost4th   = UserActMessageWriteToFlash.UserAct.MealCost_4th;
      Print_Struct.P_Cost5th   = UserActMessageWriteToFlash.UserAct.MealCost_5th;
      Print_Struct.P_Cost6th   = UserActMessageWriteToFlash.UserAct.MealCost_6th;
      Print_Struct.P_Cost7th   = UserActMessageWriteToFlash.UserAct.MealCost_7th;
      Print_Struct.P_Cost8th   = UserActMessageWriteToFlash.UserAct.MealCost_8th;
			Print_Struct.P_paymoney  = UserActMessageWriteToFlash.UserAct.PayForBills +	UserActMessageWriteToFlash.UserAct.PayForCoins +UserActMessageWriteToFlash.UserAct.PayForCards ;
			Print_Struct.P_PayShould = UserActMessageWriteToFlash.UserAct.PayShould ;
			Print_Struct.P_MoneyBack = UserActMessageWriteToFlash.UserAct.MoneyBack ;
			CurrentPoint = 0 ;
	       return  Status_OK;
		}
	  default :break;
  }
	if(WaitTime<=1) 
	{
    WaitTimeInit(&WaitTime);
		PageChange(Menu_interface);//超时退出用户餐品数量选择界面
		if(!CloseCashSystem()){CloseCashSystem();};//printf("cash system is erro6\r\n");  //关闭现金接受
		CurrentPoint = 0 ;
		temp1= UserActMessageWriteToFlash.UserAct.MoneyBack= UserActMessageWriteToFlash.UserAct.PayAlready; //超时将收到的钱以硬币的形式返还
		temp2= UserActMessageWriteToFlash.UserAct.MoneyBackShould= UserActMessageWriteToFlash.UserAct.PayAlready; //数据需要记录
		ClearUserBuffer();//清空用户数据
    UserActMessageWriteToFlash.UserAct.MoneyBack= temp1;
    UserActMessageWriteToFlash.UserAct.MoneyBackShould= temp2;
    SaveUserData();//保存用户数据  
		if(UserActMessageWriteToFlash.UserAct.MoneyBack>0)
		  Current= hpper_out;
    else
      Current= current_temperature;
	}
  else
  {
    if(WaitTime%10==1) 
    {
      if(UserActMessageWriteToFlash.UserAct.PayType==0x31)
      {
        PlayMusic(VOICE_3);
      }
      else
      {
        PlayMusic(VOICE_2);
      }        
    }    
  }
  return  Status_Action;
}												


uint8_t WaitMeal(void)
{
	uint8_t MealoutCurrentPointer=0;
  //static unsigned char Cmd[20]={0x05, 0x31, 0x30, 0x30, 0x31, 0x30 ,0x30, 0x36, 0x34, 0x30, 0x30, 0x30, 0x34, 0x4D, 0x31, 0x35, 0x43, 0x03, 0x0D ,0x0A};
	uint8_t temp;
	do
	{
		manageusart6data();  //将机械手的数据处理放在取餐头，不适用while(1)
		DealSeriAceptData(); //只处理打印数据
		switch(MealoutCurrentPointer)
		{
			case 0 : /*查找用户所选餐品的位置*/
			{
				//赋值当前UserActMessageWriteToFlash.UserAct.MealID
				if(UserActMessageWriteToFlash.UserAct.MealCnt_1st>0)
				{
					UserActMessageWriteToFlash.UserAct.MealID = 0x01; 
				}				
				else if(UserActMessageWriteToFlash.UserAct.MealCnt_2nd>0)
				{
					UserActMessageWriteToFlash.UserAct.MealID = 0x02;
				}				
				else if(UserActMessageWriteToFlash.UserAct.MealCnt_3rd>0)
				{
					UserActMessageWriteToFlash.UserAct.MealID = 0x03;	
				}
				else if(UserActMessageWriteToFlash.UserAct.MealCnt_4th>0)
				{
					UserActMessageWriteToFlash.UserAct.MealID = 0x04;	
				}
				else if(UserActMessageWriteToFlash.UserAct.MealCnt_5th>0)
				{
					UserActMessageWriteToFlash.UserAct.MealID = 0x05;	
				}
				else if(UserActMessageWriteToFlash.UserAct.MealCnt_6th>0)
				{
					UserActMessageWriteToFlash.UserAct.MealID = 0x06;	
				}
				else if(UserActMessageWriteToFlash.UserAct.MealCnt_7th>0)
				{
					UserActMessageWriteToFlash.UserAct.MealID = 0x07;	
				}
				else if(UserActMessageWriteToFlash.UserAct.MealCnt_8th>0)
				{
					UserActMessageWriteToFlash.UserAct.MealID = 0x08;	
				}
				else
				{	
					 //如果餐品全部出完，退出到主界面		
					 //printf("takeafter_meal\r\n");		
					PlayMusic(VOICE_12);				
					return takeafter_meal;
				}
      	if(FindMeal(DefineMeal)) /*查找餐品ID的位置*/
        {
					PlayMusic(VOICE_8);	
					MealoutCurrentPointer= 1;
				}
        //printf("case 0/UserAct.MealID == %d\r\n",UserAct.MealID);				
			}break;
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
				DefineMeal[UserActMessageWriteToFlash.UserAct.MealID - 1].Position[Line][Column]--;
				DefineMeal[UserActMessageWriteToFlash.UserAct.MealID - 1].MealCount--;
				WriteMeal();
				StatisticsTotal();
				DispLeftMeal(); 
				MealoutCurrentPointer = 2 ;
			}break ;   
			case 2 : //根据[Line][Column]的值发送坐标 等待ACK	
			{
				//printf("发送行和列的位置，等待响应\r\n");
				temp =0;
				temp = OrderSendCoord(Line,Column);
				
				if( temp ==1)//发送成功
				{
					LinkTime =0;
					machinerec.rerelative =0;   //不是待机位置
					MealoutCurrentPointer = 3 ;
				}
				else				//发送失败
				{
					//printf("send coord error\r\n");
					erro_record |= (1<<SendUR6Erro);
					return takemeal_erro;
				}
			}break;  
			case 3 :    /*发送取餐命令*/
			{
				//查询机械手是否准备好，如果准备好发送取餐命令
				//如果超时则 返回错误，
			  if(LinkTime > 10)    //超时
				{
				  LinkTime =0;
					//printf("move to coord timeout!\r\n");
					erro_record |= (1<<SendUR6Erro);
					return takemeal_erro;
				}       
				if(machinerec.regoal ==1)   //到达取餐点
				{
					machinerec.regoal =0 ;
					temp =0;
					temp = OrderGetMeal();   //发送取餐命令
					LinkTime =0;
					MealoutCurrentPointer=4;  				
					if(temp ==1)       // 取餐命令发送成功
					{
						LinkTime =0;
						MealoutCurrentPointer=4;  
					}
					else          //发送失败     
					{
					  //printf(" send getmeal order error\r\n");
						erro_record |= (1<<SendUR6Erro);
						return takemeal_erro;
					}
				}
			}break;
			case 4 :  	/*播放语音，请取餐*/
			{
				//如果餐品到达取餐口播放语音
				//如果餐品取出则 跳出子程序进行数据上传  
				if(machinerec.retodoor == 1)   //到达出餐口
				{
					machinerec.retodoor = 0;
					//播放请取餐语音
					PlayMusic(VOICE_9);
				}
				if(machinerec.reenablegetmeal ==1)  //取餐5秒了还未取到餐
				{
					machinerec.reenablegetmeal =0; //新加的需要把相关标记清零
					//printf("取餐5秒了还未取到餐\r\n");	 
					erro_record |= (1<<GetMealError);
					return takemeal_erro;
				}
				if(machinerec.retodoor == 1) //到达出餐口
				{
					machinerec.retodoor = 0;		
					LinkTime =0;		 
				}
				//printf("餐未超过了三分钟还未被取走\r\n");
				if( machinerec.remealaway == 1) //餐已被取走
				{
				//printf("餐已被取走\r\n");
					LinkTime =0;
					machinerec.remealaway = 0;
				  MealoutCurrentPointer=5;
					break;
				}
				if( machinerec.remealnoaway == 1)  //餐在取餐口过了20秒还未被取走
			  {
					if( LinkTime >=40) //餐在出餐口未被取走，一直等待时间时间大于20s播放语音提示取餐
					{
						if(LinkTime%20==0)
						{
							PlayMusic(VOICE_10);
						}
						LinkTime= 0;
					}					
				//语音提示“请取走出餐口的餐 "
				} 
			}break;			    
			case 5:     /*对用户数据进行减一*/  //?? 如果需要进行错误退币，需要修该返回值所在范围
			{
				machinerec.remealnoaway = 0;
				UserActMessageWriteToFlash.UserAct.Meal_takeout++;//取餐数据加
				VariableChage(mealout_already,UserActMessageWriteToFlash.UserAct.Meal_takeout);	//UI显示
				MealoutCurrentPointer= 0;
				//printf("case 5/UserAct.MealID == %d\r\n",UserAct.MealID);
				if(UserActMessageWriteToFlash.UserAct.MealID == 0x01)
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_1st--;
					//if(UserActMessageWriteToFlash.UserAct.MealCnt_1st==0)
					if(0)
					{
						//printf("tookkind_meal\r\n");
						return tookkind_meal;
					}
					else
					{
						//printf("tookone_meal\r\n");
				    return tookone_meal;
					}	
				}
				else if(UserActMessageWriteToFlash.UserAct.MealID == 0x02)
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_2nd--;
					//if(UserActMessageWriteToFlash.UserAct.MealCnt_2nd==0)
					if(0)
					{
						//printf("tookkind_meal\r\n");
						return tookkind_meal;
					}
					else
					{
						//printf("tookone_meal\r\n");
				    return tookone_meal;
					}						
				}
				else if(UserActMessageWriteToFlash.UserAct.MealID == 0x03)
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_3rd--;
					//if(UserActMessageWriteToFlash.UserAct.MealCnt_3rd==0)
					if(0)
					{
						//printf("tookkind_meal\r\n");
						return tookkind_meal;
					}
					else
					{
						//printf("tookone_meal\r\n");
				    return tookone_meal;
					}						
				}				
				else if(UserActMessageWriteToFlash.UserAct.MealID == 0x04)
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_4th--; 	
					//if(UserActMessageWriteToFlash.UserAct.MealCnt_4th==0)
					if(0)
					{
						//printf("tookkind_meal\r\n");
						return tookkind_meal;
					}
					else
					{
						//printf("tookone_meal\r\n");
				    return tookone_meal;
					}						
				}
				else if(UserActMessageWriteToFlash.UserAct.MealID == 0x05)
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_5th--; 	
					//if(UserActMessageWriteToFlash.UserAct.MealCnt_5th==0)
					if(0)
					{
						//printf("tookkind_meal\r\n");
						return tookkind_meal;
					}
					else
					{
						//printf("tookone_meal\r\n");
				    return tookone_meal;
					}						
				}
				else if(UserActMessageWriteToFlash.UserAct.MealID == 0x06)
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_6th--; 	
					//if(UserActMessageWriteToFlash.UserAct.MealCnt_6th==0)
					if(0)
					{
						//printf("tookkind_meal\r\n");
						return tookkind_meal;
					}
					else
					{
						//printf("tookone_meal\r\n");
				    return tookone_meal;
					}						
				}
				else if(UserActMessageWriteToFlash.UserAct.MealID == 0x07)
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_7th--; 	
					//if(UserActMessageWriteToFlash.UserAct.MealCnt_6th==0)
					if(0)
					{
						//printf("tookkind_meal\r\n");
						return tookkind_meal;
					}
					else
					{
						//printf("tookone_meal\r\n");
				    return tookone_meal;
					}						
				} 
				else if(UserActMessageWriteToFlash.UserAct.MealID == 0x08)
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_8th--; 	
					//if(UserActMessageWriteToFlash.UserAct.MealCnt_6th==0)
					if(0)
					{
						//printf("tookkind_meal\r\n");
						return tookkind_meal;
					}
					else
					{
						//printf("tookone_meal\r\n");
				    return tookone_meal;
					}						
				}         
				else 
				{
					printf("tookone_meal erro\r\n");
        }
			}
			default:break;
	  }
	}while(1);
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
	uint8_t cnt_t=253;
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
	uint8_t cnt_t=254;	
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
	UserActMessageWriteToFlash.UserAct.MealCnt_1st_t= UserActMessageWriteToFlash.UserAct.MealCnt_1st;
	UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t= UserActMessageWriteToFlash.UserAct.MealCnt_2nd;
	UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t= UserActMessageWriteToFlash.UserAct.MealCnt_3rd;
	UserActMessageWriteToFlash.UserAct.MealCnt_4th_t= UserActMessageWriteToFlash.UserAct.MealCnt_4th;
  UserActMessageWriteToFlash.UserAct.MealCnt_5th_t= UserActMessageWriteToFlash.UserAct.MealCnt_5th;
  UserActMessageWriteToFlash.UserAct.MealCnt_6th_t= UserActMessageWriteToFlash.UserAct.MealCnt_6th;
  UserActMessageWriteToFlash.UserAct.MealCnt_7th_t= UserActMessageWriteToFlash.UserAct.MealCnt_7th;
  UserActMessageWriteToFlash.UserAct.MealCnt_8th_t= UserActMessageWriteToFlash.UserAct.MealCnt_8th;
}

  /*******************************************************************************
 * 函数名称:PowerupAbnormalHandle                                                                    
 * 描    述:开机异常处理，只显示错误标识，更根据密码显示用户操作记录                                                                
 *            首先需要判断是否断电，再判断UserActMessageWriteToFlash.UserAct.paybacke是否大于0，显示用户选择的
              餐品数，未出的餐品数，以及钱数                                                                   
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月9日                                                                    
 *******************************************************************************/ 
void PowerupAbnormalHandle(int32_t erro_record_t)
{
  AbnormalHandle(erro_record_t);
	erro_record=0;
}

  /*******************************************************************************
 * 函数名称:PollAbnormalHandle                                                                   
 * 描    述:轮询                                                                 
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月9日                                                                    
 *******************************************************************************/ 
void PollAbnormalHandle(void)
{
	uint8_t i=0;
	for(i=32;i>0;i--) //32位记录各种异常，开机需要处理 ，可以通过管理员清理错误
	{
		if(erro_record&(1<<i))
		AbnormalHandle(i);	
	}	
}

  /*******************************************************************************
 * 函数名称:AbnormalHandle                                                                    
 * 描    述:异常处理 ，包含用户数据的显示                                                                
 *           包含开机异常处理，需要进行判断，相当于开机判断                                                                     
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
uint32_t erro_flag=0;
void AbnormalHandle(uint32_t erro)
{	
	erro_record |= (1<<erro); //在开始异常处理的时候需要用到
	//printf("erro_record2nd 0x%08X\r\n",erro_record);
	erro_flag = erro; //只是在错误处理与判断时需要用到，
	switch(erro)
	{
		case outage_erro:      //断电：只有在开机的时候判断是否有断电的情况发生 ，计算应该退的钱，还有就是付钱的时候断电
			{            /*取餐出错的情况*/              /*退币出错的情况,包含付钱的时候断电*/
			  if((UserActMessageWriteToFlash.UserAct.Meal_totoal!=UserActMessageWriteToFlash.UserAct.Meal_takeout)||(UserActMessageWriteToFlash.UserAct.MoneyBack>0))//先判断是否还有餐品没有取出和再判断用户未退的钱
				{
					/*这里加上调试信息，查看以上连个条件的数值*/
          if(UserActMessageWriteToFlash.UserAct.MealID>0)
					{
            //计算总的应退币数
            UserActMessageWriteToFlash.UserAct.MoneyBackShould +=
                         (UserActMessageWriteToFlash.UserAct.MealCnt_1st *price_1st +UserActMessageWriteToFlash.UserAct.MealCnt_2nd *price_2nd+ UserActMessageWriteToFlash.UserAct.MealCnt_3rd* price_3rd+ UserActMessageWriteToFlash.UserAct.MealCnt_4th *price_4th
                         +UserActMessageWriteToFlash.UserAct.MealCnt_5th* price_5th+ UserActMessageWriteToFlash.UserAct.MealCnt_6th* price_6th+ UserActMessageWriteToFlash.UserAct.MealCnt_7th* price_7th+ UserActMessageWriteToFlash.UserAct.MealCnt_8th* price_8th);	                         
					  //计算总的未退币数
            UserActMessageWriteToFlash.UserAct.MoneyBack +=
                         (UserActMessageWriteToFlash.UserAct.MealCnt_1st *price_1st+ UserActMessageWriteToFlash.UserAct.MealCnt_2nd *price_2nd+ UserActMessageWriteToFlash.UserAct.MealCnt_3rd* price_3rd+ UserActMessageWriteToFlash.UserAct.MealCnt_4th *price_4th
                         +UserActMessageWriteToFlash.UserAct.MealCnt_5th* price_5th+ UserActMessageWriteToFlash.UserAct.MealCnt_6th* price_6th+ UserActMessageWriteToFlash.UserAct.MealCnt_7th* price_7th+ UserActMessageWriteToFlash.UserAct.MealCnt_8th* price_8th);	 
            //计算上传的交易金额
            payfor_meal =(UserActMessageWriteToFlash.UserAct.MealCnt_1st *price_1st+ UserActMessageWriteToFlash.UserAct.MealCnt_2nd *price_2nd+ UserActMessageWriteToFlash.UserAct.MealCnt_3rd* price_3rd+ UserActMessageWriteToFlash.UserAct.MealCnt_4th *price_4th
                         +UserActMessageWriteToFlash.UserAct.MealCnt_5th* price_5th+ UserActMessageWriteToFlash.UserAct.MealCnt_6th* price_6th+ UserActMessageWriteToFlash.UserAct.MealCnt_7th* price_7th+ UserActMessageWriteToFlash.UserAct.MealCnt_8th* price_8th);	  
                        
						UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already= (UserActMessageWriteToFlash.UserAct.MoneyBackShould-UserActMessageWriteToFlash.UserAct.MoneyBack);//计算已退币的钱（需要处理，要两次退币的数据）
						DataUpload(Failed);//只有当UserActMessageWriteToFlash.UserAct.MealID!=0的时候才上传餐品的数据
						SaveUserData();
					}
					DisplayAbnormal("E070");
					PageChange(Err_interface);
        }
				else 
				{
					erro_record &= ~(1<<erro); //如果需要处理其他异常呢
					erro_flag= 0;
					return;
				}
			}break;
		case sdcard_erro:     //SD卡存储异常
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E000");
	      PageChange(Err_interface);					
				StatusUploadingFun(0xE000); //状态上送
			}break;
		case billset_erro:    //纸币机异常
			{
	      PlayMusic(VOICE_11);	  			
				DisplayAbnormal("E010"); 
        PageChange(Err_interface);	
				StatusUploadingFun(0xE010); //状态上送
			}break;
		case coinset_erro:      //投币机
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E020");
	      PageChange(Err_interface);				
				StatusUploadingFun(0xE020); //状态上送
			}break;
		case coinhooperset_erro:    //退币机
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E030");
	      PageChange(Err_interface);				
				StatusUploadingFun(0xE030); //状态上送
				//没有币可退的时候，UserActMessageWriteToFlash.UserAct.Payback 不为0
			}break;
		case coinhooperset_empty:   //找零用光
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E032");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE032); //状态上送
			}break;
		case printer_erro:      //打印机异常
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E040");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE040); //状态上送
			}break;
		case cardread_erro:     //读卡器异常
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E050");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE050); //状态上送
			}break;
		case network_erro:     //网络异常
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E060");
	      PageChange(Err_interface);
			}break;
		case signin_erro:    //签到异常
		   {
	       PlayMusic(VOICE_11);	
         DisplayAbnormal("E061");			
	       PageChange(Err_interface);  
       }break;
		case upload_erro:     //数据上传的时候突然断电
			{
        DataUpload(Success);
				erro_record &= ~(1<<upload_erro);
				return;
			};
    case arm_limit: //机械手禁止复位
		  {
			  if((UserActMessageWriteToFlash.UserAct.Meal_totoal!=UserActMessageWriteToFlash.UserAct.Meal_takeout)||(UserActMessageWriteToFlash.UserAct.MoneyBack>0))//先判断是否还有餐品没有取出和再判断用户未退的钱
				{
					/*这里加上调试信息，查看以上连个条件的数值*/
          if(UserActMessageWriteToFlash.UserAct.MealID>0)
					{					
						SaveUserData();
					}
					DisplayAbnormal("E100");
					PageChange(Err_interface);
        }
				else 
				{
					erro_record &= ~(1<<erro); //如果需要处理其他异常呢
					erro_flag= 0;
					return;
				}				
      }break;			
		case X_timeout:        //x轴传感器超时
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E101");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE101); //状态上送
				DataUpload(Failed);
			}break;
		case X_leftlimit:      //马达左动作极限输出
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E102");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE102); //状态上送
				DataUpload(Failed);
			}break;
		case X_rightlimit:     //马达右动作极限输出
			{
				DisplayAbnormal("E103");
	      PlayMusic(VOICE_11);
        StatusUploadingFun(0xE103); //状态上送	
	      PageChange(Err_interface);
				DataUpload(Failed);
			}break;
		case mealtake_timeout: //取餐口传感器超时
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E201");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE201); //状态上送
				DataUpload(Failed);
			}break;
		case Y_timeout:        //y轴传感器超时
			{
	      PlayMusic(VOICE_11);
        DisplayAbnormal("E301");	
	      PageChange(Err_interface);
				StatusUploadingFun(0xE302); //状态上送
				DataUpload(Failed);
			}break;
		case link_timeout:     //链接超时
			{
	      PlayMusic(VOICE_11);
        DisplayAbnormal("E401");	
	      PageChange(Err_interface);
				StatusUploadingFun(0xE401); //状态上送
			}break;
		case Z_timeout:        //z轴传感器超时
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E501");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE501); //状态上送
				DataUpload(Failed);
			}break;
		case Z_uplimit:        //z轴马达上动作超出
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E502");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE502); //状态上送
				DataUpload(Failed);
			}break;
		case Z_downlimit:      //z马达下动作超出
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E503");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE503); //状态上送
				DataUpload(Failed);
			}break;
		case solenoid_timeout: //电磁阀超时  ???有时有异常
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E601");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE601); //状态上送
				DataUpload(Failed);
			}break;
		case Eeprom_erro:      //eeprom 异常
			{
	      PlayMusic(VOICE_11);	
        DisplayAbnormal("E711");
	      PageChange(Err_interface);
				StatusUploadingFun(0xE711); //状态上送
				DataUpload(Failed);
			}break;
		case SendUR6Erro:      //发送数据异常或超时
			{
	      PlayMusic(VOICE_11);
        DisplayAbnormal("E801");	
	      PageChange(Err_interface);
				StatusUploadingFun(0xE801); //状态上送
				DataUpload(Failed);
      }break;
    case GetMealError:     //机械手5秒取不到餐
			{
	      PlayMusic(VOICE_11);
        DisplayAbnormal("E802");	
	      PageChange(Err_interface);
				StatusUploadingFun(0xE802); //状态上送
				DataUpload(Failed);
      }break;
    case MealNoAway:       //餐在出餐口20秒还未被取走
			{
	      PlayMusic(VOICE_11);
        DisplayAbnormal("E803");	
	      PageChange(Err_interface);
				StatusUploadingFun(0xE803); //状态上送
      }break;
		default:break;
	}
  SaveUserData(); //错误处理一次数据保存一次
  while(1)
  {
    DealSeriAceptData();
    if(erro_flag==0)
    {
      if(erro_record&(1<<arm_limit)) //开机的机械手禁止复位
      {
        PageChange(OnlymachieInit_interface);					
        OnlymachieInit(); //机械手的初始化
      }
      else
      {
        UserActMessageWriteToFlash.UserAct.MoneyBack=0;
        UserActMessageWriteToFlash.UserAct.MoneyBackShould=0;
        UserActMessageWriteToFlash.UserAct.MoneyBackAlready=0;
        ClearUserBuffer();
        SaveUserData();						
      }
      if(erro_record>=(1<<X_timeout))//如果是机械售错误值上传一次数据，对高位取反
      {
        erro_record&=~0xFFFF0000; //只显示一次异常
      }
      erro_record &= ~(1<<erro); //一次只处理一次异常
      RTC_WriteBackupRegister(RTC_BKP_DR13, erro_record);
      break;
    }
  }
}
  /*******************************************************************************
 * 函数名称:ErrRecHandle                                                                     
 * 描    述:错误记录处理                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年7月2日                                                                    
 *******************************************************************************/ 
void ErrRecHandle(void)
{
	 if(erro_record!=0) //当有错误记录，需要进行处理
	 {
		 AbnormalHandle(outage_erro);//相当于开机异常处理
		 if(erro_record&(1<<upload_erro))
		 {
			 if(UserActMessageWriteToFlash.UserAct.MealID!=0)
			   AbnormalHandle(upload_erro);//需要处理数据上传的断电
		 }
		 erro_record=0;
	 }
	 else
	 {
		 ClearUserBuffer(); //清除之前读取的数据
		 UserActMessageWriteToFlash.UserAct.MoneyBack=0;//
	 }
   SaveUserData(); 	
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
	Uart6_Configuration();   //机械手 1 ,2
 //初始化存放位置数据结构体
  for(i = 0; i <MealKindTotoal; i++)
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
 	 UserActMessageWriteToFlash.UserAct.PayForCoins     = 0;
	 UserActMessageWriteToFlash.UserAct.PayForBills     = 0;
	 UserActMessageWriteToFlash.UserAct.PayForCards     = 0;
	 UserActMessageWriteToFlash.UserAct.PayAlready      = 0;
   SystemInit();
	 PVD_Configuration();        //掉电检测初始化 0 , 0
	 Uart4_Configuration();     //纸币机串口初始化 1, 2
	 Uart1_Configuration();	    //打印机串口初始化
	 Uart3_Configuration();	    // 串口屏初始化  0 , 3
	 Uart2_Configuration();	    //深圳通、银联卡串口 1 , 0
	 Uart5_Configuration();		//网络串口初始化 1 , 1
	 TIM2_Init();		        //电机
	 TIM3_Init();		        //餐品数量选择倒计时
	 TIM4_Init();		        //打印倒计时5S
	 TIM5_Init();		        //机械手倒计时
	 TIM7_Init();				    //购物车界面倒计时
   InitCoins();		        //投币机初始化
   InitMiniGPIO() ;		   //退币器始化	 
	 InitVoice();             //语音初始化
	 MyRTC_Init();              //RTC初始化
	 //IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable); //打开看门狗
	 IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //打开看门狗
	 IWDG_SetPrescaler(IWDG_Prescaler_128); //40K /128 =312 = 0X0138
	 IWDG_SetReload(0x0138); // 1S
	 IWDG_Enable();
	 OpenTIM2();
	 delay_ms(1000);
	 SPI_FLASH_Init();          //Flash初始化
	 SPI_FLASH_Init();          //重复初始化才行
   SPI_FLASH_BufferRead(FloorMealMessageWriteToFlash.FlashBuffer, SPI_FLASH_Sector0 , FloorMealNum*6);//读取各层的餐品
   ReadCoins();//读取有多少硬币	 
	 for(i=0;i<90;i++)
	 {
	   if(FloorMealMessageWriteToFlash.FlashBuffer[i] == 0xff)
	   FloorMealMessageWriteToFlash.FlashBuffer[i]    = 0 ;
	 }
	 //WriteMeal();  //写入餐品数据,不需要因为写入餐品的数据需要花费很长世间，如果突然在开机过程突然断电数据就会丢失
	 StatisticsTotal(); //后面的程序需要使用  	
	 ReadUserData();  //需要进行数据处理，判断
   for(i=0;i<UserActiontotoaDatalSize*4;i++)
	 {
	   if(UserActMessageWriteToFlash.FlashBuffer[i] == 0xff)
	   FloorMealMessageWriteToFlash.FlashBuffer[i]    = 0 ;
	 }
}														 
