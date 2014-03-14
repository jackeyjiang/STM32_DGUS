#include "stm32f4xx.h"
#include "MsgHandle.h"
#include "serialscreen.h"
#include "vioce.h"
#include "bsp.h"
 LCDStruction  	 LCDstructure;
MealAttribute DefineMeal[9];	//定义9个菜系
FloorAttribute FloorMeal[FloorMealNum], CurFloor;
UserSelection UserAct;
FloorMealMessage FloorMealMessageWriteToFlash;
uint8_t  BillActionFlag = 1;
uint8_t  PassWordLen=0;	//密码的长度为0
uint8_t  TemperatureSwitch = 0;
uint8_t  TemperatureCur;
uint8_t  WaitTime;


 void LCD_DisTemp(void)
 {
  	LCDstructure.LCD_Temp = 0 ;
 }
 void  LCD_En_Temp(void)
 {
  	LCDstructure.LCD_Temp = 1 ;
 }
 /*******************************************************************************
 * 函数名称:StatisticsTotal                                                                     
 * 描    述:统计数目                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
 void StatisticsTotal(void)
 {	 
        uint8_t i,j;
		for(j = 0; j < 9; j++)	 //9代表餐ID号
		{
		    DefineMeal[j].MealCount = 0;
		    for(i = 0; i < 15; i++)
		    {
		        if(FloorMealMessageWriteToFlash.FloorMeal[i].MealID == j + 1)	 /*前面那个表示的是卸载flash里面的ID 与外面的进行比较*/
			    {
			        DefineMeal[j].MealCount      = DefineMeal[j].MealCount + FloorMealMessageWriteToFlash.FloorMeal[i].MealCount;
				    DefineMeal[j].Position[i][0] = FloorMealMessageWriteToFlash.FloorMeal[i].FCount;
				    DefineMeal[j].Position[i][1] = FloorMealMessageWriteToFlash.FloorMeal[i].SCount;
				    DefineMeal[j].Position[i][2] = FloorMealMessageWriteToFlash.FloorMeal[i].TCount;
			    }
		    }
		}
 }



