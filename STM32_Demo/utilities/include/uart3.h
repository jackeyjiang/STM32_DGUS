#ifndef __Uart3__h
#define __Uart3__h

extern   uint8_t LedRecv[8] ; 	//屏幕接收buffer、

void Uart3_Configuration(void);
void Uart3_Send(const uint8_t *p,uint8_t length);

void _LCD_Enable_RxInit(void);
void _LCD_Disable_RxInit(void);

char USART3_GetChar(unsigned char *Chr);
unsigned int UART3_GetCharsInRxBuf(void);
void UART3_ClrRxBuf(void);
void USART3_IRQHandler(void); /*??将串口中断放入驱动程序中利于移植*/
#endif

