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
int main(void)
{
 	uint16_t temp = 0;
	uint8_t initflag ;
	hardfawreInit(); //硬件初始化
  //SendtoServce();  //上传前七天的数据
  //ReadDatatoBuffer(); //上一个程序有这个函数
   /*从网络  获得时间，更新本地时钟*/
//  EchoFuntion(RTC_TimeRegulate);
//	/*网络签到*/
//	SignInFunction();
// 	/*餐品对比数据*/
//	MealDataCompareFun();
	Szt_GpbocAutoCheckIn();
	PageChange(Menu_interface); //显示选餐界面
	DispLeftMeal();             //显示餐品数据
	if(!CloseCashSystem()) printf("cash system is erro");  //关闭现金接受
	LinkTime =0;
	OpenTIM5();
	initflag =0;
	while(1)
  {
		//delay_ms(200);
		DealSeriAceptData();
		//ManipulatorHandler();
    switch(Current)
	  {
	    case current_temperature: /*温度处理函数*/
			{
			  StateSend();
				VariableChage(current_temprature,Temperature);
			}break;
	    case waitfor_money:	 /*等待付钱*/
			{
        if( WaitPayMoney()==Status_OK)
				{
					UserAct.Meal_totoal=UserAct.MealCnt_1st + UserAct.MealCnt_2nd  + UserAct.MealCnt_3rd+ UserAct.MealCnt_4th;
					PageChange(Mealout_interface);
					delay_ms(200);
					if(!CloseCashSystem()) printf("cash system is erro");  //关闭现金接受
					//改变用户所选餐的总数
					Current= data_record;
			  }
			}break;
			case data_record:  /*数据记录*/
			{
				//将售餐的数据全部写入SD卡
         DataRecord();
				 //Current= hpper_out;
				 Current= meal_out;
			}break;
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
				}
				else  //无需找币的时候直接进入出餐状态
				{
					Current= meal_out;break;
				}
			  if(OldCoinsCnt>NewCoinsCnt)
		    {
		      delay_ms(2000); //延时得好好控制
          UserAct.MoneyBack= OldCoinsCnt- NewCoinsCnt;//
		    }
		    else if(OldCoinsCnt==NewCoinsCnt)
		    {
			    UserAct.MoneyBack= OldCoinsCnt- NewCoinsCnt;//
		    }
			}break;
	    case meal_out:	 /*出餐状态*/
			{
			  if( WaitMeal()==Status_OK) //出餐完毕
				{
          PageChange(TicketPrint_interface);/*打印发在显示处理函数*/
			    Current = current_temperature;
				}
			}break;
	    case data_upload:	 /*数据上传*/
	    {
        DataUpload();//根据UserAct.ID 判断需要上传的数据

				UserAct.MoneyBack   = 0;
				UserAct.PayAlready  = 0;
		    UserAct.PayForBills = 0;
		    UserAct.PayForCoins = 0;
        UserAct.PayForCards = 0;
			  Current = hpper_out ;
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
