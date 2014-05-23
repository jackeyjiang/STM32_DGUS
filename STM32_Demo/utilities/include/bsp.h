#ifndef _bsp_H
#define _bsp_H

/*******主状态***********/
#define current_temperature  0x01
#define waitfor_money        0x02
#define payment_success      0x03
#define hpper_out            0x04
#define print_tick           0x05
#define meal_out             0x06
#define data_upload          0x07
#define data_record          0x08
#define erro_hanle           0x09

/**********等待餐品状态************/

#define takeing_meal   0x01 //正在取餐
#define tookone_meal   0x02 //已取一份餐
#define tookkind_meal  0x03 //已取一种餐
#define takeafter_meal 0x04 //取餐完毕
#define takemeal_erro  0x05 //取餐错误

/**********机内硬币保留数*********/
#define coins_remain   50  //保留50个

/**********异常******************/
#define outage_erro      0x01 //断电
#define sdcard_erro      0x02 //SD卡存储异常
#define billset_erro     0x03 //纸币机异常
#define coinset_erro     0x04 //投币机
#define coinhooperset_erro   0x05 //退币机
#define coinhooperset_empty  0x06 //找零用光
#define printer_erro     0x07 //打印机异常
#define cardread_erro    0x08 //读卡器异常
#define network_erro     0x09 //网络异常
#define signin_erro      0x0a //签到异常
#define cardchck_erro    0x0b //深圳通银行卡签到异常



#define X_timeout        0x10 //x轴传感器超时
#define X_leftlimit      0x11 //马达左动作极限输出
#define X_rightlimit     0x12 //马达右动作极限输出
#define mealtake_timeout 0x13 //取餐口传感器超时
#define Y_timeout        0x14 //y轴传感器超时
#define link_timeout     0x15 //链接超时
#define Z_timeout        0x16 //z轴传感器超时
#define Z_uplimit        0x17 //z轴马达上动作超出
#define Z_downlimit      0x18 //z马达下动作超出
#define solenoid_timeout 0x19 //电磁阀超时
#define Eeprom_erro      0x1a //eeprom 异常
#define SendUR6Erro      0x1b  //发送数据异常或超时
#define GetMealError     0x1c  //机械手5秒取不到餐
#define MealNoAway       0x1d   //餐在出餐口20秒还未被取走

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "MsgHandle.h"	         //处理关于点餐的数据
#include "vioce.h"			       //声音函数
#include "serialscreen.h"	       //串口屏函数
#include "protocol.h"            //机械手子程序
//#include "led.h"			       //led灯函数
//#include "ICcard.h"			       //ic卡函数
#include "timer2.h" 				 //用于倒计时
#include "timer4.h"			     //定时器4 用于显示温度的 没多少次发送一个信号给从机得到温度。然后在显示。
#include "timer3.h"		           //用于秒表
#include "timer6.h"				   //用于采集温度
#include "delay.h"			       //延时
#include "uart2.h"           //深圳通银联卡接口
#include "uart3.h"				   //串口屏接口
#include "uart4.h"           //纸币机接口
#include "uart6.h"           //机械手接口
#include "pvd.h"
#include "eeprom.h"
#include "intel_flash.h."

//#include "led.h"				   //led灯
#include "coin.h"				     //硬币机
#include "mini_hopper.h"     //退币机
#include "stdio.h"				   //标准库函数	 
#include "string.h"				   //标准库函数	
#include "stdint.h"          //定义各类型变量
#include "bill.h"				     //纸币机
#include "printer.h"				 //打印机
#include "uart1.h"				   //打印机接口
#include "ff.h"
#include "integer.h"
#include "diskio.h"
	
#include "rtc.h"
#include "sd.h"

#include "spi_flash.h"  
#include "szt_gpboc.h"       //深圳通、银联

#include "network_handle.h"    //后台网络数据的传输
#include "uart5.h"				     //网络数据传输接口

typedef enum
{
  Status_OK= 0 ,	   //函数操作成功 返回 0
  Status_Error ,	  // 函数出错  返回1
	Status_Action 	  // 函数还执行 返回 2 
} status ;




typedef struct 
{
  uint8_t   F_RX_Length ;   //接受数据长度标志
  uint8_t   Response[6];     //ACK数组
  uint8_t   F_RX1_SYNC;     //是否开始接受有效数据
  uint8_t   PacketData[1000]; //接受的数据
  uint8_t   PacketDCS;       //校验码
  uint16_t  DataLength;    //数据长度
  uint16_t   RX_NUM ;	    //定位
  uint8_t   Status   ;		 //状态
  uint8_t   Checkoutright ;	  //数据校验是否成功
  uint8_t   ACKStatus;
}USART_StatusTypeDef;

extern  USART_StatusTypeDef  STATUS ;


extern uint8_t   exitflag;
extern uint8_t   CoinCountFlag;
extern uint8_t   TemperatureFlag  ;
extern uint8_t   BillActionFlag;      //表示纸币机在收钱中。这个时候不能关闭纸币机
extern uint8_t   CurrentPoint ;
extern uint8_t   CoinFlag;			 //用于退币机的标志
extern uint8_t   TemperatureCur;
extern uint8_t   Current ;    //主状态
extern uint8_t   CurrentPointer; //餐品出餐状态
extern uint16_t  erro_flag;  //错误状态
extern int32_t   erro_record;  //错误标记位

bool CloseCashSystem(void);
bool OpenCashSystem(void);	
bool FindMeal(MealAttribute *DefineMeal); 
// uint8_t CanRecvCmd(uint8_t p);
unsigned char  WaitPayMoney(void);
uint8_t WaitMeal(void);
void hardfawreInit(void);
void  WaitTimeInit(uint8_t *Time);
void StateSend(void);
void DataUpload(char takemeal_flag);
void PrintTickFun(unsigned char *PrintTickFlag);
void PowerupAbnormalHandle(int32_t erro_record);
void AbnormalHandle(uint16_t erro);
void PollAbnormalHandle(void);
void SaveUserData(void);
void ReadUserData(void);
void AcountCopy(void);
void PayBackUserMoney(void);
// void LcdHandler(void);
// void  TempHandler(void);
// void ClearingFuntion(void) ;//退签上送
#endif
