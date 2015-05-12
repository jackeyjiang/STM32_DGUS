 /**
  ******************************************************************************
  * 文件: serialscreen.c
  * 作者: jackey
  * 版本: V1.0.0
  * 描述: DWIN 串口屏通信程序，
  *  
  ******************************************************************************
	**/
#include <string.h>
#include "stm32f4xx.h"
#include "serialscreen.h"
#include "uart3.h"
#include "bsp.h"
#include "MsgHandle.h"

#define Null 0x00
typedef struct DispMeal
{
	char meal_id;
	char meal_cnt;
	char meal_cost;
}DispMealStuct;

DispMealStuct DispMeal[4]={0};

char price=0;
char temprature_old=0;

/*将数据16位存储器地址分解为2个8位数据*/
union ScreenRam
{
		int16_t	 	adress ;
		int8_t  	adr[2] ;
}myunion;

/*将数据16位存储器地址分解为2个8位数据*/
union ScreenRamLong
{
		uint32_t	 	adress ;
		int8_t  	adr[4] ;
}myunion_long;


/*写指定寄存器帧*/
const unsigned char RegisterWrite[7]={ 
							FH0 , FH1 , /*帧头2字节 */
							0x04 , /*长度 包括命令和数据*/
              0x80 , /*指定地址写寄存器数据*/
							ADR  , /*寄存器地址*/
							0x00 ,0x00 /*数据*/
							//0x00 , 0x00	/*CRC校验侦尾*/
							};

/*读指定寄存器帧*/
const unsigned char RegisterRead[6]={
							FH0 , FH1 , /*帧头2字节 */
							0x03 , /*长度 包括命令和数据*/
              0x81 ,  /*指定地址读寄存器数据*/
							ADR  , /*寄存器地址*/
							RD_LEN , /*长度*/
							//0x00 , 0x00	/*CRC校验侦尾*/	
							};
	
/*写变量存储区帧*/							
const unsigned char VariableWrite[8]={
							FH0 , FH1 , /*帧头2字节 */
							0x00 , /*长度 包括命令和数据*/	
							0x82 , /*指定地址开始写入数据串(字数据)到变量存储区*/
							0x00 , 0x00 ,	/*变量存储空间2字节*/
							0x00 , 0x00   /*数据*/
							//0x00 , 0x00  	/*CRC校验侦尾*/
							}; 

/*读变量存储区帧*/	
const unsigned char VariableRead[7]={
							FH0 , FH1 , /*帧头2字节 */
							0x04 , /*长度 包括命令和数据*/
							0x83 , /*从变量存储区指定地址开始读入RD_LEN长度字数据*/
							0x00 , 0x00 ,	/*变量存储空间2字节*/
							RD_LEN , /*长度*/
							//0x00 , 0x00  	/*CRC校验侦尾*/
							};	

const char ABC[20]={
							FH0 , FH1 , /*帧头2字节 */
							0x05 , /*长度 包括命令和数据*/	
							0x82 , /*指定地址开始写入数据串(字数据)到变量存储区*/
							0x40 , 0x00 ,	/*变量存储空间2字节*/
							0xBA , 0xA0   /*数据*/
              };	
const unsigned char rtc_write[13]={
  						FH0 , FH1 , /*帧头2字节 */
							0x0A , /*长度 包括命令和数据*/	
							0x80 , /*指定地址开始写入数据串(字数据)到变量存储区*/
							0x1F , 0x5A ,	/*变量存储空间2字节*/
							0x00 , 0x00 ,0X00 ,   /*日期*/
              0X00 ,/*星期*/
              0x00 , 0x00 ,0x00 /*时间*/
              };
#include "stdio.h"

uint8_t *mystrcat(uint8_t *dest, const uint8_t *src, uint8_t length)
{
  int i=0;
	for(i=0;i<length;i++)
	{
		dest[6+i] = src[i];
	}
  return dest;
}


 /*******************************************************************************
 * 函数名称:VariableChage                                                                    
 * 描    述:改变指定变量的值                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void VariableChage(uint16_t Variable,uint16_t Value)							
{
	  unsigned char temp[8]={0};  //存放串口数据的临时指针
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 5;
		myunion.adress= Variable; 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		myunion.adress= Value; 
		temp[6]= myunion.adr[1];
		temp[7]= myunion.adr[0];		
		Uart3_Send(temp,sizeof(temp));	
}							

 /*******************************************************************************
 * 函数名称:VariableChagelong                                                                    
 * 描    述:改变指定变量的值                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void VariableChagelong (uint16_t Variable,uint32_t Value)							
{
	  unsigned char temp[10]={0};  //存放串口数据的临时指针
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 7;
		myunion.adress= Variable; 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		myunion_long.adress= Value;
		temp[6]= myunion_long.adr[3];
		temp[7]= myunion_long.adr[2];					
		temp[8]= myunion_long.adr[1];
		temp[9]= myunion_long.adr[0];		
		Uart3_Send(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:RegisterChage                                                                   
 * 描    述:改变指定变量的值                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void RegisterChage(uint8_t Register,uint16_t Value)							
{
	  unsigned char temp[8]={0};  //存放串口数据的临时指针
		memcpy(temp,RegisterWrite,sizeof(RegisterWrite));
		temp[3]= Register;
		myunion.adress=Value;
		temp[5]= myunion.adr[1];
		temp[6]= myunion.adr[0];		
		Uart3_Send(temp,sizeof(temp));	
}	

 /*******************************************************************************
 * 函数名称:RegisterChage                                                                   
 * 描    述:改变指定变量的值                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void ScreenControl(char cmd)							
{
	  unsigned char temp[8]={0};  //存放串口数据的临时指针
		memcpy(temp,RegisterWrite,sizeof(RegisterWrite));
		temp[3]= TPC_Enable;
		temp[5]= cmd ;	//0x00 关闭
		Uart3_Send(temp,sizeof(temp));	
}	
 

 /*******************************************************************************
 * 函数名称:DisplayPassWord                                                                 
 * 描    述:显示密码长度以*表示                                                     
 *                                                                               
 * 输    入:PassWordLen                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void DisplayPassWord(char PassWordLen)
{
	char i=0;
	char temp[20]={0};  //存放串口数据的临时数组
	memcpy(temp,VariableWrite,sizeof(VariableWrite));
	temp[2]= PassWordLen+ 4; //  0x83 00 00 ****** 如果无密码显示则清空数据
	myunion.adress= password_show; 
  temp[4]= myunion.adr[1];
  temp[5]= myunion.adr[0];
	for(i=0;i<PassWordLen;i++) temp[i+6]='*';
	Uart3_Sent(temp,sizeof(temp)); 
}	
 /*******************************************************************************
 * 函数名称:DisplayAbnormal                                                               
 * 描    述:显示错误码                                                   
 *                                                                               
 * 输    入:abnomal_code 字符串                                                                  
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void DisplayAbnormal(char *abnomal_code)
{
	char i=0;
	char temp[20]={0};  //存放串口数据的临时数组
	memcpy(temp,VariableWrite,sizeof(VariableWrite));
	temp[2]= 7; //  数据长度为4位
	myunion.adress= erro_num; 
  temp[4]= myunion.adr[1];
  temp[5]= myunion.adr[0];
	for(i=0;i<4;i++) temp[i+6]=abnomal_code[i];
	Uart3_Sent(temp,sizeof(temp)); 	
}


 /*******************************************************************************
 * 函数名称:PageChange                                                                     
 * 描    述:发送切换图片命令                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 	
uint8_t pageunitil= 0;
void PageChange(char page)
{
		unsigned char temp[7]={0};
		memcpy(temp,RegisterWrite,sizeof(RegisterWrite));	
		temp[4]=	PIC_ID;	
	  temp[6]=  page;
		pageunitil = page;
		Uart3_Send(temp,sizeof(temp));
}

 /*******************************************************************************
 * 函数名称:ReadPage                                                                     
 * 描    述:读取当前页,数据处理在DealSeriAceptData中处理                                                           
 *                                                                               
 * 输    入:无                                                                    
 * 输    出:                                                                     
 * 返    回:                                                             
 * 修改日期:2014年6月24日                                                                    
 *******************************************************************************/ 	
