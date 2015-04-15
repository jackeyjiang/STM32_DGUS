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
Urart6RecFlagInf machinerec;
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
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;//抢占优先级最高
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
  USART_ClearFlag(USART6,USART_FLAG_TC);
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
uint8_t temper_old=0;
uint8_t SetTemper(uint8_t temper)
{
  uint8_t Cmd[6]={'H','D','0','0',0x0d,0x0a};
  uint8_t i;
	if(temper_old!=temper)
	{
		temper_old= temper;
		//printf("tempratrue  set is %d\r\n",temper);
		Cmd[2] = Cmd[2] + (temper)/10;
		Cmd[3] = Cmd[3] + (temper)%10;
		
		for(i=0;i<6;i++)
		{
			USART_SendData(USART6,Cmd[i] );//串口1发送一个字符
			while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);//等待发送完成
		}
    return 0;
	}
  else
  {
    return 1;
  } 
}



/*解析返回Usart6数据*/
void manageusart6data(void)
{
  if(Usart6DataFlag ==1)
	{
		Usart6DataFlag = 0;
		switch (Usart6Buff[0])
		{
			case 'E':
				if( Usart6Buff[1] == 1)
				  {
				    if( Usart6Buff[2] == 0 )
						{
							if( Usart6Buff[3] == 1 )
							{
								ErFlag.E101 = true;
								AbnormalHandle(X_timeout);
							}
							else if( Usart6Buff[3] == 2 )
							{
								ErFlag.E102 = true;
								AbnormalHandle(X_leftlimit);
							}
							else if( Usart6Buff[3] == 3 )
							{
								ErFlag.E103 = true;
								AbnormalHandle(X_rightlimit);
							}
							else
							{
							}
						}
						else
						{
						}
				  }
				else if( Usart6Buff[1] ==2)
				{
					if( Usart6Buff[2] == 0 )
					{
						if( Usart6Buff[3] == 1 )
						{
							ErFlag.E201 = true;
							AbnormalHandle(mealtake_timeout);
						}
						else
						{
						}
					}
					else
					{
					}
				}
				else if( Usart6Buff[1] ==3)
				{
					if( Usart6Buff[2] == 0 )
					{
						if( Usart6Buff[3] == 1 )
						{
							ErFlag.E301 = true;
							AbnormalHandle(Y_timeout);
						}
						else
						{
						}
					}
					else
					{
					}
				}
				else if( Usart6Buff[1] ==4)
				{
					if( Usart6Buff[2] == 0 )
					{
						if( Usart6Buff[3] == 1 )
						{
							ErFlag.E401 = true;
							AbnormalHandle(link_timeout);
						}
						else
						{
						}
					}
					else
					{
					}
				}
				else if( Usart6Buff[1] ==5)
				{
					if( Usart6Buff[2] == 0 )
					{
						if( Usart6Buff[3] == 1 )
						{
							ErFlag.E501 = true;
							AbnormalHandle(Z_timeout);
						}
						else if( Usart6Buff[3] == 2 )
						{
							ErFlag.E502 = true;
							AbnormalHandle(Z_uplimit);
						}
						else if( Usart6Buff[3] == 3 )
						{
							ErFlag.E503 = true;
							AbnormalHandle(Z_downlimit);
						}
						else
						{
						}
					}
					else
					{
					}
				}
				else if( Usart6Buff[1] ==6)
				{
					if( Usart6Buff[2] == 0 )
					{
						if( Usart6Buff[3] == 1 )
						{
							ErFlag.E601 = true;
							AbnormalHandle(solenoid_timeout);
						}
						else
						{
						}
					}
					else
					{
					}
				}
				else if( Usart6Buff[1] ==7)
				{
					if( Usart6Buff[2] == 0 )
					{
						
					}
					else if(Usart6Buff[2] == 1 ) 
					{
						if( Usart6Buff[3] == 1 )
						{
							ErFlag.E711 = true;
							AbnormalHandle(Eeprom_erro);
						}
						else
						{
						}
					}
					else
					{
					}
				}
				else
				{
				}
				 break;
			case 'D':
				if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0'))
			  		{
			  		  //开门
			  		  machinerec.redoor = 0;
	
			  		}
				else  if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '1'))
				{
					//关门
					machinerec.redoor = 1;
				}
				else
				{
				}
				break;
			case 'N':
				if((Usart6Buff[1]== 'O')&&(Usart6Buff[2]== 'R')&&(Usart6Buff[3]== '0'))
				  		{
				  		  //到达相对原点，即待机位置
						  machinerec.rerelative = 1;
				  		}
				else
					{
					}
				break;
			case 'S':
				if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0'))
			  		{
			  		  //到达取餐点
					  machinerec.regoal = 1;
			  		}
				else
				{
				
				}
				break;
			case 'O':
				if((Usart6Buff[1]== 'U')&&(Usart6Buff[2]== 'T')&&(Usart6Buff[3]== '0'))
			  		{
			  		  //餐已到达出餐口
					  machinerec.retodoor = 1;
			  		}
				else
				{
				
				}
				break;
			case 'F':
				if((Usart6Buff[1]== 'I')&&(Usart6Buff[2]== 'N')&&(Usart6Buff[3]== '0'))
			  		{
			  		  //餐已被取餐
					  machinerec.remealaway = 1;
			  		}
				else
				{
				
				}
				break;
			case 'T':
				if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0')&&(Usart6Buff[4]== 0x0d)&&(Usart6Buff[5]== 0x0a))
					{
						//餐在取餐口过了20秒还未被取走
					machinerec.remealnoaway = 1;
					}
				else if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '1')&&(Usart6Buff[4]== 0x0d)&&(Usart6Buff[5]== 0x0a))
					{
						//取餐5秒了还未取到餐
					machinerec.reenablegetmeal = 1;
					}
				else if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '2')&&(Usart6Buff[4]== 0x0d)&&(Usart6Buff[5]== 0x0a))
					{
						//取餐5秒了还未取到餐
					machinerec.reenablegetmeal = 1;
					}
				else if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '3')&&(Usart6Buff[4]== 0x0d)&&(Usart6Buff[5]== 0x0a))
					{
						//取餐5秒了还未取到餐
					machinerec.reenablegetmeal = 1;
					}
				else if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '4')&&(Usart6Buff[4]== 0x0d)&&(Usart6Buff[5]== 0x0a))
					{
						//取餐5秒了还未取到餐
					machinerec.reenablegetmeal = 1;
					}
					else
				{
				
				}
				break;
			case 'A':
				if( Usart6Buff[1] == '+' )
				  {
				    TemperSign =0;
				  }
				else if( Usart6Buff[1] == '-' )
				{
					TemperSign =1;
				}
				else
				{
				}
				Temperature =  (Usart6Buff[2] - 48)*10 + (Usart6Buff[3] - 48);
				break;
			default : 
		  	break;
		}
		Usart6Index =0;
		memset(Usart6Buff,0,6);
	}
}






