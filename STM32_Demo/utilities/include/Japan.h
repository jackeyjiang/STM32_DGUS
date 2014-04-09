#ifndef __JAPAN___H
#define __JAPAN___H


extern uint8_t 	 rx4DataLen   ;
extern uint8_t   RX4Buffer[40];
extern uint8_t   rx4ack       ;
extern uint8_t   rx4Check ;
extern		unsigned char  Rev_Money_Flag ;   /*表示纸币机收到了钱*/
 /*读取纸币机收取的钱数*/
uint8_t  ReadBill(void);
void  BackPolls(void);
void  Polls(void);





#endif



