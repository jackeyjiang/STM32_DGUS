#ifndef __serialscreen___
#define  __serialscreen___
#include "stdint.h"
/*帧的一些字节定义*/
#define FH0 0x5A 
#define FH1 0xA5
#define RD_LEN 0x00
#define ADR 0x00

#define Logo_interface        0x00  /*logo界面*/
#define Err_interface         0x01  /*错误显示界面*/
#define Menu_interface        0x02  /*选餐界面*/
#define Meal1st_interface     0x04  /*红萝卜炒肉主界界面*/
#define Meal2rd_interface     0x07  /*香菇滑鸡主界界面*/
#define Meal3ns_interface     0x0A /*脆皮烤鸭主界面*/
#define Meal4th_interface     0x0D /*红烧鱼块主界面*/
#define Acount_interface      0x10 /*结算界面*/
#define TicketPrint_interface 0x13 /*小票打印界面*/
#define Mealout_interface     0x15 /*出餐界面显示*/
#define Password_interface    0x16 /*密码输入界面*/
#define MealSet_interface     0x18 /*放餐设置界面*/
#define MealInput_interface   0x1A /*放餐数量选择*/
#define TemperatureSet_interface 0x1C  /*温度设置界面*/
#define Data_synchronization  0x1E  /*数据同步*/
#define Coinset_interface     0x22  /*硬币设置*/
#define Cardbalence_interface 0x25  /*刷卡界面*/
#define UserAbonamalRecord_interface 0x27 /*异常用户记录界面*/
#define HardwareInit_interface         0x29 /*硬件初始化界面*/
#define OnlymachieInit_interface       0x2A /*机械手初始化界面*/
#define SignInFunction_interface       0x2B /*网络签到界面*/
#define Szt_GpbocAutoCheckIn_interface 0x2C /*深圳通签到界面*/
#define SellMeal_TimeSet_interface     0x2D /*售餐时间设置界面*/
#define SellMeal_TimeWait_interface    0x2F /*售餐等待界面*/

/*寄存器空间定义*/
#define Version 0x00  /*DGUS版本号*/
#define LED_NOW 0X01  /*LED亮度控制寄存器，0x00~0x40*/
#define BZ_IME  0X02  /*蜂鸣器鸣叫控制寄存器，单位10ms*/
#define PIC_ID  0x03  /*2字节 读：当前显示页面ID 写：切换到指定页面(页面控制)*/
#define TP_Flag 0x05  /*0x5a=触摸屏坐标有更新；其他触摸屏坐标未更新*/
#define TP_Status 0x06  /*0x01=第一次按下 0x03一直按压中 0x02=抬起 其他=无效*/
#define TP_Position 0x07 /*4字节 触摸屏按压坐标位置：X_H:L Y_H:L*/
#define TPC_Enable 0x0B /*0x00=触控不起用 其他=启用*/

#define ScreenEnable  0x01 
#define ScreenDisable 0x00

/*剩余菜品变量地址*/
#define meat     0x0021 /*红萝卜炒肉*/
#define chicken	 0x0022 /*香菇滑鸡*/
#define duck     0x0023 /*脆皮烤鸭*/
#define fish     0x0024 /*红烧鱼块*/

/*装填选项,1:放满,2:清空,3:确认,4:取消,5:返回*/
#define mealinput_choose  0x001A

/*错误代码变量地址*/
#define err_num  0x0007

/*系统设置,1:温度设置;2:餐品设置*/
#define admin_set 0x0006

/*密码长度文本显示*/
#define password_show 0x0005 

/*错误显剩*/
#define erro_num    0x4D00 

/*密码输入变量地址*/
#define password 0x0004 

/*票据打印变量地址,1:是;2:否*/
#define bill_print     0x0003 

/*支付方式变量地址,1:现金支付;2:银行卡支付;3:深圳通支付*/
#define payment_method  0x0002

/*选餐界面选项变量地址:单份数量选择*/
#define mealcnt_choose  0x0001

/*主界面选餐界面*/
#define meal_choose  0x0000

/*温度设置变量*/
#define temprature_set 0x001F

/*实时温度显示变量*/
#define current_temprature 0x0020

/*付款金额显示*/
#define  payment_coin  0x0025 //硬币支付金额
#define  payment_bill  0x0026 //纸币支付金额
#define  payment_card  0x0027 //刷卡支付金额

/*等待时间*/
#define  wait_payfor   0x0028 //付款等待时间

/*出餐界面变量*/
#define  mealout_totle  0x002A    //餐品出餐总数
#define  mealout_already  0x002B  //餐品已出餐总数

/*购物车显示*/
#define column1st_name  0x4100  //第一栏商品名
#define column1st_count 0x0042  //第一栏餐品数量
#define column1st_cost  0x0043  //第一栏商品总价
#define column2nd_name  0x4200  //第二栏商品名
#define column2nd_count 0x0045  //第二栏餐品数量
#define column2nd_cost  0x0046  //第二栏商品总价
#define column3rd_name  0x4300  //第三栏商品名
#define column3rd_count 0x0048  //第三栏餐品数量
#define column3rd_cost  0x0049  //第三栏商品总价
#define column4th_name  0x4400  //第四栏商品名
#define column4th_count 0x004B  //第四栏餐品数量
#define column4th_cost  0x004C  //第四栏商品总价

