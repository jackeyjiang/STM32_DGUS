#include "stm32f4xx.h"
#include "MsgHandle.h"
#include "serialscreen.h"
#include "bsp.h"

MealAttribute DefineMeal[MealKindTotoal];	//定义6个菜系
UserActMessage UserActMessageWriteToFlash;//用户数据写入flash
FloorAttribute FloorMeal[FloorMealNum], CurFloor;
UserSelection UserAct;
FloorMealMessage FloorMealMessageWriteToFlash;
CoinsTotoalMessage CoinsTotoalMessageWriteToFlash;
uint8_t  WaitTime=0;
uint8_t  WaitMealTime=0;

/**
  * @brief  Converts a 2 digit decimal to BCD format.
  * @param  Value: Byte to be converted.
  * @retval Converted byte
  */
static uint8_t RTC_ByteToBcd2(uint8_t Value)
{
  uint8_t bcdhigh = 0;
  
  while (Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }
  
  return  ((uint8_t)(bcdhigh << 4) | Value);
}

/**
  * @brief  Convert from 2 digit BCD to Binary.
  * @param  Value: BCD value to be converted.
  * @retval Converted word
  */
static uint8_t RTC_Bcd2ToByte(uint8_t Value)
{
  uint8_t tmp = 0;
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
  return (tmp + (Value & (uint8_t)0x0F));
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
		for(j = 0; j < MealKindTotoal; j++)	 //4代表餐ID号
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
 /*******************************************************************************
 * 函数名称:InitSetting                                                                   
 * 描    述:将放置的数据清零                                                                
 *                                                                               
 * 输    入:                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年4月9日                                                                     
 *******************************************************************************/  
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

	VariableChage(row_1st,CurFloor.FCount);
	VariableChage(row_2nd,CurFloor.SCount);
	VariableChage(row_3rd,CurFloor.TCount);
}
 /*******************************************************************************
 * 函数名称:MealArr                                                                     
 * 描    述: 餐品的属性                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
uint32_t payfor_meal = 0;        //单个餐品已付的钱

void MealArr(unsigned char index)
{
    uint32_t  PayBill=0;
    uint32_t  MoneyPayBack_Already_t=0;
    uint8_t   MealPrice_t=0;
/*-------匹配ID和价格------------------------*/
    memcpy(CustomerSel.MealID,Meal[index-1].MealID,4);
    memcpy(CustomerSel.MealPrice,Meal[index-1].MealPrice,6);
    MealPrice_t= RTC_Bcd2ToByte(CustomerSel.MealPrice[4]);  //1.先转换为bin  
    CustomerSel.MealPrice[4]= RTC_ByteToBcd2(MealPrice_t*Discount/10); //2.将bin码X折扣，然后转换为BCD 
/*------购买餐品的数量-----------------*/
		CustomerSel.MealNo  =  0x01; //取一个餐品上传一次数据
		/*购买餐品的类型*/
    CustomerSel.PayType =  UserActMessageWriteToFlash.UserAct.PayType;  //	UserAct.PayType  ;
		CustomerSel.MealName =  	index ;
	 	/*支付了多少钱*/
		PayBill= payfor_meal* 100; //已付的钱=需找币的钱+餐品的单价，元转换为分
		CustomerSel.DealBalance[0]      =	      PayBill / 10000000000 %100;
		CustomerSel.DealBalance[0]      =       CustomerSel.DealBalance[0]/10 *16 +CustomerSel.DealBalance[0]%10 ;   
		CustomerSel.DealBalance[1]      =	      PayBill / 100000000 %100;
		CustomerSel.DealBalance[1]      =       CustomerSel.DealBalance[1]/10 *16 +CustomerSel.DealBalance[1]%10 ;                              
		CustomerSel.DealBalance[2]      =	      PayBill / 1000000 %100;
		CustomerSel.DealBalance[2]      =       CustomerSel.DealBalance[2]/10 *16 +CustomerSel.DealBalance[2]%10 ;
		CustomerSel.DealBalance[3]      =	      PayBill / 10000 %100;
		CustomerSel.DealBalance[3]      =       CustomerSel.DealBalance[3]/10 *16 +CustomerSel.DealBalance[3]%10 ;
		CustomerSel.DealBalance[4]      =	      PayBill / 100 %100;
		CustomerSel.DealBalance[4]      =       CustomerSel.DealBalance[4]/10 *16 +CustomerSel.DealBalance[4]%10 ;
		CustomerSel.DealBalance[5]      =	      PayBill % 100 ;
		CustomerSel.DealBalance[5]      =       CustomerSel.DealBalance[5]/10 *16 +CustomerSel.Change[5]%10 ;		
		/*要上传的已找零分析:第一次上传的售后MoneyPayBack_Already_2nd=0；第二次上传的时候UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already=0；
    第一次就取餐失败的时候，UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already!=0,UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd!=0,两个相加就是总的退币
    */
    MoneyPayBack_Already_t= (UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already + UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd) *100;
    
    CustomerSel.Change[0]      =	     MoneyPayBack_Already_t / 10000000000 %100;
    CustomerSel.Change[0]      =         CustomerSel.Change[0]/10 *16 +CustomerSel.Change[0]%10 ;   
    CustomerSel.Change[1]      =	     MoneyPayBack_Already_t / 100000000 %100;
    CustomerSel.Change[1]      =         CustomerSel.Change[1]/10 *16 +CustomerSel.Change[1]%10 ;                              
    CustomerSel.Change[2]      =	     MoneyPayBack_Already_t / 1000000 %100;
    CustomerSel.Change[2]      =         CustomerSel.Change[2]/10 *16 +CustomerSel.Change[2]%10 ;
    CustomerSel.Change[3]      =	     MoneyPayBack_Already_t / 10000 %100;
    CustomerSel.Change[3]      =         CustomerSel.Change[3]/10 *16 +CustomerSel.Change[3]%10 ;
    CustomerSel.Change[4]      =	     MoneyPayBack_Already_t / 100 %100;
    CustomerSel.Change[4]      =         CustomerSel.Change[4]/10 *16 +CustomerSel.Change[4]%10 ;
    CustomerSel.Change[5]      =	     MoneyPayBack_Already_t % 100 ;
    CustomerSel.Change[5]      =         CustomerSel.Change[5]/10 *16 +CustomerSel.Change[5]%10 ;
		/*购买餐品的剩余份数*/
		CustomerSel.RemainMealNum[0]  =	 (DefineMeal[index-1].MealCount>>8)&0xff;
		CustomerSel.RemainMealNum[1]  =	  DefineMeal[index-1].MealCount &0xff;
    PayBill= 0; payfor_meal=0;UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already=0;UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd=0;
}
