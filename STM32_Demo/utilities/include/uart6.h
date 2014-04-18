#ifndef __UART6_H
#define __UART6_H
extern uint8_t Temperature;
void Uart6_Configuration(void);
void USART6_IRQHandler(void);

char USART6_GetChar(unsigned char *Chr);
unsigned int UART6_GetCharsInRxBuf(void);
void UART6_ClrRxBuf(void);
void USART6_IRQHandler(void); /*??将串口中断放入驱动程序中利于移植*/
void ManipulatorHandler(void);
#endif
