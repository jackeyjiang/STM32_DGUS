#include "stm32f4xx.h"
#include "bsp.h"
#include "uart6.h"


/*此为与机械手对接的协议 ，使用USART6通迅 ,
当发送的命令返回NACK（0x15)时，最多只发送原命令到机械手，
机械手也同理。
0x0d 表示回车
0x0a 表示换行
*/
uint8_t RetryFre;

uint8_t LinkTime;

ErrorFlagInf ErFlag;
  /***********************************************
 用于定时时间。
 *************************************************/
//通用定时器6初始化,对84MHz（TIM6时钟源为168MHz/2）进行TIM_scale分频后作为计数时钟
/*******************************************************************************
* Function Name  : TIM5_Init
* Description    :定时器5初始化
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
 void TIM5_Init(void)//TIM_Period为16位的数	   //设置1秒钟中断一次
{
  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	NVIC_InitTypeDef  NVIC_InitStructure; 
	 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//外设时钟使能  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;					
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级最高
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//子优先级
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = 19999;//计数器重装值
  	TIM_TimeBaseStructure.TIM_Prescaler = 4199;
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数方式
  	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM5, (4200-1), TIM_PSCReloadMode_Immediate);//设置16位时钟分频系数,立即重载模式
	  TIM_ClearFlag(TIM5, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
  	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);//计满中断
  	TIM_Cmd(TIM5, DISABLE);

}

    /*******************************************************************************
* Function Name  : OpenTIM5
* Description    : 打开定时器5
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void OpenTIM5(void)
{
   TIM_Cmd(TIM5, ENABLE);
}

    /*******************************************************************************
* Function Name  : CloseTIM5
* Description    : 关闭定时器5
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
void CloseTIM5(void)
{
   TIM_Cmd(TIM5, DISABLE);
}

/*发送ACK*/
void SendAck(void)
{
  USART_SendData(USART6,0x06 );//串口1发送一个字符
  while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
}



/*发送NACK*/

void SendNack(void)
{
  USART_SendData(USART6,0x15 );//串口1发送一个字符
  while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
}




/*下发机械手初始化命令，开机时用，机械手收到后返回0x06,异常时返回E***等错误代码*/
void MachineInit(void)
{
  uint8_t Cmd[6]={'I','0','0','0',0x0d,0x0a};
  uint8_t i;
  
  for(i=0;i<6;i++)
  {
    USART_SendData(USART6,Cmd[i] );//串口1发送一个字符
    while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
  }
}


/*发送取餐坐标,0列表示A列，1列表示B列，2列表示C列，层用2位10进制表示，发到机械手的层有高低位之分
机械手收到后返回ACK(0x06)*/
void SendCoord(uint8_t floor,uint8_t row)
{
  uint8_t Cmd[6]={'M','0','0','A',0x0d,0x0a};
  uint8_t i;

  
  Cmd[1] = Cmd[1]+(floor + 1)/10;
  Cmd[2] = Cmd[2]+ (floor +1)%10;
  Cmd[3] = Cmd[3] + row;
  for(i=0;i<6;i++)
  {
    USART_SendData(USART6,Cmd[i] );//串口1发送一个字符
    while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
  }
}



/*发确认机械手连接指令，正常返回0x06,异常时返回E000*/
void SendLink(void)
{
  uint8_t Cmd[6]={'P','0','0','0',0x0d,0x0a};
  uint8_t i;
  
  for(i=0;i<6;i++)
  {
    USART_SendData(USART6,Cmd[i] );//串口1发送一个字符
    while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
  }
}



/* 不能取餐信号，当餐取出后，此信号无效，正常返回0x06,
即不能销售*/
void StopSell(void)
{
  uint8_t Cmd[6]={'C','0','0','0',0x0d,0x0a};
  uint8_t i;
  
  for(i=0;i<6;i++)
  {
    USART_SendData(USART6,Cmd[i] );//串口1发送一个字符
    while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
  }
}



