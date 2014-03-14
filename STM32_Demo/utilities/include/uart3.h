#ifndef __Uart3__h
#define __Uart3__h

extern   uint8_t LedRecv[8] ; 	//∆¡ƒªΩ” ’buffer°¢

void Uart3_Configuration(void);

void Uart3_Send(const uint8_t *p,uint8_t length);

 void _LCD_Enable_RxInit(void);
 void _LCD_Disable_RxInit(void);
#endif

