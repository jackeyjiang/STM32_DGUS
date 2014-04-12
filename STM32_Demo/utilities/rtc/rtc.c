#include "rtc.h"
#include "stdio.h"
#include "printer.h"
#include "delay.h"
Struct_TD TimeDate ;
static RTC_TimeTypeDef RTC_TimeStructure;
static RTC_DateTypeDef RTC_DateStructure;	// RTC_DateStruct
static RTC_InitTypeDef RTC_InitStructure;
static RTC_AlarmTypeDef  RTC_AlarmStructure;
static __IO uint32_t AsynchPrediv=0,SynchPrediv=0;

u8 MyRTC_Init(void)
{
  	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)//未设置时间
  	{  
    	RTC_Config();
    	RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;//RTC 异步除数 （<0X7F）
    	RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;//RTC 同步除数 （<0X7FFF）
    	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;//24小时制
    	if(RTC_Init(&RTC_InitStructure) == ERROR)return 1;
    	RTC_TimeRegulate();//设置时间 
  	}
  	else//已设置时间
  	{   
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	    PWR_BackupAccessCmd(ENABLE);//使能RTC操作
	    RTC_WaitForSynchro();//等待RTC APB寄存器同步
	    RTC_ClearFlag(RTC_FLAG_ALRAF);//清除RTC闹钟标志
//	    EXTI_ClearITPendingBit(EXTI_Line17);//清除中断线17标志（内部连接至RTC闹钟）
//		printf("RTC_TimeStructure.RTC_Hours=%d\r\n",RTC_TimeStructure.RTC_Hours);
//   	printf("RTC_TimeStructure.RTC_Minutes=%d\r\n",RTC_TimeStructure.RTC_Minutes);
//    	printf("RTC_TimeStructure.RTC_Seconds=%d\r\n",RTC_TimeStructure.RTC_Seconds) ;
  	}
	return 0;
}
  /*******************************************************************************
 * 函数名称:RTC_GetTime_HM  得到时间                                                               
 * 描    述:void                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:无                                                                  
 * 修改日期:2013年4月20日                                                                    
 *******************************************************************************/
void RTC_Config(void)
{
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  	PWR_BackupAccessCmd(ENABLE);//使能RTC操作
  	RCC_LSEConfig(RCC_LSE_ON);//使用外部晶振
  	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);//等待外部晶振准备好
  	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择RTC时钟源  
  	SynchPrediv=0xFF;
  	AsynchPrediv=0x7F; 
  	RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
  	RTC_WaitForSynchro();//等待RTC APB寄存器同步
//	RTC_WakeUpCmd(ENABLE );
}

  /*******************************************************************************
 * 函数名称:RTC_GetTime_HM  得到时间                                                               
 * 描    述:void                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:无                                                                  
 * 修改日期:2013年4月20日                                                                    
 *******************************************************************************/
void RTC_GetTime_HM(uint16_t *time)
{
 	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
//	Hour   =RTC_TimeStructure.RTC_Hours;
//	Min    =RTC_TimeStructure.RTC_Minutes;

   *time=RTC_TimeStructure.RTC_Hours<<8 | RTC_TimeStructure.RTC_Minutes;

	printf("Time_Save\r\n");
}

  /*******************************************************************************
 * 函数名称:RTC_TimeShow  时间显示                                                               
 * 描    述:void                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:无                                                                  
 * 修改日期:2013年4月20日                                                                    
 *******************************************************************************/
void RTC_TimeShow()
{
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	TimeDate.Hours    =RTC_TimeStructure.RTC_Hours;
	TimeDate.Minutes  =RTC_TimeStructure.RTC_Minutes;
	TimeDate.Senconds =RTC_TimeStructure.RTC_Seconds;
	TimeDate.WeekDay  =RTC_DateStructure.RTC_WeekDay;
	TimeDate.Date     =RTC_DateStructure.RTC_Date;
	TimeDate.Month    =RTC_DateStructure.RTC_Month;
	TimeDate.Year     =RTC_DateStructure.RTC_Year;
}




  /*******************************************************************************
 * 函数名称:RTC_TimeRegulate  设置时间                                                                
 * 描    述:void                                                                   
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:无                                                                  
 * 修改日期:2013年4月20日                                                                    
 *******************************************************************************/
 
  u32 tmp_hh = 0x17, tmp_mm = 0x3b, tmp_ss = 0x01;
  u32 tmp_yy = 0x09, tmp_m2 = 0x01, tmp_dd = 0x01,tmp_ww=0x01;
void RTC_TimeRegulate(void)
{
	  //printf("Set time\r\n");
	  RTC_TimeStructure.RTC_H12= RTC_HourFormat_24;		
   	RTC_TimeStructure.RTC_Hours = tmp_hh;
    RTC_TimeStructure.RTC_Minutes = tmp_mm;
    RTC_TimeStructure.RTC_Seconds = tmp_ss;	
  	if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure)!=ERROR)
  	RTC_WriteBackupRegister(RTC_BKP_DR0,0x32F2);
		
  	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
  	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = tmp_hh;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = tmp_mm;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = tmp_ss;
  	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
  	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
  	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);//配置RTC闹钟寄存器
  	RTC_ITConfig(RTC_IT_ALRA, ENABLE);//使能闹钟A的中断
  	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);//使能闹钟A
	/***********************************
	一下是配置年月日
	**************************************/		  
   RTC_DateStructure.RTC_WeekDay = tmp_ww;          // RTC_Weekday_Monday;
   RTC_DateStructure.RTC_Date = tmp_dd;
   RTC_DateStructure.RTC_Month =  tmp_m2;           //RTC_Month_January;
   RTC_DateStructure.RTC_Year = tmp_yy;
   if(RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure)!=ERROR)
   RTC_WriteBackupRegister(RTC_BKP_DR0,0x32F2);	  	    
   RTC_DateStructInit(&RTC_DateStructure);	  //
}

void RTC_AlarmShow()
{
  	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
}


