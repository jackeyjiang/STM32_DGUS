#include "bsp.h"



uint16_t   Balance;
//uint8_t    CANStatus;
static     uint8_t IsCard	;
uint8_t    DisplayTimes	 ;
uint8_t    Line = 0,Column = 0 ;


void OpenMoneySystem(void)
{
 //   OpenCards();   //´ò¿ª¿¨
//	OpenCoinMachine();   //¿ª´òÓ²±Ò»ú
}


void CloseMoneySystem(void)
{
  //CloseCards();
//	CloseCoinMachine();
//    NoAcceptMoneyFun();

} 
 /*******************************************************************************
 * º¯ÊýÃû³Æ:WaitTimeInit                                                                     
 * Ãè    Êö:°ÑWaitTime´ÓÐÂ°²ÅÅµ½ÐÂµÄÖµ                                                                  
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:void                                                               
 * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
 *******************************************************************************/ 
 void  WaitTimeInit(uint8_t *Time)
 {
       	*Time = 60;
 }

 /*******************************************************************************
 * º¯ÊýÃû³Æ:FindMeal                                                                     
 * Ãè    Êö:²éÕÒÄÇ¸öµØ·½ÓÐ²Í                                                                  
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:void                                                               
 * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
 *******************************************************************************/ 
 uint8_t FindMeal(MealAttribute *DefineMeal) 
 {
     for(Line = 0; Line < FloorMealNum; Line++)
		{
			for(Column = 0; Column < 3; Column++)
			{
//			  printf("UserAct.MealID=%d\r\n",UserAct.MealID);
			  if(DefineMeal[UserAct.MealID - 1].Position[Line][Column] > 0)	
			   return 0;
				
			}
			if(Column < 3)
			{
				return 0 ;
			}
		}
		return 1 ;
 }
// /*******************************************************************************
// * º¯ÊýÃû³Æ:CanRecvCmd                                                                     
// * Ãè    Êö:½ÓÊÕcanµÄÃüÁîÊÇ·ñÕýÈ·                                                                  
// *                                                                               
// * Êä    Èë:ÎÞ                                                                     
// * Êä    ³ö:ÎÞ                                                                     
// * ·µ    »Ø:void                                                               
// * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
// *******************************************************************************/
// uint8_t CanRecvCmd(uint8_t p)
// {
//        uint8_t i = 0 ;
//  		while(i < 100)
//		{
//			if(CANStatus == p)
//			{
//				return 0;
//			}
//			else
//			{
//				i++;
//			    delay_ms(100);
//			}
//		}
//		if(i == 100)
//		{
//			PictrueDisplay(47);
//			DisplayErrorMealPrice(300);
//			return 1 ;
//		}
//	  return 0;
// }

 /*******************************************************************************
 * º¯ÊýÃû³Æ:MealArr                                                                     
 * Ãè    Êö: ²ÍÆ·µÄÊôÐÔ                                                                 
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:void                                                               
 * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
 *******************************************************************************/ 
