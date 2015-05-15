#include "bsp.h"
#include "stm32f4xx.h"

/*定义退币机的端口，用于打开电源，关闭退币机使其工作*/	  	 
#define COIN_PIN                         GPIO_Pin_0
#define COIN_GPIO_PORT                   GPIOA
#define COIN_GPIO_CLK                    RCC_AHB1Periph_GPIOA  
/*定义退币机中断端口信号口*/
#define COIN_INT_PIN                         GPIO_Pin_1
#define COIN_GPIO_INT_PORT                   GPIOB
#define COIN_GPIO_INT_CLK                    RCC_AHB1Periph_GPIOB

uint32_t Coins_cnt =0; //一键退币计数
uint32_t Coins_totoal=0; //硬币总数统计

/*
PE11为出币控制信号
P10为计币器输出
PE9为空币信号输出
PE8为禁能输输出
*/
void InitMiniGPIO(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   EXTI_InitTypeDef EXTI_InitStructure;//外部中断初始化结构体
   
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE ,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOE,&GPIO_InitStructure);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource10);
	  EXTI_InitStructure.EXTI_Line=EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode =EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		EXTI_ClearITPendingBit(EXTI_Line10);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOE,&GPIO_InitStructure);
 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
   GPIO_Init(GPIOE,&GPIO_InitStructure);
	 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOE,&GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : OpenCoins
* Description    : 开打退币机
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void OpenCoins(void)	
{
  GPIO_SetBits(COIN_GPIO_PORT, COIN_PIN);
}
	
/*******************************************************************************
* Function Name  : CloseCoins
* Description    : 关闭退币机
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void CloseCoins(void) 
{										   
  GPIO_ResetBits(COIN_GPIO_PORT, COIN_PIN); 
}

 /*******************************************************************************
* Function Name  : SendOutN_Coin
* Description    : 退币
* Input          : 要退的币数
* Output         : 未退的币数，修改
* Return         : 
*******************************************************************************/
uint8_t ErrorType;

uint8_t SendOutN_Coin(int num)
{
  int i=0,waitime_cnt=0;
  uint8_t old_coins_tmp=0,new_coins_tmp=0;
  ErrorType = 0;
  old_coins_tmp = CoinsTotoalMessageWriteToFlash.CoinTotoal;
  for(i=0;i<num;i++)
  {
      if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9) == Bit_SET)
	  {
		  GPIO_ResetBits(GPIOE,GPIO_Pin_11);
		  delay_ms(5);
		  GPIO_SetBits(GPIOE,GPIO_Pin_11);
		  delay_ms(30);
		  GPIO_ResetBits(GPIOE,GPIO_Pin_11);
		  delay_ms(60);
	  }
	  else
	  {
	    ErrorType = 1;
	    return num-i;
	  }
  }
  do
  {
	delay_ms(1);
	waitime_cnt++;
  }
	 //当PE8低高的时候，说明退币机正常了  ，当时间大于5000ms，则退出循环
  while(!((GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_8) == Bit_SET)||(waitime_cnt>3000))); //直接死等(5S/最多10个币)，等待硬币出完
  new_coins_tmp = CoinsTotoalMessageWriteToFlash.CoinTotoal;
  if((old_coins_tmp - new_coins_tmp) != i)
	{
		ErrorType = 1;
	  return (num -(old_coins_tmp - new_coins_tmp));
	}
  else
	  return 0;
}

 /*******************************************************************************
* Function Name  : refund_button
* Description    : 不检测退币吉内是否有币，直接退币
* Input          : 要退的币数
* Output         : 未退的币数，修改
* Return         : 
*******************************************************************************/
void RefundButton(uint16_t coins_cnt)
{
	uint16_t i;
	for(i=0;i<coins_cnt;i++)
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_11);
		delay_ms(15);
		GPIO_SetBits(GPIOE,GPIO_Pin_11);
		delay_ms(15);
		GPIO_ResetBits(GPIOE,GPIO_Pin_11);
		delay_ms(50);
	}	
}
 /*******************************************************************************
* Function Name  : StopRefond
* Description    : 全部退币
* Input          : 要退的币数
* Output         : 未退的币数，修改
* Return         : 
*******************************************************************************/
void StopRefond(void)
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_11);
}
