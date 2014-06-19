 /**
  ******************************************************************************
  * 文件: test_lcd.c
  * 作者: jackey
  * 版本: V1.0.0
  * 描述: DWIN 串口屏主程序，实现串口屏幕切换与数据读取
  *       test_lcd
  ******************************************************************************
  *
  *                  	作为STM32F407开发的模板
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
uint8_t Current= 0x01;        //状态机变量
uint8_t LinkMachineFlag =0;	  //与机械手连接标志，0表示没连接，1表示连接
uint8_t waitmeal_status=0;    //等待取餐状态
int32_t erro_record=0x00000000; //错误标记位

uint16_t MoneyPayBack_Already = 0;//需要上传的退币数
uint16_t MoneyPayBack_Already_total=0; //总的退币数

uint16_t VirtAddVarTab[NB_OF_VAR] = {0x0001, 0x0002, 0x0003,};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};
uint16_t VarValue = 0;

int main(void)
{
	hardfawreInit(); //硬件初始化
	DisplayRecordTime(); //初始化时获取时间作为异常的时间
	PageChange(OnlymachieInit_interface);
  OnlymachieInit();  //机械手初始化
   /*从网络  获得时间，更新本地时钟*/
	PageChange(SignInFunction_interface);
  if(!EchoFuntion(RTC_TimeRegulate)) AbnormalHandle(network_erro);
	/*网络签到*/
	if(!SignInFunction())       AbnormalHandle(signin_erro);
  SendtoServce();  //上传前七天的数据
	/*深圳通签到*/
	PageChange(Szt_GpbocAutoCheckIn_interface);
	if(!Szt_GpbocAutoCheckIn()) AbnormalHandle(cardchck_erro);
	if((CoinsTotoalMessageWriteToFlash.CoinTotoal<50)||( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)== 0)) 	
	  AbnormalHandle(coinhooperset_erro); //当机内硬币数小于50 和 硬币机传感器线 报错 
	PageChange(Logo_interface);	
	delay_ms(1500);
	if(!CloseCashSystem())  AbnormalHandle(billset_erro);	
	//加入
	DispLeftMeal();             //显示餐品数据	
	PageChange(Menu_interface); //显示选餐界面


	while(1)
  {
		DealSeriAceptData();
		manageusart6data();   //
    switch(Current)
	  {
	    case current_temperature: /*温度处理函数*/
			{
				StateSend();
				if((LinkTime==1)||(LinkTime==2)||(LinkTime==3))
				{
				  VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal+CoinTotoal_t);//显示机内硬币数
					VariableChage(coins_back,Coins_cnt);
				}
				if(LinkTime >=5)
				{
					OrderSendLink();  //为1成功，为0失败
					VariableChage(current_temprature,Temperature); //5S一次
				}
				//显示倒计时,可以更具标记为对选餐倒计时进行更新
				if(UserAct.MealID)
				{
					if(WaitTime==0)
					{					 
						PageChange(Menu_interface);//超时退出用户餐品数量选择界面
						WaitTimeInit(&WaitTime);
					}
					else if(WaitTime!=60)
					{
						VariableChage(count_dowm,WaitTime); //短小的程序可以在终端中直接进行
					}
				}
			}break;
	    case waitfor_money:	 /*等待付钱*/
			{
        if( WaitPayMoney()==Status_OK)
				{
          PageChange(TicketPrint_interface);/*打印发在显示处理函数*/
					PlayMusic(VOICE_7);					
					CloseTIM3();
					CloseTIM7();					
					//改变用户所选餐的总数
					MoneyBack =UserAct.MoneyBack *100 ;  /*需退币退币的临时变量*/
					MoneyPayBack_Already_total = UserAct.MoneyBack; //计算应退币的数
			    mealvariety =0; 
					UserAct.Cancle= 0x00; //以免出错
					Current= hpper_out;
					UserAct.Meal_takeout= 0;	
          VariableChage(mealout_already,UserAct.Meal_takeout);	//UI显示					
					if(UserAct.PayType == '1')
					{
						CloseCoinMachine();			    //关闭投币机	
						delay_ms(1500);
						if(!CloseCashSystem()){};// printf("cash system is erro1\r\n");  //关闭现金接受
					}
			  }
				SaveUserData();
			}break;
      case hpper_out:	 /*退币状态:该程序的前提是在一次退10个硬币，不会出现退币机空转的情况*/
			{
				uint16_t i=0,cnt_t=0;
			  uint16_t coins_time=0;
        if(CoinsTotoalMessageWriteToFlash.CoinTotoal>=UserAct.MoneyBack)	//条件是机内的硬币>应退的币
				{					
					if(UserAct.MoneyBack >0) //需要找币的时候进入,
					{
						coins_time= (UserAct.MoneyBack/10); 
						cnt_t =  UserAct.MoneyBack%10;		
						UserAct.MoneyBack= 0;			
						for(i=0;i<coins_time+1;i++) //一次退币10个，
						{
							if(i!=coins_time)
							{
								UserAct.MoneyBack+= SendOutN_Coin(10);		
							}
							else
							{
								if(cnt_t>0)
									UserAct.MoneyBack+= SendOutN_Coin(cnt_t);	
								else
									break;
							}
						} 							
						if(ErrorType ==1) //出错再次发送退币 
						{
							delay_ms(1500); 
							UserAct.MoneyBack= SendOutN_Coin(UserAct.MoneyBack);
							if(ErrorType ==1)//退币机无币错误,直接进入错误状态
							{
								erro_record |= (1<<coinhooperset_empty);	
								if(erro_record>=(1<<X_timeout))//如果是机械手异常，直接进行错误处理
								{
									Current = erro_hanle;	
								}
								else
								{  /*只有在机械手没有错误的情况下 才计算上传退币的值*/
									MoneyPayBack_Already = MoneyBack -UserAct.MoneyBack *100 ;  /*已找出的币*/
									Current= meal_out;//找币错误的时候还是继续出餐
								}
								SaveUserData();
								break;
							}
							else
								erro_record &= ~(1<<coinhooperset_empty);//
						}											
						if(erro_record>=(1<<X_timeout))//如果是机械手异常，直接进行错误处理
						{
							Current = erro_hanle;	
						}
						else
						{  /*只有在机械手没有错误的情况下 才计算上传退币的值*/
							MoneyPayBack_Already = MoneyBack -UserAct.MoneyBack *100 ;  /*已找出的币*/
							Current= meal_out;//找币错误的时候还是继续出餐
						}
						break;						
				  }
				  else  //无需找币的时候直接进入出餐状态,
				  {
					  if(UserAct.Cancle== 0x00) //如果不是取消购买的话，就直接进入出餐界面
					  {
						  Current= meal_out; 
						  WaitTime=5;//5S计时   
						  OpenTIM4(); 
						  break;
					  }
					  else if(erro_record!=0x00) //出餐后的异常处理
					  {
						  Current= erro_hanle;
						  break;
					  }
					  else
					  {
						  UserAct.Cancle= 0x00;
						  Current= current_temperature;
						  break;
					  }
				  }
			  }					
				else //机内硬币不够时,MoneyBack不变
				{ 
					if(erro_record>=(1<<X_timeout))//如果是机械手异常，直接进行错误处理
					{
						Current = erro_hanle;	
					  break;
					}
					MoneyPayBack_Already= 0; //不够的时候那就是一个币都没有退
					erro_record |= (1<<coinhooperset_empty);
					Current= meal_out; //找币错误的时候还是继续出餐	
				}
			}break;
	    case meal_out:	 /*出餐状态：正在出餐，已出一种餐品，出餐完毕*/
			{
				if(WaitTime==0)
				{
		      PlayMusic(VOICE_8);			
				}
				waitmeal_status= WaitMeal();       
			  if(waitmeal_status == takeafter_meal) //出餐完毕
				{
          UserAct.PayAlready  = 0;
          UserAct.PayForBills = 0;
          UserAct.PayForCoins = 0;
          UserAct.PayForCards = 0;
          UserAct.Meal_takeout= 0;
          UserAct.Meal_totoal = 0;
					//清楚购物车
					ClearUserBuffer(); 
					if(UserAct.MoneyBack>0) //出餐完毕如果UserAct.MoneyBack>0 直接进入错误处理
					{
						erro_record |= (1<<coinhooperset_empty);
						Current = erro_hanle; 
					}
					else
					{
						/*无错状态进入到售餐界面*/
						PageChange(Menu_interface);
					  Current = current_temperature;
					}
					SaveUserData();
				}
				else if(waitmeal_status == tookkind_meal) //取完一种餐品
				{
					PageChange(Mealout_interface);//不知道需要加些什么
					Current = data_upload;	
				}
				else if(waitmeal_status == tookone_meal)  //取完一个餐品
				{
					PageChange(Mealout_interface);
					Current = data_upload;					
				}
        else if(waitmeal_status == takemeal_erro)
				{
					//机械手复位中请等待界面		
					NewCoinsCnt= 0; 
					MoneyPayBack_Already_total+= (UserAct.MealCnt_1st *price_1st+UserAct.MealCnt_2nd *price_2nd+UserAct.MealCnt_3rd *price_3rd+UserAct.MealCnt_4th*price_4th);//计算总的应该退币的钱
          UserAct.MoneyBack+= (UserAct.MealCnt_1st *price_1st+UserAct.MealCnt_2nd *price_2nd+UserAct.MealCnt_3rd *price_3rd+UserAct.MealCnt_4th*price_4th); //应该需要退币的钱	
					OldCoinsCnt= UserAct.MoneyBack;
					PageChange(Err_interface);
					UserAct.Cancle= 0x01;
				  /*如果有币进入退币，如果无币进入错误处理*/
					if(erro_record&(1<<coinhooperset_empty))
						Current = erro_hanle;
          else 
					{
					  Current = hpper_out;
					}
          SaveUserData();					
          break;					
				}			
			}break;
	    case data_upload:	 /*数据上传*/
	    {  		
        DataUpload(Success);//根据UserAct.ID 判断需要上传的数据
			  Current = meal_out;		
	    }break ;
      case erro_hanle: /*异常状态处理:需要对程序一直死在错误处理*/
      {
				//获取当前时间，并显示
				DisplayRecordTime();
				PollAbnormalHandle(); //异常处理 一直处于异常处理程序
				PageChange(Logo_interface);
				StatusUploadingFun(0xE800); //处理后返回正常
        while(1);				
        //Current = current_temperature;					
		  }
	  }
  }
}
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }

}
#endif

