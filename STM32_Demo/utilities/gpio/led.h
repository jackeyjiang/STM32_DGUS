#ifndef __led___h
#define  __led___h
 #include "stm32f4xx.h"

 //LED∂®“Â
typedef enum 
{
	LED1 = 0,
	LED2 = 1,
	LED3 = 2,
	LED4 = 3,
	LED5 = 4
} Led_Def;

static GPIO_TypeDef* LED_PORT[5]={GPIOA, GPIOB, GPIOB, GPIOD,GPIOD};
static const u16 LED_PIN[5]={GPIO_Pin_5, GPIO_Pin_0, GPIO_Pin_14,GPIO_Pin_13,GPIO_Pin_12};

void LED_Init(void);
void LEDOn(Led_Def Led);
void LEDOff(Led_Def Led);
void LEDTog(Led_Def Led);
#endif