//void GetMealPrice(uint8_t MealId)
//{
//    if(MealId ==1 || MealId == 2 || MealId == 3)
//	{
//	    DefineMeal[MealId-1].MealPrice = 15;
//		UserAct.MealPrice = 15;
//		UserAct.PayShould = 15;
//	}
//	else if(MealId ==4 || MealId == 5 || MealId == 6)
//	{
//	    DefineMeal[MealId-1].MealPrice = 20;
//		UserAct.MealPrice = 20;
//		UserAct.PayShould = 20;
//	}
//	else if(MealId ==7 || MealId == 8 || MealId == 9)
//	{
//	    DefineMeal[MealId-1].MealPrice = 25;
//		UserAct.MealPrice = 25;
//		UserAct.PayShould = 25;
//	}
//}
 void GetMealPrice(uint8_t MealId)
{
    if(MealId ==1 || MealId == 2 || MealId == 3)
	{
	    DefineMeal[MealId-1].MealPrice = 4;
		UserAct.MealPrice = 4;
		UserAct.PayShould = 4;
	}
	else if(MealId ==4 || MealId == 5 || MealId == 6)
	{
	    DefineMeal[MealId-1].MealPrice = 5;
		UserAct.MealPrice = 5;
		UserAct.PayShould = 5;
	}
	else if(MealId ==7 || MealId == 8 || MealId == 9)
	{
	    DefineMeal[MealId-1].MealPrice = 5;
		UserAct.MealPrice = 5;
		UserAct.PayShould = 5;
	}
}
void InitSetting(void)
{
    if(FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].MealID == CurFloor.MealID)
    {
		CurFloor.MealCount = FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].MealCount;
		CurFloor.FCount    = FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].FCount;
		CurFloor.SCount    = FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].SCount;
		CurFloor.TCount    = FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].TCount;
	}
	else
	{
		CurFloor.MealCount = 0;
		CurFloor.FCount    = 0;
		CurFloor.SCount    = 0;
		CurFloor.TCount    = 0;
	}
	DisplayFColumn(CurFloor.FCount);
	DisplaySColumn(CurFloor.SCount);
	DisplayTColumn(CurFloor.TCount);
    
}
  /*******************************************************************************
 * 函数名称:cancel                                                                     
 * 描    述:返回主界面                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void cancel(void)
{
  	
    PictrueDisplay(HOME_PAGE);
    DisplayMealCount();
}
  /*******************************************************************************
 * 函数名称:MsgHandle                                                                     
 * 描    述:触摸屏处理                                                                   
 *                                                                               
 * 输    入:uint8_t cmd                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 

uint8_t MsgHandle(uint8_t cmd)
{
  static 	uint8_t  InputPassWord[6],PassWord[6];
  uint8_t  PictrueNum = 0;
  uint8_t i= 0,j=0;
  unsigned	char ReturnMessage = 0 ;
 //  printf("cmd=%x\r\n",cmd);
  switch(cmd)
  {
	case 0x00:		   /*无意义*/
	case 0x01:		   /*选择1号餐盒*/
	case 0x02:		   /*选择2号餐盒*/
	case 0x03:
	case 0x04:		   /*选择3号餐盒*/ //4-4=0
	case 0x05:		   /*选择4号餐盒*/ //5-4=0
	case 0x06:		   /*选择5号餐盒*/ //6-4=0
	case 0x07:		   /*选择6号餐盒*///7-4=0
	case 0x08:		   /*选择7号餐盒*/ //8-4=0
	case 0x09:		   /*选择8号餐盒*/ //9-4=0
	case 0x0A:		   /*选择9号餐盒*/ //10-4=0
	case 0x0B:		   /*选择10号餐盒*///11-4=0
	case 0x0C:		   /*选择11号餐盒*///12-4=0
		{
		 /*判断选择的菜是否大于0 */
	    if(DefineMeal[cmd-1].MealCount > 0)	   //要选择哪个菜
		{
		/*默认用户选择的份数为1*/
		 UserAct.MealCount = 1 ;
		 /*定义用户选择的是那个快餐*/
		 UserAct.MealID  = cmd  ;               //用户选择的是什么菜系	
		 /*界面显示相应的图片的序号*/
		 PictrueNum = 3+(cmd-1)*7;            //要显示那张图片
		 /*得到选择盒饭的价格*/
		 GetMealPrice(cmd);	                   //得到价钱
		 /*显示图片*/
		 PictrueDisplay(PictrueNum);	           //显示图片
		 /*打开收银系统*/
		 OpenMoneySystem();			               //打开收银系统
		 /*延时300ms 上电时刻系统不稳定，等待稳定*/
		 delay_ms(300);			                   //打开硬币机的时候系统可能处于不稳定状态 延时等系统稳定
		 /**/
		 CoinCountFlag  = 0;					   //
		 /*显示默认的一份*/
         DisplayMealNum(1); 	                   //显示份数
		 /*显示客户投币的金额*/
		 DisplayBills(UserAct.PayForBills);	       //显示纸币
		 /*显示客户投币的硬币数*/
		 DisplayCoins(UserAct.PayForCoins);	       //显示硬币
		 /*显示用户刷卡金额*/
		 DisplayCards(UserAct.PayForCards);	       //显示刷卡
		 /*显示总金额*/	  
		 DisplayMealPrice(UserAct.MealPrice);      //显示总价格
		 /*等待时间*/
		 WaitTime = 60 ;						   //定时时间
		 /*显示等待时间*/
		 DisplayWaitTime(WaitTime);
		 /*播放语音*/
		 PlayMusic(VOICE_1);	                   //请选择分数并付款
		 /*开启倒计时 60*/
		 OpenTIM3();							   //用于倒计时		

		 ReturnMessage = 1 ;
	 	}
		else
		{
		   /* 显示主页*/
		   PictrueDisplay(HOME_PAGE);
		   /*显示数量*/
		   DisplayMealCount();
		   /*显示温度*/
		   DisplayTemperature(Temperature);
		}

  	  break;
  }
   case 0x0D:  //进入LOG页面
	{

	}break;
   case 0x0E:
	{
	   /*显示用户设置界面*/
	   LCDstructure.LCD_Temp = 0 ;	
	    PictrueDisplay(66);
	    PassWordLen = 0;
		for(i = 0; i < 6; i++)
		{
			InputPassWord[i] = 0;
		}
		
	}break;	