/*返回1表示连接成功，返回0表示连接失败*/
uint8_t OrderSendLink(void)
{
	uint8_t RetryFre;
	
	LinkTime =0;
	RetryFre =0;
	SendLink();
	while(1)
	{
		if( machinerec.reack ==1)	  //ack
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 1;
		}
		
		if( LinkTime >1)  //超时
		{
			LinkTime =0;
			return 0;
		}
		
		if(machinerec.renack ==1)  //nack
		{
			machinerec.reack = 0;
			machinerec.renack = 0;
			RetryFre ++;
			SendLink();			
		} 
		
		if( RetryFre>=3)
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0; 
			return 0;
		}
		
	}
	
}



/*返回1表示初始化成功，返回0表示初始化失败*/
uint8_t OrderMachineInit(void)
{
	uint8_t RetryFre;
	
	LinkTime =0;
	RetryFre =0;
	MachineInit();
	delay_ms(5);
	while(1)
	{
		if( machinerec.reack ==1)	  //ack
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 1;
		}
		
		if( LinkTime >1)  //超时
		{
			LinkTime =0;
			return 0;
		}
		
		if(machinerec.renack ==1)  //nack
		{
			machinerec.reack = 0;
			machinerec.renack = 0;
			RetryFre ++;
			MachineInit();			
		} 
		
		if( RetryFre>=3)
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 0;
		}
		
	}
	
}

