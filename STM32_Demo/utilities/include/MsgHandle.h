#ifndef __MSGHANDLE__
#define __MSGHANDLE__

extern uint8_t Temperature;
#define FloorMealNum  15				  /*定义有多少个行*/
typedef struct MealAt
{
    
	uint8_t MealPrice;         //餐价格
	uint8_t Position[15][3];   //餐放置的位置
	uint8_t MealCount;         //餐剩余总数
	
}MealAttribute;	  ////定义9个菜系的属性。

extern MealAttribute DefineMeal[9];	//定义9个菜系




typedef struct UserAction
{
  unsigned char MealID;           //用户选择的餐ID
	unsigned char MealPrice;        //用户选择的餐价格
	unsigned char MealCount;        //用户选择的餐份数
	unsigned char PayShould;        //用户应付款总额
	uint16_t PayForBills;           //用户投入的纸币数
	uint16_t PayForCards;           //用户应经刷卡的数
	uint16_t PayForCoins;           //用户投入的硬币数
	uint16_t PayAlready;            //用户已经付款总额
	unsigned char MoneyBack;        //用户找零
	uint16_t RechxargeCounter;      //充值金额
	uint16_t ICCardID;		          //IC卡的ID
	uint16_t IC_Card_Balance;       //IC卡的余额
	unsigned char  PrintTick;       //是否打印小票标志；
	unsigned char  PayType ;        //支付方式
}UserSelection;

extern UserSelection UserAct;


typedef struct FloorMeal
{
    uint8_t MealID;	 //当前层存放餐的ID
	uint8_t MealCount;	 //当前层存放餐的总数
	uint8_t FCount;     //第一列放餐的数量
	uint8_t SCount;	 //第二列放餐的数量
	uint8_t TCount;	 //第三列放餐的数量
	uint8_t FloorNum;	 //当前是第几层
}FloorAttribute;

		  

typedef union _FloorMealMessage
{
  FloorAttribute      FloorMeal[FloorMealNum];
  uint8_t	          FlashBuffer[FloorMealNum*6]; /*这里我改变了一个*/
}FloorMealMessage;
extern 	FloorMealMessage FloorMealMessageWriteToFlash;
 /*
 定义屏幕显示温度，只有在主界面的时候显示温度
 */
typedef struct LCD
{
  unsigned char LCD_Temp;
  				
}LCDStruction ;
extern 	 LCDStruction  	 LCDstructure;

/****************/
void cancel(void) ;
uint8_t MsgHandle(uint8_t cmd);	
void StatisticsTotal(void);
extern uint8_t WaitTime ;
void MealArr(unsigned char index) ;/*统计餐品的数量*/

void LCD_DisTemp(void);
void  LCD_En_Temp(void);



#endif