void ReadPage(void)
{
		unsigned char temp[6]={0};
		memcpy(temp,RegisterRead,sizeof(RegisterRead));	
		temp[4]=	PIC_ID;	
		Uart3_Send(temp,sizeof(temp));
}

 /*******************************************************************************
 * 函数名称:SetScreenRtc                                                                    
 * 描    述:读取当前页,数据处理在DealSeriAceptData中处理                                                           
 *                                                                               
 * 输    入:无                                                                    
 * 输    出:                                                                     
 * 返    回:                                                             
 * 修改日期:2014年6月24日                                                                    
 *******************************************************************************/ 	
void SetScreenRtc(void)
{
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_DateTypeDef RTC_DateStructure;
  unsigned char temp[13]={0};
  RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);
  memcpy(temp,rtc_write,13);
  temp[6]=  RTC_DateStructure.RTC_Year;
  temp[7]=  RTC_DateStructure.RTC_Month;
  temp[8]=  RTC_DateStructure.RTC_Date;
  temp[9]=  RTC_DateStructure.RTC_WeekDay;
  temp[10]= RTC_TimeStructure.RTC_Hours;
  temp[11]= RTC_TimeStructure.RTC_Minutes;
  temp[12]= RTC_TimeStructure.RTC_Seconds;
  Uart3_Send(temp,sizeof(temp));
}
 /*******************************************************************************
 * 函数名称:DispMenu
 * 描    述:显示四个餐品的名称和价格                                                          
 *                                                                               
 * 输    入:无                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年11月16日                                                                    
 *******************************************************************************/ 
void DispMenu(void)
{
		uint8_t i;
	  unsigned char temp[30]={0};  //存放串口数据的临时指针
    GetMealDetail();   //获取餐品的信息
		for(i=0;i<4;i++)  //显示菜名
		{
      memcpy(temp,VariableWrite,sizeof(VariableWrite));
      temp[2]= sizeof(Meal_Union.Meal[i].MealName);
      myunion.adress= menu_name_1st+i*0x0100; //在基地址推移位置
      temp[4]= myunion.adr[1];
      temp[5]= myunion.adr[0];
			memcpy(temp+6,Meal_Union.Meal[i].MealName,sizeof(Meal_Union.Meal[i].MealName));	
			Uart3_Send(temp,sizeof(temp));	
		}
    for(i=0;i<4;i++) //显示单价
    {
      memcpy(temp,VariableWrite,sizeof(VariableWrite));
      temp[2]= 5;
      myunion.adress= menu_price_1st+i*0x0010; //在基地址推移位置
      temp[4]= myunion.adr[1];
      temp[5]= myunion.adr[0];
      switch(i)
      {
        case 0:temp[7]= price_1st;break;
        case 1:temp[7]= price_2nd;break;
        case 2:temp[7]= price_3rd;break;
        case 3:temp[7]= price_4th;break;
        default:break;
      }
			Uart3_Send(temp,sizeof(temp));      
    }    
}

 /*******************************************************************************
 * 函数名称:DispMenuNone
 * 描    述:在中间显示餐品为服务器无数据                                                         
 *                                                                               
 * 输    入:无                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2015年1月17日                                                                    
 *******************************************************************************/ 
const uint8_t menu_err[14]={"服务器无数据"};
void DispMenuNone(void)
{
  uint8_t temp[30]={0};  //存放串口数据的临时指针
  memcpy(temp,VariableWrite,sizeof(VariableWrite));
  temp[2]= sizeof(menu_err)+5;
  myunion.adress= menu_name_none;
  temp[4]= myunion.adr[1];
  temp[5]= myunion.adr[0];
  memcpy(temp+6,menu_err,sizeof(menu_err));	
  Uart3_Send(temp,sizeof(temp));	  
}
 /*******************************************************************************
 * 函数名称:DispLeftMeal                                                                     
 * 描    述:在菜单选择界面显示各菜品所剩余的                                                           
 *                                                                               
 * 输    入:无                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 	

void DispLeftMeal(void)
{
		uint8_t i;
	  unsigned char temp[8]={0};  //存放串口数据的临时指针
		for(i=0;i<4;i++)
		{
      memcpy(temp,VariableWrite,sizeof(VariableWrite));
      temp[2]= 5;
      myunion.adress= meal1st_left+i; //在基地址推移位置
      temp[4]= myunion.adr[1];
      temp[5]= myunion.adr[0];	
      temp[7]= DefineMeal[sell_type[i]-1].MealCount;//sell_type[i]存储的是四个菜品的ID			
			Uart3_Send(temp,sizeof(temp));	
		}
}
 /*******************************************************************************
 * 函数名称:CutDownDisp                                                                     
 * 描    述:显示倒计时60-0                                                          
 *                                                                               
 * 输    入:time                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 	
void CutDownDisp(char time)
{
	  unsigned char temp[8]={0};
	  memcpy(temp,VariableWrite,sizeof(VariableWrite));
	  temp[2]= 5;
		myunion.adress= count_dowm; //在基地址推移位置
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
    temp[7]= time;	
		Uart3_Send(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:GetMealPrice                                                                     
 * 描    述:获取当前餐品的价格                                                        
 *                                                                               
 * 输    入:                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
uint32_t GetMealPrice(uint8_t meal_type,uint8_t count)
{
	char price= 0;
  switch(meal_type)
	{
		case 0x01:price= price_1st*count;break;
		case 0x02:price= price_2nd*count;break;
		case 0x03:price= price_3rd*count;break;
		case 0x04:price= price_4th*count;break;
		default:break;
	}
	return price;
}

 /*******************************************************************************
 * 函数名称:GetMealLastPrice                                                                     
 * 描    述:获取当前餐品的折后价格                                                        
 *           先判断餐品的ID，后根据付款方式，获取当前的餐品在当前的付款方式下的折扣。                                                                  
 * 输    入:                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2015年1月17日                                                                    
 *******************************************************************************/ 
