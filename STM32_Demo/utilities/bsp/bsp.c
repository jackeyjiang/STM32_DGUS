
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
	uint8_t temp = 0;
	uint8_t temp1;
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
			  WaitTimeInit(&WaitTime); 
		  }
			CurrentPoint = 4 ;
		}break;    					
	  case 4 :   //读刷卡机的钱
	  {
	    if( temp != 0)  //刷卡
	    { 
			  IsCard =1 ;	  //表示的是刷卡啦。
			  UserAct.PayAlready  += temp;
			  UserAct.PayForCards += temp ;
			  temp = 0 ;      
        CurrentPoint = 5 ;
 	    }
		  else
		    CurrentPoint = 5;
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
			  if(UserAct.PayAlready <UserAct.PayShould)
			  {
			    CurrentPoint = 3;
			    return Status_Error ;
			  }  
	   	}
	    else
	    { 
		    CurrentPoint = 3; 
	    } 	
		}break;   				 
    case 7 :  /*银行卡支付由屏幕控制*/
		{
			UserAct.PayType = '2' ;/* 银行卡支付*/
			temp1 = 0;
			//temp1 = GpbocDeduct(UserAct.PayShould *100);
			temp1 = GpbocDeduct(1);
			if(temp1 == 1)
			{
			  UserAct.PayForCards = UserAct.PayShould ;
			  UserAct.PayAlready += UserAct.PayForCards ;
			  CurrentPoint =6;
			}
	  }break;
	  case 8 :/*深圳通支付由屏幕控制*/
	  {
	    UserAct.PayType = '3' ;/* 深圳通支付*/
			temp1 = 0;
			//temp1 = SztDeduct(UserAct.PayShould * 100);
			temp1 = SztDeduct(1);
			if(temp1 == 1)
			{
			  UserAct.PayForCards = UserAct.PayShould ;
        UserAct.PayAlready += UserAct.PayForCards ;
			  CurrentPoint =6;
			}
		}break;
	  case 9 :  //付款成功关闭所有的收银系统
		{
			UserAct.MoneyBack = UserAct.PayAlready - UserAct.PayShould;	
      OldCoinsCnt= UserAct.MoneyBack ; //在这里程序只执行一次
			VariableChage(mealout_totle,UserAct.Meal_totoal);	
      NewCoinsCnt= 0; 
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
			WaitTime  = 0;
			TIM_Cmd(TIM4, ENABLE);
			CurrentPoint = 0 ;
	       return  Status_OK;
		}
	  default :break;
  }
  return  Status_Action;
}												


uint8_t  CurrentPointer = 0 ;

