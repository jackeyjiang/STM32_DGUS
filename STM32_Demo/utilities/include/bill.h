#ifndef __bill__
#define __bill__
#include "stdint.h"
#include "stm32f4xx.h"
typedef enum {NACK = 0, ACK = !RESET}FLAG_ENUM; 
extern FLAG_ENUM EnableBillFlag, DisableBillFlag;
extern unsigned char NewCoinsCnt;
extern unsigned char OldCoinsCnt;
void SetBills(void);
uint8_t  ReadBills(void);
void DisableBills(void);
void Uart4_Bill(uint8_t *p,uint8_t sizeData);
unsigned char  ReadMoneyFun(uint8_t BillActionStep);
uint8_t   CashCodeInit(void);
unsigned char   SetReceUsersMoney(void);
unsigned char   SetReceChangeMoney(void);
unsigned char   NoAcceptMoneyFun(void);
bool StringToHexGroup(unsigned char *OutHexBuffer, char *InStrBuffer, unsigned int strLength);

#endif