uint32_t GetMealLastPrice(uint8_t meal_type,uint8_t count)
{
	uint32_t price= 0;
  uint32_t last_discount_t= 0; /*折扣数据*/
  switch(meal_type)
	{
		case 0x01:  /*先确定餐品ID*/
		{
      switch(UserActMessageWriteToFlash.UserAct.PayType)
      {
        /*后根据付款方式UserActMessageWriteToFlash.UserAct.PayType，获取从后台下发的原始折扣数据*/
        case 0x31: /*现金*/
          last_discount_t= cashcut_1st;break;
        case 0x32: /*银联*/
          last_discount_t= gboccut_1st;break;
        case 0x33: /*深圳通*/
          last_discount_t= sztcut_1st;break;
        case 0x34: /*会员卡*/
          last_discount_t= vipcut_1st;break;
        default:break;
      }
		}break;
		case 0x02:  /*根据餐品的顺序改变*/
		{
      switch(UserActMessageWriteToFlash.UserAct.PayType)
      {
        case 0x31: /*现金*/
          last_discount_t= cashcut_2nd;break;
        case 0x32: /*银联*/
          last_discount_t= gboccut_2nd;break;
        case 0x33: /*深圳通*/
          last_discount_t= sztcut_2nd;break;
        case 0x34: /*会员卡*/
          last_discount_t= vipcut_2nd;break;
        default:break;
      }
		}break;
		case 0x03:  /*根据餐品的顺序改变*/
		{
      switch(UserActMessageWriteToFlash.UserAct.PayType)
      {
        case 0x31: /*现金*/
          last_discount_t= cashcut_3rd;break;
        case 0x32: /*银联*/
          last_discount_t= gboccut_3rd;break;
        case 0x33: /*深圳通*/
          last_discount_t= sztcut_3rd;break;
        case 0x34: /*会员卡*/
          last_discount_t= vipcut_3rd;break;
        default:break;
      }
		}break;
		case 0x04:  /*根据餐品的顺序改变*/
		{
      switch(UserActMessageWriteToFlash.UserAct.PayType)
      {
        case 0x31: /*现金*/
          last_discount_t= cashcut_4th;break;
        case 0x32: /*银联*/
          last_discount_t= gboccut_4th;break;
        case 0x33: /*深圳通*/
          last_discount_t= sztcut_4th;break;
        case 0x34: /*会员卡*/
          last_discount_t= vipcut_4th;break;
        default:break;
      }
		}break;    
		default:break;
	}
  price= price_1st*last_discount_t/100*count;
	return price;
}
 /*******************************************************************************
 * 函数名称:MealCostDisp                                                                     
 * 描    述:显示当前餐品应付的金额                                                          
 *                                                                               
 * 输    入:                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void MealCostDisp(char meal_id,char meal_count)
{
	  unsigned char temp[8]={0};
	  memcpy(temp,VariableWrite,sizeof(VariableWrite));
	  temp[2]= 5;
		temp[5]= (0x31+(meal_id-1)*2); //钱币变量地址
    temp[7]= GetMealPrice(meal_id,meal_count);	
		Uart3_Send(temp,sizeof(temp));		
}
 /*******************************************************************************
 * 函数名称:ClearUserBuffer                                                                     
 * 描    述:清除用户选择数据                                                          
 *                                                                               
 * 输    入:无                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void ClearUserBuffer(void)
{
	UserActMessageWriteToFlash.UserAct.MealType_1st=0;
	UserActMessageWriteToFlash.UserAct.MealType_2nd=0;
	UserActMessageWriteToFlash.UserAct.MealType_3rd=0;
	UserActMessageWriteToFlash.UserAct.MealType_4th=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_1st=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_1st_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_2nd=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_3rd=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_4th=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_4th_t=0;
	UserActMessageWriteToFlash.UserAct.MealPrice_1st=0;
	UserActMessageWriteToFlash.UserAct.MealPrice_2nd=0;
	UserActMessageWriteToFlash.UserAct.MealPrice_3rd=0;
	UserActMessageWriteToFlash.UserAct.MealPrice_4th=0;   
  UserActMessageWriteToFlash.UserAct.LastMealPrice_1st=0;
	UserActMessageWriteToFlash.UserAct.LastMealPrice_2nd=0;
	UserActMessageWriteToFlash.UserAct.LastMealPrice_3rd=0;
	UserActMessageWriteToFlash.UserAct.LastMealPrice_4th=0; 
	UserActMessageWriteToFlash.UserAct.MealCost_1st=0;
	UserActMessageWriteToFlash.UserAct.MealCost_2nd=0;
	UserActMessageWriteToFlash.UserAct.MealCost_3rd=0;
	UserActMessageWriteToFlash.UserAct.MealCost_4th=0;
	UserActMessageWriteToFlash.UserAct.LastMealCost_1st=0;
	UserActMessageWriteToFlash.UserAct.LastMealCost_2nd=0;
	UserActMessageWriteToFlash.UserAct.LastMealCost_3rd=0;
	UserActMessageWriteToFlash.UserAct.LastMealCost_4th=0;  
	UserActMessageWriteToFlash.UserAct.MealID=0;
	UserActMessageWriteToFlash.UserAct.Meal_totoal=0;
	UserActMessageWriteToFlash.UserAct.Meal_takeout=0;
	UserActMessageWriteToFlash.UserAct.PayShould=0;
  UserActMessageWriteToFlash.UserAct.LastPayShould=0;
	UserActMessageWriteToFlash.UserAct.PayType=0x30;            //未选择支付方式
  UserActMessageWriteToFlash.UserAct.PayForCoins=0;           //用户投入的硬币数	
	UserActMessageWriteToFlash.UserAct.PayForBills=0;           //用户投入的纸币数
	UserActMessageWriteToFlash.UserAct.PayForCards=0;           //用户应经刷卡的数
  UserActMessageWriteToFlash.UserAct.PayAlready=0;
  UserActMessageWriteToFlash.UserAct.MoneyBackShould=0;
  UserActMessageWriteToFlash.UserAct.MoneyBackAlready=0;
  UserActMessageWriteToFlash.UserAct.MoneyBack=0;
  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already=0;
  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st=0;
  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd=0;  
}
 /*******************************************************************************
 * 函数名称:PutIntoShopCart                                                                     
 * 描    述:在选产界面将点击放入购物车之后的程序响应                                                        
 *                                                                               
 * 输    入:                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void PutIntoShopCart(void)
{

  UserActMessageWriteToFlash.UserAct.MealCnt_1st= UserActMessageWriteToFlash.UserAct.MealCnt_1st_t;
  UserActMessageWriteToFlash.UserAct.MealCost_1st = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealType_1st,UserActMessageWriteToFlash.UserAct.MealCnt_1st);

  UserActMessageWriteToFlash.UserAct.MealCnt_2nd=UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t;
  UserActMessageWriteToFlash.UserAct.MealCost_2nd = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealType_2nd,UserActMessageWriteToFlash.UserAct.MealCnt_2nd);

  UserActMessageWriteToFlash.UserAct.MealCnt_3rd=UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t;
  UserActMessageWriteToFlash.UserAct.MealCost_3rd = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealType_3rd,UserActMessageWriteToFlash.UserAct.MealCnt_3rd);

  UserActMessageWriteToFlash.UserAct.MealCnt_4th=UserActMessageWriteToFlash.UserAct.MealCnt_4th_t;
  UserActMessageWriteToFlash.UserAct.MealCost_4th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealType_4th,UserActMessageWriteToFlash.UserAct.MealCnt_4th);
  
  UserActMessageWriteToFlash.UserAct.PayShould= UserActMessageWriteToFlash.UserAct.MealCost_1st +UserActMessageWriteToFlash.UserAct.MealCost_2nd\
                                                +UserActMessageWriteToFlash.UserAct.MealCost_3rd+UserActMessageWriteToFlash.UserAct.MealCost_4th;

	UserActMessageWriteToFlash.UserAct.Meal_totoal= +UserActMessageWriteToFlash.UserAct.MealCost_4th+UserActMessageWriteToFlash.UserAct.MealCnt_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_2nd+UserActMessageWriteToFlash.UserAct.MealCnt_1st;
}	

 /*******************************************************************************
 * 函数名称:SettleAccounts                                                                 
 * 描    述:当按下结算按钮后的程序流程                                                                                                                        
 * 输    入:无                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                   
 *******************************************************************************/ 