void MealArr(unsigned char index)
{
      int  MoneyBack = 0 ,PayBill = 0;
	  switch(index)
	  {
     
      case 1 :	/*¹ºÂò²ÍÆ·µÄID*/	 
 		     	CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x20;
				/*¹ºÂò²ÍÆ·µÄ¼Û¸ñ*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x15 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;

 

				 break ;
      case 2 :	/*¹ºÂò²ÍÆ·µÄID*/
 		        CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x21;
				/*¹ºÂò²ÍÆ·µÄ¼Û¸ñ*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x15 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
				 break ;
      case 3 :	/*¹ºÂò²ÍÆ·µÄID*/
 		  	    CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x22;
				/*¹ºÂò²ÍÆ·µÄ¼Û¸ñ*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x15 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
				 break ;
       case 4 :	/*¹ºÂò²ÍÆ·µÄID*/
 		  	    CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x23;
				/*¹ºÂò²ÍÆ·µÄ¼Û¸ñ*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x20 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
				 break ;
        case 5 :	/*¹ºÂò²ÍÆ·µÄID*/
 		        CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x24;
				/*¹ºÂò²ÍÆ·µÄ¼Û¸ñ*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x20 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
				 break ;
      case 6 :	/*¹ºÂò²ÍÆ·µÄID*/
 		        CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x25;
				/*¹ºÂò²ÍÆ·µÄ¼Û¸ñ*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x20 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
				 break ;
      case 7 :	/*¹ºÂò²ÍÆ·µÄID*/
 		        CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x26;
				/*¹ºÂò²ÍÆ·µÄ¼Û¸ñ*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x25 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
				 break ;
      case 8 :	/*¹ºÂò²ÍÆ·µÄID*/
 		     	CustomerSel.MealID[0]		   =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x27;
				/*¹ºÂò²ÍÆ·µÄ¼Û¸ñ*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x25 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
				 break ;
      case 9 :	/*¹ºÂò²ÍÆ·µÄID*/
 		       CustomerSel.MealID[0]		       =   0x10;
				CustomerSel.MealID[1]		   =   0x00;
				CustomerSel.MealID[2]		   =   0x00;
				CustomerSel.MealID[3]		   =   0x28;
				/*¹ºÂò²ÍÆ·µÄ¼Û¸ñ*/
				CustomerSel.MealPrice[0]      =    0x00 ;
				CustomerSel.MealPrice[1]      =	   0x00 ;
				CustomerSel.MealPrice[2]      =	   0x00	;
				CustomerSel.MealPrice[3]      =	   0x00 ;
				CustomerSel.MealPrice[4]      =	   0x25 ;
				CustomerSel.MealPrice[5]      =	   0x00 ;
				 break ;
	default : break ;

	}			/*¹ºÂò²ÍÆ·µÄÊýÁ¿*/
				CustomerSel.MealNo         =  Print_Struct.P_Number;
				/*¹ºÂò²ÍÆ·µÄÀàÐÍ*/
                CustomerSel.PayType        =  UserAct.PayType; //	UserAct.PayType  ;
				if(CustomerSel.PayType == '1')	/*Èç¹ûÊÇÏÖ½ð¹ºÂò*/
				{
				MoneyBack  =Print_Struct.P_MoneyBack *100 ;  /*À©´ó10±¶*/
			//	printf("UserAct.MoneyBack=%d\r\n",UserAct.MoneyBack);
				/*Ê®½øÖÆ×ª»»³É16*/
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
				/*¹ºÂò²ÍÆ·µÄÊ£Óà·ÝÊý*/
				CustomerSel.RemainMealNum[0]  =	 (DefineMeal[index-1].MealCount>>8)&0xff;
				CustomerSel.RemainMealNum[1]  =	  DefineMeal[index-1].MealCount &0xff;
//				printf("CustomerSel.RemainMealNum[0]  =%d\r\n",CustomerSel.RemainMealNum[0]);
//				printf("CustomerSel.RemainMealNum[1]  =%d\r\n",CustomerSel.RemainMealNum[1]);
				/*¹ºÂòµÄÀàÐÍ*/
				CustomerSel.MealName	   =  	   index ;
				/*¸¶ÁË¶àÉÙÏÖ½ð*/
			//	 PayBill  =UserAct.PayForBills +*100 ;  /*À©´ó10±¶*/
		        PayBill  =	UserAct.PayForBills +	UserAct.PayForCoins +UserAct.PayForCards ;
	 			 /*Ö§¸¶ÁË¶àÉÙÇ®*/
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
  /*******************************************************************************
 * º¯ÊýÃû³Æ:PrintTick                                                                     
 * Ãè    Êö:ÊÇ·ñ´òÓ¡Ð¡Æ±                                                                
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:void                                                               
 * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
 *******************************************************************************/ 
			
void PrintTickFun(unsigned char *PrintTickFlag)
{
       
	   if(*PrintTickFlag == 0x01 )
	  {
//	    	 printf(" Print_Struct.P_Number  =%d\r\n",Print_Struct.P_Number );		 //ID
//		     printf(" Print_Struct.P_ID =%d\r\n",Print_Struct.P_ID);   //ÊýÁ¿
//			 printf(" UserAct.MealPrice =%d\r\n",UserAct.MealPrice);	//µ¥¼Û
//			 printf(" Print_Struct.P_paymoney =%d\r\n",Print_Struct.P_paymoney);	//×Ü¼Û
//			 printf(" Print_Struct.P_PayShould =%d\r\n",Print_Struct.P_PayShould);  //Í¶ÁË¶àÉÙÖ½±Ò
//			 printf(" Print_Struct.P_MoneyBack =%d\r\n",Print_Struct.P_MoneyBack);   //Í¶ÁË¶àÉÙÓ²±Ò
		 TIM_Cmd(TIM4, DISABLE);
		 PictrueDisplay((UserAct.MealID-1)*7+9);	           //ÏÔÊ¾Í¼Æ¬
		 /*ÏÔÊ¾Ä¬ÈÏµÄÒ»·Ý*/
		 /*ÏÔÊ¾¿Í»§Í¶±ÒµÄ½ð¶î*/
		 DisPlayChar(Display_Bills,sizeof(Display_Bills),UserAct.PayForBills);
//		 printf(" UserAct.PayForBills =%d\r\n",UserAct.PayForBills);  //Í¶ÁË¶àÉÙÖ½±Ò
	//	 DisplayBills(UserAct.PayForBills);	       //ÏÔÊ¾Ö½±Ò
		 /*ÏÔÊ¾¿Í»§Í¶±ÒµÄÓ²±ÒÊý*/
		 DisPlayChar(Display_Coins,sizeof(Display_Coins),UserAct.PayForCoins);
	//	 DisplayCoins(UserAct.PayForCoins);	       //ÏÔÊ¾Ó²±Ò
		 /*ÏÔÊ¾ÓÃ»§Ë¢¿¨½ð¶î*/
		  DisPlayChar(Display_Cards,sizeof(Display_Cards),UserAct.PayForCards);
		 /*ÏÔÊ¾×Ü½ð¶î*/

		 DisplayMealNum1(Print_Struct.P_Number);

		 DisplayMealPrice1(UserAct.PayShould); 
	 	*PrintTickFlag = 0 ;
		/*´òÓ¡Ð¡Æ±µÄº¯Êý*/
	
		 SPRT(UserAct.MealID-1);
	    TIM_Cmd(TIM4, ENABLE);
	  }	 

	  if(*PrintTickFlag == 0x02 )
	  {
	     TIM_Cmd(TIM4, DISABLE);
	     PictrueDisplay((UserAct.MealID-1)*7+9);	    //ÏÔÊ¾Í¼Æ¬
			 /*ÏÔÊ¾Ä¬ÈÏµÄÒ»·Ý*/
		 /*ÏÔÊ¾¿Í»§Í¶±ÒµÄ½ð¶î*/
     	 DisPlayChar(Display_Bills,sizeof(Display_Bills),UserAct.PayForBills);
		 /*ÏÔÊ¾¿Í»§Í¶±ÒµÄÓ²±ÒÊý*/
		 DisPlayChar(Display_Coins,sizeof(Display_Coins),UserAct.PayForCoins);
		 /*ÏÔÊ¾ÓÃ»§Ë¢¿¨½ð¶î*/
		  DisPlayChar(Display_Cards,sizeof(Display_Cards),UserAct.PayForCards);
		 /*ÏÔÊ¾×Ü½ð¶î*/
	 	 DisplayMealNum1(Print_Struct.P_Number);
		 DisplayMealPrice1(UserAct.PayShould); 
		 *PrintTickFlag = 0;
		 TIM_Cmd(TIM4, ENABLE);
	  }
	
}	
  /*******************************************************************************
 * º¯ÊýÃû³Æ:WaitPayMoney                                                                     
 * Ãè    Êö:µÈ´ý½ÓÊÕÓÃ»§Í¶µÄÇ®                                                                 
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:void                                                               
 * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
 *******************************************************************************/ 
uint8_t CurrentPoint = 0 ; 
uint8_t UserPayMoney =0 ;
unsigned char  WaitPayMoney(void)
{
    uint8_t PicNumber;
	uint8_t temp = 0 ;
	uint8_t temp1 ;
	unsigned char times = 10 ;
	unsigned char Status = 0;
    switch(CurrentPoint)
	{
	case 0 :	//·¢ËÍÄÇ¸öÎ»ÖÃµÄ²Í
	     {	  
	        if(FindMeal(DefineMeal) == 0)
	       {
//		     printf("Line = %d,Column =%d\r\n",Line,Column)	  ;   
			 CurrentPoint = 1 ; 
		   }
		   else 
		    return Status_Error	;
 	     }
	 


            break ;
	case 1 : 
	         /*ÏÔÊ¾¸¶¿î·½Ê½£¬ÏÖ½ð£¬ÒøÐÐ¿¨£¬ÉîÛÚÍ¨*/
			  if(WaitTime<56)
			  {
			    /*Ìøµ½Ñ¡Ôñ¸¶¿î·½Ê½½çÃæ*/		 
				PictrueDisplay(5+(UserAct.MealID-1)*7);
				DisPlayChar(Display_Bills,sizeof(Display_Bills),UserAct.PayForBills);

		        /*ÏÔÊ¾¿Í»§Í¶±ÒµÄÓ²±ÒÊý*/
		        DisPlayChar(Display_Coins,sizeof(Display_Coins),UserAct.PayForCoins);
		        /*ÏÔÊ¾ÓÃ»§Ë¢¿¨½ð¶î*/
		        DisPlayChar(Display_Cards,sizeof(Display_Cards),UserAct.PayForCards);

				/*ÏÔÊ¾×Ü½ð¶î*/
			    DisplayMealPrice(UserAct.PayShould);      //ÏÔÊ¾×Ü¼Ûñ 

			    DisplayMealNum(UserAct.MealCount); //ÏÔÊ¾·ÝÊý£¬  ¿ªÊ¼Ä¬ÈÏÎªÒ»·Ý

			    CurrentPoint = 3 ;

			   /*Ö§¸¶·½Ê½*/			 
			   UserAct.PayType = '1' ;/* ÏÖ½ðÖ§¸¶*/
			  }

           	break;	
	case 2 :  //Éè¼Æ½ÓÊÕÊ²Ã´ÑùµÄÇ®
	              /*Ìøµ½Ñ¡Ôñ¸¶¿î·½Ê½½çÃæ*/
	            PictrueDisplay(5+(UserAct.MealID-1)*7);
		        DisPlayChar(Display_Bills,sizeof(Display_Bills),UserAct.PayForBills);
		  
		       /*ÏÔÊ¾¿Í»§Í¶±ÒµÄÓ²±ÒÊý*/
		        DisPlayChar(Display_Coins,sizeof(Display_Coins),UserAct.PayForCoins);
		       /*ÏÔÊ¾ÓÃ»§Ë¢¿¨½ð¶î*/
		        DisPlayChar(Display_Cards,sizeof(Display_Cards),UserAct.PayForCards);
				/*ÏÔÊ¾×Ü½ð¶î*/
				DisplayMealPrice(UserAct.PayShould);      //ÏÔÊ¾×Ü¼Ûñ 

				DisplayMealNum(UserAct.MealCount); //ÏÔÊ¾·ÝÊý£¬  ¿ªÊ¼Ä¬ÈÏÎªÒ»·Ý
			    CurrentPoint = 3 ;
				UserAct.PayType = '1' ;/* ÏÖ½ðÖ§¸¶*/
		   break;	         
	case 3 :  //¶ÁÇ®	  

	       	  UserPayMoney = ReadBills();			 
		      if(UserPayMoney !=0 )	   //±íÊ¾ÊÕµ½ÁËÇ®
		    {
			    UserAct.PayAlready  += UserPayMoney;
			    UserAct.PayForBills += UserPayMoney;	
			   	UserPayMoney = 0 ;
			   	WaitTimeInit(&WaitTime);
				DisplayBills(UserAct.PayForBills);
			    CurrentPoint = 5 ;
		    }
		   
	case 4 :   //¶ÁË¢¿¨»úµÄÇ®
	     	   //temp = 	ChangeforMeal();
	           if( temp != 0)  //Ë¢¿¨
	        { 
			   IsCard =1 ;	  //±íÊ¾µÄÊÇË¢¿¨À²¡£
			   UserAct.PayAlready  += temp;
			   UserAct.PayForCards += temp ;
			   temp = 0 ;
//			   printf(" UserAct.PayForCards =%d",UserAct.PayForCards);	           
			   DisplayCards(UserAct.PayForCards);
        	   CurrentPoint = 5 ;
 	        }
		    else
		       CurrentPoint = 5;
	case 5 ://ÏÔÊ¾½ÓÊÕÁË¶àÉÙÓ²±Ò		    
		 	  //DisplayCoins(UserAct.PayForCoins);	
			   DisPlayChar(Display_Coins,sizeof(Display_Coins),UserAct.PayForCoins);
		       CurrentPoint = 6 ;
	case 6 :  
             /*½ûÖ¹µã»÷²ÍÆÁÄ»*/
          	  _LCD_Disable_RxInit();
	       if(UserAct.PayAlready +UserAct.PayForCards>=UserAct.PayShould)		//Í¶µÄÇ®´óÓÚµÈÓÚÒª¸¶µÄÇ®
		  {     
		    CurrentPoint = 9;	                //²âÊÔ	 
			if(UserAct.PayAlready <UserAct.PayShould)
			{
			  CurrentPoint = 3;
			  return Status_Error ;
			} 
		 	CloseTIM3();
			/*ÏÔÊ¾³ö²ÍÖÐ£¬´òÓ¡½çÃæµÄ½çÃæ*/
			PictrueDisplay(7+(UserAct.MealID-1)*7);	  
            CloseMoneySystem();			 
	   	  }
	  else
	    { 	 /*ÅÐ¶ÏÊÇ·ñÊÕµ½ÁËÇ®µÄ±êÖ¾Î»*/
		     if(Rev_Money_Flag == 1)
			  {
			   Rev_Money_Flag =0 ;
			   /*°ÑÇ®·Å½øÇ®Ïä*/
		      Polls();/*±£Áô×îºÃÒ»ÕÅÇ®*/
			  }
			  _LCD_Enable_RxInit();
		      CurrentPoint = 3; 
		
	    } 	
	     	  break;   				 
    case 7 :  /*ÒøÐÐ¿¨Ö§¸¶*/
			  UserAct.PayType = '2' ;/* ÏÖ½ðÖ§¸¶*/
			  temp1 = 0;
			  //temp1 = GpbocDeduct(UserAct.PayShould *100);
			  temp1 = GpbocDeduct(1);
			  if(temp1 == 1)
			  {
			    UserAct.PayForCards = UserAct.PayShould ;
			    UserAct.PayAlready += UserAct.PayForCards ;
			    CurrentPoint =6;
			  }
	          break;
	case 8 :/*ÉîÛÚÍ¨Ö§¸¶*/
	          UserAct.PayType = '3' ;/* ÏÖ½ðÖ§¸¶*/
			  temp1 = 0;
			  //temp1 = SztDeduct(UserAct.PayShould * 100);
			  temp1 = SztDeduct(1);
			  if(temp1 == 1)
			  {
			    UserAct.PayForCards = UserAct.PayShould ;
			    UserAct.PayAlready += UserAct.PayForCards ;
			    CurrentPoint =6;
			  }
	          break;
	case 9 :  //¹Ø±ÕËùÓÐµÄÊÕÒøÏµÍ³
			BillActionFlag = 1;		
			UserAct.MoneyBack = UserAct.PayAlready - UserAct.PayShould;	
			PlayMusic(VOICE_2);
			DisplayMealNum1(UserAct.MealCount);
			DisplayMealPrice1(UserAct.PayShould); 
		    DisPlayChar(Display_Bills,sizeof(Display_Bills),UserAct.PayForBills);
		 /*ÏÔÊ¾¿Í»§Í¶±ÒµÄÓ²±ÒÊý*/
		    DisPlayChar(Display_Coins,sizeof(Display_Coins),UserAct.PayForCoins);

		 /*ÏÔÊ¾ÓÃ»§Ë¢¿¨½ð¶î*/
		    DisPlayChar(Display_Cards,sizeof(Display_Cards),UserAct.PayForCards);
//			DisplayBills(UserAct.PayForBills);
//			DisplayCoins(UserAct.PayForCoins);
//			DisplayCards(UserAct.PayForCards);	
//			printf(" UserAct.MealID =%d\r\n",UserAct.MealID);		 //ID
//		    printf(" UserAct.MealCount =%d\r\n",UserAct.MealCount);   //ÊýÁ¿
//			printf(" UserAct.MealPrice =%d\r\n",UserAct.MealPrice);	//µ¥¼Û
//			printf(" UserAct.PayShould =%d\r\n",UserAct.PayShould);	//×Ü¼Û
//			printf(" UserAct.PayForBills =%d\r\n",UserAct.PayForBills);  //Í¶ÁË¶àÉÙÖ½±Ò
//			printf(" UserAct.PayForCoins =%d\r\n",UserAct.PayForCoins);   //Í¶ÁË¶àÉÙÓ²±Ò
//			printf(" UserAct.PayForCards =%d\r\n",UserAct.PayForCards);  //¿¨Ö§¸¶ÁË¶àÉÙÇ®
//		    printf(" UserAct.PayAlready =%d\r\n",UserAct.PayAlready);	   //ÒÑ½ü¸¶ÁË¶àÉÙÇ®
 //	     	printf(" UserAct.MoneyBack  =%d\r\n",UserAct.MoneyBack );		//ÕÒ»Ø¶àÉÙÇ®
//			printf(" UserAct.IC_Card_Balance =%d\r\n",UserAct.IC_Card_Balance);	   //³äÖµ
//			printf(" UserAct.RechxargeCounter =%d\r\n",UserAct.RechxargeCounter);
            /*´òÓ¡ÐÅÏ¢*/
		     Print_Struct.P_Number    = UserAct.MealCount ;
			 Print_Struct.P_ID        = UserAct.MealID ;
			 Print_Struct.P_paymoney  = UserAct.PayForBills +	UserAct.PayForCoins +UserAct.PayForCards ;
			 Print_Struct.P_PayShould = UserAct.PayShould ;
			 Print_Struct.P_MoneyBack =	UserAct.MoneyBack ;


			WaitTime  = 0;
			DisplayWaitTime1(WaitTime);//µÈ´ýÊ±¼ä
		     _LCD_Enable_RxInit();
			/*µ¹¼ÆÊ±*/
			TIM_Cmd(TIM4, ENABLE);
			if(UserAct.MoneyBack > 0)
			{
			   // CoinFlag = 1;    //ÏµÍ³Ã¿´ÎÖØÐÐÆô¶¯¶¼»áÅÜ½øÖÐ¶Ï²»ÖªµÀÎªÊ²Ã´
			    OpenCoins();    //ÕÒ±Ò
				CoinFlag = 1; 
				
			}
			LedCmd = 0 ;
			CurrentPoint = 0 ;
		

			return  Status_OK;
	case 10 :  //¼ì²âÊÇ·ñÒªÌø³öÕâ¸ö
	{
	  

	}
	default :break;
   }

  	return  Status_Action;
 }												



/*										  14 15 16
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 31 43 03 0D 0A        1c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 31 42 03 0D 0A		   1b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 31 41 03 0D 0A        1a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 32 43 03 0D 0A        2c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 32 42 03 0D 0A		   2b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 32 41 03 0D 0A        2a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 33 43 03 0D 0A        3c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 33 42 03 0D 0A		   3b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 33 41 03 0D 0A        3a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 34 43 03 0D 0A        4c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 34 42 03 0D 0A		   4b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 34 41 03 0D 0A        4a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 35 43 03 0D 0A        5c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 35 42 03 0D 0A		   5b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 35 41 03 0D 0A        5a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 36 43 03 0D 0A        6c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 36 42 03 0D 0A		   6b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 36 41 03 0D 0A        6a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 37 43 03 0D 0A        7c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 37 42 03 0D 0A		   7b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 37 41 03 0D 0A        7a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 38 43 03 0D 0A        8c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 38 42 03 0D 0A		   8b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 38 41 03 0D 0A        8a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 39 43 03 0D 0A        9c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 39 42 03 0D 0A		   9b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 39 41 03 0D 0A        9a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 30 43 03 0D 0A        10c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 30 42 03 0D 0A		   10b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 30 41 03 0D 0A        10a


05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 31 43 03 0D 0A        11c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 31 42 03 0D 0A		   11b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 31 41 03 0D 0A        11a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 32 43 03 0D 0A        12c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 32 42 03 0D 0A		   12b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 32 41 03 0D 0A        12a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 33 43 03 0D 0A        13c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 33 42 03 0D 0A		   13b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 33 41 03 0D 0A        13a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 34 43 03 0D 0A        14c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 34 42 03 0D 0A		   14b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 34 41 03 0D 0A        14a

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 35 43 03 0D 0A        15c
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 35 42 03 0D 0A		   15b
05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 35 41 03 0D 0A        15a

100100640004M01C   
1C

05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 31 41 03 0D 0A 
100100640004M01A                         


05 31 30 30 31 30 30 36 34 30 30 30 34 4D 30 32 43 03 0D 0A 
 100100640004M02C                 


05 31 30 30 31 30 30 36 34 30 30 30 34 4D 31 35 43 03 0D 0A 
100100640004M15C
*/
unsigned char  Record_buffer[1024] = {0} ;  //ÎªÊ²Ã´¶¨Òå¾Ö²¿±äÁ¿Ã»ÓÐÓÃÄØ£¿
uint8_t WaitMeal(void)
{
   
	uint8_t  i ;
	static uint8_t  CurrentPointer = 0 ;
  static unsigned char Cmd[20]={0x05, 0x31, 0x30, 0x30, 0x31, 0x30 ,0x30, 0x36, 0x34, 0x30, 0x30, 0x30, 0x34, 0x4D, 0x31, 0x35, 0x43, 0x03, 0x0D ,0x0A};
	 /*ÅÐ¶ÏÊÇ·ñ´òÓ¡Ð¡Æ±*/ 			
    PrintTickFun(&UserAct.PrintTick);
	switch(CurrentPointer)
	{
	 case 0 :  
	            /* ·¢ËÍ²ÍµÄÊýÄ¿¼õÒ»*/
	            FloorMealMessageWriteToFlash.FloorMeal[Line].MealCount --; 
	            switch(Column)
				{
				 case 0 :   FloorMealMessageWriteToFlash.FloorMeal[Line].FCount -- ;	  break;
				 case 1 :   FloorMealMessageWriteToFlash.FloorMeal[Line].SCount -- ;	  break;
				 case 2 :   FloorMealMessageWriteToFlash.FloorMeal[Line].TCount -- ;	  break;
				 default :break;
				}
				 UserAct.MealCount--;
				 DefineMeal[UserAct.MealID - 1].Position[Line][Column]--;
				 DefineMeal[UserAct.MealID - 1].MealCount--;
			    CurrentPointer = 1 ;
			    break ;
     case 1 :   /*·¢ËÍÐÐ¸úÁÐ¡£ÑÓÊ±µÈ´ý£¬µÈ´ý»úÐµÊÖµ½ÖÆ¶¨Î»ÖÃ*/
	        switch(Column)
				 {
				 case 0 :   Column =3;	  break;
				 case 1 :   Column =2;	  break;
				 case 2 :   Column =1;	  break;
				 default :break;
				 }
	            Cmd[16]=0x40 + Column ;
		 		Cmd[15]=0x30 + (Line+1) %10 ;
				Cmd[14]=0x30 + (Line+1) /10 ;
				for(i=0;i<20;i++)
				{
				  USART_SendData(USART6,Cmd[i] );//´®¿Ú1·¢ËÍÒ»¸ö×Ö·û
 	              while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//µÈ´ý·¢ËÍÍê³É
				  delay_ms(10);
				}
				CurrentPointer = 2 ;
				TIM_Cmd(TIM2, ENABLE);
				 MealArr(UserAct.MealID);
		      /*·¢ËÍÈ¡²ÍÊý¾Ý¸ø·þÎñÆ÷*/
		           memset(Record_buffer,0,1024);    
					Record_buffer[1012] = 'L' ;
					Record_buffer[1013] = 'e' ; 
					Record_buffer[1014] = 'n' ;
					Record_buffer[1015] = 'g' ;  
					Record_buffer[1016] = 'h' ;
					Record_buffer[1017] = 't' ;
					Record_buffer[1018] = ':' ; 
					 if(TakeMealsFun(Record_buffer) == 0x01) //±íÊ¾·¢ËÍÊ§°Ü
					 {
					        Record_buffer[1022] = 'N' ;
							Record_buffer[1023] = 'O' ; /*±íÊ¾·¢ËÍÊ§°Ü£¬Ð´Èë±êÖ¾Î»£¬´ýÏÂ´Î·¢ËÍ*/  
					 }
					 else 
					 {  
						  Record_buffer[1021] = 'Y' ;
						  Record_buffer[1022] = 'e' ;
					      Record_buffer[1023] = 's' ; /*±íÊ¾·¢ËÍÊ§°Ü£¬Ð´Èë±êÖ¾Î»£¬´ýÏÂ´Î·¢ËÍ*/  
         			 }
		  		WriteDatatoSD(Record_buffer);
				break ;
	case 2 :   	  
	             /*ÑÓÊ±3Ãë£¬È»ºó¿ª¿ª¹Ø£¬ÈÃ»úÆ÷³ö²Í*/	             
	            if (TimerFlag > 4 )
			   {
			    TIM_Cmd(TIM2, DISABLE); 
			    TimerFlag = 0;				 
                CurrentPointer = 3 ;
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				TIM_Cmd(TIM2, ENABLE);  	
				}
			    break;
	 case 3 :   /*¶Ï¿ª³ö²Í¿ª¹Ø£¬*/
			
	            if (TimerFlag > 2 )
			   {
			    TIM_Cmd(TIM2, DISABLE);
			    TimerFlag = 0;				  
                CurrentPointer = 4 ;
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);	
				TIM_Cmd(TIM4, DISABLE);
				DisplayWaitTime1(WaitTime);
				PlayMusic(VOICE_3);		//¡£3.ÇëÈ¡²Í
				TIM_Cmd(TIM2, ENABLE);  
				}			 
			    break;
	 case 4 :  	/*²¥·ÅÓïÒô£¬ÇëÈ¡²Í*/
	           	
	            if (TimerFlag > 2 )
			   {
			    Polls();/*±£Áô×îºÃÒ»ÕÅÇ®*/
			    TimerFlag = 0;
				TIM_Cmd(TIM2, DISABLE);  
                CurrentPointer = 5 ;
				 if(IsCard == 1)
				  {
					   	IsCard = 0 ;   //ÇåÁã 
					    PictrueDisplay(GETMEAL_PAGE);  //ÇëÈ¡²Í½çÃæ
					    DisplayBalance(UserAct.IC_Card_Balance);
				  }
				}
	 	       
			    break;
	 case 5 :  /*ÏÔÊ¾½ð¶î*/	
	            WriteMeal();   /*±£´æµ±Ç°µÄ·ÝÊý*/
				PlayMusic(VOICE_4);	    //4.»¶Ó­ÏÂ´Î¹âÁÙ
				CurrentPointer = 6 ;			   
			    break;
	 case 6 :	/*»¶Ó­ÏÂ´Î¹âÁÙ*/	 
	 	  	 
				CurrentPointer = 7;				
			    break;
	 case 7 : 	/*»Øµ½Ö÷½çÃæ*/  
				//
			
				PictrueDisplay(HOME_PAGE);  //ÇëÈ¡²Í½çÃæ
				TemperatureDisplay(Temperature);
				DisplayMealCount();								  
				UserAct.MealCount= 0 ;
				UserAct.MoneyBack   = 0 ;
				UserAct.PayAlready  = 0;
		    	UserAct.PayForBills = 0;
		    	UserAct.PayForCoins = 0;
		    	UserAct.PayForCards = 0;
				TimerFlag = 0 ;
				CurrentPointer = 0 ;
				return 0 ;
	case 8 :
	      
	           break;		       
    default : break ;	 	
   }      

   return 1 ;
}   



  /*******************************************************************************
 * º¯ÊýÃû³Æ:ClearingFuntion                                                                     
 * Ãè    Êö:½áËãÃüÁî ¼ìÑéÊý¾ÝÊÇ·ñ·¢ËÍ³É¹¦£¬¼ÓÈëÃ»ÓÐ·¢ËÍ³É¹¦£¬¼ÌÐø·¢ËÍ¡£È«²¿·¢ËÍÍê±Ïºó½áËã¡£                                                                   
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:void                                                               
 * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
 *******************************************************************************/ 
void ClearingFuntion(void)
{
	 
	RTC_TimeShow();//»ñµÃÊ±¼ä
	if(TimeDate.Hours == 5 || TimeDate.Hours == 19)
	{
	  // ½áËãÃüÁî £»
	      ClearingFun();	  //ºóÌ¨½áËãÃüÁî
		 // BankFlashCard_Upload(); //Êý¾ÝÉÏËÍ	 ²»ÊÇÍËÇ©

	}		 
			 
}

    /*******************************************************************************
 * º¯ÊýÃû³Æ:TempHandler                                                                     
 * Ãè    Êö:ÎÂ¶È´¦Àíº¯Êý                                                                   
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:void                                                               
 * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
 *******************************************************************************/ 
 void  TempHandler(void)
 {
 	if(TemperatureFlag == 1 &&  LCDstructure.LCD_Temp == 1 )
	{    
	 
	   TemperatureFlag = 0 ;
	   if(DS18B20_DEMO()==0)
	   {
	    if(Temperature < 70)//¶Á³ö´íÁË¡£È»ºó²»ÏÔÊ¾
	    TemperatureDisplay(Temperature); 
	   }
	   OpenTIM7(); 
	}
}
  /*******************************************************************************
 * º¯ÊýÃû³Æ:LcdHandler                                                                     
 * Ãè    Êö:LCD´¦Àíº¯Êý                                                                   
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:void                                                               
 * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
 *******************************************************************************/ 

 void LcdHandler(void)
 {
   if(LedCmd != 0)
    {
   	 LedCmd = 0 ;
	 Current=MsgHandle(LedRecv[4]);
	 USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);	
    }
 }
 /*
  PVD ---- µÍµçÑ¹¼ì²â                ÇÀÕ¼ÓÅÏÈ¼¶  0      ÑÇÓÅÏÈ¼¶ 0 		ÓÃÓÚ±£»¤sd¿¨
  Ó²±Ò»ú               Íâ²¿ÖÐ¶Ï5                 0                1
  Ö½±Ò»ú               ´®¿Ú4                     0                2
  IC¿¨                 ´®¿Ú2                     0                3  
  ÆÁÄ»                 ´®¿ÚÖÐ¶Ï3                 0                4
                       CANÍ¨ÐÅ                   0                5
  ´òÓ¡                 ´®¿Ú1                     0                6
  ÍË±Òµ¹¼ÆÊ±           ¶¨Ê±Æ÷4                   1                0
  ¶¨Ê±µ¹¼ÆÊ±60s        ¶¨Ê±Æ÷3                   1                1
  Çý¶¯µç»ú             ¶¨Ê±Æ÷2                   1                2
  Çý¶¯µç»ú             Íâ²¿ÖÐ¶Ï4                 1                3
  ¶¨Ê±²É¼¯ÎÂ¶È²É¼¯     ¶¨Ê±Æ÷7                   1                4
 */
  /*******************************************************************************
 * º¯ÊýÃû³Æ:hardfawreInit                                                                     
 * Ãè    Êö:Ó²¼þ³õÊ¼»¯                                                                   
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:void                                                               
 * ÐÞ¸ÄÈÕÆÚ:2013Äê8ÔÂ28ÈÕ                                                                    
 *******************************************************************************/ 