uint8_t WaitMeal(void)
{
  //static unsigned char Cmd[20]={0x05, 0x31, 0x30, 0x30, 0x31, 0x30 ,0x30, 0x36, 0x34, 0x30, 0x30, 0x30, 0x34, 0x4D, 0x31, 0x35, 0x43, 0x03, 0x0D ,0x0A};
	switch(CurrentPointer)
	{
		case 0 : /*查找用户所选餐品的位置*/
		{
			//赋值当前UserAct.MealID
	    if(UserAct.MealCnt_1st>0)
         UserAct.MealID = 0x01;	
      else if(UserAct.MealCnt_2nd>0)
				 UserAct.MealID = 0x02;	
      else if(UserAct.MealCnt_3rd>0)
				 UserAct.MealID = 0x03;	
      else if(UserAct.MealCnt_4th>0)
				 UserAct.MealID = 0x04;	
			else
			{	
         //如果餐品全部出完，退出到主界面				
				 printf("UserAct.MealCnt is none");
			}
			if(FindMeal(DefineMeal)) /**/
			   CurrentPointer= 1;break;	
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
			DefineMeal[UserAct.MealID - 1].Position[Line][Column]--;
		  DefineMeal[UserAct.MealID - 1].MealCount--;
			CurrentPointer = 2 ;
		}break ;
    case 2 : /*发送行和列的位置，等待响应*/
		{
       //根据[Line][Column]的值发送坐标 等待ACK	 
			CurrentPointer = 3 ;
	  }break;  
		case 3 :    /*发送取餐命令*/
		{
			//查询机械手是否准备好，如果准备好发送取餐命令
			//如果超时则 返回错误，
		  //如果没有餐品 CurrentPointer=0;  else CurrentPointer=3
			CurrentPointer=4;
		}break;
	  case 4 :  	/*播放语音，请取餐*/
		{ 	
      //如果餐品到达取餐口播放语音
			//如果餐品取出则 跳出子程序进行数据上传  
			CurrentPointer=5;
	  }break;			    
    case 5:     /*对用户数据进行减一*/
		{
	    if(UserAct.MealID == 0x01)
			  UserAct.MealCnt_1st--;
      else if(UserAct.MealID == 0x02)
				UserAct.MealCnt_2nd--;
      else if(UserAct.MealID == 0x03)
				UserAct.MealCnt_3rd--; 	
      else if(UserAct.MealID == 0x04)
				UserAct.MealCnt_4th--; 	
      UserAct.Meal_takeout++;//取餐数据加一
      VariableChage(mealout_already,UserAct.Meal_takeout++);	//UI显示减一	
			CurrentPointer= 0;
			return 0;
		}
		default:break;
	}
	return Status_Action;
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
bool CloseCashSystem(void)
{
	uint8_t cnt_t=20,money=0;
  CloseCoinMachine();			    //关闭投币机	
	DisableBills();             //设置并关闭纸币机
	do
  {
		cnt_t--;
		delay_ms(10);
    ReadBill();		
	}  // 超时
	while((cnt_t>0)&&(DisableBillFlag== NACK)); //当超时或者接收到ACK后
	if(DisableBillFlag== ACK) 
	{
		DisableBillFlag= NACK;
			return true;
	}
	else 
	{
		DisableBillFlag= NACK;
			return false;
	}
}
bool OpenCashSystem(void)
{
	int cnt_t=20;	
	OpenCoinMachine();    //打开投币机	
	SetBills();           //设置并打开纸币机
	do
  {
		cnt_t--;
		ReadBill();
		delay_ms(10);	
	}
	while((cnt_t>=0)&&(EnableBillFlag== NACK));//
	if(EnableBillFlag== ACK) 
	{
		EnableBillFlag= NACK;
			return true;
	}
	else 
	{
		EnableBillFlag= NACK;
			return false;
	}
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
	 Uart4_Configuration();     //纸币机串口初始化 
	 Uart1_Configuration();	    //打印机串口初始化
	 Uart3_Configuration();	    // 串口屏初始化
	 Uart2_Configuration();	    //深圳通、银联卡串口
	 Uart5_Configuration();		//网络串口初始化
	 Uart6_Configuration();
//	 TIM2_Init();		        //电机
//	 TIM3_Init();		        //用于定时，倒计时
//	 TIM4_Init();		        //待定
	 TIM5_Init();		        //倒计时退币
//	 TIM7_Init();				  //用于定时采集温度
   InitCoins();		        //投币机初始化
   InitMiniGPIO() ;		   //退币器始化
	 PageChange(Logo_interface); //显示logo
	 PageChange(Logo_interface); //重复一次就可以成功
	 InitVoice()  ;             //语音初始化
	 MyRTC_Init();              //RTC初始化
	 SPI_FLASH_Init();          //Flash初始化
	 SPI_FLASH_Init();          //重复初始化才行
   SPI_FLASH_BufferRead(FloorMealMessageWriteToFlash.FlashBuffer, SPI_FLASH_Sector0 , FloorMealNum*6);//读取各层的餐品
   //ReadCoins();//读取有多少硬币	 
	 VariableChage(coins_in,Coins_totoal);//显示机内硬币数
	 for(i=0;i<90;i++)
	 {
	   if(FloorMealMessageWriteToFlash.FlashBuffer[i] == 0xff)
	   FloorMealMessageWriteToFlash.FlashBuffer[i]    = 0 ;
	 }
	 WriteMeal();  //写入餐品数据
	 StatisticsTotal(); //后面的程序需要使用  	
}														 

