 /**
  ******************************************************************************
  * 文件: test_lcd.c
  * 作者: jackey
  * 版本: V4.0.3
  * 描述: 离职之后的最后一版，已经基本稳定。
  *       如果要增加微信支付，需要将网络通信部分全部修改，作者有将网络重写。
  ******************************************************************************
  *
  *                  	作为STM32F407开发的模板
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
uint8_t Current= 0x00;        //状态机变量
uint8_t LinkMachineFlag =0;	  //与机械手连接标志，0表示没连接，1表示连接
bool   cash_limit_flag=false;  //禁止打开现金收银系统
uint8_t waitmeal_status=0;    //等待取餐状态
uint32_t erro_record=0x00000000; //错误标记位
uint16_t VirtAddVarTab[NB_OF_VAR] = {0x0001, 0x0002, 0x0003,};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};
uint16_t VarValue = 0;
uint32_t sellsecond_remain_old=0;
uint8_t cid_data[50]={0};
int main(void)
{
  hardfawreInit(); //硬件初始化
	if(SD_Initialize()) AbnormalHandle(sdcard_erro);	//SD卡检测*/ 
	PageChange(OnlymachieInit_interface);
  if(erro_record&(1<<arm_limit)) 
  {
		AbnormalHandle(arm_limit);//需要处理数据上传的断电
	}
  else 
  {
    delay_ms(1000);    //延时优化机械手复位
	  OnlymachieInit();  //机械手初始化
  }
	PageChange(SignInFunction_interface);
  if(!EchoFuntion(RTC_TimeRegulate)) 
    AbnormalHandle(network_erro);  /*从网络获得时间,更新本地时钟*/
  else
    SetScreenRtc();/*设置屏幕的RTC*/
	MachineHeatSet();//根据时间判断是否开启加热
	PageChange(SignInFunction_interface);
	if(!SignInFunction())       
		AbnormalHandle(signin_erro); /*网络签到*/
  if(signin_state == 3) DispMenu();
  else  DispMenuNone(); 
	Current= current_temperature;
  DispLeftMeal();             //显示餐品数据	
	ErrRecHandle();          //用户数据断电的数据处理与上传
	PageChange(Szt_GpbocAutoCheckIn_interface);
	delay_ms(1000);
  SendtoServce();          //上传前七天的数据
	//if(!Szt_GpbocAutoCheckIn()) AbnormalHandle(cardchck_erro);//深圳通签到
	Szt_GpbocAutoCheckIn();
 	if((CoinsTotoalMessageWriteToFlash.CoinTotoal<50)||( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)== 0)) 	
 	  AbnormalHandle(coinhooperset_erro); //当机内硬币数小于50 和 硬币机传感器线 报错 
	StatusUploadingFun(0xE800); //开机加入正常上传
	PageChange(Logo_interface);	
	if(!CloseCashSystem())
	{		
		if(!CloseCashSystem())AbnormalHandle(billset_erro);	
	}
  delay_ms(200);
	PageChange(Menu_interface); //显示选餐界面
  PageChange(Menu_interface); //显示选餐界面

	while(1)
  {
		DealSeriAceptData();
		manageusart6data();  
    switch(Current)
	  {
	    case current_temperature: /*温度处理函数*/
			{
        if(sellsecond_remain!=sellsecond_remain_old)
        {
          selltime_hour_r= sellsecond_remain/3600;
          selltime_minute_r= (sellsecond_remain%3600)/60;
          selltime_second_r= (sellsecond_remain%3600)%60;
          VariableChage(wait_sellmeal_hour,selltime_hour_r);
          VariableChage(wait_sellmeal_minute,selltime_minute_r);
          VariableChage(wait_sellmeal_second,selltime_second_r);
          sellsecond_remain_old= sellsecond_remain;
          if(sellsecond_remain==0) 
          {
            PageChange(Menu_interface);
            sellmeal_flag= true;           
          }
        }
				StateSend(); //状态上送
				MachineHeatSet();//设置加热或者制冷
				if((LinkTime==1)||(LinkTime==2)||(LinkTime==3))
				{
				  VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal+CoinTotoal_t);//显示机内硬币数
					VariableChage(coins_back,Coins_cnt);
				}
				if(LinkTime >=5)
				{
					if(OrderSendLink()==0)//链接失败直接报错
          {
						if(OrderSendLink()==0)
						{
							erro_record |= (1<<link_timeout);
							Current= erro_hanle;
            }
          }
				}
				//显示倒计时,可以更具标记为对选餐倒计时进行更新
				if(UserActMessageWriteToFlash.UserAct.MealID)
				{
					if(WaitTime==0)
					{					 
						PageChange(Menu_interface);//超时退出用户餐品数量选择界面
						WaitTimeInit(&WaitTime);
					}
          else
          {
            if(WaitTime%5==1) 
            {
              if(pageunitil==MealNumChoose_interface)
              {
                if(sellmeal_flag) PlayMusic(VOICE_1); //只有当在可以售餐的时候播放声音     
              }                
            }             
          }
				}
        if(machinerec.redoor ==0) //开门状态
        { 
          machinerec.rerelative = 0;
          PageChange(DoorOpened_interface);//显示开门界面，不能进行任何操作
        }
        else if(machinerec.redoor ==1) //关门状态
        {
          if(machinerec.rerelative ==1) //当机械手在适当的位置，则切换为售餐界面
          {
            PageChange(Menu_interface);
            //PageChange(pageunitil); /*这个可以有，直接转到当前页面*/
            machinerec.redoor = 2; //将门的状态改为其他状态
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
          payfor_meal= UserActMessageWriteToFlash.UserAct.MoneyBackShould; /*测试，有一些问题，当无币可退的时候需要将应该退币的数量加上*/
          UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already =0;
          UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st=0;
          UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd=0;
          UserActMessageWriteToFlash.UserAct.PrintTick=0x00000000; //初始为0，可以使打印小票降为一次
/*------------------------------强行答应小票开头----------------------------------------
          if(UserActMessageWriteToFlash.UserAct.PrintTick==0x00000000)
          {
            UserActMessageWriteToFlash.UserAct.PrintTick= 0x00000001;
            PrintTickFun(&UserActMessageWriteToFlash.UserAct.PrintTick);
            CloseTIM4();
          }
--------------------------------强行答应小票结尾----------------------------------------*/
					UserActMessageWriteToFlash.UserAct.Cancle= 0x00; //以免出错
          if(UserActMessageWriteToFlash.UserAct.MoneyBack>0) //当有币要找时进入退币
					  Current= hpper_out;
          else
            Current= meal_out;
					UserActMessageWriteToFlash.UserAct.Meal_takeout= 0;	
          VariableChage(mealout_already,UserActMessageWriteToFlash.UserAct.Meal_takeout);	//UI显示					
					if(UserActMessageWriteToFlash.UserAct.PayType == '1') //现金
					{
						CloseCoinMachine();			    //关闭投币机	
						delay_ms(500);
						if(!CloseCashSystem()){CloseCashSystem();};// printf("cash system is erro1\r\n");  //关闭现金接受
					}
          TrackDateToBuff(); //传输将数据保存刷卡器的数据数组中
			  }
				SaveUserData();
			}break;
      case hpper_out:	 /*退币状态:该程序的前提是在一次退10个硬币，不会出现退币机空转的情况*/
			{
				uint16_t i=0,cnt_t=0;
			  uint16_t coins_time=0;
				OldCoinsCnt= CoinsTotoalMessageWriteToFlash.CoinTotoal; //记录之前还未退币时机内的硬币数
        NewCoinsCnt= UserActMessageWriteToFlash.UserAct.MoneyBackShould; //记录退币的应该要退的钱，可以对断电的时候数据进行记录
        if(CoinsTotoalMessageWriteToFlash.CoinTotoal>=UserActMessageWriteToFlash.UserAct.MoneyBack)	//条件是机内的硬币>应退的币
				{					
					if(UserActMessageWriteToFlash.UserAct.MoneyBack >0) //需要找币的时候进入
					{
						coins_time= (UserActMessageWriteToFlash.UserAct.MoneyBack/10); 
						cnt_t =  UserActMessageWriteToFlash.UserAct.MoneyBack%10;		
						UserActMessageWriteToFlash.UserAct.MoneyBack= 0;			
						for(i=0;i<coins_time+1;i++) //一次退币10个，
						{
							if(i!=coins_time)
							{
								UserActMessageWriteToFlash.UserAct.MoneyBack+= SendOutN_Coin(10);		
							}
							else
							{
								if(cnt_t>0)
									UserActMessageWriteToFlash.UserAct.MoneyBack+= SendOutN_Coin(cnt_t);	
								else
									break;
							}
						} 							
						if(ErrorType ==1) //出错再次发送退币 
						{
              erro_record |= (1<<coinhooperset_empty);
							delay_ms(1500); 
							UserActMessageWriteToFlash.UserAct.MoneyBack= SendOutN_Coin(UserActMessageWriteToFlash.UserAct.MoneyBack);//还有多少币未退
							if(ErrorType ==1)//退币机无币错误,直接进入错误状态
							{
								erro_record |= (1<<coinhooperset_empty);	
								if(erro_record>=(1<<X_timeout))//这种情况下是第一次退币成功的情况下 如果是机械手异常，计算应该要退的钱 和 已退的钱 直接进行错误处理
								{
                 //计算已经退了多少钱 
                  UserActMessageWriteToFlash.UserAct.MoneyBackAlready= UserActMessageWriteToFlash.UserAct.MoneyBackShould- UserActMessageWriteToFlash.UserAct.MoneyBack;                                  
                 //计算第二次退了多少币
                  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd= UserActMessageWriteToFlash.UserAct.MoneyBackAlready- UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;
                  
                 //需要上传交易金额 = 就是没有出的餐品的总价+
                   payfor_meal+=(UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.LastMealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd\
                                +UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.LastMealPrice_4th);
                                  	                                 		           
									Current = erro_hanle;	
								}
                else if(UserActMessageWriteToFlash.UserAct.Cancle== 0x01)//如果是取消购买,出错进入错误处理
                {
                  UserActMessageWriteToFlash.UserAct.Cancle= 0x00;
                  Current = erro_hanle;	
                }
								else /*交易成功，机械手没有错误*/
								{
                  //计算上传的已付的钱= 现在该退的钱+ （之后在正长出餐了一个餐品的时候补齐当前餐品的价格）
                  payfor_meal= UserActMessageWriteToFlash.UserAct.MoneyBackShould;
                  //计算总的已退的钱
									UserActMessageWriteToFlash.UserAct.MoneyBackAlready= UserActMessageWriteToFlash.UserAct.MoneyBackShould- UserActMessageWriteToFlash.UserAct.MoneyBack;
                  //第一次退币的数据保存
                  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st = UserActMessageWriteToFlash.UserAct.MoneyBackAlready;     
                  //将第一次退币的数据赋值给UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already用来上传
                  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already = UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;                      
									Current= meal_out;//找币错误的时候还是继续出餐
								}
								SaveUserData();
								break;
							}
							else  //退币OK ,还是进行判断
              {
                erro_record &= ~(1<<coinhooperset_empty);//退币错误标记位清0 
								if(erro_record>=(1<<X_timeout))//如果是机械手异常，直接进行错误处理
								{
                  //计算已经退了多少钱 
                  UserActMessageWriteToFlash.UserAct.MoneyBackAlready = UserActMessageWriteToFlash.UserAct.MoneyBackShould- UserActMessageWriteToFlash.UserAct.MoneyBack;      
                 //计算第二次退了多少币
                  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd= UserActMessageWriteToFlash.UserAct.MoneyBackAlready- UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;
                 //需要上传交易金额 = 就是没有出的餐品的总价+(未上传的钱币数)
                   payfor_meal+= (UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.LastMealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd\
                                 +UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.LastMealPrice_4th);	          
									Current = erro_hanle;	
								}
                else if(UserActMessageWriteToFlash.UserAct.Cancle== 0x01)//如果是取消购买,出错进入错误处理
                {
                  ClearUserBuffer();//清空用户数据
                  SaveUserData();//保存用户数据  
                  Current = current_temperature;	
                }
								else /*正常退币，机械手没有错误*/
								{
                  //计算上传的已付的钱
                  payfor_meal= UserActMessageWriteToFlash.UserAct.MoneyBackShould;
                  //计算总的已退的钱
									UserActMessageWriteToFlash.UserAct.MoneyBackAlready= UserActMessageWriteToFlash.UserAct.MoneyBackShould- UserActMessageWriteToFlash.UserAct.MoneyBack;
                  //第一次退币的数据保存
                  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st = UserActMessageWriteToFlash.UserAct.MoneyBackAlready;		
                  //将第一次退币的数据赋值给UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already用来上传
                  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already = UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;    
                  //找币错误的时候还是继续出餐
                  Current= meal_out;
								}               
                SaveUserData(); 
              }
						}
            else //退币OK ,还是进行判断
            {
              erro_record &= ~(1<<coinhooperset_empty);//退币错误标记位清0 
              if(erro_record>=(1<<X_timeout))//如果是机械手异常，直接进行错误处理
              {
               //计算已经退了多少钱 
                UserActMessageWriteToFlash.UserAct.MoneyBackAlready = UserActMessageWriteToFlash.UserAct.MoneyBackShould- UserActMessageWriteToFlash.UserAct.MoneyBack;                                
               //计算第二次退了多少币
                UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd= UserActMessageWriteToFlash.UserAct.MoneyBackAlready- UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;
               //需要上传交易金额 = 就是没有出的餐品的总价+（没有上传的钱的数）
                payfor_meal+= (UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.LastMealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd\
                              +UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.LastMealPrice_4th);	                 
                Current = erro_hanle;	
              }
              else if(UserActMessageWriteToFlash.UserAct.Cancle== 0x01)//如果是取消购买,出错进入错误处理
              {
                ClearUserBuffer();//清空用户数据
                SaveUserData();//保存用户数据  
                Current = current_temperature;	
              }
              else /*退币成功，机械手没有错误*/
              {   
                //计算上传的已付的钱，在取餐的时候有累加
                payfor_meal= UserActMessageWriteToFlash.UserAct.MoneyBackShould;
                //计算总的已退的钱
                UserActMessageWriteToFlash.UserAct.MoneyBackAlready = UserActMessageWriteToFlash.UserAct.MoneyBackShould- UserActMessageWriteToFlash.UserAct.MoneyBack;
                //第一次退币的数据保存
                UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st = UserActMessageWriteToFlash.UserAct.MoneyBackAlready;
                //将第一次退币的数据赋值给UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already用来上传
                UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already = UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;
                //找币错误的时候还是继续出餐
                Current= meal_out;
              }
							SaveUserData();
              break;                
            }              
          }   
          else  //无需找币时，直接进入出餐
          {
            //第一次退币的数据保存
            UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st = UserActMessageWriteToFlash.UserAct.MoneyBackAlready;
            //将第一次退币的数据赋值给UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already用来上传
            UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already = UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;
            Current= meal_out; 
            SaveUserData();
            break;            
          }
        }          
				else //机内硬币不够时,MoneyBack不变
				{ 
					if(erro_record>=(1<<X_timeout))//如果是机械手异常，直接进行错误处理
					{
           //计算已经退了多少钱 
            UserActMessageWriteToFlash.UserAct.MoneyBackAlready = UserActMessageWriteToFlash.UserAct.MoneyBackShould- UserActMessageWriteToFlash.UserAct.MoneyBack;
           //计算第二次退了多少币
            UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd= UserActMessageWriteToFlash.UserAct.MoneyBackAlready- UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;                       
           //需要上传交易金额 = 就是没有出的餐品的总价
            payfor_meal+= (UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.MealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.MealPrice_2nd
                          +UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.MealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.MealPrice_4th);	                 
            Current = erro_hanle;	
					}
          else
          {
            //第一次退币的数据保存
            UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st = UserActMessageWriteToFlash.UserAct.MoneyBackAlready;
            //将第一次退币的数据赋值给UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already用来上传
            UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already = UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;
            //找币错误的时候还是继续出餐	
            Current= meal_out; 
          }
					erro_record |= (1<<coinhooperset_empty);
				}
				SaveUserData();
			}break;
	    case meal_out:	 /*出餐状态：正在出餐，已出一种餐品，出餐完毕*/
			{
        uint8_t meal_kind=0;//餐品种类变量
        uint8_t meal_empty_cnt=0;
        uint8_t check_cnt=0; //刷卡器上传计数
        uint8_t check_cnt_t=0;
				waitmeal_status= WaitMeal();  
			  if(waitmeal_status == takeafter_meal) //出餐完毕
				{
					if(UserActMessageWriteToFlash.UserAct.MoneyBack>0) //出餐完毕如果UserActMessageWriteToFlash.UserAct.MoneyBack>0 直接进入错误处理
					{
						erro_record |= (1<<coinhooperset_empty);
						Current = erro_hanle; 
					}
					else
					{
						/*无措状态清楚购物车*/
						ClearUserBuffer(); 
            SaveUserData();			
						/*无错状态进入到售餐界面*/
						PageChange(Menu_interface);
					  Current = current_temperature;
					}
          for(meal_kind=0;meal_kind<MealKindTotoal;meal_kind++)
          {
            if(DefineMeal[meal_kind].MealCount==0)
              ++meal_empty_cnt;
          }
          if(meal_empty_cnt==MealKindTotoal)
          {
            PageChange(Menu_interface+1);//显示售罄界面
            SendtoServce(); //当售完餐品后，需要在之前没有上传的数据上传
            while(1)
            {
              if(1== UpperGpboc()) ++check_cnt; 
              if(1== SztAutoSend()) ++check_cnt; 
              check_cnt_t++;
              if((check_cnt_t>10)||(check_cnt>=2))//超时退出主循环 成功退出主循环
              {
                check_cnt=0;
                check_cnt_t=0;
                break; 
              }
            }
          }
          else
          {
            meal_empty_cnt=0;
          }
				}
				else if(waitmeal_status == tookone_meal)  //取完一个餐品
				{
          payfor_meal+= GetMealLastPrice(UserActMessageWriteToFlash.UserAct.MealID,1);
					Current = data_upload;					
				}
        else if(waitmeal_status == takemeal_erro)
				{
					//计算总的应退币数	
				  UserActMessageWriteToFlash.UserAct.MoneyBackShould+= (UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.LastMealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd\
                                                               +UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.LastMealPrice_4th);
          //计算还有多少币没有退
          UserActMessageWriteToFlash.UserAct.MoneyBack+= (UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.LastMealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd\
                                                         +UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.LastMealPrice_4th);
					PageChange(Err_interface);
					UserActMessageWriteToFlash.UserAct.Cancle= 0x01;
				  /*如果有币进入退币，如果无币进入错误处理*/
					if(erro_record&(1<<coinhooperset_empty))
					{
            //计算要上传的交易金额
            payfor_meal+= (UserActMessageWriteToFlash.UserAct.MealCnt_1st*UserActMessageWriteToFlash.UserAct.LastMealPrice_1st+UserActMessageWriteToFlash.UserAct.MealCnt_2nd*UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd
                          +UserActMessageWriteToFlash.UserAct.MealCnt_3rd*UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_4th*UserActMessageWriteToFlash.UserAct.LastMealPrice_4th);	              
            //计算要上传的已退币的金额不变
            UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd= UserActMessageWriteToFlash.UserAct.MoneyBackAlready- UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st;
            /*总的需要上传的币*/  /*之前未被上传的应退的币*/        /*计算新增的应退的币*/                                                                                                         
						Current = erro_hanle;
					}
          else 
					{
					  Current = hpper_out;
					}						
				}
        SaveUserData();			        
			}break;
	    case data_upload:	 /*数据上传*/
	    {  		
        DataUpload(Success);//根据UserActMessageWriteToFlash.UserAct.ID 判断需要上传的数据
			  Current = meal_out;		
				SaveUserData();
	    }break ;
      case erro_hanle: /*异常状态处理:需要对程序一直死在错误处理*/
      {
				//获取当前时间，并显示
				DisplayRecordTime();
        if(erro_record==(1<<coinhooperset_empty))
        {
          AbnormalHandle(coinhooperset_empty);
          erro_record&=~(1<<coinhooperset_empty);
          cash_limit_flag=true;
          PageChange(Menu_interface);
          StatusUploadingFun(0xE800); //处理后返回正常
          SaveUserData();
          Current = current_temperature;
        }
        else
        {
          PollAbnormalHandle(); //异常处理 一直处于异常处理程序
          PageChange(Logo_interface);
          StatusUploadingFun(0xE800); //处理后返回正常
          SaveUserData();
          while(1);	         
        }          
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