char Floor= 0; //作为有商品种类的层数
void SettleAccounts(void)
{
  PageChange(Acount_interface);//结算界面
  //显示硬币数
	VariableChage(payment_coin,UserActMessageWriteToFlash.UserAct.PayForCoins);
  //显示纸币数
	VariableChage(payment_bill,UserActMessageWriteToFlash.UserAct.PayForBills);
  //刷卡数
	VariableChage(payment_card,UserActMessageWriteToFlash.UserAct.PayForCards);
  /*计算账单*/
	OpenTIM7();
  
  VariableChage(mealtotoal_cost,UserActMessageWriteToFlash.UserAct.PayShould);
}

 /*******************************************************************************
 * 函数名称:SyncMealNameDisp                                                                  
 * 描    述:数据同步时显示餐品名称                                                          
 *           根据ID 显示名称 ，根据floor 改变现实变量的地址                                                                    
 * 输    入:meal_id ,floor                                                               
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void SyncMealNameDisp(uint8_t meal_id,uint8_t floor)							
{
	  uint8_t temp[50]={0};  //存放串口数据的临时数组
    uint8_t length_t=0;
    uint8_t *p_t=" ";
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 17; //0x83 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= sync_column1st_name+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		switch(meal_id)
		{
			case 0x00:break;
			case 0x01:mystrcat(temp,mealname_1st,strcspn(mealname_1st,p_t));break;		
			case 0x02:mystrcat(temp,mealname_2nd,strcspn(mealname_2nd,p_t));break;		
			case 0x03:mystrcat(temp,mealname_3rd,strcspn(mealname_3rd,p_t));break;	
			case 0x04:mystrcat(temp,mealname_4th,strcspn(mealname_4th,p_t));break;	   
			default:break;			
		}
		Uart3_Sent(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:SyncMealCntDisp                                                                 
 * 描    述:数据同步时显示餐品数量  ，以文本方式显示                                                        
 *           更具floor 改变变量 ，meal_cnt 是要现实的数量                                                                    
 * 输    入:meal_id                                                                
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void SyncMealCntDisp(uint8_t meal_cnt,uint8_t floor)							
{
	  char temp[20]={0};  //存放串口数据的临时数组
		char buffer[5]={0};
		int cnt_t=0,i=0;
		cnt_t= sprintf(buffer,"%d",meal_cnt);
		if(meal_cnt==0xff) cnt_t=0;
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 4+cnt_t; //0x83 0x00 0x41 0x00 0x00 0x00 0x00 
		myunion.adress= sync_column1st_number+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		for(i=0;i<cnt_t;i++) temp[i+6]=buffer[i]; 
		Uart3_Sent(temp,sizeof(temp));	
}

 /*******************************************************************************
 * 函数名称:AbnomalMealNameDisp                                                                  
 * 描    述:错误记录时显示餐品名称                                                          
 *           根据ID 显示名称 ，根据floor 改变现实变量的地址                                                                    
 * 输    入:meal_id ,floor                                                               
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月9日                                                                    
 *******************************************************************************/ 							
void AbnomalMealNameDisp(uint8_t meal_id,uint8_t floor)							
{
	  uint8_t temp[50]={0};  //存放串口数据的临时数组
    uint8_t *p_t=" ";
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 17; //0x83 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= record_column1st_name+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		switch(meal_id)
		{
			case 0x00:break;
			case 0x01:mystrcat(temp,mealname_1st,strcspn(mealname_1st,p_t));break;		
			case 0x02:mystrcat(temp,mealname_2nd,strcspn(mealname_2nd,p_t));break;		
			case 0x03:mystrcat(temp,mealname_3rd,strcspn(mealname_3rd,p_t));break;	
			case 0x04:mystrcat(temp,mealname_4th,strcspn(mealname_4th,p_t));break;	   
			default:break;			
		}
		Uart3_Sent(temp,sizeof(temp));	
}

/*******************************************************************************
 * 函数名称:AbnomalMealCnttDisp                                                                 
 * 描    述:错误记录时显示餐品数量  ，以文本方式显示                                                        
 *           更具floor 改变变量 ，meal_cnt 是要现实的数量                                                                    
 * 输    入:meal_id                                                                
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void AbnomalMealCnttDisp(uint8_t meal_cnt,uint8_t floor)							
{
	  char temp[20]={0};  //存放串口数据的临时数组
		char buffer[5]={0};
		int cnt_t=0,i=0;
		cnt_t= sprintf(buffer,"%d",meal_cnt);
		if(meal_cnt==0xff) cnt_t=0;
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 4+cnt_t; //0x83 0x00 0x41 0x00 0x00 0x00 0x00 
		myunion.adress= record_column1st_cnt_t+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		for(i=0;i<cnt_t;i++) temp[i+6]=buffer[i]; 
		Uart3_Sent(temp,sizeof(temp));	
}

/*******************************************************************************
 * 函数名称:AbnomalMealCntDisp                                                                 
 * 描    述:错误记录时显示餐品数量  ，以文本方式显示                                                        
 *           更具floor 改变变量 ，meal_cnt 是要现实的数量                                                                    
 * 输    入:meal_id                                                                
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void AbnomalMealCntDisp(uint8_t meal_cnt,uint8_t floor)							
{
	  char temp[20]={0};  //存放串口数据的临时数组
		char buffer[5]={0};
		int cnt_t=0,i=0;
		cnt_t= sprintf(buffer,"%d",meal_cnt);
		if(meal_cnt==0xff) cnt_t=0;
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 4+cnt_t; //0x83 0x00 0x41 0x00 0x00 0x00 0x00 
		myunion.adress= record_column1st_cnt+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		for(i=0;i<cnt_t;i++) temp[i+6]=buffer[i]; 
		Uart3_Sent(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:DisplayTimeCutDown                                                                    
 * 描    述:显示离售餐时间的倒计时                                                            
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年7月22日                                                                    
 *******************************************************************************/ 
uint32_t sellsecond_remain=0;
void DisplayTimeCutDown(void)
{
  uint32_t sellsecond_totoal=0,realsecond_totoal=0;
	RTC_TimeShow();	//获取当前的时间
  sellsecond_totoal= selltime_hour*3600+selltime_minute*60; //设置售餐的时间换位秒
  realsecond_totoal= TimeDate.Hours*3600+TimeDate.Minutes*60+TimeDate.Senconds; //当前时间转换为秒
  if(sellsecond_totoal>realsecond_totoal)  
  {
    sellmeal_flag= false;
    sellsecond_remain = sellsecond_totoal-realsecond_totoal;
    selltime_hour_r= sellsecond_remain/3600;
    selltime_minute_r= (sellsecond_remain%3600)/60;
    selltime_second_r= (sellsecond_remain%3600)%60;
    VariableChage(wait_sellmeal_hour,selltime_hour_r);
    VariableChage(wait_sellmeal_minute,selltime_minute_r);
    VariableChage(wait_sellmeal_second,selltime_second_r);
    PageChange(SellMeal_TimeWait_interface); 
    OpenTIM4(); 
  }
  else
  {
    sellsecond_remain=0;
    CloseTIM4();
    sellmeal_flag= true;
    PageChange(Menu_interface); 
  }
}
 /*******************************************************************************
 * 函数名称:DisplayRecordTime                                                                     
 * 描    述:显示出错时的错误记录                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月23日                                                                    
 *******************************************************************************/  