//	case 0x0F:	//进入IC卡充值页面
//	{
//
//	    CloseTIM3();
//		OpenCoinMachine();   //开打硬币机
//	   	PictrueDisplay(IC_PAGE);
//		if(UserAct.RechxargeCounter == 0)
//		{	
//	    PlayMusic(VOICE_7); 
//		ReturnMessage = 3 ;	 //充值界面
//		}
//		else
//		{
//	  	 ReturnMessage = 4 ;
//		 PlayMusic(VOICE_8);
//		
//		}
//		DisplayRechangeBills(UserAct.RechxargeCounter);
//	}break;
	case 0x10:   //输入份数按键1-10	   16   --  20
	case 0x11:	 // 17				   17	  --23
	case 0x12:	//					   18	  --26
	case 0x13:	//					   19	  --29
	case 0x14:	//					   20	  --32
	case 0x15:	//					   21	  --23
	case 0x16:	//					   17	  --23
	case 0x17:	//					   17	  --23
	case 0x18:	//					   17	  --23
	case 0x19:	//					   17	  --23
	         {
			  LCDstructure.LCD_Temp = 0 ;
			 /*用户选择的份数*/
			 UserAct.MealCount = cmd - 15;    //选择的份数
			 /*判断盒饭的数目是否大于用户选择的分数*/
			 if(UserAct.MealCount <= DefineMeal[UserAct.MealID - 1].MealCount)	  //判断用机器里面的分数是否>用户的分数
		      {
			    /*计算总价钱=单价*数量*/
		       UserAct.PayShould = UserAct.MealPrice * UserAct.MealCount;
//			   printf("UserAct.PayShould=%d\r\n",UserAct.PayShould);
			   /*判断数量和是否正确*/
			   if(UserAct.MealCount > 0 && UserAct.MealCount <= 5)
			   {
			    /*显示分数和价格*/
						
			//	DisPlayChar(Display_MealNum,sizeof(Display_MealNum),UserAct.MealCount);
		        DisplayMealNum(UserAct.MealCount); //显示份数，  开始默认为一份
				 
			//	DisPlayChar(Display_MealPrice,sizeof(Display_MealPrice),UserAct.PayShould);
		        DisplayMealPrice(UserAct.PayShould);//显示总价格 
			   }
		      }
			 else
			 {
			 /*默认为一份*/
			  UserAct.MealCount = 1 ;
			  /*显示价格和总价格*/
		//	  DisPlayChar(Display_MealNum,sizeof(Display_MealNum),UserAct.MealCount);
		      DisplayMealNum(UserAct.MealCount); //显示份数，  开始默认为一份
		      UserAct.PayShould = UserAct.MealPrice*UserAct.MealCount;
		      DisplayMealPrice(UserAct.PayShould);//显示总价格 
		//	  DisPlayChar(Display_MealPrice,sizeof(Display_MealPrice),UserAct.PayShould);
			 }
		     }
			    /*跳到下一个界面*/
			    CurrentPoint =   2;	
				/*返回值*/
				ReturnMessage =  1;  
		 break;
   	case 0x1A: //付款取消
	{
		LCDstructure.LCD_Temp = 1 ;	   //需要显示温度
		CloseTIM3();
		BackPolls();  
	    PictrueDisplay(HOME_PAGE);
        DisplayMealCount();    	
		ReturnMessage = 0; 
	    CurrentPoint = 0;
	    TemperatureDisplay(Temperature);
	}break;
	case 0x1B: //付款返回
	{	 
	      LCDstructure.LCD_Temp = 0 ;
	     PictrueDisplay((UserAct.MealID-1)*7 + 3);
	     DisplayMealNum(UserAct.MealCount); 	                   //显示份数
		 /*显示客户投币的金额*/
	     DisplayBills(UserAct.PayForBills);	       //显示纸币
		 /*显示客户投币的硬币数*/
		 DisplayCoins(UserAct.PayForCoins);	       //显示硬币
		 /*显示用户刷卡金额*/
		 DisplayCards(UserAct.PayForCards);	       //显示刷卡
		 /*显示总金额*/	  
	     DisplayBills(UserAct.PayForBills);	       //显示纸币
		 /*显示客户投币的硬币数*/
	     DisplayCoins(UserAct.PayForCoins);	       //显示硬币
		 /*显示用户刷卡金额*/
		 /*显示总金额*/		 
	     DisplayMealPrice(UserAct.PayShould);      //显示总价格
		 BackPolls();
//		CloseTIM3(); 
//	    PictrueDisplay(HOME_PAGE);	    
//	    cancel(); 	
//		CloseMoneySystem();
//		TemperatureDisplay(Temperature);
//	    CurrentPoint = 0; 
	
	}break;																			 
	 	case 0x1C: //密码退格键
	{
	    if(PassWordLen > 0)
		{
	        PassWord[PassWordLen] = 0;
	        PassWordLen--;
		    DisplayPassWord(PassWordLen);
		}
		
	}break;

	case 0x1D:	//密码确认
	{
	    LCDstructure.LCD_Temp = 0 ;
	    GetPassWord(PassWord);
    	if(VerifyPassword(PassWord, InputPassWord,6) == 0)
		{
		    /*密码验证错误，进入密码输入界面*/
		    PictrueDisplay(66);
			PassWordLen = 0;
			
		}
		else
		{
		    /*进入 管理员界面*/
		    PictrueDisplay(68);
		    /*显示数目*/
			LCDstructure.LCD_Temp = 1 ;
			DisplayMealCount();
			TemperatureDisplay(Temperature);
			PassWordLen = 0;
		}

	}break;
	case 0x1E: //密码返回
	{
	    LCDstructure.LCD_Temp = 1 ;
		PictrueDisplay(HOME_PAGE);
		TemperatureDisplay(Temperature);
		PassWordLen = 0;
		DisplayMealCount();
	}break;

	 
   	case 0x1F:	          //对应密码 0
	case 0x20:			  //对应密码 1
	case 0x21:			  //对应密码 2
	case 0x22:			  //对应密码 3
	case 0x23:			  //对应密码 4
	case 0x24:			  //对应密码 5
	case 0x25:			  //对应密码 6
	case 0x26:			  //对应密码 7
	case 0x27:			  //对应密码 8
	case 0x28:			  //对应密码 9
	case 0x29:			  //对应密码 A
	case 0x2A:			  //对应密码 B
	case 0x2B:			  //对应密码 C
	case 0x2C:			  //对应密码 D
	case 0x2D:			  //对应密码 E
	case 0x2E:			  //对应密码 F
	{
	    if(PassWordLen < 6)
		{
		    InputPassWord[PassWordLen] = (cmd - 0x1F);
		    PassWordLen++;
			DisplayPassWord(PassWordLen);

		}
	}break;


	case 0x30:	 //第一列+
	{
	   if(CurFloor.FCount < 3)
		{
		    CurFloor.FCount++;
			CurFloor.MealCount++;
			DisplayFColumn(CurFloor.FCount);
		}
	}break;
	case 0x31:   //第二列+
	{
	 	if(CurFloor.SCount < 3)
		{
		    CurFloor.SCount++;
			CurFloor.MealCount++;
			DisplaySColumn(CurFloor.SCount);
		}
	
	}break;
	case 0x32:	 //第三列+
	{
	  if(CurFloor.TCount < 3)
		{
		    CurFloor.TCount++;
			CurFloor.MealCount++;
			DisplayTColumn(CurFloor.TCount);
		}

	}break;
		case 0x33:	 //第一列-
	{  
	  	if(CurFloor.FCount > 0)
		{
		    CurFloor.FCount--;
			CurFloor.MealCount--;
			DisplayFColumn(CurFloor.FCount);
		}

	}break;
	case 0x34:	 //第二列-
	{
	 	if(CurFloor.SCount > 0)
		{
		    CurFloor.SCount--;
			CurFloor.MealCount--;
			DisplaySColumn(CurFloor.SCount);
		}

	}break;
		case 0x35:	 //第三列-
	{
	     if(CurFloor.TCount > 0)
		{
		    CurFloor.TCount--;
			CurFloor.MealCount--;
			DisplayTColumn(CurFloor.TCount);
		}
	}break;
	case 0x36:	 //层数+
	{
	   if(CurFloor.FloorNum < FloorMealNum)
		{
		    CurFloor.FloorNum++;
			DisplayFloor(CurFloor.FloorNum);
			InitSetting();
		}
	}break;
	case 0x37:	 //层数-
	{
	    if(CurFloor.FloorNum > 1)
		{
		    CurFloor.FloorNum--;
			DisplayFloor(CurFloor.FloorNum);
			InitSetting();
		}
	}break;  
	case 0x38:	 //ID号+
	{
	  	    if(CurFloor.MealID < 9)
		{
		    CurFloor.MealID++;
			DisplayMealID(CurFloor.MealID);
			InitSetting();
		}
	}break;
	case 0x39:	 //ID号-
	{
	    if(CurFloor.MealID > 1)
		{
		    CurFloor.MealID--;
//			DisPlayChar(Display_MealID,sizeof(Display_MealID),CurFloor.MealID);
			DisplayMealID(CurFloor.MealID);
			InitSetting();
		}
	}break;
		case 0x3A:	 //放满
	{

        CurFloor.FCount = 3;
		CurFloor.SCount = 3;
		CurFloor.TCount = 3; 
		CurFloor.MealCount = 9;
		DisplayFColumn(CurFloor.FCount);
		DisplaySColumn(CurFloor.SCount);
		DisplayTColumn(CurFloor.TCount);	  
	}break;
		case 0x3B:	 //清空
	{				 
	    CurFloor.FCount = 0;
		CurFloor.SCount = 0;
		CurFloor.TCount = 0; 
		CurFloor.MealCount = 0;
		DisplayFColumn(CurFloor.FCount);
		DisplaySColumn(CurFloor.SCount);
		DisplayTColumn(CurFloor.TCount);
	}break;
		case 0x3C:	 //确认
	{
	   //printf("CurFloor.MealID=%d\r\n",CurFloor.MealID);
		/*把当前第一列设置的参数保存起来*/
	    FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].FCount    = CurFloor.FCount;
		/*把当前第2列设置的参数保存起来*/
		FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].SCount    = CurFloor.SCount;
		/*把当前第3列设置的参数保存起来*/
		FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].TCount    = CurFloor.TCount;
		/*把当前设置餐的ID号保存起来*/
		FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].MealID    = CurFloor.MealID;
		/*把当前设置的餐的总数保存起来*/
		FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].MealCount = CurFloor.MealCount;
		/*把设置餐的层数保存起来*/
		FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].FloorNum  = CurFloor.FloorNum;
		/*九种菜品*/
		for(j = 0; j < 9; j++)	 //9代表餐ID号
		{
		   /*各个餐的数目清零*/
		    DefineMeal[j].MealCount = 0;
			  /*各个餐的列对应的数目清零*/
			 for(i = 0; i < FloorMealNum; i++)
			 {
				DefineMeal[j].Position[i][0] = 0;
			    DefineMeal[j].Position[i][1] = 0;
				DefineMeal[j].Position[i][2] = 0;
			}
			 /*统计各个餐的数目的总和*/
		    for(i = 0; i < FloorMealNum; i++)
		    {														 /*餐的ID是从1-9*/
		        if(FloorMealMessageWriteToFlash.FloorMeal[i].MealID == j + 1)
			    {
				   
			        DefineMeal[j].MealCount      = DefineMeal[j].MealCount + FloorMealMessageWriteToFlash.FloorMeal[i].MealCount;
				    DefineMeal[j].Position[i][0] = FloorMealMessageWriteToFlash.FloorMeal[i].FCount;
				    DefineMeal[j].Position[i][1] = FloorMealMessageWriteToFlash.FloorMeal[i].SCount;
				    DefineMeal[j].Position[i][2] = FloorMealMessageWriteToFlash.FloorMeal[i].TCount;
				//	printf("DefineMeal[%d].Position[%d][0]=%d\r\n",j,i,DefineMeal[j].Position[i][0]);
			    }
		    }
		}
