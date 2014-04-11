#include "stm32f4xx.h"
#include "MsgHandle.h"
#include "serialscreen.h"
#include "bsp.h"

MealAttribute DefineMeal[4];	//定义4个菜系
FloorAttribute FloorMeal[FloorMealNum], CurFloor;
UserSelection UserAct;
FloorMealMessage FloorMealMessageWriteToFlash;
uint8_t  WaitTime=0;

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
		for(j = 0; j < 4; j++)	 //4代表餐ID号
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
		CurFloor.MealCount = 0;
		CurFloor.FCount    = 0;
		CurFloor.SCount    = 0;
		CurFloor.TCount    = 0;			
		VariableChage(row_1st,CurFloor.FCount);
		VariableChage(row_2nd,CurFloor.SCount);
		VariableChage(row_3rd,CurFloor.TCount);
		//之后在加入放餐设置
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
void MealArr(unsigned char index)
{
    int  MoneyBack = 0 ,PayBill = 0;
	  switch(index)
	  {
     
      case 1 :	/*购买餐品的ID*/
			{				
 		    CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x20;
				/*购买餐品的价格*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x15 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
			}
				break ;
      case 2 :	/*购买餐品的ID*/
			{
 		    CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x21;
				/*购买餐品的价格*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x15 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
			}
				 break ;
      case 3 :	/*购买餐品的ID*/
			{
 		  	CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x22;
				/*购买餐品的价格*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x15 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
				 break ;
			}
       case 4 :	/*购买餐品的ID*/
			 {
 		  	CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x23;
				/*购买餐品的价格*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x20 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
			 }
				 break ;
	   default : break ;

	}	
/*------购买餐品的数量-----------------*/
				CustomerSel.MealNo         =  Print_Struct.P_Number;
				/*购买餐品的类型*/
        CustomerSel.PayType        =  UserAct.PayType; //	UserAct.PayType  ;
				if(CustomerSel.PayType == '1')	/*如果是现金购买*/
				{
				MoneyBack  =Print_Struct.P_MoneyBack *100 ;  /*扩大10倍*/
			//	printf("UserAct.MoneyBack=%d\r\n",UserAct.MoneyBack);
				/*十进制转换成16*/
				CustomerSel.Change[0]      =	     MoneyBack / 10000000000 %100;
				CustomerSel.Change[0]      =         CustomerSel.Change[0]/10 *16 +CustomerSel.Change[0]%10 ;   
				CustomerSel.Change[1]      =	     MoneyBack / 100000000 %100;
				CustomerSel.Change[1]      =         CustomerSel.Change[1]/10 *16 +CustomerSel.Change[1]%10 ;                              
				CustomerSel.Change[2]      =	     MoneyBack / 1000000 %100;
				CustomerSel.Change[2]      =         CustomerSel.Change[2]/10 *16 +CustomerSel.Change[2]%10 ;
				CustomerSel.Change[3]      =	     MoneyBack / 10000 %100;
				CustomerSel.Change[3]      =         CustomerSel.Change[3]/10 *16 +CustomerSel.Change[3]%10 ;
				CustomerSel.Change[4]      =	     MoneyBack / 100 %100;
				CustomerSel.Change[4]      =         CustomerSel.Change[4]/10 *16 +CustomerSel.Change[4]%10 ;
				CustomerSel.Change[5]      =	     MoneyBack % 100 ;
				CustomerSel.Change[5]      =         CustomerSel.Change[5]/10 *16 +CustomerSel.Change[5]%10 ;
				MoneyBack = 0 ;
			 	}
				/*购买餐品的剩余份数*/
				CustomerSel.RemainMealNum[0]  =	 (DefineMeal[index-1].MealCount>>8)&0xff;
				CustomerSel.RemainMealNum[1]  =	  DefineMeal[index-1].MealCount &0xff;
				/*购买的类型*/
				CustomerSel.MealName	   =  	   index ;
				/*付了多少现金*/
			  //PayBill  =UserAct.PayForBills +*100 ;  /*扩大10倍*/
		    PayBill  =	UserAct.PayForBills +	UserAct.PayForCoins +UserAct.PayForCards ;
	 			 /*支付了多少钱*/
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
}