void hardfawreInit(void)
{
    uint8_t i, j, k;

    for(i = 0; i < 9; i++)
	{
	
		DefineMeal[i].MealPrice = 0;
		DefineMeal[i].MealCount = 0;
		for(j = 0; j < 15; j++)
		{
			for(k = 0; k < 3; k++)
			{
				DefineMeal[i].Position[j][k] = 0;
			}
		}
		
	}
 	 UserAct.PayForCoins     = 0;
	 UserAct.PayForBills     = 0;
	 UserAct.PayForCards     = 0;
	 UserAct.RechxargeCounter= 0;
     UserAct.IC_Card_Balance	= 0;
	 UserAct.PayAlready      = 0;
	 IsCard            = 0;
	 LCD_En_Temp();

     SystemInit();
	 delay_ms(30000);
	 LED_Init();
     InitBills();               //Ö½±Ò»ú³õÊ¼»¯ 
 	 Uart1_Configuration();	    //´®¿Ú1³õÊ¼»¯
	 Uart3_Configuration();	    // ´®¿ÚÆÁ³õÊ¼»¯
	 Uart2_Configuration();	    //ÉîÛÚÍ¨¡¢ÒøÁª¿¨´®¿Ú
	 Uart5_Configuration();		//ÍøÂç´®¿Ú³õÊ¼»¯
	 Uart6_Configuration();
	 //InitCards();			    //IC¿¨³õÊ¼»¯
	 TIM2_Init();		        //µç»ú
	 TIM3_Init();		        //ÓÃÓÚ¶¨Ê±£¬µ¹¼ÆÊ±
	 TIM4_Init();		        //´ý¶¨
	 TIM5_Init();		        //µ¹¼ÆÊ±ÍË±Ò
	 TIM7_Init();				//ÓÃÓÚ¶¨Ê±²É¼¯ÎÂ¶È
	 InitCoins();		        //Ó²±Ò³õÊ¼»¯
	 InitCoinsHopper() ;		 //ÍË±ÒÆ÷Ê¼»¯
//	 InitCoinMachine();		    //Ó²±Ò»ú³õÊ¼»¯
//	 CAN_InitConfig();	        //can³õÊ¼»¯
 	 PictrueDisplay(0);        //ÏÔÊ¾LOGO
	 InitVoice()  ;             //ÓïÒô³õÊ¼»¯
	 MyRTC_Init();              //RTC³õÊ¼»¯
	 SPI_FLASH_Init();          //Flash³õÊ¼»¯
	 LED_Init();
  // SPI_FLASH_SectorErase(SPI_FLASH_Sector0);
	 ReadMeal();
	 for(i=0;i<90;i++)
	 {
	   if(FloorMealMessageWriteToFlash.FlashBuffer[i] == 0xff)
	   FloorMealMessageWriteToFlash.FlashBuffer[i]    = 0 ;
	 }
	 WriteMeal();
	 StatisticsTotal();   	
// PictrueDisplay(HOME_PAGE);	       //ÏÔÊ¾²Ëµ¥Ò³Ãæ
// DS18B20_DEMO();
// TemperatureDisplay(Temperature);  //ÏÔÊ¾ÎÂ¶È	  
// DisplayMealCount();			   //ÏÔÊ¾²ÍµÄÊýÄ¿ 
 //delay_ms(30000);
	 
}														 

