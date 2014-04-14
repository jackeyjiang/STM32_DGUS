#include "stm32f4xx.h"
#include "protocol.h"
#include "string.h"

uint8_t Temperature=0;

void Uart6_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  /* Enable USART clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  
  /* Connect USART pins to AF7 */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART6, &USART_InitStructure);
  
  /* NVIC configuration */
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

    /* Enable USART */
  USART_Cmd(USART6, ENABLE);
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
  USART_ClearFlag(USART6, USART_FLAG_TC);     // 清标志
}

 /*******************************************************************************
* Function Name	: UART6_IRQHandler
* Description	: 与机械手通迅接收数据
* Input			: 无
* Output 		: 无
* Return 		: void
*修改时间：2014.2.14
*修改人	： yao
*******************************************************************************/
uint8_t Usart6Buff[6];
uint8_t Usart6Index;

uint8_t Usart6DataFlag =0;  //此标志作为是否有数据接收，0无，1有数据，平时清0
uint8_t TemperSign ;

void USART6_IRQHandler(void)
{
 	if(USART_GetITStatus(USART6,USART_IT_RXNE)!=RESET)//数据接收扫描
	{
		USART_ClearITPendingBit(USART6,USART_FLAG_RXNE);//清接收标志
		Usart6Buff[Usart6Index]	= USART_ReceiveData(USART6)  ;
		Usart6Index++;
		Usart6DataFlag =1;
		/*以下作测试用，可能用不到*/
		switch (Usart6Buff[0])
	  {
	  	case 0x06 :  //ACK
			{
		    Usart6Buff[0] =0;
		  	Usart6Index =0;
			  machinerec.reack =1;
			  machinerec.renack = 0;
			}break;
		  case 0x15 :  //NACK
			{
		  	//最多重发三次原命令
			  Usart6Buff[0] =0;
			  Usart6Index =0;
			  machinerec.reack =0;
			  machinerec.renack = 1;
			}break;
		  case 'E':  //错误标志
			{
		  	if(Usart6Index >= 6)
			  {
			    if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
			  	{
			  	  SendAck();
				    if( Usart6Buff[1] == 1)
				    {
				      if( Usart6Buff[2] == 0 )
					    {
					      if( Usart6Buff[3] == 1 )
					      {
					        ErFlag.E101 = true;
					      }
					      else if( Usart6Buff[3] == 2 )
					      {
					        ErFlag.E102 = true;
					      }
					      else if( Usart6Buff[3] == 3 )
					      {
					        ErFlag.E103 = true;
					      }
					    }
				    }
				    else if( Usart6Buff[1] ==2)
				    {
				      if( Usart6Buff[2] == 0 )
					    {
					      if( Usart6Buff[3] == 1 )
					      {
					        ErFlag.E201 = true;
					      }
					    }
				    }
				    else if( Usart6Buff[1] ==3)
				    {
				      if( Usart6Buff[2] == 0 )
					    {
					      if( Usart6Buff[3] == 1 )
					      {
					        ErFlag.E301 = true;
					      }
					    }
				    }
				    else if( Usart6Buff[1] ==4)
				    {
				      if( Usart6Buff[2] == 0 )
					    {
					      if( Usart6Buff[3] == 1 )
					      {
					        ErFlag.E401 = true;
					      }
					    }
				    }
				    else if( Usart6Buff[1] ==5)
				    {
				      if( Usart6Buff[2] == 0 )
					    {
					      if( Usart6Buff[3] == 1 )
					      {
					        ErFlag.E501 = true;
					      }
					    }
				    }
				    else if( Usart6Buff[1] ==6)
				    {
				      if( Usart6Buff[2] == 0 )
					    {
					      if( Usart6Buff[3] == 1 )
					      {
					        ErFlag.E601 = true;
					      }
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
					      }
					    }
			  	  }
			      else
			  	  {
			  	    SendNack();
			  	  }
			    }
			    //mem_set_00(Usart6Buff,6);
			    memset(Usart6Buff,0,6);
			    Usart6Index =0;
			  }
			}break;
		  case 'D': //门状态
			{
		    if(Usart6Index >= 6)
			  {
			    if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
			  	{
			  	  SendAck();
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
			  	}
			    else
			  	{
			  	  SendNack();
			    }
			    memset(Usart6Buff,0,6);
			    Usart6Index =0;  
			  }
			}break;
		  case 'N':
      {				
		    if(Usart6Index >= 6)
			  {
			    if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
			  	{
			  	  SendAck();
				    if((Usart6Buff[1]== 'O')&&(Usart6Buff[2]== 'R')&&(Usart6Buff[3]== '0'))
				  		{
				  		  //到达相对原点，即待机位置
						    machinerec.rerelative = 1;
				  		}
				  }
			    else
			  	{
			  	  SendNack();
			  	}
			    memset(Usart6Buff,0,6);
			    Usart6Index =0;
			  }
			}break;
		  case 'S':
			{				
		    if(Usart6Index >= 6)
			  {
			    if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
			  	{
			  	  SendAck();
				    if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0'))
			  		{
			  		  //到达取餐点
					    machinerec.regoal = 1;
			  		}
				  }
			    else
			  	{
			  	  SendNack();
			  	}
			    memset(Usart6Buff,0,6);
			    Usart6Index =0;  
			  }
			}break;  	
		  case 'O':
			{				
		    if(Usart6Index >= 6)
			  {
			    if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
			  	{
			  	  SendAck();
				    if((Usart6Buff[1]== 'U')&&(Usart6Buff[2]== 'T')&&(Usart6Buff[3]== '0'))
			  		{
			  		  //餐已到达出餐口
					    machinerec.retodoor = 1;
			  		}
				  }
			    else
			  	{
			  	  SendNack();
			  	}
			    memset(Usart6Buff,0,6);
			    Usart6Index =0;	  
			  }
		 	}break;
		  case 'F':
			{				
		    if(Usart6Index >= 6)
			  {
			    if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
			  	{
			  	  SendAck();
				    if((Usart6Buff[1]== 'I')&&(Usart6Buff[2]== 'N')&&(Usart6Buff[3]== '0'))
			  		{
			  	    //餐已被取餐
					    machinerec.remealaway = 1;
			  		}
				  }
			    else
			  	{
			  	  SendNack();
			  	}
			    memset(Usart6Buff,0,6);
			    Usart6Index =0;
			  }
		  }break;  
		  case 'T': //餐在取餐口未被取走，或餐架上的餐取不到
			{
		    if(Usart6Index >= 6)
			  {
			    if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
			  	{
			  	  SendAck();
				    if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0')&&(Usart6Buff[4]== 0x0d)&&(Usart6Buff[5]== 0x0a))
			  		{
			  		  //餐在取餐口过了20秒还未被取走
					    machinerec.remealnoaway = 1;
			  		}
				    else if((Usart6Buff[1]== '0')&&(Usart6Buff[2]== '0')&&(Usart6Buff[3]== '0')&&(Usart6Buff[4]== 0x0d)&&(Usart6Buff[5]== 0x0a))
			  		{
			  		  //取餐5秒了还未取到餐
					    machinerec.reenablegetmeal = 1;
			  		}
				  }
			    else
			  	{
			  	  SendNack();
			  	}
			    memset(Usart6Buff,0,6);
			    Usart6Index =0;
			  }
			}break;
		  case 'A': //机械手返回的温度数据，格式 ：A、+-、温度高位、温度低位、CR、LF
		  {
		    if(Usart6Index >= 6)
			  {
			    if((Usart6Buff[4]==0x0d) &&(Usart6Buff[5] == 0x0a))
			  	{
			  	  SendAck();
				    if( Usart6Buff[1] == '+' )
				    {
				      TemperSign =0;
				    }
				    else if( Usart6Buff[1] == '-' )
				    {
				      TemperSign =1;
				    }
				    Temperature =  (Usart6Buff[2] - 48)*10 + (Usart6Buff[3] - 48);
				  }
			    else
			  	{
			  	  SendNack();
			  	}
			    memset(Usart6Buff,0,6);
			    Usart6Index =0;
			  }
	    }break;
		  default:
			{				
				if( Usart6Index >=6)
		    {
		      Usart6Index =0;
		      memset(Usart6Buff,0,6);
		    }
		  }break;			
	  } 
  }
}
