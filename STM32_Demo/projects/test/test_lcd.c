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

uint16_t VirtAddVarTab[NB_OF_VAR] = {0x0001, 0x0002, 0x0003,};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};
uint16_t VarValue = 0;

int main(void)
{
 	uint16_t temp = 0;
	hardfawreInit(); //硬件初始化
	printf("hardfawreInit is ok\r\n");  //关闭现金接受
	//if(!CloseCashSystem())  
  //OnlymachieInit();  //机械手初始化
	printf("OnlymachieInit ok\r\n");  //
	//delay_ms(30000);
   /*从网络  获得时间，更新本地时钟*/
  if(!EchoFuntion(RTC_TimeRegulate)) AbnormalHandle(network_erro);
	printf("EchoFuntion ok\r\n");  //	
	/*网络签到*/
	if(!SignInFunction())       AbnormalHandle(signin_erro);
	printf("SignInFunction ok\r\n");  //
  SendtoServce();  //上传前七天的数据
	printf("SendtoServce");
	/*深圳通签到*/
	if(!Szt_GpbocAutoCheckIn()) AbnormalHandle(cardchck_erro);
	printf("Szt_GpbocAutoCheckIn ok\r\n");	
	PageChange(Menu_interface); //显示选餐界面
	delay_ms(1500);
	if(!CloseCashSystem())  AbnormalHandle(billset_erro);	
	DispLeftMeal();             //显示餐品数据
	while(1)
  {
		//delay_ms(200);
		DealSeriAceptData();
		manageusart6data();   //
    switch(Current)
	  {
	    case current_temperature: /*温度处理函数*/
			{
				StateSend();
				if(LinkTime >=5)
				{
					temp =0;
					temp = OrderSendLink();  //为1成功，为0失败
					VariableChage(current_temprature,Temperature); //5S一次
					//if(!CloseCashSystem()) printf("cash system is erro\r\n");  //关闭现金接受
					printf("temp = %d",temp);
				}
				//显示倒计时
				if(WaitTime==0)
				{					 
					PageChange(Menu_interface);//超时退出用户餐品数量选择界面
					WaitTimeInit(&WaitTime);
				}
				else if(WaitTime!=60)
				{
					VariableChage(count_dowm,WaitTime); //短小的程序可以在终端中直接进行
				}
			}break;
	    case waitfor_money:	 /*等待付钱*/
			{
				SaveUserData();
        if( WaitPayMoney()==Status_OK)
				{
          PageChange(TicketPrint_interface);/*打印发在显示处理函数*/
					PlayMusic(VOICE_7);					
					CloseTIM3();
					CloseTIM7();					
					WaitTime=5;//5S计时   
	       	OpenTIM4(); 
					//改变用户所选餐的总数
					MoneyBack =UserAct.MoneyBack *100 ;  /*扩大10倍*/
			    mealvariety =0; 
					Current= hpper_out;
					UserAct.Meal_takeout= 0;					
					if(UserAct.PayType == '1')
					{
						delay_ms(3000);
						if(!CloseCashSystem()) printf("cash system is erro1\r\n");  //关闭现金接受
					}
			  }
			}break;
//			case data_record:  /*数据记录*/
//			{
//				//将售餐的数据全部写入SD卡
//         //DataRecord();
//				 Current= hpper_out;
//				 //Current= meal_out;
//			}break;
      case hpper_out:	 /*退币状态*/
			{
		    if(UserAct.MoneyBack > 0) //需要找币的时候进入
		    {
          while(UserAct.MoneyBack > 0) //if >1
          {
             SendOutN_Coin(1);		//找币
	           --UserAct.MoneyBack;
	           delay_ms(200); //延时得好好控制
          }
					if(ErrorType ==1)
					{
						AbnormalHandle(coinhooperset_empty);
					}
				}
				else  //无需找币的时候直接进入出餐状态,
				{
					if(UserAct.Cancle== 0x00) //判断是不是取消购买
					{
					  Current= meal_out; 
						break;
					}
					else
					{
						UserAct.Cancle= 0x00;
						Current= current_temperature;
					}
				}
			  if(OldCoinsCnt>NewCoinsCnt)
		    {
		      delay_ms((OldCoinsCnt-NewCoinsCnt)/10*1000+1000); //延时得好好控制
          UserAct.MoneyBack= OldCoinsCnt- NewCoinsCnt;//
		    }
		    else if(OldCoinsCnt==NewCoinsCnt)
		    {
			    UserAct.MoneyBack= OldCoinsCnt- NewCoinsCnt;//
		    }
			}break;
	    case meal_out:	 /*出餐状态：正在出餐，已出一种餐品，出餐完毕*/
			{
				SaveUserData();
				if(WaitTime==0)
				{
					WaitTimeInit(&WaitTime);
		      PlayMusic(VOICE_8);			
				}
				waitmeal_status= WaitMeal();       
			  if(waitmeal_status == takeafter_meal) //出餐完毕
				{
					PageChange(Menu_interface);
					//出餐完毕，跳到回温度处理
          UserAct.MoneyBack   = 0;
          UserAct.PayAlready  = 0;
          UserAct.PayForBills = 0;
          UserAct.PayForCoins = 0;
          UserAct.PayForCards = 0;
          UserAct.Meal_takeout= 0;
          UserAct.Meal_totoal = 0;
					//清楚购物车
					ClearUserBuffer();          
					VariableChage(mealout_already,UserAct.Meal_takeout);	//UI显示
					Current = current_temperature;
				}
				else if(waitmeal_status == tookkind_meal) //取完一种餐品
				{
					PageChange(Mealout_interface);
					Current = data_upload;
				}
				else if(waitmeal_status == tookone_meal)  //取完一个餐品
				{
					PageChange(Mealout_interface);//不知道需要加些什么
				}				
			}break;
	    case data_upload:	 /*数据上传*/
	    {  		
        DataUpload();//根据UserAct.ID 判断需要上传的数据
			  Current = meal_out;		
	    }break ;
      case status_upload: /*状态上传*/
      {
				Current = current_temperature;
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

// FLASH_Unlock();

//  /* EEPROM Init */
//  EE_Init();

///* --- Store successively many values of the three variables in the EEPROM ---*/
//  /* Store 0x1000 values of Variable1 in EEPROM */
//  for (VarValue = 1; VarValue <= 0x1000; VarValue++)
//  {
//    EE_WriteVariable(VirtAddVarTab[0], VarValue);
//  }

//  /* read the last stored variables data*/
//  EE_ReadVariable(VirtAddVarTab[0], &VarDataTab[0]);


//  /* Store 0x2000 values of Variable2 in EEPROM */
//  for (VarValue = 1; VarValue <= 0x2000; VarValue++)
//  {
//    EE_WriteVariable(VirtAddVarTab[1], VarValue);
//  }

//  /* read the last stored variables data*/
//  EE_ReadVariable(VirtAddVarTab[0], &VarDataTab[0]);
//  EE_ReadVariable(VirtAddVarTab[1], &VarDataTab[1]);


//  /* Store 0x3000 values of Variable3 in EEPROM */
//  for (VarValue = 1; VarValue <= 0x3000; VarValue++)
//  {
//    EE_WriteVariable(VirtAddVarTab[2], VarValue);
//  }

//  /* read the last stored variables data*/
//  EE_ReadVariable(VirtAddVarTab[0], &VarDataTab[0]);
//  EE_ReadVariable(VirtAddVarTab[1], &VarDataTab[1]);
//  EE_ReadVariable(VirtAddVarTab[2], &VarDataTab[2]);

//  while (1);	
