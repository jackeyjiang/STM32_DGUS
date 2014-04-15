#ifndef __Mini_hopper_h
#define __Mini_hopper_h

extern uint16_t Coins_cnt; //一键退币计数
extern uint16_t Coins_totoal; //硬币总数统计
extern uint8_t ErrorType;
void InitMiniGPIO(void);
u8 SendOutN_Coin(int num);
uint8_t StatisticalCoin(void);
void CloseCoins(void);
void OpenCoins(void);
void RefundButton(void);

#endif

