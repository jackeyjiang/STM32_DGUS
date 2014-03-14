#ifndef _bsp_H
#define _bsp_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
//#include "MsgHandle.h"	           //处理触摸屏时间
//#include "vioce.h"			       //声音函数
//#include "serialscreen.h"	       //串口屏函数
//#include "led.h"			       //led灯函数
//#include "ICcard.h"			       //ic卡函数
//#include "timer2.h" 				//用于倒计时
//#include "timer4.h"			       //定时器4 用于显示温度的 没多少次发送一个信号给从机得到温度。然后在显示。
//#include "timer3.h"		           //用于秒表
//#include "timer6.h"				   //用于采集温度
#include "delay.h"			       //延时
#include "uart3.h"				   //串口屏
//#include "led.h"				   //led灯
//#include "coin.h"				   //硬币机
//#include "MS_d245.h"			   //打印机
#include "stdio.h"				   //标准库函数	 
#include "string.h"				   //标准库函数	 
//#include "bill.h"				   //纸币机头文件
//#include "printf.h"				   //答应函数
//#include "vioce.h"				   //声音初始化
#include "ff.h"
#include "integer.h"
#include "diskio.h"
	
#include "rtc.h"
#include "sd.h"
//#include "eeprom.h"
//#include "can.h"
//#include "ds18b20.h"
//#include "japan.h"
//#include "uart5.h"
//#include "spi_flash.h"            //w25q64头文件
//#include "szt_gpboc.h"       //深圳通、银联


//   typedef enum
//   {
//      Status_OK= 0 ,	   //函数操作成功 返回 0
//	    Status_Error ,	  // 函数出错  返回1
//	    Status_Action 	  // 函数还执行 返回 2 
//
//   } status ;
//
//
//
// typedef struct 
//{
//  uint8_t   F_RX_Length ;   //接受数据长度标志
//  uint8_t   Response[6];     //ACK数组
//  uint8_t   F_RX1_SYNC;     //是否开始接受有效数据
//  uint8_t   PacketData[1000]; //接受的数据
//  uint8_t   PacketDCS;       //校验码
//  uint16_t  DataLength;    //数据长度
//  uint16_t   RX_NUM ;	    //定位
//  uint8_t   Status   ;		 //状态
//  uint8_t   Checkoutright ;	  //数据校验是否成功
//  uint8_t   ACKStatus;
//}USART_StatusTypeDef;
//
//extern  USART_StatusTypeDef  STATUS ;



//extern uint8_t   CoinCountFlag;
//extern uint8_t   TemperatureFlag  ;
//extern uint8_t   BillActionFlag;      //表示纸币机在收钱中。这个时候不能关闭纸币机
//extern uint8_t   CurrentPoint ;
//extern uint8_t   CoinFlag;			 //用于退币机的标志
//extern uint8_t   TemperatureCur;
////extern uint8_t   CANStatus;
//extern uint8_t   Current ;
//extern uint8_t   LedCmd  ;
//extern  unsigned int  CoinsCount;	 //用于統計投的没硬币数
// #define CAN_MOVE 1
//#define CAN_GETMEAL 2
//
// #define USER_NO_ACTION   0
//#define USER_WAIT_PAY     1
//#define USER_WAIT_MEAL    2
//#define USER_IC_RECHARGE  3
//
//#define CAN_NOACTION        0   
//#define CAN_SNDCOMMAND      1
//#define CAN_READYGETMEAL    2     //通知去哪里取餐的返回
//#define CAN_REGETMEAL       3	  //通知取餐命令已执行
//#define CAN_MEALARRIVE      4	  //餐已经到达出餐口
//#define CAN_RELAYDOWNMEAL   5	  //放餐通知返回
//#define CAN_RESETCOMPLETION 6	  //初始化复位完成
//#define CAN_RESTSTART1      7     //通知驱动板到相对原点命令返回
//#define CAN_RESTSTART2      8     //通知驱动板到绝对原点命令返回
//#define CAN_TEMPERATURE     9     //温度开关返回
//#define CAN_RESTSTART0		10    //就是出错了，人工复位。
//#define CAN_RESETSUCCESS	11


// void CloseMoneySystem(void);
// void OpenMoneySystem(void);
// uint8_t FindMeal(MealAttribute *DefineMeal) ;
// uint8_t CanRecvCmd(uint8_t p);
// unsigned char  WaitPayMoney(void);
// uint8_t WaitMeal(void);
//void hardfawreInit(void);
// void  WaitTimeInit(uint8_t *Time);
//  void LcdHandler(void);
//  void  TempHandler(void);
//  void ClearingFuntion(void) ;//退签上送
#endif
