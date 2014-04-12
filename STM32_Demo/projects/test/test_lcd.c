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

#define current_temperature  0x01
#define waitfor_money        0x02
#define payment_success      0x03
#define hpper_out            0x04
#define print_tick           0x05
#define meal_out             0x06
#define data_upload          0x07
#define data_record          0x08

uint8_t Current= 0x01;  //
int main(void)
{
	hardfawreInit(); //硬件初始化
  SendtoServce();  //上传前七天的数据
//  ReadDatatoBuffer(); //上一个程序有这个函数
//   /*从网络获得时间，更新本地时钟*/
//  EchoFuntion(RTC_TimeRegulate);
//	/*网络签到*/
//	SignInFunction();	
// 	/*餐品对比数据*/
//	MealDataCompareFun();	 
//	Szt_GpbocAutoCheckIn(); 
	PageChange(Menu_interface); //显示选餐界面 
	DispLeftMeal();             //显示餐品数据
	if(!CloseCashSystem()) printf("cash system is erro");  //关闭现金接受
	while(1) 
  {		
		DealSeriAceptData();
    switch(Current)
	  {  
	    case current_temperature : /*温度处理函数*/
			{

			}break;
	    case waitfor_money:	  /*等待付钱*/
			{				
        if( WaitPayMoney()==Status_OK)
				{
					Current= data_record;
			  }
				}break;
			case data_record: /*数据记录*/
			{
				Current= hpper_out;
			}break;
      case hpper_out:	//退币状态
			{
		    if(UserAct.MoneyBack > 0) //需要找币的时候进入
		    {
          while(UserAct.MoneyBack > 0) //if >1
          {
             SendOutN_Coin(1);		//找币
	           --UserAct.MoneyBack;	
	           delay_ms(100); //延时得好好控制			
          }
				}
				else  //无需找币的时候直接进入出餐状态
				{					
					Current= hpper_out;break;
				}
			  if(OldCoinsCnt>NewCoinsCnt)
		    {
		      delay_ms(1000); //延时得好好控制	
          UserAct.MoneyBack= OldCoinsCnt- NewCoinsCnt;// 
		    }
		    else if(OldCoinsCnt==NewCoinsCnt)
		    {
			    UserAct.MoneyBack= OldCoinsCnt- NewCoinsCnt;//
		    }		
			}break;				
	    case meal_out:	 /*出餐状态*/
			{
			  if( WaitMeal()==Status_OK) //出一次餐发一次数据
			    Current = data_upload;
				else
					Current = current_temperature;
			}break;
	    case data_upload:	 //数据上传
	    {	
			  Current = meal_out ; 
	    }break ; 
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