//		    for(i=0;i<90;i++)
//			{
//			 printf("FloorMealMessageWriteToFlash.FlashBuffer[%d]=%d\r\n",i,FloorMealMessageWriteToFlash.FlashBuffer[i]);
//			}
		           WriteMeal();
//		            DataWriteStmFlash(0X080f0002,FloorMealMessageWriteToFlash.FlashBuffer,FloorMealNum*6);
	                for(i=0;i<9;i++)
					;
              //     printf("DefineMeal[%d].MealCount=%d\r\n",i,DefineMeal[i].MealCount);
	}break;
		case 0x3D:	 //取消
	{

		CurFloor.FCount    = 0;
		CurFloor.SCount    = 0;
		CurFloor.TCount    = 0;
		CurFloor.MealID    = 0;
		CurFloor.MealCount = 0;
		CurFloor.FloorNum  = 0;	
		LCDstructure.LCD_Temp = 1 ;
		PictrueDisplay(HOME_PAGE);
		DisplayMealCount();	
		TemperatureDisplay(Temperature); 
	    
	}break;
		case 0x3E:	 //返回
	{
	   
		CurFloor.FCount    = 0;
		CurFloor.SCount    = 0;
		CurFloor.TCount    = 0;
		CurFloor.MealID    = 0;
		CurFloor.MealCount = 0;
		CurFloor.FloorNum  = 0;	 
		LCDstructure.LCD_Temp = 1 ;
	    PictrueDisplay(SETMEAL_PAGE);
		DisplayMealCount();
		TemperatureDisplay(Temperature); 
	}break;
	case 0x42:	 //设置菜单ID
	case 0x43:   
	case 0x44:	 
	case 0x45:	 
	case 0x46:	 
	case 0x47:	 
	case 0x48:	 
	case 0x49:  
	case 0x4A:
	{
	   /*进入设置界面*/
	   LCDstructure.LCD_Temp = 0;
	    PictrueDisplay(INPUTMEAL_PAGE);
		/*用户选择的饭盒ID*/
		UserAct.MealID     = cmd -65;	
		/*当前行列清零*/
		CurFloor.FCount    = 0;
		CurFloor.SCount    = 0;
		CurFloor.TCount    = 0;
		CurFloor.MealID    = UserAct.MealID;
		CurFloor.MealCount = 0;
		CurFloor.FloorNum  = cmd -65;
		DisplayMealID(CurFloor.MealID);
		DisplayFloor(CurFloor.FloorNum); 
		/*判断当前行和列是否已近设置*/
		CurFloor.FCount = FloorMealMessageWriteToFlash.FloorMeal[UserAct.MealID-1].FCount;
		CurFloor.SCount = FloorMealMessageWriteToFlash.FloorMeal[UserAct.MealID-1].SCount;
		CurFloor.TCount = FloorMealMessageWriteToFlash.FloorMeal[UserAct.MealID-1].TCount;
		CurFloor.MealCount = FloorMealMessageWriteToFlash.FloorMeal[UserAct.MealID-1].MealCount;
   		DisplayFColumn(CurFloor.FCount);
		DisplaySColumn(CurFloor.SCount);
		DisplayTColumn(CurFloor.TCount);

	}break;
	case 0x4B:   //放餐设置
	{	 
	   /*点击设置进入放餐设置*/
	   	LCDstructure.LCD_Temp = 0 ;
	    PictrueDisplay(INPUTMEAL_PAGE);
		UserAct.MealID     = 1;		 //客户选择的菜单号
		CurFloor.FCount    = 0;		 //第一列的数目
		CurFloor.SCount    = 0;		 // 第2层的数目
		CurFloor.TCount    = 0;	     //第3层的数目
		CurFloor.MealID    = 1;	     //客户选择的菜单号
		CurFloor.MealCount = 0;		 //当前层的总数
		CurFloor.FloorNum  = 1;		 //当前是哪一层
	    DisplayMealID(CurFloor.MealID);
		DisplayFloor(CurFloor.FloorNum);   
		DisplayFColumn(FloorMealMessageWriteToFlash.FloorMeal[CurFloor.MealID-1].FCount);
		DisplaySColumn(FloorMealMessageWriteToFlash.FloorMeal[CurFloor.MealID-1].SCount);
		DisplayTColumn(FloorMealMessageWriteToFlash.FloorMeal[CurFloor.MealID-1].TCount);
           
   
   }break;
  	case 0x4C:	 //放餐设置取消
	{
	   /*显示主要，数目和温度*/
	   	LCDstructure.LCD_Temp = 1 ;	
	    PictrueDisplay(HOME_PAGE);
	    DisplayMealCount();
		TemperatureDisplay(Temperature);	
	}break;
	case 0x4D:	 //IC卡充值返回
	{
       	if(BillActionFlag == 1)
		{
	    CurrentPoint = 0 ;
	    ReturnMessage = 6 ; 
		PictrueDisplay(HOME_PAGE);
	    DisplayMealCount();
		TemperatureDisplay(Temperature);
		}
		else
		ReturnMessage = 4; 
	}break;
	case 0x4E:	 //IC卡充值确认 
	{
		//写数据进卡
		ReturnMessage = 5;

	}break;
	case 0x4F:	 //IC卡继续充值
	{
	//开纸币机
	 PlayMusic(VOICE_7);
	 DisplayRechangeBills(UserAct.RechxargeCounter);
 	 ReturnMessage = 3 ;  
 	 
	}break;
	case 0x50:	//温度开关按钮
	{
	 
	}break;
	 
	case 0x51 :   /*选择现金支付*/
	{
	 	   CurrentPoint = 3; 
		   ReturnMessage =  1;
	}
	break ;
	case 0x52:  /*银行卡支付*/
	{
	      ReturnMessage =  1;
		  CurrentPoint = 7; 

	} break;
	case 0x53:  /*选择深圳通支付*/
	{

	     CurrentPoint =  8;
       ReturnMessage =  1;
	} break;
	case 0x54:  /*是否打印小票--是*/
	{ 
	    /*客户要选择打票*/
		
         UserAct.PrintTick = 1 ;
		 ReturnMessage =  2;
	 
	} break;
   	case 0x55:  /*是否打印小票--否*/
	{	 
	     /*客户不选择打票*/
	      UserAct.PrintTick = 2; 
		    ReturnMessage =  2;
	} break;
	 case 0x56:  /*退币功能*/
	{
	     ReturnMessage =  9;
	} break;
	case 0x57 :	 //选择分数返回
	 {
	   	LCDstructure.LCD_Temp = 1 ;	
	    BackPolls();
	  	CloseTIM3(); 
	    PictrueDisplay(HOME_PAGE);	    
	    cancel(); 	
		CloseMoneySystem();
	    TemperatureDisplay(Temperature);
	    CurrentPoint = 0; 
	 }
	 break;
  default : break;
} 
 //printf("LCDstructure.LCD_Temp=%d\r\n",LCDstructure.LCD_Temp);
return ReturnMessage ;

 }
