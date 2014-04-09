#ifndef  __MS_d245__h																									
#define  __MS_d245__h
#include "stdint.h"
#include "rtc.h"

typedef struct
{
   unsigned char  P_Number;    /*购买的分数*/
   unsigned char  P_ID;        /*购买的餐品名字*/
   unsigned char  P_paymoney;  /*服了多少钱*/
   unsigned char  P_PayShould ;/*该付多少钱*/
   unsigned	char  P_MoneyBack ;/*找回多少钱给客户*/
   unsigned char  P_PayForCards ;/*卡服了多少钱*/
   unsigned char  P_MealPrice ;	  /*单价多少钱*/
   unsigned char  P_PayForBills ;  /*纸币付了多少钱*/
   unsigned char  P_PayForCoins ;  /*硬币付了多少钱*/
}P_stuction;

extern P_stuction Print_Struct;

void PrintInit(void);
uint8_t CheckPrintStatus(void);
uint8_t PrintMessage(uint8_t flag);
void  SPRT(uint8_t flag);
void COPY(Struct_TD  a,unsigned char *p0,unsigned char *p1);

#endif	