/*取餐信号，机械手收到此信号后，返回0x06,并从餐架上取走餐，并把餐送到出餐口*/
void GetMeal(void)
{
  uint8_t Cmd[6]={'G','0','0','0',0x0d,0x0a};
  uint8_t i;
  
  for(i=0;i<6;i++)
  {
    USART_SendData(USART6,Cmd[i] );//串口1发送一个字符
    while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
  }
}


/*设置温度,*/
void SetTemper(uint8_t temper)
{
  uint8_t Cmd[6]={'H','D','0','0',0x0d,0x0a};
  uint8_t i;

  Cmd[2] = Cmd[2] + (temper+1)/10;
  Cmd[3] = Cmd[3] + (temper+1)%10;
  
  for(i=0;i<6;i++)
  {
    USART_SendData(USART6,Cmd[i] );//串口1发送一个字符
    while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
  }
}

Urart6RecFlagInf machinerec;
/*处理接收数据*/
//void ManageUsart6(void)
//{
//  if(Usart6DataFlag ==1)
//  	{
//  	  switch (Usart6Buff[0])
//	  	{
//	  	  case 0x06 :  //ACK
//		    Usart6Buff[0] =0;
//		  	Usart6Index =0;
//			machinerec.reack =1;
//			machinerec.renack = 0;
//		 	break;
//		  case 0x15 :  //NACK
//		  	//最多重发三次原命令
//			Usart6Buff[0] =0;
//			Usart6Index =0;
//			machinerec.reack =0;
//			machinerec.renack = 1;
//			break;
//		  case 'E':  //错误标志
//		  	if(Usart6Index >= 6)
//			{
//			  if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
//			  	{
//			  	  SendAck();

//			  	}
//			  else
//			  	{
//			  	  SendNack();
//			  	}
//			  //mem_set_00(Usart6Buff,6);
//			  memset(Usart6Buff,0,6);
//			  Usart6Index =0;
//			}
//			else
//			{
//			}
//			break;
//		  case 'D': //门状态
//		    if(Usart6Index >= 6)
//			{
//			  if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
//			  	{
//			  	  SendAck();
//				  if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0'))
//			  		{
//			  		  //开门
//			  		  machinerec.redoor = 0;
//	
//			  		}
//				  else  if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '1'))
//					{
//					  //关门
//					  machinerec.redoor = 1;
//					}
//				  else
//				  {
//				  }
//			  	}
//			  else
//			  	{
//			  	  SendNack();
//			  	}
//			  memset(Usart6Buff,0,6);
//			  Usart6Index =0;
//			  
//			}
//			else
//			{
//			}
//					
//		 	break;
//		  case 'N': 
//		    if(Usart6Index >= 6)
//			{
//			  if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
//			  	{
//			  	  SendAck();
//				  if((Usart6Buff[1]== 'O')&&(Usart6Buff[2]== 'R')&&(Usart6Buff[3]== '0'))
//				  		{
//				  		  //到达相对原点，即待机位置
//						  machinerec.rerelative = 1;
//				  		 
//				  		}
//				  else
//						{
//						
//						}
//				}
//			  else
//			  	{
//			  	  SendNack();
//			  	}
//			  memset(Usart6Buff,0,6);
//			  Usart6Index =0;
//			  
//			}
//			else
//			{
//			}
//			break;
//		  case 'S': 
//		    if(Usart6Index >= 6)
//			{
//			  if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
//			  	{
//			  	  SendAck();
//				  if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0'))
//			  		{
//			  		  //到达取餐点
//					  machinerec.regoal = 1;
//			  		}
//				  else
//					{
//					
//					}
//				}
//			  else
//			  	{
//			  	  SendNack();
//			  	}
//			  memset(Usart6Buff,0,6);
//			  Usart6Index =0;
//			  
//			}
//			else
//			{
//			}
//		  	
//			break;
//		  case 'O': 
//		    if(Usart6Index >= 6)
//			{
//			  if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
//			  	{
//			  	  SendAck();
//				  if((Usart6Buff[1]== 'U')&&(Usart6Buff[2]== 'T')&&(Usart6Buff[3]== '0'))
//			  		{
//			  		  //餐已到达出餐口
//					  machinerec.retodoor = 1;
//			  		}
//				  else
//					{
//					
//					}
//				}
//			  else
//			  	{
//			  	  SendNack();
//			  	}
//			  memset(Usart6Buff,0,6);
//			  Usart6Index =0;
//			  
//			}
//			else
//			{
//			}
//		    
//		 	break;
//		  case 'F': 
//		    if(Usart6Index >= 6)
//			{
//			  if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
//			  	{
//			  	  SendAck();
//				  if((Usart6Buff[1]== 'I')&&(Usart6Buff[2]== 'N')&&(Usart6Buff[3]== '0'))
//			  		{
//			  		  //餐已被取餐
//					  machinerec.remealaway = 1;
//			  		}
//				  else
//					{
//					
//					}
//				}
//			  else
//			  	{
//			  	  SendNack();
//			  	}
//			  memset(Usart6Buff,0,6);
//			  Usart6Index =0;
//			  
//			}
//			else
//			{
//			}
//		    
//			break;
//		  case 'T': //餐在取餐口未被取走，或餐架上的餐取不到
//		    if(Usart6Index >= 6)
//			{
//			  if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
//			  	{
//			  	  SendAck();
//				  if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0')&&(Usart6Buff[4]== 0x0d)&&(Usart6Buff[5]== 0x0a))
//			  		{
//			  		  //餐在取餐口过了20秒还未被取走
//					  machinerec.remealnoaway = 1;
//			  		}
//				  else if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0')&&(Usart6Buff[4]== 0x0d)&&(Usart6Buff[5]== 0x0a))
//			  		{
//			  		  //取餐5秒了还未取到餐
//					  machinerec.reenablegetmeal = 1;
//			  		}
//				  else
//					{
//					
//					}
//				}
//			  else
//			  	{
//			  	  SendNack();
//			  	}
//			  memset(Usart6Buff,0,6);
//			  Usart6Index =0;
//			}
//			else
//			{
//			}
//		    
//			break;
//		  default : 
//		  	break;
//	  	
//	  	}
//  	}
//}