/*返回1表示发送坐标成功，返回0表示发送坐标失败*/
uint8_t OrderSendCoord(uint8_t floor,uint8_t row)
{
	uint8_t RetryFre;
	
	LinkTime =0;
	RetryFre =0;
	SendCoord(floor,row);
	delay_ms(5);
	while(1)
	{
		if( machinerec.reack ==1)	  //ack
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 1;
		}
		
		if( LinkTime >1)  //超时 ??? 
		{
			LinkTime =0;
			return 0;
		}
		
		if(machinerec.renack ==1)  //nack
		{
			machinerec.reack = 0;
			machinerec.renack = 0;
			RetryFre ++;
			SendCoord(floor,row);			
		} 
		
		if( RetryFre>=3)
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 0;
		}
		
	}
	
}


/*返回1表示停止销售成功，返回0表示停止销售失败*/
uint8_t OrderStopSell(void)
{
	uint8_t RetryFre;
	
	LinkTime =0;
	RetryFre =0;
	StopSell();
	delay_ms(5);
	while(1)
	{
		if( machinerec.reack ==1)	  //ack
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 1;
		}
		
		if( LinkTime >1)  //超时
		{
			LinkTime =0;
			return 0;
		}
		
		if(machinerec.renack ==1)  //nack
		{
			machinerec.reack = 0;
			machinerec.renack = 0;
			RetryFre ++;
			StopSell();			
		} 
		
		if( RetryFre>=3)
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 0;
		}
		
	}
	
}


/*返回1表示取餐命令成功，返回0表示取餐命令失败*/
uint8_t OrderGetMeal(void)
{
	uint8_t RetryFre;
	
	LinkTime =0;
	RetryFre =0;
	GetMeal();
	delay_ms(5);
	while(1)
	{
		if( machinerec.reack ==1)	  //ack
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 1;
		}
		
		if( LinkTime >1)  //超时
		{
			LinkTime =0;
			return 0;
		}
		
		if(machinerec.renack ==1)  //nack
		{
			machinerec.reack = 0;
			machinerec.renack = 0;
			RetryFre ++;
			GetMeal();			
		} 
		if( RetryFre>=3)
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 0;
		}
	}
}


/*返回1表示发送设置温度成功，返回0表示表示发送设置温度失败*/
uint8_t OrderSetTemper(uint8_t inputtemper)
{
	uint8_t RetryFre;
	
	LinkTime =0;
	RetryFre =0;
	if(1 == SetTemper(inputtemper) )return 1; //表示已存储当前温度，返回1
	delay_ms(5);
	while(1)
	{
		if( machinerec.reack ==1)	  //ack
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 1;
		}
		
		if( LinkTime >1)  //超时
		{
			LinkTime =0;
			return 0;
		}
		
		if(machinerec.renack ==1)  //nack
		{
			machinerec.reack = 0;
			machinerec.renack = 0;
			RetryFre ++;
			SetTemper(inputtemper);			
		} 
		
		if( RetryFre>=3)
		{
			LinkTime =0;
			machinerec.reack = 0;
			machinerec.renack = 0;
			return 0;
		}
		
	}
	
}



/*机械手初始化*/
void OnlymachieInit(void)
{
  uint8_t temp;
  uint8_t InitOkflag =0; 
  uint8_t mancineinitflag;
	uint8_t linkflag =0;  //
  
  LinkTime =0;
	OpenTIM5();
	Usart6Index =0; 
	memset(Usart6Buff,0,6); 
  machinerec.rerelative = 0;
	linkflag = OrderSendLink(); 
	while(1)
	{
		if(linkflag ==1)
		{
			while(1)
			{
				temp =0;
				temp = OrderSendLink(); 
				if( temp == 1)
				{
					mancineinitflag = OrderMachineInit();		
					if(mancineinitflag ==1)
					{
						LinkTime =0;
						while(1)
						{
							if(Usart6DataFlag ==1) 
							{
								manageusart6data();
							}
							if(machinerec.rerelative == 1)
							{
								InitOkflag =1;
								break;
							}					
							if(LinkTime >= 60)
							{
								break;
							}
						}	
					}
				}
				if(LinkTime >= 60)
				{
					break;
				}
				if(InitOkflag ==1 )
				{
					break;
				}
			}
			break;
		}
		else
		{
			linkflag = OrderSendLink(); 
		}
	}  
}