#define mealtotoal_cost 0x004F  //总价

/*单页餐品变量显示*/
//红萝卜炒肉数量选择界面
#define meat_cnt        0x0030 
#define meat_prince     0x0031
#define meat_cost       0x0032
//香菇滑鸡数量选择界面
#define chicken_cnt     0x0033
#define chicken_prince  0x0034
#define chicken_cost    0x0035
//脆皮烤鸭数量选择界面
#define duck_cnt        0x0036
#define duck_prince     0x0037
#define duck_cost       0x0038
//红烧鱼块数量选择界面
#define fish_cnt        0x0039
#define fish_prince     0x003A
#define fish_cost       0X003B
//选餐倒计时
#define count_dowm      0x003F
//商品价格
/*餐品装填变量*/
#define row_1st   0x0011 
#define row_2nd   0x0012
#define row_3rd   0x0013
#define floor_num 0x0014
#define meal_num  0x0015

//退币变量
#define coins_in     0x0050 //退币机内的硬币总量   
#define coins_input  0x0051 //硬币放置数量
#define coins_retain 0x0052 //保留硬币数量
#define coins_back	 0x0053 //当前退币数量
#define coins_key    0x0054 //退币的按键

#define caedbalence_cancel    0x0055  //刷卡取消按键

#define record_clear  0x0056    /*清楚记录按键*/

//刷卡金额变量
#define cardbalence_before    0x0060  //刷卡钱余额
#define amountof_consumption  0x0065 //本次消费金额
#define cardbalence_after     0x006A  //刷卡后余额



//同步界面
#define sync_column1st_name   0x4500
#define sync_column2nd_name   0x4600
#define sync_column3rd_name   0x4700
#define sync_column4th_name   0x4800
#define sync_column1st_number 0x4900
#define sync_column2nd_number 0x4A00
#define sync_column3rd_number 0x4B00
#define sync_column4th_number 0X4C00

//用户错误记录界面
#define record_column1st_name   0x5000 //第一栏名字
#define record_column2nd_name   0x5100 //第二栏名字
#define record_column3rd_name   0x5200 //第三栏名字
#define record_column4th_name   0x5300 //第四栏名字

#define record_column1st_cnt_t  0x5400 //第一栏餐品总数
#define record_column2nd_cnt_t  0x5500 //第二栏餐品总数
#define record_column3rd_cnt_t  0x5600 //第三栏餐品总数
#define record_column4th_cnt_t  0x5700 //第四栏餐品总数

#define record_column1st_cnt    0x5800 //第一栏餐品已出总数
#define record_column2nd_cnt    0x5900 //第二栏餐品已出总数
#define record_column3rd_cnt    0x5A00 //第三栏餐品已出总数
#define record_column4th_cnt    0x5B00 //第四栏餐品已出总数

#define record_UserActPayAlready     0x5C00  //用户已付款
#define record_UserActPayBack        0x5D00  //用户应退币
#define record_UserActPayBackAlready 0x5E00  //已退用户硬币数

#define record_UserDataTime          0x5F00  //出错时间显示

//售餐时间设置界面
#define set_sellmeal_hour     0x4E00    //售餐时间设置的小时变量
#define set_sellmeal_minute   0x4E01    //售餐时间设置的分钟变量
#define set_sellmeal_key      0x4E02    //售餐时间设置的按键变量

//等待售餐的时间显示界面
#define wait_sellmeal_hour    0x4E03    //等待售餐显示的小时变量
#define wait_sellmeal_minute  0x4E04    //等待售餐显示的分钟变量
#define wait_sellmeal_second  0x4E05    //等待售餐显示的秒钟变量


extern const char price_1st;
extern const char price_2nd;
extern const char price_3rd;
extern const char price_4th;
extern bool cardbalence_cancel_flag;
extern bool sellmeal_flag; 
extern char record_time[20];
extern int16_t CoinTotoal_t;
extern int8_t	selltime_hour,selltime_hour_t,selltime_hour_r;
extern int8_t	selltime_minute, selltime_minute_t,selltime_minute_r;
extern int8_t  selltime_second_r;
extern uint32_t sellsecond_remain;
void ClearUserBuffer(void);
void PageChange(char page);
void ReadPage(void);
void ScreenControl(char cmd);
void DispLeftMeal(void);
void DealSeriAceptData(void);
void VariableChage(uint16_t Variable,uint16_t Value);	
void DisplayAbnormal(char *abnomal_code);
void VariableChagelong (uint16_t Variable,uint32_t Value);
void DisplayRecordTime(void);
void DisplayTimeCutDown(void);
void SetScreenRtc(void);

#endif
