#ifndef __coin__
#define __coin__

#include "stm32f4xx.h"	           //stm32F4¿âº¯Êý

void  InitCoins(void);
void  OpenCoinMachine(void);
void  CloseCoinMachine(void);
void  CloseTIM5(void);
void  OpenTIM5(void);
void  TIM5_Init(void);
#endif