char record_time[20]={"20yy-mm-dd hh:mm:ss"};
void DisplayRecordTime(void)
{ 
	  char temp[30]={0};  //存放串口数据的临时数组  
		RTC_TimeShow();     /*时间作为错误记录时间*/
		record_time[0] =    20/10+'0'; 
		record_time[1] =    20%10 +'0'; 
		record_time[2] =    TimeDate.Year/10+ '0'; 
		record_time[3] =    TimeDate.Year%10+ '0' ;
		record_time[5] =    TimeDate.Month/10+ '0';
		record_time[6] =    TimeDate.Month%10+ '0';
		record_time[8] =    TimeDate.Date/10+ '0'; 
		record_time[9] =    TimeDate.Date%10+ '0';
		record_time[11] =   TimeDate.Hours/10+ '0';
		record_time[12] =   TimeDate.Hours%10+ '0';
		record_time[14] =   TimeDate.Minutes/10+ '0';
		record_time[15] =   TimeDate.Minutes%10+ '0' ;
		record_time[17] =   TimeDate.Senconds/10+ '0'; 
		record_time[18] =   TimeDate.Senconds%10+ '0';	
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 23; //22 0x83 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= record_UserDataTime;
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		mystrcat(temp,record_time,20);
		Uart3_Sent(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:DisplayUserRecord                                                                     
 * 描    述:显示出错时的错误记录                                                              
 *               需要修改                                                                
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月9日                                                                    
 *******************************************************************************/  
void DisplayUserRecord(void)
{
	uint8_t floor=0;
	for(floor=0;floor<4;floor++)		//清楚屏幕数据
	{
		AbnomalMealNameDisp(0,floor);
		AbnomalMealCnttDisp(0xff,floor);
		AbnomalMealCntDisp(0xff,floor);		
	}
	floor=0; //复原
	if(UserActMessageWriteToFlash.UserAct.MealCnt_1st_t>0)  
	{
		AbnomalMealNameDisp(UserActMessageWriteToFlash.UserAct.MealType_1st,floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_1st_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_1st,floor);		
		floor++;
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t>0)
	{
		AbnomalMealNameDisp(UserActMessageWriteToFlash.UserAct.MealType_2nd,floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_2nd,floor);	
		floor++;
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t>0)
	{
		AbnomalMealNameDisp(UserActMessageWriteToFlash.UserAct.MealType_3rd,floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_3rd,floor);	
		floor++;
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_4th_t>0)
	{
		AbnomalMealNameDisp(UserActMessageWriteToFlash.UserAct.MealType_4th,floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_4th_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_4th,floor);	
		floor++;
	}
		//显示用户已付 和  应退  和 已退
	VariableChage(record_UserActPayAlready,UserActMessageWriteToFlash.UserAct.PayAlready);
		//应退的钱 = 总的应该退币的钱
	VariableChage(record_UserActPayBack,UserActMessageWriteToFlash.UserAct.MoneyBackShould);
	  //已退的钱 = 总的应该退币的钱- 还未退的钱
	VariableChage(record_UserActPayBackAlready,UserActMessageWriteToFlash.UserAct.MoneyBackShould-UserActMessageWriteToFlash.UserAct.MoneyBack);
}
	
 /*******************************************************************************
 * 函数名称:GetPassWord                                                                     
 * 描    述:放餐密码                                                            
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年4月9日                                                                    
 *******************************************************************************/  
void GetPassWord(unsigned char *PassWord)
{
  PassWord[0] = 5;
	PassWord[1] = 1;
	PassWord[2] = 8;
	PassWord[3] = 2;
	PassWord[4] = 0;
	PassWord[5] = 7;
}
 /*******************************************************************************
 * 函数名称:GetAdminPassWord                                                                     
 * 描    述:硬币退币与装填密码                                                           
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年8月13日                                                                    
 *******************************************************************************/  
void GetAdminPassWord(unsigned char *PassWord)
{
  PassWord[0] = 0x01;
	PassWord[1] = 0x08;
	PassWord[2] = 0x04;
	PassWord[3] = 0x07;
	PassWord[4] = 0x05;
	PassWord[5] = 0x0b;
}
 /*******************************************************************************
 * 函数名称:GetUserPassword                                                                     
 * 描    述:错误记录清楚密码                                                         
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年8月13日                                                                    
 *******************************************************************************/  
void GetUserPassWord(unsigned char *PassWord)
{
  PassWord[0] = 6;
	PassWord[1] = 1;
	PassWord[2] = 6;
	PassWord[3] = 6;
	PassWord[4] = 1;
	PassWord[5] = 6;  
}
 /*******************************************************************************
 * 函数名称:VerifyPassword                                                                   
 * 描    述:数组比较                                                                 
 *                                                                               
 * 输    入:数组指针1，数组指针2，需比较的长度                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年4月9日                                                                     
 *******************************************************************************/  
uint8_t VerifyPassword( uint8_t *Src1, uint8_t *Src2, uint8_t Length)
{
  uint8_t i = 0; 
	for(i = 0; i < Length; i++)
	{
	  if(*(Src1+i) != *(Src2+i))
		{
		    return 0;
		}
	}
	return 1;
}

 /*******************************************************************************
 * 函数名称:RecRegisterValues                                                                   
 * 描    述:获取寄存器的值                                                                                                                          
 * 输    入:寄存器地址,寄存器数据,寄存器数据长度                                                                  
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年6月24日                                                                   
 *******************************************************************************/ 
char current_page =0;//当前页面
void RecRegisterValues(char VariableAdress,char *VariableData,char length)
{
	if(VariableAdress==PIC_ID)//读取判断当前的页面的ID
	{
		current_page =VariableData[length-1];
		if(current_page!=pageunitil)
		{
			//PageChange(cmd_page);
    }
  }
}

 /*******************************************************************************
 * 函数名称:ChangeVariableValues                                                                   
 * 描    述:改变数据变量的值                                                                                                                          
 * 输    入:变量地址,变量数据,变量长度                                                                  
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                   
 *******************************************************************************/ 
char meal1st_cnt_t= 0,chicken_cnt_t= 0,duck_cnt_t= 0,fish_cnt_t= 0; /*临时数量*/
uint8_t  PassWordLen=0;	//密码的长度为0
uint8_t  clear_cnt=0; //清楚硬币数计数
uint8_t  PassWord[6]={0};
uint8_t  InputPassWord[6]={0};
bool cardbalence_cancel_flag= false;
bool mealneed_sync = false;  //餐品同步标记
int16_t CoinTotoal_t=0; //作为基数 		
int8_t	selltime_hour= 11,selltime_hour_t=11,selltime_hour_r=0;
int8_t	selltime_minute=30, selltime_minute_t=30,selltime_minute_r=0;
int8_t  selltime_second_r=0;
bool sellmeal_flag = true;
void ChangeVariableValues(int16_t VariableAdress,char *VariableData,char length)
{
	uint8_t i= 0,j=0;
	  switch (VariableAdress)
		{
			case meal_choose: /*主角面用户选择*/
			{ 
				//如果是按的是管理用户按键的话，直接弹出密码对话框，break
				if(VariableData[1]==0x0f) 
				{
					if((Current == current_temperature)||(Current == erro_hanle)||(Current == 0))
						PageChange(Password_interface); 
						break;
				}	
				//判断是不是在出餐状态,如果不是空闲状态则break;
				else if(Current != current_temperature)
        {
          break;
        }
				//在这里根据四个按键的返回值，结合套餐判断是哪一个ID
				else
				{
          //需要加入当没有餐品的时候，不跳入选餐界面
				  UserActMessageWriteToFlash.UserAct.MealID= sell_type[VariableData[1]-1];
          if(DefineMeal[UserActMessageWriteToFlash.UserAct.MealID-1].MealCount > 0)	   //判断餐品是否大于0
          {
            SearchMeal(UserActMessageWriteToFlash.UserAct.MealID ,1); //没有份数的缺省值，不选择的话不录入1
            WaitTimeInit(&WaitTime);
            OpenTIM3();
            PageChange(MealNumChoose_interface);//显示相应界面
            PlayMusic(VOICE_1);
				  }
        }
			}break;      				
			case meal_choose_cnt: /*当前餐品的份数选择*/
			{
        //pageunitil = MealNumChoose_interface;
				if(VariableData[1]<= DefineMeal[UserActMessageWriteToFlash.UserAct.MealID-1].MealCount)	//设置餐品选择的上限
				{
          PageChange(MealNumChoose_interface+VariableData[1]); //切换到选了几个餐品
					SearchMeal(UserActMessageWriteToFlash.UserAct.MealID ,VariableData[1]); //查找当前的餐品ID是否在用户已选的餐品中			
				}
				else
				{
					PageChange(MealNumChoose_interface+ DefineMeal[UserActMessageWriteToFlash.UserAct.MealID-1].MealCount); //切换到选了几个餐品
					SearchMeal(UserActMessageWriteToFlash.UserAct.MealID ,DefineMeal[UserActMessageWriteToFlash.UserAct.MealID-1].MealCount); //查找当前的餐品ID是否在用户已选的餐品中			          
				}	 						 
			}break;     
      case mealcnt_choose:/*单页选择按钮*/
      {
			  switch(VariableData[1])
				{
					case 0x01:  /*放入购物车*/
					{
								
					}break;
					case 0x02:  /*继续选餐*/
					{
            PutIntoShopCart(); //需要修改
						PageChange(Menu_interface);
						WaitTimeInit(&WaitTime);
					}break;
 					case 0x03:  /*进入购物车*/
					{
            if(sellmeal_flag)
            {
              PutIntoShopCart();
              SettleAccounts();
              AcountCopy();
              if(UserActMessageWriteToFlash.UserAct.Meal_totoal>0)
              {  
                CloseTIM3();
                WaitTimeInit(&WaitTime);
                OpenTIM7();
                Current= waitfor_money;//进入读钱界面
                CurrentPoint= 1;       //倒计时60s，打开现金接受
                PlayMusic(VOICE_2);
              }
            }
            else
            {
              PageChange(SellMeal_TimeWait_interface);
            }
					}break;
					case 0x04:  /*取消*/
					{
						//清空所有的用户数据???
						ClearUserBuffer();
            SaveUserData();
						PageChange(Menu_interface);
						CloseTIM3();
						Current= current_temperature;//取消进入空闲程序
					}break;
					default:break;
				}
			}break;
			case payment_method: /*付款方式*/ 
			{
        uint32_t temp1= 0,temp2= 0;
				if(UserActMessageWriteToFlash.UserAct.PayShould==0) goto loop7;
				switch(VariableData[1])
				{
					case 0x01:   /*现金支付*/
					{
            //if(cash_limit_flag==true) break; //如果零钱找完，无法进入
            PageChange(PayWithCash_interface);
						CurrentPoint =2;
            PlayMusic(VOICE_3);
						if(!OpenCashSystem()){OpenCashSystem();};// printf("cash system is erro2");  //关闭现金接受
					}break;
					case 0x02:   /*银行预付卡*/
					{
						CurrentPoint =7;
						PlayMusic(VOICE_5);
						if(!CloseCashSystem()){CloseCashSystem();};// printf("cash system is erro3");  //关闭现金接受
					}break;
					case 0x03:   /*深圳通支付*/
					{
						CurrentPoint =8;
						PlayMusic(VOICE_5);
						if(!CloseCashSystem()){CloseCashSystem();};//printf("cash system is erro4");  //关闭现金接受			
					}break;
          case 0x04:   /*会员卡支付*/
          {
#ifdef test
            CurrentPoint =5;
            if(!CloseCashSystem()){CloseCashSystem();};// printf("cash system is erro3");  //关闭现金接受
#endif
          }break;      
					case 0x05:   /*取消*/
					{
	 loop7:		temp1= UserActMessageWriteToFlash.UserAct.MoneyBack= UserActMessageWriteToFlash.UserAct.PayAlready; //超时将收到的钱以硬币的形式返还
	          temp2= UserActMessageWriteToFlash.UserAct.MoneyBackShould = UserActMessageWriteToFlash.UserAct.PayAlready; //总的应该退的钱
            ClearUserBuffer();
            UserActMessageWriteToFlash.UserAct.MoneyBack= temp1;
            UserActMessageWriteToFlash.UserAct.MoneyBackShould= temp2;
            SaveUserData();
			      if(!CloseCashSystem()){CloseCashSystem();};//printf("cash system is erro5");  //关闭现金接受
            PageChange(Menu_interface);
						CloseTIM3();
						CloseTIM7();
						CurrentPoint = 0 ;
            if(UserActMessageWriteToFlash.UserAct.MoneyBack>0) //当有钱需要退的时候，标记用户取消购买，无需退币的时候直接进入温度显示状态
            {
              UserActMessageWriteToFlash.UserAct.Cancle= 0x01;
						  Current= hpper_out; 
            }
            else
            {              
              Current= current_temperature; 
            }
					}break;
					default:break;		
				}					
			}break;
			case caedbalence_cancel:/*刷卡取消*/
			{
				cardbalence_cancel_flag=true;
			}break;      
			case bill_print:
			{
				switch(VariableData[1])
				{
					case 0x01:   /*打印小票*/
          {
            if(UserActMessageWriteToFlash.UserAct.PrintTick==0x00000000)
            {
              UserActMessageWriteToFlash.UserAct.PrintTick= 0x00000001;
              PrintTickFun(&UserActMessageWriteToFlash.UserAct.PrintTick);
							PageChange(Menu_interface);
              CloseTIM4();
            }              
          }break;
					case 0x02:   /*不打印小票*/
					{
            if(UserActMessageWriteToFlash.UserAct.PrintTick==0x00000000)
            {
              UserActMessageWriteToFlash.UserAct.PrintTick= 0x00000002;
              PrintTickFun(&UserActMessageWriteToFlash.UserAct.PrintTick);
							PageChange(Menu_interface);
              CloseTIM4();				
            }
					}break;  
					default:break;
				}
			}break;
			case password:
			{
				switch(VariableData[1])
				{
					case 0x00:
					case 0x01:
					case 0x02:
					case 0x03:
					case 0x04:
					case 0x05:
					case 0x06:
					case 0x07:
					case 0x08:
					case 0x09:
					case 0x0A:						
					case 0x0B:
					case 0x0C:
					case 0x0D:
					case 0x0E:
					case 0x0F:
					{
	          if(PassWordLen < 6)
		        {
		          InputPassWord[PassWordLen] = (VariableData[1]);
		          PassWordLen++;
			        DisplayPassWord(PassWordLen);//显示一个字符串******
		        }
	        }break;					
					case 0x10:/*退格键*/
					{
	          if(PassWordLen > 0)
		        {
	            InputPassWord[PassWordLen] = 0;
	            PassWordLen--;
		          DisplayPassWord(PassWordLen);
		        }
					}break;
					case 0x11:/*确认件*/ //需要加入超级密码
					{
	          GetPassWord(PassWord);
    	      if(VerifyPassword(PassWord, InputPassWord,6) == 1) //放餐密码正确
		        {
							PassWordLen = 0;		
							DisplayPassWord(0);//清楚密码显示
							memset(InputPassWord,0,6);
						  if(Current== 0x00) 
						  {
							  break;
						  }              
						  if(erro_record!=0) 
						  {
							  break;
						  }							
							//如果有错误标记则break;
		           /*进入餐品放置界面*/
		           PageChange(MealSet_interface);					 
							 InitSetting();//清空第一二三列的数据 //对放餐的数据进行初始化
			         break;
		        }
						GetAdminPassWord(PassWord);//硬币设置密码
    	      if(VerifyPassword(PassWord, InputPassWord,6) == 1) 
		        {
							//进入管理员功能设置，设置硬币设置界面
		           /*进入管理员界面*/
							memset(InputPassWord,0,6);	
              DisplayPassWord(0);//清楚密码显示
              PassWordLen = 0;
						  if(Current== 0x00) 
						  {
							  break;
						  }
						  if(erro_record!=0) 
						  {
							  break;
						  }              
               VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal);//显示机内硬币总数
               VariableChage(coins_input,0);//显示放币数
               VariableChage(coins_retain,coins_remain);//保留硬币数
               VariableChage(coins_back,0); //显示退币数
               PageChange(Coinset_interface);
               //当前退币数
               break;
		        }
            GetUserPassWord(PassWord);   //出错数据显示密码
            if(VerifyPassword(PassWord, InputPassWord,6) == 1) 
            {
               memset(InputPassWord,0,6);
               DisplayPassWord(0);//清楚密码显示
               PassWordLen = 0;	
               if(erro_record!=0)
               {
                 /*当*/
                 if(!((erro_record&1<<arm_limit)||(erro_record&1<<signin_erro)||erro_record&1<<network_erro))
                   DisplayUserRecord();
                 PageChange(UserAbonamalRecord_interface);
               }                 
               break;              
            }
		        else
		        {
		          /*密码验证错误，进入密码输入界面*/   
							 DisplayPassWord(0);	
               PageChange(Password_interface);						
			         PassWordLen = 0;
		        }						
	        }break;						
					case 0x12:/*返回*/
					{
						if(erro_record!=0) //有错误的时候只能返回到错误界面
						{
						  PageChange(Err_interface);
							break;
						}								
		        PageChange(Menu_interface);
						DisplayPassWord(0);//清楚密码显示
		        PassWordLen = 0;	
					}break;
          default:break;					
        } 					
			}break;
			case record_clear:
			{
				switch(VariableData[1])
			  {
					case 0x01: //清除数据
					{
						erro_flag=0; //清除数据标记,数据清除在AbnormalHandle函数里
						if(Current==current_temperature)//限定在初始化状态
						{
							UserActMessageWriteToFlash.UserAct.MoneyBack=0;
							ClearUserBuffer();
					    SaveUserData();
							PageChange(Logo_interface);
						}
   				}break;
					case 0x02: //返回
					{
						PageChange(Err_interface);
					}break;
          default:break;
				}	
			}break;
			case admin_set:
			{
				switch(VariableData[1])
				{ 
					case 0x01:  /*温度设置按钮*/
					{
						PageChange(TemperatureSet_interface);
					}break;
					case 0x02:  /*餐品设置按钮 应该改为一键清空所有数据 */
					{
							for(i=0;i<90;i++)
	            {
	              FloorMealMessageWriteToFlash.FlashBuffer[i] = 0 ;
	            }
	            WriteMeal();  //写入餐品数据
							StatisticsTotal(); 
							DispLeftMeal();
					}break;
					case 0x03:	/*取消键*/
					{
						if(mealneed_sync == false)
						{
						  PageChange(Menu_interface);
						}
					}
          default:break;
				}					
			}break;
			case meal_num: //当改变餐品的编号的时候，需要查找当前的ID
			{
				uint8_t cnt_t= 0;
			  CurFloor.MealID= sell_type[VariableData[1]-1];	//当前的就是餐品的ID
				InitSetting();
				for(cnt_t = 0; cnt_t < FloorMealNum; cnt_t++)  //查找那一层有是空的
				{
					if(FloorMealMessageWriteToFlash.FloorMeal[cnt_t].MealCount ==0)
					{
						 CurFloor.FloorNum = cnt_t+1;
						 break;
					}			
				}
				VariableChage(floor_num,CurFloor.FloorNum);				
			}break;
      case floor_num:
			{
			  CurFloor.FloorNum= VariableData[1];
				InitSetting();
			}break;
      case row_1st:
      {
				CurFloor.FCount= VariableData[1];
			}break;	
      case row_2nd:
      {
				CurFloor.SCount= VariableData[1];
			}break;	
      case row_3rd:
      {
				CurFloor.TCount= VariableData[1];
			}break;	
      case mealinput_choose:
			{
				switch(VariableData[1])
				{
					case 0x01:  /*放满*/
					{
						CurFloor.FCount = 3;
		        CurFloor.SCount = 3;
		        CurFloor.TCount = 3; 
		        CurFloor.MealCount = 9;
					  VariableChage(row_1st,CurFloor.FCount);
		        VariableChage(row_2nd,CurFloor.SCount);
		        VariableChage(row_3rd,CurFloor.TCount);	
					}break;
					case 0x02:  /*清空*/
					{
						CurFloor.FCount = 0;
		        CurFloor.SCount = 0;
		        CurFloor.TCount = 0; 
		        CurFloor.MealCount = 0;
					  VariableChage(row_1st,CurFloor.FCount);
		        VariableChage(row_2nd,CurFloor.SCount);
		        VariableChage(row_3rd,CurFloor.TCount);							
					}break;
					case 0x03:  /*确认*/
					{
						/*计算总共的餐品数*/
						CurFloor.MealCount=  CurFloor.FCount + CurFloor.SCount + CurFloor.TCount;
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
						for(j = 0; j < MealKindTotoal; j++)	 //统计每个餐品的数据
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
							{														 /*餐的ID是从1-6*/
								if(FloorMealMessageWriteToFlash.FloorMeal[i].MealID == j + 1)
								{
									DefineMeal[j].MealCount      = DefineMeal[j].MealCount + FloorMealMessageWriteToFlash.FloorMeal[i].MealCount;
									DefineMeal[j].Position[i][0] = FloorMealMessageWriteToFlash.FloorMeal[i].FCount;
									DefineMeal[j].Position[i][1] = FloorMealMessageWriteToFlash.FloorMeal[i].SCount;
									DefineMeal[j].Position[i][2] = FloorMealMessageWriteToFlash.FloorMeal[i].TCount;
									//printf("DefineMeal[%d].Position[%d][0]=%d\r\n",j,i,DefineMeal[j].Position[i][0]);
								}
							}
						}
						StatisticsTotal(); //先统计数目在写入
						WriteMeal(); //写数据到flash
						DispLeftMeal(); //更新餐品数据
						mealneed_sync = true;
						//printf("DefineMeal[%d].MealCount=%d\r\n",i,DefineMeal[i].MealCount);
					}break;
					case 0x04:  /*数据同步*/
					{
						uint8_t cnt_t=0,floor=0;
						for(cnt_t=0x00;cnt_t<0x04;cnt_t++)
						{
							/*显示餐品,数量,清空显示*/									
              SyncMealNameDisp(0,cnt_t);
              SyncMealCntDisp(0xFF,cnt_t); 									 
						}	
						//禁止屏幕点击*/
            ScreenControl(ScreenDisable);
						/*餐品同步时尽量避免与服务器断开的情况*/
						/*不能加入签到函数，直接死掉*/
						//if(!SignInFunction())  AbnormalHandle(signin_erro);
						/*上传断网后的数据，供后台实时更新数据*/
            // SendtoServce(); //不要，因为没有用， 
						//数据同步子程序
						if(0==MealDataCompareFun()) /*同步超时返回，不显示对比数据*/      
            {
              PageChange(Data_synchronization+2);
              break;        
            }
						if(MealCompareData.MealCompareTotoal==0xFFFFFFFF) //正确
						 {
							 mealneed_sync = false;
							 PageChange(Data_synchronization);
							 for(cnt_t=0x00;cnt_t<0x04;cnt_t++)
							 {
									 /*显示餐品,数量*/									
                  SyncMealNameDisp(sell_type[cnt_t],floor);
                  SyncMealCntDisp(DefineMeal[sell_type[cnt_t]-1].MealCount,floor); 
                  floor++;									 
							 }							 
						 }
						 /*可以加入对比的返回值进行判断*/
             else
             {
							 mealneed_sync = true;
							 PageChange(Data_synchronization+2);
							 for(cnt_t=0x00;cnt_t<0x04;cnt_t++)
							 {
								 if(MealCompareData.MealComparePart[cnt_t]==0xff)
								 {
									//餐品同步成功不做任何事情 
								 }
								 else
								 {
									 /*显示餐品,数量*/									
                  SyncMealNameDisp(sell_type[cnt_t],floor);
                  SyncMealCntDisp(MealCompareData.MealComparePart[cnt_t],floor); 
                  floor++;									 
								 }
               }						 
						 }
						//超时时退出，错误退出
						ScreenControl(ScreenEnable);
						DispLeftMeal();//改变剩余餐品数量显示
					}break;
					case 0x05:  /*返回*/
					{
						CurFloor.FCount    = 0;
						CurFloor.SCount    = 0;
						CurFloor.TCount    = 0;
						CurFloor.MealID    = 0;
						CurFloor.MealCount = 0;
						CurFloor.FloorNum  = 0;	 
	          PageChange(MealSet_interface);
					}break;
					case 0x06: /*售餐设置*/
					{
            VariableChage(set_sellmeal_hour,selltime_hour);
            VariableChage(set_sellmeal_minute,selltime_minute);  
						PageChange(SellMeal_TimeSet_interface); 
					}break;
          case 0x07: /*餐品对比成功返回*/
          {
            PageChange(MealInput_interface);
          }break;
					default:break;	
				}
			}break;
			case temprature_set:
			{
        if(VariableData[1]==0xFF)
        {
          PageChange(MealSet_interface);
          VariableChage(temprature_set,temprature_old);
        }
        else
        {
          SetTemper(VariableData[1]);
          temprature_old= VariableData[1];  
          auto_heat_flag = false;					
          if(VariableData[1]==99)  VariableChage(temprature_set,temprature_old);  //显示当前的温度   
        }
				
			}break;
			case coins_key:  //按一次退一次
      {
				int16_t cnt_t=0,i=0;
			  int16_t coins_time=0;
				int16_t MoneyBack_cnt=0;
				/*当按下任意一个按键的时候需要读硬币数据进行处理*/
				CoinsTotoalMessageWriteToFlash.CoinTotoal+= CoinTotoal_t;
				CoinTotoal_t=0;
				WriteCoins();
				if(VariableData[1]==0x01) /*部分退币*/
				{
					if(CoinsTotoalMessageWriteToFlash.CoinTotoal-coins_remain>0)
					{
						Coins_cnt=0;
						coins_time= ((CoinsTotoalMessageWriteToFlash.CoinTotoal-coins_remain)/10);
						cnt_t= ((CoinsTotoalMessageWriteToFlash.CoinTotoal-coins_remain)%10);
						VariableChage(coins_back,Coins_cnt);
						for(i=0;i<coins_time+1;i++) //一次退币10个，
						{
							if(i!=coins_time)
							{
								UserActMessageWriteToFlash.UserAct.MoneyBack+=SendOutN_Coin(10);		
							}
							else
							{
								if(cnt_t>0)
									UserActMessageWriteToFlash.UserAct.MoneyBack+=SendOutN_Coin(cnt_t);	
								else
									break;
							}
							delay_ms(1000); 
							VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal);//显示机内硬币数
							VariableChage(coins_back,Coins_cnt);						
							if(ErrorType ==1)  //退币机无币错误,直接进入错误状态
							{
								break;
							}														
						}
					}	
					//采用能发送几个是几个的方法是不可行的，//CoinsTotoalMessageWriteToFlash.CoinTotoal = SendOutN_Coin(CoinsTotoalMessageWriteToFlash.CoinTotoal);			
				}
				else if(VariableData[1] == 0x02) /*全部退币*/
				{
					if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)!=0)//加入限定
					{
						Coins_cnt=0;
						coins_time= (CoinsTotoalMessageWriteToFlash.CoinTotoal/10);
						cnt_t= (CoinsTotoalMessageWriteToFlash.CoinTotoal%10);
						VariableChage(coins_back,Coins_cnt);
						for(i=0;i<coins_time+1;i++) //一次退币10个，
						{
							if(i!=coins_time)
							{
								MoneyBack_cnt+=SendOutN_Coin(10);		
							}
							else
							{
								if(cnt_t>0)
									MoneyBack_cnt+=SendOutN_Coin(cnt_t);	
								else
									break;
							}
							delay_ms(1500); 
							VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal);//显示机内硬币数
							VariableChage(coins_back,Coins_cnt);												
						}							
						if(ErrorType ==1)  //退币机无币错误,再次发送退币
						{
							MoneyBack_cnt= SendOutN_Coin(MoneyBack_cnt);
						}	
					}						
				}
				else if(VariableData[1] == 0x03) /*返回*/
				{
          clear_cnt=0;
					StopRefond();
					PageChange(Menu_interface);
				}
			}break;	
      case coins_input: /*将要将放置的硬币与总数联合起来*/
      {
        myunion.adr[0] =	VariableData[1];
				myunion.adr[1] =	VariableData[0];
				CoinTotoal_t   =  myunion.adress;
				VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal+ CoinTotoal_t);//显示机内硬币数			
			}break;
      case coins_clear: /*清除硬币计数*/
      {
        if(VariableData[1]==0x01)
        {
          clear_cnt++;
          if(clear_cnt>5)
          {
            clear_cnt=0;
            CoinTotoal_t  =  0;
            CoinsTotoalMessageWriteToFlash.CoinTotoal = 0;
            VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal+ CoinTotoal_t);//显示机内硬币数	        
            VariableChage(coins_input,CoinsTotoalMessageWriteToFlash.CoinTotoal+ CoinTotoal_t); //将增加的硬币数清零
            WriteCoins();
          }
        }
      }break;
      case set_sellmeal_hour: /*售餐设置的小时*/	
			{
				selltime_hour_t= VariableData[1];		
			}break;				
			case set_sellmeal_minute:/*售餐设置的分*/	
			{
				selltime_minute_t= VariableData[1];				
      }break;				
			case set_sellmeal_key: /*售餐设置的按键*/
			{
				if(VariableData[1]==0x01) /*确认*/
				{
					selltime_hour= selltime_hour_t;
					selltime_minute= selltime_minute_t;
					DisplayTimeCutDown();
        }
				else if(VariableData[1]==0x02) /*返回*/
				{
          PageChange(Data_synchronization);				
        }
      }break;
			  default:break;		
		}
}
 /*******************************************************************************
 * 函数名称:DealSeriAceptData                                                                   
 * 描    述:处理从迪文屏幕返回的数据                                                        
 *          如何实现三个连续的数据：0xA5 0XA5 0X5A                                                                           
 * 输    入:无                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                   
 *******************************************************************************/ 	