///*返回0表示发送成功，返回1表示发送失败*/
uint8_t manageretry1(void (*fun) (void) )
{
  RetryFre =0;
  LinkTime =0;

//  OpenTIM5();
  while(1)
  {
    delay_ms(5);
		if(UART6_GetCharsInRxBuf()>=0)
	if( machinerec.reack ==1)	  //ack
	  {
	    LinkTime =0;
		machinerec.reack = 0;
//		CloseTIM5();
		return 0;
	  } 
	if( LinkTime >1)
	{
	  LinkTime =0;
	  RetryFre =0;
	  machinerec.reack = 0;
//	  CloseTIM5();
	  return 1;
	}

	if( RetryFre >3)
	{
	  LinkTime =0;
	  RetryFre =0;
	  machinerec.reack = 0;
//	  CloseTIM5();
	  return 1;
	}

	if(machinerec.renack ==1)
	{
	  machinerec.reack = 0;
	  machinerec.renack = 0;
	  RetryFre ++;
	  (*fun)();
	  
	} 
  }

//  CloseTIM5();
//  return 1;
}



/*返回0表示发送成功，返回1表示发送失败*/
uint8_t manageretry2(void )
{
  RetryFre =0;
  LinkTime =0;

//  OpenTIM5();
  while(1)
  {
    delay_ms(5);
	if( machinerec.reack ==1)	  //ack
	  {
	    LinkTime =0;
		machinerec.reack = 0;
//		CloseTIM5();
		return 0;
	  } 
	if( LinkTime >1)
	{
	  LinkTime =0;
	  RetryFre =0;
	  machinerec.reack = 0;
//	  CloseTIM5();
	  return 1;
	}

	if( RetryFre >3)
	{
	  LinkTime =0;
	  RetryFre =0;
	  machinerec.reack = 0;
//	  CloseTIM5();
	  return 0;
	}

	if(machinerec.renack ==1)
	{
	  machinerec.reack = 0;
	  machinerec.renack = 0;
	  SendCoord(Line,Column);
	  RetryFre ++;
	} 
  }

//  CloseTIM5();
//  return 1;
}



