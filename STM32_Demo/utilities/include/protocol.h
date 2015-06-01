#ifndef __PROTOCOL_H
#define __PROTOCOL_H


typedef struct
{
  uint8_t reack;  //ack标志,0表示无ack,1表示有ack
  uint8_t renack;	//nack标志，0表示无nack,1表示有nack
  uint8_t redoor;	//门状态标志，0开，1关
  uint8_t rerelative;	//到达待机位置（相对原点）标志，0表示不在待机位置，1表示在待机位置
  uint8_t regoal;	  //到达取餐点，等待取餐命令标志，0表示不在取餐点，1表示已到取餐点，等待取餐命令
  uint8_t retodoor;	   //到达出餐口标志，0表示未到达出餐口，1表示到达出餐口，等待取走餐
  uint8_t remealaway;	//餐已被取走标志，0表示餐未被取走，1表示餐已被取走
  uint8_t reenablegetmeal;	 //取餐5秒还取不到餐标志，0表示取餐正常，1表示不能取到餐
  uint8_t remealnoaway; 	//出餐口的餐超过20秒还未被取走标志，0表示常态或餐已被取走，1表示出餐口的餐超过20秒还未被取走
}Urart6RecFlagInf;

typedef struct
{
bool E101;
bool E102;
bool E103;
bool E201;
bool E301;
bool E401;
bool E501;
bool E502;
bool E503;
bool E601;
bool E711;
bool E070;
bool E000;
bool E010;
bool E020;
bool E030;
bool E032;
bool E040;
bool E050;
}ErrorFlagInf;


extern uint8_t Usart6Buff[6];	//用于接收机械手发过来的数据
extern uint8_t Usart6Index;
extern uint8_t Usart6DataFlag;   //此标志作为是否有数据接收，0无，1有数据，平时清0
extern Urart6RecFlagInf machinerec;
extern uint8_t RetryFre;  //重发原命令计数，最多三次
extern uint8_t LinkTime;
extern uint8_t Line ,Column  ;
extern ErrorFlagInf ErFlag;
extern uint8_t TemperSign ;

void Uart6_Configuration(void);

void TIM5_Init(void);
void OpenTIM5(void);
void CloseTIM5(void);

void SendAck(void);
void SendNack(void);
void MachineInit(void);
void SendCoord(uint8_t floor,uint8_t row);
void SendLink(void);
void StopSell(void);
void GetMeal(void);
uint8_t SetTemper(uint8_t temper);

bool manageusart6data(void);

uint8_t OrderSendLink(void);
uint8_t OrderMachineInit(void);
uint8_t OrderSendCoord(uint8_t floor,uint8_t row);
uint8_t OrderStopSell(void);
uint8_t OrderGetMeal(void);
uint8_t OrderSetTemper(uint8_t inputtemper);
void OnlymachieInit(void);


#endif