void DealSeriAceptData(void)
 {
	unsigned char i;
	unsigned char temp,temp1,length,checkout;
	char RegisterAdress=0;
	int16_t VariableAdress=0;
	
	unsigned char Rx3DataBuff[10]={0};/*设置一个数组大小，?以免越界(out of bounds),?可变长度数组*/
	char RegisterData[5]={0};  //寄存器数据数组
	char VariableData[5]={0};  //变量数据数组
	char RegisterLength= 0;   //寄存器数据的长度
	char VariableLength= 0;   //变量数据的长度
	while(UART3_GetCharsInRxBuf()>=9) //获取缓冲区大小，直至缓冲区无数据
	{	
		if(USART3_GetChar(&temp)==1)
		{	
			if(temp==FH0)	//匹配帧头第一个字节
			{ 
loop:	  if(USART3_GetChar(&temp1)==1)  
				{
					if(temp1==FH1)  //匹配帧头第二个字节：
					{
						if(USART3_GetChar(&length)==1)  //获取命令字节长度
						{
							checkout =1;//置位开始读取命令与数据标志	
						}	
					}
					else if(temp1==FH0)	goto loop; //如果持续出现0xA5则继续判断下一个字节是否为0x5A
				}
			}
		}
	  if(checkout==1) //直到获取数据长度，然后读缓存
	  {
	 	  checkout =0;  //复位标志
		  for(i=0;i<length;i++) USART3_GetChar(Rx3DataBuff+i);
		  if(Rx3DataBuff[0]==0x81)  //读寄存器返回命令
		  {
			  RegisterAdress =Rx3DataBuff[1];
			  for(i=0;i<(length-2);i++)
			  RegisterData[i]=Rx3DataBuff[2+i];
				//加入修改相关数据的功能
				RecRegisterValues(RegisterAdress,RegisterData,RegisterLength);
		  }
		  else if(Rx3DataBuff[0]==0x83) //读数据存储器返回命令
		  {
			  myunion.adr[1] =Rx3DataBuff[1]; //有可能是小端模式引起的
			  myunion.adr[0] =Rx3DataBuff[2];
			  VariableAdress =myunion.adress;
			  VariableLength =Rx3DataBuff[3];
			  for(i=0;i<(length-4);i++)
			  VariableData[i]=Rx3DataBuff[4+i];
			  //加入修改变量地址数据的功能
			  ChangeVariableValues(VariableAdress,VariableData,VariableLength);
		   }
	   } 
	 }		
}

