#ifndef _bsp_H
#define _bsp_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "MsgHandle.h"	         //处理关于点餐的数据
#include "vioce.h"			       //声音函数
#include "serialscreen.h"	       //串口屏函数
//#include "led.h"			       //led灯函数
//#include "ICcard.h"			       //ic卡函数
//#include "timer2.h" 				//用于倒计时
//#include "timer4.h"			       //定时器4 用于显示温度的 没多少次发送一个信号给从机得到温度。然后在显示。
//#include "timer3.h"		           //用于秒表
//#include "timer6.h"				   //用于采集温度
#include "delay.h"			       //延时
#include "uart3.h"				   //串口屏接口
#include "uart4.h"           //纸币机接口

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
#include "uart5.h"
#include "spi_flash.h"  
//#include "szt_gpboc.h"       //深圳通、银联

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



extern uint8_t   CoinCountFlag;
extern uint8_t   TemperatureFlag  ;
extern uint8_t   BillActionFlag;      //表示纸币机在收钱中。这个时候不能关闭纸币机
extern uint8_t   CurrentPoint ;
extern uint8_t   CoinFlag;			 //用于退币机的标志
extern uint8_t   TemperatureCur;


bool CloseCashSystem(void);
bool OpenCashSystem(void);	
uint8_t FindMeal(MealAttribute *DefineMeal) ;
// uint8_t CanRecvCmd(uint8_t p);
unsigned char  WaitPayMoney(void);
uint8_t WaitMeal(void);
void hardfawreInit(void);
void  WaitTimeInit(uint8_t *Time);
// void LcdHandler(void);
// void  TempHandler(void);
// void ClearingFuntion(void) ;//退签上送
#endif
