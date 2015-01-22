#include "stm32f4xx.h"
#include "szt_gpboc.h"
#include "bsp.h"
#include "rtc.h"

sendorderDef SztSendData;
sendorderDef GpbocSendData;
uint8_t MustSendData[21]; //必须要发送的数据
uint8_t  AckOver; //Ack数据是否接收完
uint8_t  DataOver;//返回数据是否接收完
uint8_t  Usart2Flag;//数据类型，分为 无数据：0、Ack：1、data：2

uint8_t PrintBuf2[640];
uint32_t PrintIndex2;

uint8_t SztSamNum[4]; //深圳通Sam卡号
uint8_t GpbocSamNum[4];//银联Sam卡号
uint8_t UserNo[3] = {0x00,0x01,0x23};//操作员编号,BCD码,3位,如123表示为0x00,0x01,0x23
uint8_t newtime[8];//同步 时间值，
uint8_t timeMode;//同步方式，0为一般同步；1为强制同步
extern Struct_TD TimeDate ;		 //系统时间，共7Data
uint8_t SztReadCardTime; //深圳通读卡时间

/*银联初始化输入参数，共32个，除了第一个数要作修改外，其它的都为保留字，全为0
第一个数表示收单行个数，默认为0x00，表示一个收单行“银联";
若非0时，如 0x01一个收单行，银联；0x02：两个收单行，银联&商联商用；
*/
uint8_t Gpboc_Init_PutInParameter[32] ={
0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
} ;

/* 银联上传参数共32个，都为预留值0x00*/
uint8_t Gpboc_SendDataParameter[32] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00	
};


/*银联批结、签退的参数共32个，都为预留值0x00*/
uint8_t Gpboc_CheckOutParameter[32] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00	
};

DeviceInf Szt_deviceInf; //深圳通状态查询返回值

uint8_t TimeResult;	 //时间同步的结果
MachineTimeDef MachineTime;	  //当前设备时间

SztReductInfStr SztReductInf;	//深圳通扣款返回值
SztReadCardInfStr SztCardInf;	//深圳通读卡返回值

GpbocInitInf UpInitData[10];   //定义10个银联签到内容结构体
uint8_t UpInitErrNum[2];	   //银联返回错误
uint8_t UpInitGpbocErrNum[2];  //银联错误
GpbocReadCardInf UpReadCardData;  //读卡返回内容
TRANSLOG_TO_HOST UpDeductData;	//银联扣款交易内容
//uint8_t UpDedectBackup[600];  //一个灾难备份数据
UPLOADLOG_TO_HOS UpSendData;   //银联上传数据返回结构体
LOGINLOG_TO_HOST UpCheckOutData; //银联批结返回结构体

//交易成功次数记数，达到要求次数后上送银联数据，上送完了之后此数归0
uint8_t freq_toSendGpboc;
//交易成功次数记数，达到要求次数后上送深圳通数据，上送完了之后此数归0
uint8_t freq_toSendSzt;
uint8_t Flag_szt_gpboc_ok =0;


//extern u32 tmp_hh = 0x17, tmp_mm = 0x3b, tmp_ss = 0x01;
//extern u32 tmp_yy = 0x09, tmp_m2 = 0x01, tmp_dd = 0x01,tmp_ww=0x01;
/***************
串口2作为深圳通、银联刷卡通道
****************/



/*打开深圳通\银联通道*/
void Open_Szt_Gpboc(void)
{
  USART_Cmd(USART2,ENABLE);
  //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

/*关闭深圳通\银联通道*/
void Close_Szt_Gpboc(void)
{
  USART_Cmd(USART2,DISABLE);
  //USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
}



/*回正确应答:ACK*/
void return_ACK(void)
{
   uint8_t ackdata[6] = {DataHead,BeginCode1,BeginCode2,0x00,0xff,EndCode};
   uint8_t i;

   for(i=0;i<6;i++)
   {
     USART_SendData(USART2,ackdata[i]);
	 while( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完成
   }
}


/*回错误应答:NACK*/
void return_NACK(void)
{
  uint8_t nackdata[6] = {DataHead,BeginCode1,BeginCode2,0xff,0x00,EndCode};
  uint8_t i;

  for(i=0;i<6;i++)
  {
    USART_SendData(USART2,nackdata[i]);
	while( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//等待发送完成
  }
}




/*深圳通固定用到的数据，输入参数为从saddr开始到Dcs之前所有的数据长度，以及命令类型*/
void SztFixData(uint8_t LenH,uint8_t LenL,uint8_t OrderNum)
{
  SztSendData.datahead = DataHead;
  SztSendData.startPacket[0] = BeginCode1;
  SztSendData.startPacket[1] = BeginCode2;
  SztSendData.dataLen[0] = LenH;
  SztSendData.dataLen[1] = LenL;
  SztSendData.saddr[0] = 0x06;
  SztSendData.saddr[1] = 0xff;
  SztSendData.saddr[2] = 0xff;
  SztSendData.saddr[3] = 0xff;
  SztSendData.saddr[4] = 0xff;
  SztSendData.taddr[0] = 0x05;
  SztSendData.taddr[1] = SztSamNum[0];
  SztSendData.taddr[2] = SztSamNum[1];
  SztSendData.taddr[3] = SztSamNum[2];
  SztSendData.taddr[4] = SztSamNum[3];
  SztSendData.startNum ++ ; //再改过
  SztSendData.SendCommand = OrderNum;
  SztSendData.plen1[0] = 0x00;
  SztSendData.plen1[1] = 0x01;
  SztSendData.plen2[0] = 0x00;
  SztSendData.plen2[1] = 0x01;
  SztSendData.dataend = 0x00;

  MustSendData[0] = SztSendData.datahead ;
  MustSendData[1] =   SztSendData.startPacket[0];
  MustSendData[2] =   SztSendData.startPacket[1];
  MustSendData[3] =   SztSendData.dataLen[0];
  MustSendData[4] =   SztSendData.dataLen[1];
  MustSendData[5] =   SztSendData.saddr[0];
  MustSendData[6] =   SztSendData.saddr[1];
  MustSendData[7] =   SztSendData.saddr[2];
  MustSendData[8] =   SztSendData.saddr[3];
  MustSendData[9] =   SztSendData.saddr[4];
  MustSendData[10] =   SztSendData.taddr[0];
  MustSendData[11] =   SztSendData.taddr[1];
  MustSendData[12] =   SztSendData.taddr[2];
  MustSendData[13] =   SztSendData.taddr[3];
  MustSendData[14] =   SztSendData.taddr[4];
  MustSendData[15] =   SztSendData.startNum;
  MustSendData[16] =   SztSendData.SendCommand;
  MustSendData[17] =   SztSendData.plen1[0];
  MustSendData[18] =   SztSendData.plen1[1];
  MustSendData[19] =   SztSendData.plen2[0];
  MustSendData[20] =   SztSendData.plen2[1];
}




/* 银联固定用到的数据，输入参数为从saddr开始到Dcs之前所有的数据长度，以及命令类型,
对比深圳通，银联没用Sam卡，taddr全为0xff，若有Sam卡，taddr就用Sam卡号*/
void GpbocFixData(uint8_t LenH,uint8_t LenL,uint8_t OrderNum)
{
  GpbocSendData.datahead = DataHead;
  GpbocSendData.startPacket[0] = BeginCode1;
  GpbocSendData.startPacket[1] = BeginCode2;
  GpbocSendData.dataLen[0] = LenH;
  GpbocSendData.dataLen[1] = LenL;
  GpbocSendData.saddr[0] = 0x06;
  GpbocSendData.saddr[1] = 0xff;
  GpbocSendData.saddr[2] = 0xff;
  GpbocSendData.saddr[3] = 0xff;
  GpbocSendData.saddr[4] = 0xff;
  GpbocSendData.taddr[0] = 0x05;
  GpbocSendData.taddr[1] = GpbocSamNum[0];
  GpbocSendData.taddr[2] = GpbocSamNum[1];
  GpbocSendData.taddr[3] = GpbocSamNum[2];
  GpbocSendData.taddr[4] = GpbocSamNum[3];
  SztSendData.startNum ++;//再改过
  GpbocSendData.SendCommand = OrderNum;
  GpbocSendData.plen1[0] = 0x00;
  GpbocSendData.plen1[1] = 0x01;
  GpbocSendData.plen2[0] = 0x00;
  GpbocSendData.plen2[1] = 0x01;
  GpbocSendData.dataend = 0x00;

  MustSendData[0] = GpbocSendData.datahead ;
  MustSendData[1] =   GpbocSendData.startPacket[0];
  MustSendData[2] =   GpbocSendData.startPacket[1];
  MustSendData[3] =   GpbocSendData.dataLen[0];
  MustSendData[4] =   GpbocSendData.dataLen[1];
  MustSendData[5] =   GpbocSendData.saddr[0];
  MustSendData[6] =   GpbocSendData.saddr[1];
  MustSendData[7] =   GpbocSendData.saddr[2];
  MustSendData[8] =   GpbocSendData.saddr[3];
  MustSendData[9] =   GpbocSendData.saddr[4];
  MustSendData[10] =   GpbocSendData.taddr[0];
  MustSendData[11] =   GpbocSendData.taddr[1];
  MustSendData[12] =   GpbocSendData.taddr[2];
  MustSendData[13] =   GpbocSendData.taddr[3];
  MustSendData[14] =   GpbocSendData.taddr[4];
  MustSendData[15] =   SztSendData.startNum;
  MustSendData[16] =   GpbocSendData.SendCommand;
  MustSendData[17] =   GpbocSendData.plen1[0];
  MustSendData[18] =   GpbocSendData.plen1[1];
  MustSendData[19] =   GpbocSendData.plen2[0];
  MustSendData[20] =   GpbocSendData.plen2[1];
}


/*判断接收的数据是ACK还是NACK*/
uint8_t ManageACK(void)
{
  uint8_t i;
  if(( PrintBuf2[0] == 0x00) && ( PrintBuf2[1] == 0x00) &&( PrintBuf2[2] == 0xff ) && (PrintBuf2[3] == 0x00 ) && (PrintBuf2[4] ==0xff ) && (PrintBuf2[5] ==0x00 ))
  {
    return IsAck;
  }
  else if(( PrintBuf2[0] == 0x00) && ( PrintBuf2[1] == 0x00) &&( PrintBuf2[2] == 0xff ) && (PrintBuf2[3] == 0xff ) && (PrintBuf2[4] ==0x00 ) && (PrintBuf2[5] ==0x00 ))
  {
    return IsNack;
  }
  else
  {
    //printf("\ndata is :");
	for(i=0;i<6;i++)
	{
	  //printf(" %x ",PrintBuf2[i]); 
	}
    return IsAckError;
  }

}


/*对比接收到的数据，判断其是否完整*/
uint8_t ManageRxData(void)
{
   uint16_t i;
   uint8_t sumdata =0;

   /*
   for( i=0;i<((UsartBuff[9]<< 8) + UsartBuff[10] +7);i++)
   {
     //printf(" %x ",UsartBuff[i]);  //打印所有接收到的数据
   }
   */
   for( i=0;i<((PrintBuf2[9]<< 8) + PrintBuf2[10] +1);i++)
   {
     sumdata = sumdata + PrintBuf2[11+i]; //从第12个数开始相加，一直加到校验值
   }

   if(( sumdata == 0 ) && ( PrintBuf2[6] == 0x00) && (PrintBuf2[7] == 0x00) &&(PrintBuf2[8] == 0xff ) && (PrintBuf2[PrintIndex2 - 1] == 0x00))
   {
     return_ACK();
	 return DataReOk;
   }
   else
   {
     return_NACK();
	 return DataReErr;
   }
}

uint8_t ManageReOrderType(void)
{
  uint32_t temp; 
  uint8_t  ackflagbuff = IsNc;
  uint8_t  dataflagbuff = DataNc;
  uint8_t  orderType = 0 ;
  //Open_Szt_Gpboc();
  delay_us(5);  
  //Order_Ifn_QueryStatus(); 
  while(1) 
  {
    if((Usart2Flag == AckFlag) || (AckOver == RxOk))
       break;
	  temp ++;
	  delay_us(50);
	  if(temp >= 0x0fff0 )
			break;;
  }

  //printf("\n????Ack????:%d \n",temp );

  temp =0;
  while(1)
  {
    if( AckOver == RxOk )
	    break;
	  temp ++;
	  delay_us(50);
	  if(temp >= 0x0fff0)
	    break;  
	}
  ackflagbuff = ManageACK();
  if(ackflagbuff == IsNc)
  {
    //printf("\n?????\n");
  }
  else if(ackflagbuff == IsAck)
  {
    //printf("\n??????\n");
  }
  else if( ackflagbuff == IsNack)
  {
     //printf("\n???????\n");
  }
  else
  {
     //printf("\n??????,ack????:%d\n",ackflagbuff);
  }
  if( ackflagbuff == IsAck )
  {  
	  temp =0;
		while(1) 
		{
		  if( Usart2Flag == DataFlag )
			  break;
		  temp ++;
		  delay_us(50);
		  if( temp >= 0x0fff0 )
			  break;
		}
		temp =0;
		while(1)
		{
		  if(DataOver == RxOk)
			  break;
		  temp ++;
		  delay_us(50);
		  if(temp >= 0x0fff0)
			  break;
		}	  
		dataflagbuff = ManageRxData();
	  
		if( dataflagbuff == DataNc)
		{
		  //printf("\n????????\n");
		}
		else if( dataflagbuff == DataReOk)
		{
		   //printf("\n??????\n");   
		   orderType = PrintBuf2[22];  
		}
		else if( dataflagbuff == DataReErr)
		{
		   //printf("\n???????\n");
		}
		else 
		{
		   //printf("\n??????,????????%d\n",dataflagbuff);
		}
  	}
  //printf("\nPrintIndex2= %d",PrintIndex2);
	//printf("\n??????:%x",orderType);
  
  //Close_Szt_Gpboc();

  return orderType;
}








/*发送设备状态查询命令*/
void Order_Ifn_QueryStatus(void)
{
  uint8_t data[23];
  uint8_t i;
  uint8_t temp=0;

  SztSamNum[0] = 0;
  SztSamNum[1] = 0;
  SztSamNum[2] = 0;
  SztSamNum[3] = 0;
  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x10,T_QueryStatus); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }
  
  for(i=0;i<16;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[21] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[22] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<23;i++)			 //发送设备状态查询命令
  {
    USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<23;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}






/*发送深圳通操作员登录命令*/
void Order_SztUserLogin(void)
{
  uint8_t data[26];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x13,T_UserLogin); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }

  for(i=0;i<3;i++)
	{
	  data[21+i] = UserNo[i];
	}
  
  for(i=0;i<19;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[24] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[25] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<26;i++)			 //发送深圳通操作员登录命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<26;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}






/*发送深圳通时钟同步命令*/
void Order_SztTimeCheck(void)
{
  uint8_t data[32];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x19,T_TimeCheck); //必须要发送的数据
  
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }
  /*
  for(i=0;i<8;i++)
  	{
  	  data[21+i] = newtime[i];
  	}
	*/
	RTC_TimeShow();

  data[21] = Year_Frist;
  data[22] = DectoBCD(TimeDate.Year);
  data[23] = DectoBCD(TimeDate.Month);
  data[24] = DectoBCD(TimeDate.Date);
  data[25] = DectoBCD(TimeDate.Hours);
  data[26] = DectoBCD(TimeDate.Minutes);
  data[27] = DectoBCD(TimeDate.Senconds);
  data[28] = DectoBCD(TimeDate.WeekDay);

  data[29] = timeMode;  //时间同步方式
  //printf("\n系统时钟为:%x%x-%x-%x %x:%x:%x",data[21],data[22],data[23],data[24],data[25],data[26],data[27],data[28]);
  for(i=0;i<25;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[30] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[31] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<32;i++)			 //发送深圳通时间同步命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<32;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}






/*发送深圳通首次扣款命令，输入参数为扣款金额*/
void Order_SztDeductOnce(int32_t money)
{
  uint8_t data[34];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x1B,T_SztDeductOnce); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }

  data[21] = money;
  data[22] = money>>8;
  data[23] = money>>16;
  data[24] = money>>24;

  /*

  for(i=0;i<7;i++)  //扣款时间
  	{
  	  data[25+i] = newtime[i];
  	}
  */
  //扣款时间
  RTC_TimeShow();
  data[25] = Year_Frist;;
  data[26] = DectoBCD(TimeDate.Year);
  data[27] = DectoBCD(TimeDate.Month);
  data[28] = DectoBCD(TimeDate.Date);
  data[29] = DectoBCD(TimeDate.Hours);
  data[30] = DectoBCD(TimeDate.Minutes);
  data[31] = DectoBCD(TimeDate.Senconds);
  
  //printf("\n扣款时间为:%x%x-%x-%x %x:%x:%x",data[25],data[26],data[27],data[28],data[29],data[30],data[31]);

  for(i=0;i<27;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[32] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[33] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<34;i++)			 //发送深圳通时间同步命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<34;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}





/*发送深圳通再次扣款命令，输入参数为扣款金额*/
void Order_SztDeductAgain(int32_t money)
{
  uint8_t data[34];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x1B,T_SztDeductAgain); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }

  data[21] = money;
  data[22] = money>>8;
  data[23] = money>>16;
  data[24] = money>>24;
  
  /*
  for(i=0;i<7;i++)  //扣款时间
  	{
  	  data[25+i] = newtime[i];
  	}
  	*/
  //扣款时间
  RTC_TimeShow();
  data[25] = Year_Frist;;
  data[26] = DectoBCD(TimeDate.Year);
  data[27] = DectoBCD(TimeDate.Month);
  data[28] = DectoBCD(TimeDate.Date);
  data[29] = DectoBCD(TimeDate.Hours);
  data[30] = DectoBCD(TimeDate.Minutes);
  data[31] = DectoBCD(TimeDate.Senconds);
  
  //printf("\n扣款时间为:%x%x-%x-%x %x:%x:%x",data[25],data[26],data[27],data[28],data[29],data[30],data[31]);

  for(i=0;i<27;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[32] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[33] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<34;i++)			 //发送深圳通时间同步命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<34;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}





/*发送深圳通首次读卡命令*/
void Order_SztReadCardOnce(void)
{
  uint8_t data[24];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x11,T_SztReadCardOnce); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }

  
  data[21] = SztReadCardTime;
  	
  
  for(i=0;i<17;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[22] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[23] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  

  for(i=0;i<24;i++)			 //发送深圳通操作员登录命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<24;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}



/*发送深圳通再次读卡命令*/
void Order_SztReadCardAgain(void)
{
  uint8_t data[24];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x11,T_SztReadCardAgain); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }

  
  data[21] = SztReadCardTime;
  	
  
  for(i=0;i<17;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[22] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[23] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<24;i++)			 //发送深圳通操作员登录命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<24;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}




/*发送深圳通断开连接命令*/
void Order_SztUnlink(void)
{
  uint8_t data[23];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x10,T_SztUnlink); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }
  
  for(i=0;i<16;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[21] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[22] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<23;i++)			 //发送设备状态查询命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<23;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}



/*深圳通自主签到等命令，子命令参数为0x03*/
void Order_SztAutoCheckIn(void)
{
  uint8_t data[24];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x11,T_SztCheck_In); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }

  data[21] = 0x03;
  
  for(i=0;i<17;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[22] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[23] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<24;i++)			 //发送深圳通操作员登录命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<24;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}




/*深圳通自主参数下载命令，子命令参数为0x01*/
void Order_SztAutoDownload(void)
{
  uint8_t data[24];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x11,T_SztCheck_In); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }

  data[21] = 0x01;
  
  for(i=0;i<17;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[22] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[23] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<24;i++)			 //发送深圳通操作员登录命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<24;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}



/*深圳通自主上传数据命令，子命令参数为0x02*/
void Order_SztAutoSendData(void)
{
  uint8_t data[24];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x11,T_SztCheck_In); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }

  data[21] = 0x02;
  
  for(i=0;i<17;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[22] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[23] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<24;i++)			 //发送深圳通操作员登录命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<24;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}






/*深圳通充值抵消首次命令*/
void Order_SztRechargeOnce(void)
{
  uint8_t data[30];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x17,T_SztRechargeOnce); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }

  for(i=0;i<7;i++)
  	{
  	  data[21+i] = newtime[i];
  	}
  
  for(i=0;i<23;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }

  data[28] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[29] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<30;i++)			 //发送设备状态查询命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<30;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}



/*深圳通充值抵消再次命令*/
void Order_SztRechargeAgain(void)
{
  uint8_t data[30];
  uint8_t i;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

 
  delay_us(5);
  
  SztFixData(0x00,0x17,T_SztRechargeAgain); //必须要发送的数据
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];		  //把必须要发送的数据写入数组中
  }
  
  for(i=0;i<16;i++)			 //计算要校验的数据的和
  {
    temp = temp + data[5+i];
  }
  
  for(i=0;i<7;i++)
  	{
  	  data[21+i] = newtime[i];
  	}

  data[28] = 0 - temp;		   //求检验码，此数+要校验的数据的和 = 0
  data[29] = SztSendData.dataend;	   //结束码
  //printf(" %x ",data[21]);
  
  

  for(i=0;i<30;i++)			 //发送设备状态查询命令
  {
    //USART_ClearFlag(USART2,USART_FLAG_TC);
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
  /*
  for(i=0;i<30;i++)
  {
     printf(" %x ",data[i]);
  }
  */
}






/*银联初始化签到命令*/
void Order_Gpboc_Init(void)
{
  uint8_t data[55];	//48个长度，其中32个为数据参数,从0到20为固定数，21到52为32个输入参数，53为校验码，54为结束标志
  uint8_t i;
  //sendorderDef senddata;
  uint8_t temp=0;


  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;
  GpbocSamNum[0] = 0xff;
  GpbocSamNum[1] = 0xff;
  GpbocSamNum[2] = 0xff;
  GpbocSamNum[3] = 0xff;
  
  delay_us(5);

  GpbocFixData(0x00,0x30,T_GpbocCheck_In); 
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];
  }
  
  for( i=0;i<32;i++)
  {
    data[i+21] = Gpboc_Init_PutInParameter[i];
  }

  for(i=0;i<48;i++)
  {
    temp = temp + data[5+i];
  }
 // senddata.Dcs= 0 - temp;
  data[53] = 0 - temp;//senddata.Dcs;
  data[54] = SztSendData.dataend;
  //printf("校验码为： %x \n",data[53]);
  /*
  for(i=0;i<55;i++)
  	{
  	  printf(" %x ",data[i]);
  	}
	*/

  for(i=0;i<55;i++)
  {
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
} 



 
/*读取银联卡内的余额命令,卡设置为深圳通\银联复合卡*/
void Order_Gpboc_ReadCard (void)
{
  uint8_t data[25];	//18个长度，其中2个为数据参数,从0到20为固定数，21到22为2个输入参数，23为校验码，24为结束标志
  uint8_t i;
  sendorderDef senddata;
  uint8_t temp=0;

 
  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;

  delay_us(5);

  GpbocFixData(0x00,0x12,T_GpbocQueryReadCard); 
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];
  }
  
  data[21] = 0x02;//0x01为只读银行卡，0x02为一张卡既有深圳通又有银联功能
  data[22] = 0x00;

  for(i=0;i<18;i++)
  {
    temp = temp + data[5+i];
  }
  senddata.Dcs= 0 - temp;
  data[23] = senddata.Dcs;
  data[24] = SztSendData.dataend;
  //printf("校验码为： %x \n",data[23]);
  /*
  for(i=0;i<25;i++)
  	{
  	  printf(" %x ",data[i]);
  	}
*/

  for(i=0;i<25;i++)
  {
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
}




/*银联卡扣款命令*/
void Order_Gpboc_Deduct(uint32_t submoney)
{
  uint8_t data[28];	//21个长度，其中5个为数据参数,从0到20为固定数，21到25为32个输入参数，26为校验码，27为结束标志
  uint8_t i;
  sendorderDef senddata;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;
  
  delay_us(5);

  GpbocFixData(0x00,0x15,T_GpbocDeduct); 
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];
  }

  
  data[21] = submoney;//0x01;
  data[22] = submoney >>8;//0x00;
  data[23] = submoney >>16;//0x00;
  data[24] = submoney >>24; //0x00; 
  /*
  data[21] =0x01;
  data[22] =0x00;
  data[23] =0x00;
  data[24] =0x00; 
  */
  data[25] = 0x20;  // 寻卡时间，为0x00时，验卡15秒；为其它值时，分别为其所代表的值
  
  for(i=0;i<21;i++)
  {
    temp = temp + data[5+i];
  }
  senddata.Dcs= 0 - temp;
  data[26] = senddata.Dcs;
  data[27] = SztSendData.dataend;
  //printf("校验码为： %x \n",data[26]);
  /*
  for(i=0;i<28;i++)
  	{
  	  printf(" %x ",data[i]);
  	}
	*/

  for(i=0;i<28;i++)
  {
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
}




/*银联上传数据命令*/
void Order_Gpboc_SendData(void)
{
  uint8_t data[55];	//48个长度，其中32个为数据参数,从0到20为固定数，21到52为32个输入参数，53为校验码，54为结束标志
  uint8_t i;
  //sendorderDef senddata;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;
  
  delay_us(5);

  GpbocFixData(0x00,0x30,T_GpbocSendData); 
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];
  }
  
  for( i=0;i<32;i++)
  {
    data[i+21] = Gpboc_SendDataParameter[i];
  }

  for(i=0;i<48;i++)
  {
    temp = temp + data[5+i];
  }
 // senddata.Dcs= 0 - temp;
  data[53] = 0 - temp;//senddata.Dcs;
  data[54] = SztSendData.dataend;
  //printf("校验码为： %x \n",data[53]);
  /*
  for(i=0;i<55;i++)
  	{
  	  printf(" %x ",data[i]);
  	}
	*/

  for(i=0;i<55;i++)
  {
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
}






/* 银联批结算、签退命令*/
void Order_Gpboc_Check_Out(void)
{
  uint8_t data[55];	//48个长度，其中32个为数据参数,从0到20为固定数，21到52为32个输入参数，53为校验码，54为结束标志
  uint8_t i;
  sendorderDef senddata;
  uint8_t temp=0;

  PrintIndex2 = 0;
  memset(PrintBuf2,0xff,640);
  Usart2Flag = NormalFlag;
  DataOver = RxNo;
  AckOver = RxNo;
  
  delay_us(5);

  GpbocFixData(0x00,0x30,T_GpbocCheck_Out); 
  for( i=0;i<21;i++)
  {
    data[i] = MustSendData[i];
  }
  
  for( i=0;i<32;i++)
  {
    data[i+21] = Gpboc_CheckOutParameter[i];
  }

  for(i=0;i<48;i++)
  {
    temp = temp + data[5+i];
  }
  senddata.Dcs= 0 - temp;
  data[53] = senddata.Dcs;
  data[54] = SztSendData.dataend;
  //printf("校验码为： %x \n",data[53]);
  /*
  for(i=0;i<55;i++)
  	{
  	  printf(" %x ",data[i]);
  	}
	*/

  for(i=0;i<55;i++)
  {
    USART_SendData(USART2,data[i]);
  	while( USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
  }
}





/*处理设备状态查询,返回1成功，返回0失败*/
uint8_t ManageQueryStatus(void)
{
   uint8_t i;
    
   if( PrintBuf2[27] == 0x00)	 //正常
   {
      //printf("\n状态查询成功\n");

	  SztSamNum[0] = PrintBuf2[12];
	  SztSamNum[1] = PrintBuf2[13];
	  SztSamNum[2] = PrintBuf2[14];
	  SztSamNum[3] = PrintBuf2[15];
	  
      for(i=0;i<4;i++)
	  {
	    Szt_deviceInf.Device_ID[i] = PrintBuf2[28 +i];       
	  }
	   //直接16进制输出
	  //printf("\nSAMID为:%x%x%x%x",Szt_deviceInf.Device_ID[0],Szt_deviceInf.Device_ID[1],Szt_deviceInf.Device_ID[2],Szt_deviceInf.Device_ID[3]); 


	  Szt_deviceInf.Terminal_ID = PrintBuf2[32] + (PrintBuf2[33]<<8) + (PrintBuf2[34]<<16) + (PrintBuf2[35]<<24);
	  
	  //printf("\n深圳通终端编号为：%d\n",Szt_deviceInf.Terminal_ID);


	  for(i=0;i<10;i++)
	  {
	    Szt_deviceInf.SoftWare_Ver[i] = PrintBuf2[36+i];
	  }
	  /*
	  printf("\n设备软件版本号为：");
	  for(i=0;i<10;i++)
	  {
	    printf("%c",Szt_deviceInf.SoftWare_Ver[i]);
	  }
		*/

	  for(i=0;i<4;i++)
	  {
	     Szt_deviceInf.Device_Status[i] = PrintBuf2[46 +i];
	  }
	  //设备状态字

	  
	  Szt_deviceInf.Curr_RemainMoney = PrintBuf2[50] + (PrintBuf2[51]<<8) + (PrintBuf2[52]<<16) +(PrintBuf2[53]<<24);
	  
	   
	  //printf("\n当前剩余额度为:%ld\n",Szt_deviceInf.Curr_RemainMoney);


	  for(i=0;i<28;i++)
	  {
	    Szt_deviceInf.Curr_ParFileName[i] = PrintBuf2[54 +i];
	  }
	  /*
	  printf("\n当前额度参数名为：");
	  for(i=0;i<28;i++)
	  {
	     printf("%c",Szt_deviceInf.Curr_ParFileName[i]);
	  }
	  */

	  
	  Szt_deviceInf.Curr_OKRecNum = PrintBuf2[82] + (PrintBuf2[83]<<8);
	  //printf("\n当前未上传的交易记录数为：%d\n",Szt_deviceInf.Curr_OKRecNum);


	 
	  Szt_deviceInf.Curr_BadRecNum = PrintBuf2[84] + (PrintBuf2[85]<<8);
	 
	  
	  //printf("\n当前未上传的交易记录数为：%d\n",Szt_deviceInf.Curr_BadRecNum );

	  for(i=0;i<8;i++)
	  {
	     Szt_deviceInf.Curr_DeviceTime[i] = PrintBuf2[86 +i];
	  }
	  //printf("\n当前设备时钟为：%x%x年%x月%x日%x时%x分%x秒\n",Szt_deviceInf.Curr_DeviceTime[0],Szt_deviceInf.Curr_DeviceTime[1],Szt_deviceInf.Curr_DeviceTime[2],Szt_deviceInf.Curr_DeviceTime[3],Szt_deviceInf.Curr_DeviceTime[4],Szt_deviceInf.Curr_DeviceTime[5],Szt_deviceInf.Curr_DeviceTime[6]);
	  //星期可不显示，若显，如0x03表示星期三

	  /*
	  if( Szt_deviceInf.Device_Status[0] & 0x80 )
	  {
	    printf("\n当前记录满\n");
	  }
	  else
	  {
	   printf("\n当前记录未满\n"); 
	  }

	  if( Szt_deviceInf.Device_Status[0] & 0x40 )
	  {
	    printf("\n当前有未上传记录\n");
	  }
	  else
	  {
	    printf("\n当前没有未上传记录\n");
	  }


	  if( Szt_deviceInf.Device_Status[0] & 0x20 )
	  {
	    printf("\n黑名单有效存在\n");
	  }
	  else
	  {
	    printf("\n黑名单无效或不存在\n");
	  }

	  
	  if( Szt_deviceInf.Device_Status[0] & 0x10 )
	  {
	    printf("\n系统用户参数有效\n");
	  }
	  else
	  {
	     printf("\n系统用户参数无效或不存在\n");
	  }

	  if( Szt_deviceInf.Device_Status[1] & 0x08 )
	  {
	    printf("\n联机签到\n");
	  }
	  else
	  {
	    printf("\n联机签退\n");
	  }

	  if( Szt_deviceInf.Device_Status[1] & 0x04 )
	  {
	    printf("\n设备时间故障，需要同步时间\n");
	  }
	  else
	  {
	    printf("\n无设备硬件故障\n");
	  }

	  if( Szt_deviceInf.Device_Status[1] & 0x02 )
	  {
	    printf("\n额度授权正常\n");
	  }
	  else
	  {
	    printf("\n额度授权过期或无效\n");
	  }


	  if( Szt_deviceInf.Device_Status[1] & 0x01 )
	  {
	    printf("\n请求上传系统应用数据\n");
	  }
	  else
	  {
	    printf("\n无请求上传系统应用数据\n");
	  }
	  */

	   
   }
   else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
   {
     //printf("\n状态查询失败1\n");
   }
   else if( PrintBuf2[27] == 0x51)  //重作交易
   {
     //printf("\n状态查询失败2\n");
   }
   else
   {
     //printf("\n状态查询失败3\n");
   }

   return PrintBuf2[27]+1;
}



/*解析深圳通操作员登录,返回1成功，返回0失败*/
uint8_t ManageSztUserLogin(void)
{
  uint8_t temp =0;
  if(PrintBuf2[27] == 0x00)
	{
	  //printf("\n操作员登录成功");
	}
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
  {
	  printf("\n操作员登录失败1\n");
  }
  else if( PrintBuf2[27] == 0x51)	//重作交易
  {
  	//printf("\n操作员登录失败2\n");
  }
  else
  {
  	//printf("\n操作员登录失败3\n");
  }

  temp = PrintBuf2[27] +1;

  return temp;

}







/*解析深圳通时间同步,没有同步结果信息(2014.2.11),返回1成功，返回0失败*/
uint8_t ManageSztTimeCheck(void)
{
  uint8_t temp =0;
  TimeResult = 0xff;
  if(PrintBuf2[27] == 0x00)
  	{
  	  //printf("\n时间同步成功");
	  TimeResult = PrintBuf2[27]; //2014.2.11
	  /*
	  if( TimeResult == 0x00)
	  	{
	  	  printf("\n时钟同步成功");
	  	}
	  else if( TimeResult == 0x01)
	  	{
	  	  printf("\n输入的时钟信息超前太多");
	  	}
	  else if( TimeResult == 0x02)
	  	{
	  	  printf("\n输入的时钟信息滞后太多");
	  	}
	  else if( TimeResult == 0x03)
	  	{
	  	  printf("\n输入的时钟信息无效");
	  	}
	  else if( TimeResult == 0x04)
	  	{
	  	  printf("\n时间未同步");
	  	}
	  else
	  	{
	  	  printf("\n时间同步参数错误:%d",TimeResult);
	  	}
	  	 */

	  MachineTime.Year_H = PrintBuf2[28];
	  MachineTime.Year_L = PrintBuf2[29];
	  MachineTime.Month = PrintBuf2[30];
	  MachineTime.Day = PrintBuf2[31];
	  MachineTime.Hours = PrintBuf2[32];
	  MachineTime.Minutes = PrintBuf2[33];
	  MachineTime.Senconds = PrintBuf2[34];
	  MachineTime.WeekDay = PrintBuf2[35];
	   /*
	  tmp_yy = MachineTime.Year_L;
	  tmp_m2 = MachineTime.Month;
	  tmp_dd = MachineTime.Day;
	  tmp_hh = MachineTime.Hours;
	  tmp_mm = MachineTime.Minutes;
	  tmp_ss = MachineTime.Senconds;
	  tmp_ww = MachineTime.WeekDay;
	  RTC_TimeRegulate();
*/
	 // printf("\n当前设备时钟为:%2x%2x-%x-%x %x:%x:%x",MachineTime.Year_H,MachineTime.Year_L,MachineTime.Month,MachineTime.Day,MachineTime.Hours,MachineTime.Minutes,MachineTime.Senconds);

	 //新加修改系统时间
	 tmp_yy = BCDtoDec(MachineTime.Year_L);
	 tmp_m2 = BCDtoDec(MachineTime.Month);
	 tmp_dd = BCDtoDec(MachineTime.Day);
	 tmp_hh = BCDtoDec(MachineTime.Hours);
	 tmp_mm = BCDtoDec(MachineTime.Minutes);
	 tmp_ss = BCDtoDec(MachineTime.Senconds);
	 tmp_ww = BCDtoDec(MachineTime.WeekDay);
	 //printf("\n   time:%d-%d-%d %d:%d:%d",tmp_yy,tmp_m2,tmp_dd,tmp_hh,tmp_mm,tmp_ss);
	 RTC_TimeRegulate();  
  	}
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
   {
     //printf("\n时间同步失败1\n");
   }
   else if( PrintBuf2[27] == 0x51)  //重作交易
   {
     //printf("\n时间同步失败2\n");
   }
   else
   {
     //printf("\n时间同步失败3\n");
   }

   temp = PrintBuf2[27] +1;

   return temp;

}



/*解析深圳通初次扣款,返回1成功，返回0失败*/
uint8_t ManageSztDeductOnce(void)
{
  uint8_t temp =0;
  if(PrintBuf2[27] == 0x00)
  	{
  	   //printf("\n深圳通首次扣款成功\n");
	   SztReductInf.BeginMoney = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n深圳通扣款前余额为%ld\n",SztReductInf.BeginMoney);
	   SztReductInf.EndMoney = PrintBuf2[32] + (PrintBuf2[33]<<8) + (PrintBuf2[34]<<16) + (PrintBuf2[35]<<24);
	   //printf("\n深圳通扣款后余额为%ld\n",SztReductInf.EndMoney);
	   SztReductInf.CardNum = PrintBuf2[36] + (PrintBuf2[37]<<8) + (PrintBuf2[38]<<16) + (PrintBuf2[39]<<24);
	   //printf("\n深圳通卡号为%d\n",SztReductInf.CardNum);
	   SztReductInf.TradeNum = PrintBuf2[40] + (PrintBuf2[41]<<8) + (PrintBuf2[42]<<16) + (PrintBuf2[43]<<24);
	   //printf("\n深圳通本次交易序号为%d\n",SztReductInf.TradeNum);
	   SztReductInf.TACNum = PrintBuf2[44] + (PrintBuf2[45]<<8) + (PrintBuf2[46]<<16) + (PrintBuf2[47]<<24);
	   //printf("\n深圳通本次交易TAC码为%d\n",SztReductInf.TACNum);
	   SztReductInf.MachineAmount = PrintBuf2[48] + (PrintBuf2[49]<<8) + (PrintBuf2[50]<<16) + (PrintBuf2[51]<<24);
	   //printf("\n设备授权额度为%d\n",SztReductInf.MachineAmount);
	   SztReductInf.machineNum = PrintBuf2[52] + (PrintBuf2[53]<<8) + (PrintBuf2[54]<<16) + (PrintBuf2[55]<<24);
	   //printf("\n设备终端编号为%d\n",SztReductInf.machineNum);
  	  
  	}
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
   {
     //printf("\n深圳通扣款失败1\n");
   }
   else if( PrintBuf2[27] == 0x51)  //重作交易
   {
     //printf("\n深圳通扣款失败2\n");
   }
   else
   {
     //printf("\n深圳通扣款失败3\n");
   }

   temp = PrintBuf2[27] +1;

   return temp;

}





/*解析深圳通再次扣款,返回1成功，返回0失败*/
uint8_t ManageSztDeductAgain(void)
{
  uint8_t temp =0;
  if(PrintBuf2[27] == 0x00)
  	{
  	   //printf("\n深圳通首次扣款成功\n");
	   SztReductInf.BeginMoney = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n深圳通扣款前余额为%ld\n",SztReductInf.BeginMoney);
	   SztReductInf.EndMoney = PrintBuf2[32] + (PrintBuf2[33]<<8) + (PrintBuf2[34]<<16) + (PrintBuf2[35]<<24);
	   //printf("\n深圳通扣款后余额为%ld\n",SztReductInf.EndMoney);
	   SztReductInf.CardNum = PrintBuf2[36] + (PrintBuf2[37]<<8) + (PrintBuf2[38]<<16) + (PrintBuf2[39]<<24);
	   //printf("\n深圳通卡号为%d\n",SztReductInf.CardNum);
	   SztReductInf.TradeNum = PrintBuf2[40] + (PrintBuf2[41]<<8) + (PrintBuf2[42]<<16) + (PrintBuf2[43]<<24);
	   //printf("\n深圳通本次交易序号为%d\n",SztReductInf.TradeNum);
	   SztReductInf.TACNum = PrintBuf2[44] + (PrintBuf2[45]<<8) + (PrintBuf2[46]<<16) + (PrintBuf2[47]<<24);
	   //printf("\n深圳通本次交易TAC码为%d\n",SztReductInf.TACNum);
	   SztReductInf.MachineAmount = PrintBuf2[48] + (PrintBuf2[49]<<8) + (PrintBuf2[50]<<16) + (PrintBuf2[51]<<24);
	   //printf("\n设备授权额度为%d\n",SztReductInf.MachineAmount);
	   SztReductInf.machineNum = PrintBuf2[52] + (PrintBuf2[53]<<8) + (PrintBuf2[54]<<16) + (PrintBuf2[55]<<24);
	   //printf("\n设备终端编号为%d\n",SztReductInf.machineNum);
  	  
  	}
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
   {
     //printf("\n深圳通扣款失败1\n");
   }
   else if( PrintBuf2[27] == 0x51)  //重作交易
   {
     //printf("\n深圳通扣款失败2\n");
   }
   else
   {
     //printf("\n深圳通扣款失败3\n");
   }

   temp = PrintBuf2[27] +1;

   return temp;

}




/*解析深圳通首次读卡,返回1成功，返回0失败*/
uint8_t ManageSztReadCardOnce(void)
{
  uint8_t temp =0;
  uint8_t i;
  if(PrintBuf2[27] == 0x00)
  {
	  //printf("\n深圳通首次读卡成功\n");
	  SztCardInf.CardStatus = PrintBuf2[28];
	  //printf("\n卡状态为:%d",SztCardInf.CardStatus);
	  SztCardInf.CardNum = PrintBuf2[29] + (PrintBuf2[30]<<8) + (PrintBuf2[31]<<16) + (PrintBuf2[32]<<24);
	  //printf("\n卡号为:%d",SztCardInf.CardNum);
	  SztCardInf.CardType = PrintBuf2[33];
	  //printf("\n卡类型为:%d",SztCardInf.CardStatus);
	  SztCardInf.SztMoney = PrintBuf2[34] + (PrintBuf2[35]<<8) + (PrintBuf2[36]<<16) + (PrintBuf2[37]<<24);
	  //printf("\n深圳通当前余额为:%ld",SztCardInf.SztMoney);
	  SztCardInf.MachinMoney = PrintBuf2[38] + (PrintBuf2[39]<<8) + (PrintBuf2[40]<<16) + (PrintBuf2[41]<<24);
	  //printf("\n设备当前剩余额度为:%ld",SztCardInf.MachinMoney);
	  SztCardInf.SztDeposit = PrintBuf2[42] + (PrintBuf2[43]<<8) + (PrintBuf2[44]<<16) + (PrintBuf2[45]<<24);
	  //printf("\n深圳通押金为:%d",SztCardInf.SztDeposit);
	  for(i=0;i<4;i++)
	  {
		  SztCardInf.SellCardDay[i] = PrintBuf2[46+i];
	  }
	  //printf("\n深圳通出售日期为:%x%x-%x-%x",SztCardInf.SellCardDay[0],SztCardInf.SellCardDay[1],SztCardInf.SellCardDay[2],SztCardInf.SellCardDay[3]);
	  SztCardInf.GetRentsMonth = PrintBuf2[50];
	  //printf("\n卡收租月份为:%d",SztCardInf.GetRentsMonth);
	  SztCardInf.RentsNum = PrintBuf2[51] + (PrintBuf2[52]<<8) + (PrintBuf2[53]<<16) + (PrintBuf2[54]<<24);
	  //printf("\n应收租金为:%d",SztCardInf.RentsNum);
	  SztCardInf.ReturnableMoney = PrintBuf2[55] + (PrintBuf2[56]<<8) + (PrintBuf2[57]<<16) + (PrintBuf2[58]<<24);
	  //printf("\n卡可退实际金额为:%ld",SztCardInf.ReturnableMoney);
	  SztCardInf.Poundage = PrintBuf2[59] + (PrintBuf2[60]<<8) + (PrintBuf2[61]<<16) + (PrintBuf2[62]<<24);
	  //printf("\n退卡手续费为:%ld",SztCardInf.Poundage);
	  SztCardInf.NoSellCardDeposit = PrintBuf2[63] + (PrintBuf2[64]<<8) + (PrintBuf2[65]<<16) + (PrintBuf2[66]<<24);

	  //printf("\n未售卡押金为:%ld",SztCardInf.NoSellCardDeposit);
	  SztCardInf.NoSellCardMoney = PrintBuf2[67] + (PrintBuf2[68]<<8) + (PrintBuf2[69]<<16) + (PrintBuf2[70]<<24);
	  //printf("\n未售卡金额为:%ld",SztCardInf.NoSellCardMoney);
	  
  }
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
  {
     //printf("\n深圳通首次读卡失败1\n");
  }
  else if( PrintBuf2[27] == 0x51)  //重作交易
  {
     //printf("\n深圳通首次读卡失败2\n");
  }
  else
  {
     //printf("\n深圳通首次读卡失败3\n");
  }

  temp = PrintBuf2[27] +1;

  return temp;

}





/*解析深圳通再次读卡,返回1成功，返回0失败*/
uint8_t ManageSztReadCardAgain(void)
{
  uint8_t temp =0;
  uint8_t i;
  if(PrintBuf2[27] == 0x00)
  	{
  	  //printf("\n深圳通再次读卡成功\n");
  	  SztCardInf.CardStatus = PrintBuf2[28];
  	  //printf("\n卡状态为:%d",SztCardInf.CardStatus);
  	  SztCardInf.CardNum = PrintBuf2[29] + (PrintBuf2[30]<<8) + (PrintBuf2[31]<<16) + (PrintBuf2[32]<<24);
  	  //printf("\n卡号为:%d",SztCardInf.CardNum);
  	  SztCardInf.CardType = PrintBuf2[33];
  	  //printf("\n卡类型为:%d",SztCardInf.CardStatus);
  	  SztCardInf.SztMoney = PrintBuf2[34] + (PrintBuf2[35]<<8) + (PrintBuf2[36]<<16) + (PrintBuf2[37]<<24);
  	  //printf("\n深圳通当前余额为:%ld",SztCardInf.SztMoney);
  	  SztCardInf.MachinMoney = PrintBuf2[38] + (PrintBuf2[39]<<8) + (PrintBuf2[40]<<16) + (PrintBuf2[41]<<24);
  	  //printf("\n设备当前剩余额度为:%ld",SztCardInf.MachinMoney);
  	  SztCardInf.SztDeposit = PrintBuf2[42] + (PrintBuf2[43]<<8) + (PrintBuf2[44]<<16) + (PrintBuf2[45]<<24);
  	  //printf("\n深圳通押金为:%d",SztCardInf.SztDeposit);
  	  for(i=0;i<4;i++)
  	  	{
  		  SztCardInf.SellCardDay[i] = PrintBuf2[46+i];
  	  	}
  	  //printf("\n深圳通出售日期为:%x%x-%x-%x",SztCardInf.SellCardDay[0],SztCardInf.SellCardDay[1],SztCardInf.SellCardDay[2],SztCardInf.SellCardDay[3]);
  	  SztCardInf.GetRentsMonth = PrintBuf2[50];
  	  //printf("\n卡收租月份为:%d",SztCardInf.GetRentsMonth);
  	  SztCardInf.RentsNum = PrintBuf2[51] + (PrintBuf2[52]<<8) + (PrintBuf2[53]<<16) + (PrintBuf2[54]<<24);
  	  //printf("\n应收租金为:%d",SztCardInf.RentsNum);
  	  SztCardInf.ReturnableMoney = PrintBuf2[55] + (PrintBuf2[56]<<8) + (PrintBuf2[57]<<16) + (PrintBuf2[58]<<24);
  	  //printf("\n卡可退实际金额为:%ld",SztCardInf.ReturnableMoney);
  	  SztCardInf.Poundage = PrintBuf2[59] + (PrintBuf2[60]<<8) + (PrintBuf2[61]<<16) + (PrintBuf2[62]<<24);
  	  //printf("\n退卡手续费为:%ld",SztCardInf.Poundage);
  	  SztCardInf.NoSellCardDeposit = PrintBuf2[63] + (PrintBuf2[64]<<8) + (PrintBuf2[65]<<16) + (PrintBuf2[66]<<24);
  
  	  //printf("\n未售卡押金为:%ld",SztCardInf.NoSellCardDeposit);
  	  SztCardInf.NoSellCardMoney = PrintBuf2[67] + (PrintBuf2[68]<<8) + (PrintBuf2[69]<<16) + (PrintBuf2[70]<<24);
  	  //printf("\n未售卡金额为:%ld",SztCardInf.NoSellCardMoney);
    
  	}
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
  {
     //printf("\n深圳通再次读卡失败1\n");
  }
  else if( PrintBuf2[27] == 0x51)  //重作交易
  {
     //printf("\n深圳通再次读卡失败2\n");
  }
  else
  {
     //printf("\n深圳通再次读卡失败3\n");
  }

  temp = PrintBuf2[27] +1;

  return temp;

}




/*解析深圳通断开连接,返回1成功，返回0失败*/
uint8_t ManageSztUnlink(void)
{
  uint8_t temp =0;
  
  if(PrintBuf2[27] == 0x00)
  	{
  	  //printf("\n断开连接成功\n");
  	}
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
   {
     //printf("\n断开连接失败1\n");
   }
   else if( PrintBuf2[27] == 0x51)  //重作交易
   {
     //printf("\n断开连接失败2\n");
   }
   else
   {
     //printf("\n断开连接失败3\n");
   }
  /* */

   temp = PrintBuf2[27] +1;

   return temp;

}




/*解析深圳通自主签到、参数下载、数据上传,返回1成功，返回0失败*/
uint8_t ManageSztAutoManage(void)
{
  uint8_t temp =0;
  
  if(PrintBuf2[27] == 0x00)
  	{
  	  //printf("\n自主签到或参数下载或数据上传成功\n");
  	}
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
   {
     //printf("\n自主签到或参数下载或数据上传失败1\n");
   }
   else if( PrintBuf2[27] == 0x51)  //重作交易
   {
     //printf("\n自主签到或参数下载或数据上传失败2\n");
   }
   else
   {
     //printf("\n自主签到或参数下载或数据上传失败3\n");
   }
   /**/

   temp = PrintBuf2[27] +1;

   return temp;

}




/*解析深圳通充值抵消,返回1成功，返回0失败*/
uint8_t ManageSztRechargeOnce(void)
{
  uint8_t temp =0;
  if(PrintBuf2[27] == 0x00)
  	{
  	   //printf("\n深圳通首次充值抵消成功\n");
	   SztReductInf.BeginMoney = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n深圳通扣款前余额为%ld\n",SztReductInf.BeginMoney);
	   SztReductInf.EndMoney = PrintBuf2[32] + (PrintBuf2[33]<<8) + (PrintBuf2[34]<<16) + (PrintBuf2[35]<<24);
	   //printf("\n深圳通扣款后余额为%ld\n",SztReductInf.EndMoney);
	   SztReductInf.CardNum = PrintBuf2[36] + (PrintBuf2[37]<<8) + (PrintBuf2[38]<<16) + (PrintBuf2[39]<<24);
	   //printf("\n深圳通卡号为%d\n",SztReductInf.CardNum);
	   SztReductInf.TradeNum = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n深圳通本次交易序号为%d\n",SztReductInf.TradeNum);
	   SztReductInf.TACNum = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n深圳通本次交易TAC码为%d\n",SztReductInf.TACNum);
	   SztReductInf.MachineAmount = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n设备授权额度为%d\n",SztReductInf.MachineAmount);
	   SztReductInf.machineNum = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n设备终端编号为%d\n",SztReductInf.machineNum);
  	}
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
   {
     //printf("\n失败1\n");
   }
   else if( PrintBuf2[27] == 0x51)  //重作交易
   {
     //printf("\n状态查询失败2\n");
   }
   else
   {
     //printf("\n状态查询失败3\n");
   }

   temp = PrintBuf2[27] +1;

   return temp;

}





/*解析深圳通充值抵消,返回1成功，返回0失败*/
uint8_t ManageSztRechargeAgain(void)
{
  uint8_t temp =0;
  if(PrintBuf2[27] == 0x00)
  	{
  	   //printf("\n深圳通再次充值抵消成功\n");
	   SztReductInf.BeginMoney = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n深圳通扣款前余额为%ld\n",SztReductInf.BeginMoney);
	   SztReductInf.EndMoney = PrintBuf2[32] + (PrintBuf2[33]<<8) + (PrintBuf2[34]<<16) + (PrintBuf2[35]<<24);
	   //printf("\n深圳通扣款后余额为%ld\n",SztReductInf.EndMoney);
	   SztReductInf.CardNum = PrintBuf2[36] + (PrintBuf2[37]<<8) + (PrintBuf2[38]<<16) + (PrintBuf2[39]<<24);
	   //printf("\n深圳通卡号为%d\n",SztReductInf.CardNum);
	   SztReductInf.TradeNum = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n深圳通本次交易序号为%d\n",SztReductInf.TradeNum);
	   SztReductInf.TACNum = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n深圳通本次交易TAC码为%d\n",SztReductInf.TACNum);
	   SztReductInf.MachineAmount = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n设备授权额度为%d\n",SztReductInf.MachineAmount);
	   SztReductInf.machineNum = PrintBuf2[28] + (PrintBuf2[29]<<8) + (PrintBuf2[30]<<16) + (PrintBuf2[31]<<24);
	   //printf("\n设备终端编号为%d\n",SztReductInf.machineNum); 
  	}
  else if( PrintBuf2[27] == 0x88)	 //错误，需要判断返回数据域的前两字节错误代码
   {
     //printf("\n失败1\n");
   }
   else if( PrintBuf2[27] == 0x51)  //重作交易
   {
     //printf("\n状态查询失败2\n");
   }
   else
   {
     //printf("\n状态查询失败3\n");
   }

   temp = PrintBuf2[27] +1;

   return temp;

}




/*解析银联初始化签到,返回1成功，返回0失败*/
uint8_t ManageGpbocInit(void)
{
  uint8_t i;
  uint8_t j;

  if( PrintBuf2[27] == 0x00)	 //正常
   {
     //printf("\n银联签到成功\n");
	 GpbocSamNum[0] = PrintBuf2[12];
	 GpbocSamNum[1] = PrintBuf2[13];
	 GpbocSamNum[2] = PrintBuf2[14];
	 GpbocSamNum[3] = PrintBuf2[15];

	 

	 UpInitErrNum[0] = PrintBuf2[28];	 
	 UpInitErrNum[1] = PrintBuf2[29];
	 
	 UpInitGpbocErrNum[0] = PrintBuf2[30];
	 UpInitGpbocErrNum[1] = PrintBuf2[31];

	 for(j=0;j<10;j++)
	 	{
		 	 UpInitData[j].ucLogInLogVersion = PrintBuf2[32 + j*34];
			 UpInitData[j].ucAcqBankNum = PrintBuf2[33 + j*34];
			 UpInitData[j].ucLogInOK = PrintBuf2[34 + j*34];
			 UpInitData[j].ucOper = PrintBuf2[35 + j*34];
			 UpInitData[j].ucKeyMode = PrintBuf2[36 + j*34];
			 
			 UpInitData[j].lBatchNumber = PrintBuf2[37 + j*34] + (PrintBuf2[38 + j*34]<<8) + (PrintBuf2[39 + j*34]<<16) + (PrintBuf2[40 + j*34]<<24);
			 
			 for(i=0;i<9;i++)
			 	{
			 	  UpInitData[j].szPosId[i] = PrintBuf2[41 +i + j*34] ;
			 	}

			 for(i=0;i<16;i++)
			 	{
			 	  UpInitData[j].szMerchantId[i] = PrintBuf2[50 +i + j*34];
			 	}
	 	}
	 /*
	 printf("\n结构体版本号为:%d\n",UpInitData[0].ucLogInLogVersion);
	 printf("\n收单行号:%d\n",UpInitData[0].ucAcqBankNum);
	 printf("\n签到成功标志:%d\n",UpInitData[0].ucLogInOK);
	 printf("\n操作员:%d\n",UpInitData[0].ucOper);
	 printf("\n签到密钥类型，(0表示双倍长；1表示单倍长):%c\n",UpInitData[0].ucKeyMode);
	 
	 printf("\n批次号为:%d\n",UpInitData[0].lBatchNumber);
	 printf("\n\nPOS终端号为:");
	 for(i=0;i<9;i++)
	 {
	   printf("%c",UpInitData[0].szPosId[i]);
	 }
	 delay_us(5);
	 printf("\n\n商户号为:");
	 for(i=0;i<16;i++)
	 {
	   printf("%c",UpInitData[0].szMerchantId[i]);
	 }
	   */

   }
  
    else if( PrintBuf2[27] == 0x88)  //错误，需要判断返回数据域的前两字节错误代码
	 {
	   //printf("\n银联签到失败1\n");
	 }
	 else if( PrintBuf2[27] == 0x51)  //重作交易
	 {
	  //printf("\n银联签到失败2\n");
	 }
	 else
	 {
	   //printf("\n银联签到失败3\n");
	 }


	 return PrintBuf2[27]+1;
  
}






/*解析查询银联卡余额,返回1成功，返回0失败*/
uint8_t ManageGpbocReadCard(void)
{
  uint8_t i;
  uint8_t temp=0;
  
  
  if( PrintBuf2[27] == 0x00)	 //正常
   {
     //printf("\n银联读取余额成功\n");

	 //以下为把接收到的数存到对应的数组中
	 UpInitErrNum[0] = PrintBuf2[28];	 
	 UpInitErrNum[1] = PrintBuf2[29];
	 
	 UpInitGpbocErrNum[0] = PrintBuf2[30];
	 
	 UpInitGpbocErrNum[1] = PrintBuf2[31];

	 UpReadCardData.SztStatus = PrintBuf2[32];
	 
	 UpReadCardData.SztNum = PrintBuf2[33] + (PrintBuf2[34]<<8) + (PrintBuf2[35]<<16) + (PrintBuf2[36]<<24);
	 
	 UpReadCardData.SztMoney = PrintBuf2[37] + (PrintBuf2[38]<<8) + (PrintBuf2[39]<<16) + (PrintBuf2[40]<<24);
	 //printf("\ncard money is %ld",UpReadCardData.SztMoney);
	 
	 for(i=0;i<64;i++)
	 	{
	 	  UpReadCardData.SztReserve[i] = PrintBuf2[41+i];
	 	}
	 /*
	 if( UpReadCardData.SztStatus ==0 )
	 	{
	 	  printf("\n不存在深圳通数据\n");
	 	}
	 else if (UpReadCardData.SztStatus == 0x03)
	 	{
	 	  printf("\n深圳通为正常卡\n");
	 	}
	 else if( UpReadCardData.SztStatus == 0x09)
	 	{
	 	  printf("\n深圳通为黑名单卡\n");
	 	}
	 else if( UpReadCardData.SztStatus == 0x07)
	 	{
	 	  printf("\n深圳通为回收卡\n");
	 	}
	 else 
	 	{
	 	  printf("\n深圳通无效\n");
	 	}

	 if(UpReadCardData.SztStatus != 0x00)
	 	{
	 	  printf("\n深圳通卡号为:%d\n",UpReadCardData.SztNum);
		  
		  printf("\n深圳通余额为:%ld\n",UpReadCardData.SztMoney);		
	 	}
	  */

	 

	 UpReadCardData.GpbocStatus = PrintBuf2[105];
	 if( UpReadCardData.GpbocStatus != 0)
	 {
		 for(i=0;i<20;i++)
		 	{
		 	  UpReadCardData.GpbocNum[i] = PrintBuf2[106+i];
		 	}
			/*
		 printf("\n银联卡号为:");
		 for(i=0;i<20;i++)
			{
			  printf("%c",UpReadCardData.GpbocNum[i]);
			}
			  */
		 
		 UpReadCardData.GpbocMoney = PrintBuf2[126] +(PrintBuf2[127]<<8) + (PrintBuf2[128]<<16) + (PrintBuf2[129]<<24);
		 
		 
		 for(i=0;i<64;i++)
		 	{
		 	  UpReadCardData.GpbocReserve[i] = PrintBuf2[130+i];
		 	}
			delay_us(5);
		//printf("\n银联卡内余额为:%ld\n",UpReadCardData.GpbocMoney);
		 /*
		 if( UpReadCardData.GpbocStatus != 0)
		 	{
		 	  printf("\n银联卡号为:");
			  for(i=0;i<20;i++)
			  	{
			  	  printf("%c",UpReadCardData.GpbocNum[i]);
			  	}
		
			  delay_us(5);
		
			  printf("\n银联卡内余额为:%ld\n",UpReadCardData.GpbocMoney);
		 	}
		 else
		 	{
		 	  printf("\n不存在银联数据\n");
		 	}
		   */
		 }
		 else 
		 {
		   //printf("\n不存在银联数据");
		 }
   }
  
  else if( PrintBuf2[27] == 0x88)  //错误，需要判断返回数据域的前两字节错误代码
	 {
	   //printf("\n银联读卡查询余额失败1\n");
	 }
  else if( PrintBuf2[27] == 0x51)  //重作交易
	 {
	   //printf("\n银联读卡查询余额失败1\n");
	 }
  else
	 {
	   //printf("\n银联读卡查询余额失败1\n");
	 }
  temp = PrintBuf2[27] +1;

  //printf("printBuf2[27]= %x",temp);
  
  
  return temp;
}





//uint32_t BackupsIndex;
/*解析扣款返回数据,返回1成功，返回0失败*/
uint8_t ManageGpbocDeduct(void)
{
  uint8_t i;
  uint32_t temp=0;
  if( PrintBuf2[27] == 0x00)	 //正常
   {
     //printf("\n银联扣款成功");

  	 //以下为把接收到的数存到对应的数组中
  	 UpInitErrNum[0] = PrintBuf2[28];	 
  	 UpInitErrNum[1] = PrintBuf2[29];
  	 
  	 UpInitGpbocErrNum[0] = PrintBuf2[30];
  	 UpInitGpbocErrNum[1] = PrintBuf2[31];
  
  
  	 UpDeductData.ucLogVersion = PrintBuf2[32];
  	 //printf("\n银联扣款结构体版本号为:%d\n",UpDeductData.ucLogVersion);
  	 
  	 UpDeductData.ucAcqBankNum = PrintBuf2[33];
  	 //printf("\n银联扣款收单行号为:%d\n",UpDeductData.ucAcqBankNum);
  	 
  	 UpDeductData.iTransNo = PrintBuf2[34] + (PrintBuf2[35]<<8) + (PrintBuf2[36]<<16) + (PrintBuf2[37]<<24);
  	 //printf("\n银联扣款交易号为:%ld\n",UpDeductData.iTransNo);
  	  
  	 UpDeductData.ucBlklistFail = PrintBuf2[38];
  	 //printf("\n银联扣款黑名单卡标志为:%d\n",UpDeductData.ucBlklistFail);
  	 
  	 UpDeductData.ucqPBOCSaleFail = PrintBuf2[39];
  	 //printf("\n银联扣款交易失败标志为:%d\n",UpDeductData.ucqPBOCSaleFail);
  	 
  	 UpDeductData.ucqPBOCLastRd = PrintBuf2[40];
  	 //printf("\n银联扣款读最后一条记录失败标志为:%d\n",UpDeductData.ucqPBOCLastRd);
  	 
  	 UpDeductData.ucOfflineSaleFail = PrintBuf2[41];
  	 //printf("\n银联扣款脱机交易拒绝标志为:%d\n",UpDeductData.ucOfflineSaleFail);
  	 
  	 UpDeductData.lTransAmount = PrintBuf2[42] + (PrintBuf2[43]<<8) + (PrintBuf2[44]<<16) + (PrintBuf2[45]<<24);
  	 //printf("\n银联本次交易金额为:%ld\n",UpDeductData.lTransAmount);
  	 
  	 UpDeductData.lCardBalance = PrintBuf2[46] + (PrintBuf2[47]<<8) + (PrintBuf2[48]<<16) + (PrintBuf2[49]<<24);
  	 //printf("\n银联卡内EC余额为:%ld\n",UpDeductData.lCardBalance);
  	 
  	 UpDeductData.lTraceNo = PrintBuf2[50] + (PrintBuf2[51]<<8) + (PrintBuf2[52]<<16) + (PrintBuf2[53]<<24);
  	 //printf("\n银联扣款流水号为:%ld\n",UpDeductData.lTraceNo);
  	 
  	 UpDeductData.lBatchNumber = PrintBuf2[54] + (PrintBuf2[55]<<8) + (PrintBuf2[56]<<16) + (PrintBuf2[57]<<24);
  	 //printf("\n银联扣款批次号为:%ld\n",UpDeductData.lBatchNumber);
  	 
  	 for(i=0;i<9;i++)
  	 	{
  	 	  UpDeductData.szPosId[i] = PrintBuf2[58+i];
  	 	}
  		/*
  	 printf("\nPOS终端号为:");
  	 for(i=0;i<9;i++)
  	 	{
  	 	  printf("%c",UpDeductData.szPosId[i]);
  	 	}
  		  */
  	 
  	 for(i=0;i<21;i++)
  	 	{
  	 	  UpDeductData.szCardNo[i] = PrintBuf2[67+i];
  	 	}
  		/*
  	 printf("\n卡号为:");
  	 for(i=0;i<21;i++)
  	 	{
  	 	  printf("%c",UpDeductData.szCardNo[i]);
  	 	}
  		  */
  	 
  	 for(i=0;i<9;i++)
  	 	{
  	 	  UpDeductData.szDate[i] = PrintBuf2[88+i];
  	 	}
  		/*
  	 printf("\n\n银联扣款日期为:");
  	 for(i=0;i<9;i++)
  	 	{
  	 	  printf("%c",UpDeductData.szDate[i]);
  	 	}
  		  */
  	 
  	 for(i=0;i<7;i++)
  	 	{
  	 	  UpDeductData.szTime[i] = PrintBuf2[97+i];
  	 	}
  		/*
  	 printf("\n银联扣款时间为:");	 
  	 for(i=0;i<7;i++)
  	 	{
  	 	  printf("%c",UpDeductData.szTime[i]);
  	 	}
  		  */
  	 
  	 for(i=0;i<5;i++)
  	 	{
  	 	  UpDeductData.szExpDate[i] = PrintBuf2[104+i];
  	 	}
  		/*
  	 printf("\n\n银联扣款EXp日期为:");	 
  	 for(i=0;i<5;i++)
  	 	{
  	 	  printf("%c",UpDeductData.szExpDate[i]);
  	 	}
  		  */
  
  	 
  	 for(i=0;i<4;i++)
  	 	{
  	 	  UpDeductData.szTellerNo[i] = PrintBuf2[109+i];
  	 	}
  		/*
  	 printf("\n银联扣款操作员为:");
  	 
  	 for(i=0;i<4;i++)
  	 	{
  	 	  printf("%c",UpDeductData.szTellerNo[i]);
  	 	}
  		  */
  
  	 
  	 for(i=0;i<4;i++)
  	 	{
  	 	  UpDeductData.szCardUnit[i] = PrintBuf2[113+i];
  	 	}
  		/*
  	 printf("\n银联卡所属组织为:");
  	 
  	 for(i=0;i<4;i++)
  	 	{
  	 	  printf("%c",UpDeductData.szCardUnit[i]);
  	 	}
  		  */
  	 
  	 
  	 UpDeductData.BkDataLen = PrintBuf2[117] + (PrintBuf2[118]<<8) + (PrintBuf2[119]<<16) + (PrintBuf2[120]<<24);
  	 //UpDeductData.BkDataLen = PrintBuf2[104] + (PrintBuf2[105]<<8);
  	  //printf("\n备份交易数据的长度为:%ld",UpDeductData.BkDataLen);
  
  	 // 备份数据保存
  	 
  	 for(temp =0;temp<UpDeductData.BkDataLen;temp++)
  	 	{
  	 	 // UpDedectBackup[temp] = PrintBuf2[121 + temp];  //unit32
  		  // UpDedectBackup[temp] = PrintBuf2[121 + temp];   //uint16
  	 	}
  	   
  	   delay_us(100);
  	  //printf("\n备份交易数据完成");
	 
   }
  else if( PrintBuf2[27] == 0x88)  //错误，需要判断返回数据域的前两字节错误代码
	 {
	   //printf("\n银联扣款失败1\n");
	   //return 0x88;
	 }
  else if( PrintBuf2[27] == 0x51)  //重作交易
	 {
	    //printf("\n银联扣款失败2\n");
		//return 0x51;
	 }
  else
	 {
	   // printf("\n银联扣款失败3\n");  

		
	 }

   //printf("printBuf2[27]= %x",PrintBuf2[27]);
  return PrintBuf2[27]+1;
  
	//return 0x00;
}




/*解析银联上传数据,返回1成功，返回0失败*/
uint8_t ManageGpbocSendData(void)
{
  uint8_t i;
  uint8_t temp=0xff ;
  if( PrintBuf2[27] == 0x00)	 //正常
   {
     //printf("\n银联上传数据成功\n");

	 UpInitErrNum[0] = PrintBuf2[28];	 
	 UpInitErrNum[1] = PrintBuf2[29];
	 
	 UpInitGpbocErrNum[0] = PrintBuf2[30];
	 UpInitGpbocErrNum[1] = PrintBuf2[31];

	 UpSendData.ucUpLogVersion = PrintBuf2[32];
	  //printf("\n银联上传数据的结构体版本号为:%d\n",UpSendData.ucUpLogVersion);
	  
	 UpSendData.ucAcqBankNum = PrintBuf2[33];
	 //printf("\n银联上传数据的收单行号为:%d\n",UpSendData.ucAcqBankNum);
	 
	 UpSendData.ucUploadOK = PrintBuf2[34];
	 //printf("\n银联上传数据的签到成功标志为:%d\n",UpSendData.ucUploadOK);
	 
	 UpSendData.lBatchNumber = PrintBuf2[35]+ (PrintBuf2[36]<<8) + (PrintBuf2[37]<<16) + (PrintBuf2[38]<<24);
	 //printf("\n银联上传数据的批次号为:%d\n",UpSendData.lBatchNumber);

	 
	 UpSendData.lTotalNum = PrintBuf2[39] + (PrintBuf2[40]<<8) + (PrintBuf2[41]<<16) + (PrintBuf2[42]<<24);
	 //printf("\n需要上送文件中的总笔数为:%d\n",UpSendData.lTotalNum);

	 
	 UpSendData.lToUploadNum = PrintBuf2[43] + (PrintBuf2[44]<<8) + (PrintBuf2[45]<<16) + (PrintBuf2[46]<<24);
	 //printf("\n真正需要上送的总笔数:%d\n",UpSendData.lToUploadNum);

	 
	 UpSendData.lSuccNum = PrintBuf2[47] + (PrintBuf2[48]<<8) + (PrintBuf2[49]<<16) + (PrintBuf2[50]<<24);
	 //printf("\n成功上送的笔数:%d\n",UpSendData.lSuccNum);

	 
	 UpSendData.lSuccTotalAmt = PrintBuf2[51] + (PrintBuf2[52]<<8) + (PrintBuf2[53]<<16) + (PrintBuf2[54]<<24);
	 //printf("\n成功上送的总金额:%d",UpSendData.lSuccTotalAmt);

	 
	 UpSendData.lFailNum = PrintBuf2[55] + (PrintBuf2[56]<<8) + (PrintBuf2[57]<<16) + (PrintBuf2[58]<<24);
	 //printf("\n上送失败的总笔数:%d",UpSendData.lFailNum);

	 
	 UpSendData.lFailTotalAmt = PrintBuf2[59] + (PrintBuf2[60]<<8) + (PrintBuf2[61]<<16) + (PrintBuf2[62]<<24);
	 //printf("\n上送失败的总金额:%d",UpSendData.lFailTotalAmt);

	 
	 for(i=0;i<9;i++)
	 	{
	 	   UpSendData.szPosId[i] = PrintBuf2[63+i];
	 	}
		/*
	 printf("\nPOS终端号为:");
	 for(i=0;i<9;i++)
	 	{
	 	  printf("%c",UpSendData.szPosId[i]);
	 	}
		  */
	 

	 for(i=0;i<16;i++)
	 	{
	 	   UpSendData.szMerchantId[i] = PrintBuf2[72+i];
	 	}
		/*
	 printf("\n\n商户号为:");
	 for(i=0;i<16;i++)
	 	{
	 	  printf("%c",UpSendData.szMerchantId[i]);
	 	}
		  */
		 
   }
  else if( PrintBuf2[27] == 0x88)  //错误，需要判断返回数据域的前两字节错误代码
	 {
	   //printf("\n银联上传记录失败1\n");
	 }
  else if( PrintBuf2[27] == 0x51)  //重作交易
	 {
	   //printf("\n银联上传记录失败2\n");
	 }
  else
	 {
	   //printf("\n银联上传记录失败3\n");
	 }

  temp = PrintBuf2[27]+1; 
  return temp;

}







/*解析银联批结、签退*/
uint8_t ManageGpbocCheckOut(void)
{
  uint8_t i;
  if( PrintBuf2[27] == 0x00)	 //正常
   {
     //printf("\n银联批结签退成功\n");

  	 UpInitErrNum[0] = PrintBuf2[28];	 
  	 UpInitErrNum[1] = PrintBuf2[29];
  	 
  	 UpInitGpbocErrNum[0] = PrintBuf2[30];
  	 UpInitGpbocErrNum[1] = PrintBuf2[31];
  
  	 UpCheckOutData.ucLogInLogVersion = PrintBuf2[32];
  	 //printf("\n银联批结签退的结构体版本号为:%d\n",UpCheckOutData.ucLogInLogVersion);
  	 
  	 UpCheckOutData.ucAcqBankNum = PrintBuf2[33];
  	 //printf("\n银联批结签退的收单行号为:%d\n",UpCheckOutData.ucAcqBankNum);
  	 
  	 UpCheckOutData.ucLogInOK = PrintBuf2[34];
  	  //printf("\n银联批结签退的签到成功标志为:%d\n",UpCheckOutData.ucLogInOK);
  	  
  	 UpCheckOutData.ucOper = PrintBuf2[35];
  	  //printf("\n银联批结签退的操作员号为:%d\n",UpCheckOutData.ucOper);
  	  
  	 UpCheckOutData.ucKeyMode = PrintBuf2[36];
  	 //printf("\n银联批结签退所用的密钥类型为:%c\n",UpCheckOutData.ucKeyMode);
  	 
  	 UpCheckOutData.lBatchNumber = PrintBuf2[37] + (PrintBuf2[38]<<8) + (PrintBuf2[39]<<16) + (PrintBuf2[40]<<24);
  	 //printf("\n银联批结签退的批次号为:%d\n",UpCheckOutData.lBatchNumber);
  	 
  	 for(i=0;i<9;i++)
  	 {
  		UpCheckOutData.szPosId[i] = PrintBuf2[41+i];
  	 }
  	 /*
  	 printf("\nPOS终端号为:");
  	 for(i=0;i<9;i++)
  	 	{
  	 	  printf("%c",UpCheckOutData.szPosId[i]);
  	 	}
  	   */
  	 
  
     for(i=0;i<16;i++)
  	 {
  		UpCheckOutData.szMerchantId[i] = PrintBuf2[50+i];
  	 }
  	 /*
  	 printf("\n\n商户号为:");
  	 for(i=0;i<16;i++)
  	 	{
  	 	  printf("%c",UpCheckOutData.szMerchantId[i]);
  	 	}
  	   */

   }
  else if( PrintBuf2[27] == 0x88)  //错误，需要判断返回数据域的前两字节错误代码
	 {
	   //printf("\n银联批结、签退失败1\n");
	 }
  else if( PrintBuf2[27] == 0x51)  //重作交易
	 {
	    //printf("\n银联批结、签退失败2\n");
	 }
  else
	 {
	    //printf("\n银联批结、签退失败3\n");
	 }

  return PrintBuf2[27]+1;

}




/*测试*/
void ManageGpbocAll(uint8_t PutInNum)
{
  uint8_t orderFlag;
  
  if( PutInNum ==1 )
  {
    printf("PutInNum =1");
    Order_Ifn_QueryStatus();
  }
  else if( PutInNum == 2)
  {
    printf("PutInNum =2");
    Order_Gpboc_Init();
  }
  else if( PutInNum == 3)
  {
    printf("PutInNum =3");
    Order_Gpboc_ReadCard();
  }
   else if( PutInNum == 4)
  {
    printf("PutInNum =4");
    Order_Gpboc_Deduct(1);
  }
    else if( PutInNum == 5)
  {
    printf("PutInNum =5");
    Order_Gpboc_SendData();
  }
	 else if( PutInNum == 6)
  {
    printf("PutInNum =6");
    Order_Gpboc_Check_Out();
  }
  else
  {
    //printf("\nOver");
  }
  
  
  orderFlag = ManageReOrderType();
  if( orderFlag == 0)
  	{
  	  //printf("\n错误，错误标志为:1\n");
  	}
  else if( orderFlag == R_QueryStatus)
  	{
  	  ManageQueryStatus();
  	}
  else if( orderFlag == R_GpbocCheck_In)
  	{
  	  ManageGpbocInit();
  	}
  else if( orderFlag == R_GpbocReadCard)
  	{
  	  ManageGpbocReadCard();
  	}
  else if( orderFlag == R_GpbocDeduct)
  	{
  	  ManageGpbocDeduct();
  	}
  else if( orderFlag == R_GpbocSendData)
  	{
  	  ManageGpbocSendData();
  	}
  else if( orderFlag == R_GpbocCheck_Out)
  	{
  	  ManageGpbocCheckOut();
  	}
  else
  {
    //printf("\n错误，错误标志为:%d\n",orderFlag);
  }
  //PrintIndex2 = 0;
  //PrintBuf2[0] = 0xff;

  //Close_Szt_Gpboc();

  
}




/*银联签到,成功返回1，失败返回0*/
uint8_t Gpboc_CheckIn (void)
{
  uint8_t temp;
  
  uint8_t flag1 =0;
  uint8_t flag2 =0;
  uint8_t flag3 =0;
  uint8_t flag4 =0;
  
  uint8_t result_End = 0;
  
  Order_Ifn_QueryStatus();
  temp = 0xff;
  temp = ManageReOrderType();
  if( temp == R_QueryStatus)
	{
	  flag1 = ManageQueryStatus();
	}
  delay_us(50);

  if( flag1 == 0x01)
  	{
  	  Order_Gpboc_Init();
  	  temp = 0xff;
  	  temp = ManageReOrderType();
  	  if( temp == R_GpbocCheck_In)
	  	{
	  	  flag2 = ManageGpbocInit();
	  	}
  
  		//printf("\n银联初始化标志=%x",flag2);
  		delay_ms(1000);
  
  	  if(flag2 == 0x01)
	  	{
	  	  Order_Gpboc_SendData();
  		  delay_ms(1000);
  		  temp = 0xff;
  		  temp = ManageReOrderType();
  		  if( temp == R_GpbocSendData)
		  	{
		  	  flag3 = ManageGpbocSendData();
			  
		  	}
		  //printf("\n银联上传标志=%x",flag3);
  		  delay_ms(1000);
  		  if( flag3 == 0x01)
		  	{
		  	  Order_Gpboc_Check_Out();
  			  delay_ms(1000);
  			  temp = 0xff;
  			  temp = ManageReOrderType();
  			  if( temp == R_GpbocCheck_Out)
			  	{
			  	  flag4 = ManageGpbocCheckOut();
			  	}
		  	}
		   	//printf("\n银联签退标志=%x",flag4);
  		}

  	  delay_ms(1000);
  	  if( flag4 == 0x01)
	  	{
	  	  Order_Gpboc_Init();
  		  temp = 0xff;
  		  temp = ManageReOrderType();
  		  if( temp == R_GpbocCheck_In)
		  	{
		  	  result_End = ManageGpbocInit();
		  	}
	  	}
  	}

	return result_End;
  
  
}




/*银联只验卡不消费, 成功验卡返回1，失败返回0*/
uint8_t OnlyReadGpbocCard(void)
{
  uint8_t orderFlag1;
  uint8_t result1 =0;
  uint8_t endreturn =0;
  UpReadCardData.SztMoney =0;
  UpReadCardData.GpbocMoney =0;

  Order_Gpboc_ReadCard();
  orderFlag1 = ManageReOrderType();
  if( orderFlag1 == R_GpbocReadCard)
  	{
  	  result1 = ManageGpbocReadCard();
  	}

  if( result1 == 1)
  {
    endreturn =1;
  }

  return endreturn;
  
}


/*银联卡消费,返回0表示扣款失败；返回1表示扣款正确*/
uint8_t GpbocDeduct(uint32_t money_deduct)
{
  uint8_t orderFlag1 =0;
  uint8_t result1 =0;
  uint8_t result2 =0;
  int32_t FristMoney=0;
	uint8_t cnt_t=0,i=0;
  //初次验卡
  UpDeductData.lTransAmount = 0;	  //设扣款初始为0

	VariableChagelong(amountof_consumption,0);
	VariableChagelong(cardbalence_before,0);
	VariableChagelong(cardbalence_after,0);
	
loop5:	
	Order_Gpboc_ReadCard();
  orderFlag1 = ManageReOrderType();
  if( orderFlag1 == R_GpbocReadCard)
  {
  	result1 = ManageGpbocReadCard();
  }
  if( result1 == 0x01)          //验卡成功
  {
  	FristMoney = UpReadCardData.GpbocMoney;
    //在显示屏上显示刷卡前金额UpReadCardData.GpbocMoney
    VariableChagelong(cardbalence_before,UpReadCardData.GpbocMoney);	
    if( FristMoney < money_deduct )
    {
      //printf("卡内余额不足，请先充值\r\n");
      PlayMusic(VOICE_6);
      return 0;
    }
    else
    {
      orderFlag1 =0;
	  	Order_Gpboc_Deduct(money_deduct);
			for(i=0;i<20;i++)
			{
				orderFlag1 = ManageReOrderType();
				if(orderFlag1 == R_GpbocDeduct)
				{
					result2 = ManageGpbocDeduct();
					if( result2 == 1)   //付款成功
					{
						//printf("付款成功\r\n");
						//在显示屏上显示扣款金额与余额，UpDeductData.lTransAmount（扣款），UpDeductData.lCardBalance（余额）
						VariableChagelong(amountof_consumption,UpDeductData.lTransAmount);
						VariableChagelong(cardbalence_after,UpDeductData.lCardBalance);	
						delay_ms(1000);
						return 1;
					}
					else
					{
						//printf("付款失败\r\n");
						return 0;
					}
				}
      }  
    }
  }
  else
  {
	  cnt_t++;
		DealSeriAceptData();
		if(cardbalence_cancel_flag)
		{
			cardbalence_cancel_flag=false;
			return 0;
		}
	  if(cnt_t<30)
			goto loop5;
    return 0;
  }
	return 0;
 }
 
/*上送银联数据，成功上送返回1，失败返回0*/
uint8_t UpperGpboc(void)
{
  uint8_t orderFlag;
  uint8_t result1;
  uint8_t endresult =0;
  Order_Gpboc_SendData();
  orderFlag = ManageReOrderType();
  if( orderFlag == R_GpbocSendData)
  	{
  	  result1 = ManageGpbocSendData();
  	}
  else
  	{
  	  endresult= 0;  	  
  	}

  if(result1 == 1)
  	{
  	  //printf("\n上送银联数据成功\n");
  	  endresult =1;
  	}
  else
  	{
  	  //printf("\n上送银联数据失败\n");
  	  endresult =0;
  	}
  return endresult;
}
/*银联批结流程，成功批结返回1，失败返回0*/
uint8_t GpbocVout(void)
{
  uint8_t orderFlag;
  uint8_t result1;
  uint8_t endresult =0;
  Order_Gpboc_Check_Out();
  orderFlag = ManageReOrderType();
  if( orderFlag == R_GpbocCheck_Out)
  	{
  	  result1 = ManageGpbocCheckOut();
  	}
  else
  	{
  	  endresult= 0;  	  
  	}

  if(result1 == 1)
  	{
  	  //printf("\n银联批结、签退成功\n");
  	  endresult =1;
  	}
  else
  	{
  	  //printf("\n银联批结、签退失败\n");
  	  
  	  endresult =0;
  	}

  return endresult;
}


void TestSzt(uint8_t PutInNum)
{
  uint8_t orderFlag;

  if( PutInNum ==1 )
  {
    //printf("PutInNum =1");
    Order_Ifn_QueryStatus();
  }
  else if( PutInNum == 2)
  {
    //printf("PutInNum =2");
    Order_SztUserLogin(); //登录
  }
  else if( PutInNum == 3)
  {
    //printf("PutInNum =3");
    Order_SztTimeCheck(); //时间同步
  }
   else if( PutInNum == 4)
  {
    //printf("PutInNum =4");
    Order_SztAutoCheckIn();  //自主签到
  }
    else if( PutInNum == 5)
  {
    //printf("PutInNum =5");
    Order_SztAutoDownload();  //自主下载参数
  }
	else if( PutInNum == 6)
  {
    //printf("PutInNum =6");
    Order_SztAutoSendData();  //自主上传数据
  }
	else if( PutInNum == 7)
  {
    //printf("PutInNum =7");
    Order_SztUnlink();  //自主上传数据
  }
	else if( PutInNum == 8)
  {
    //printf("PutInNum =8");
    //Order_SztReadCardOnce();  //首次读卡
    Order_Gpboc_ReadCard();
  }
	else if( PutInNum == 9)
  {
    //printf("PutInNum =8");    
    Order_SztDeductOnce(1); //初次扣款
  }
	else if( PutInNum == 10)
  {
    //printf("PutInNum =8");    
    Order_SztDeductAgain(1); //初次扣款
  }
  else
  {
    //printf("\nOver");
  }
  
  
  orderFlag = ManageReOrderType();
  if( orderFlag == 0)
  	{
  	  //printf("\n错误，错误标志为:1\n");
  	}
  else if( orderFlag == R_QueryStatus)
  	{
  	  ManageQueryStatus();
  	}
  else if( orderFlag == R_UserLogin)
  	{
  	  ManageSztUserLogin();
  	}
  else if( orderFlag == R_TimeCheck)
  	{
  	  ManageSztTimeCheck();
  	}
  else if( orderFlag == R_SztCheck_In)
  	{
  	  ManageSztAutoManage();
  	}
  else if( orderFlag == R_SztUnlink)
  	{
  	  ManageSztUnlink();
  	}
  else if( orderFlag == R_GpbocReadCard)
  	{
  	  ManageGpbocReadCard();
  	}  
  else if( orderFlag == R_SztDeductOnce)
  	{
  	  ManageSztDeductOnce();
  	} 
  else if( orderFlag == R_SztDeductAgain)
  	{
  	  ManageSztDeductAgain();
  	} 
  else
  {
    //printf("\n错误，错误标志为:%d\n",orderFlag);
  }
}



/*深圳通签到，成功返回1，失败返回0*/
uint8_t Szt_CheckIn(void)
{
  uint8_t flag1 =0;//状态查询
  uint8_t flag2 =0;//时间校验
  uint8_t flag3 =0;//操作员登录
  uint8_t flag4 =0;//深圳通自主签到
  uint8_t flag5 =0;//深圳通自主下载参数
  uint8_t flag6 =0;//深圳通自主上传数据
  uint8_t flag7 =0;//总签到成功与否标志
  uint8_t temp =0xff;

  Order_Ifn_QueryStatus();
  delay_us(10);
  temp = ManageReOrderType();
  if( temp == R_QueryStatus)       //状态查询
  	{
  	  flag1 = ManageQueryStatus();	  
  	}

  if(flag1 == 1)
  	{
	/**/
    	Order_SztTimeCheck();        //时间校验
  	  delay_us(5);
  	  temp = 0xff;
  	  temp = ManageReOrderType();
  	  if( temp == R_TimeCheck )
  	  	{
  	  	  flag2 = ManageSztTimeCheck();
  	  	}
  	  
  	  Order_SztUserLogin();          //操作员登录
  	  delay_us(5);
  	  temp = 0xff;
  	  temp = ManageReOrderType();
  	  if( temp == R_UserLogin )
  	  	{
  	  	  flag3 = ManageSztUserLogin();
  	  	}

  	}
  
  if((flag1 == 1) && ( flag2 == 1) && ( flag3 ==1))
  	{
  	  Order_SztAutoCheckIn();              //深圳通自动签到
  	  temp = 0xff;
  	  temp = ManageReOrderType();
  	  if( temp == R_SztCheck_In)
  	  	{
  	  	  flag4 = ManageSztAutoManage();			  
  	  	}
  
  	  if( flag4 == 1)
  	  	{
  		  Order_SztAutoDownload();            //深圳通自动下载数据
  		  temp = 0xff;
  		  temp = ManageReOrderType();
  		  if( temp == R_SztCheck_In)
  		    {
  			  flag5 = ManageSztAutoManage();				
  		    }
  
  
  		  Order_SztAutoSendData();            //深圳通自动上传数据
  		  temp = 0xff;
  		  temp = ManageReOrderType();
  		  if( temp == R_SztCheck_In)
  		    {
  			  flag6 = ManageSztAutoManage();				
  		    }
  	  	}
  	}

  if( (flag1 ==1) && (flag2 ==1) && (flag3 ==1) && (flag4 ==1) && (flag5 ==1) && (flag6 ==1))
  	{
  	  flag7 = 1;  //全部内容都返回正确，签到过程才成功
  	}

  return flag7;
}


 

/*深圳通扣款*/
uint8_t SztDeduct(int32_t money)
{
  uint8_t temp ;
  uint8_t flag0 =0;
  uint8_t flag1 =0;
  uint8_t flag2 =0;
  uint8_t flag3 =0;
  uint8_t flag4 = 0;
  uint8_t endflag =0;
  uint8_t FristMoney =0;
  uint8_t EndMoney =0;
  int32_t TrueMoney=0;
	uint8_t cnt_t=0;
	uint8_t tempcount=0;

  SztReductInf.BeginMoney =0;
  SztReductInf.EndMoney =0;

	VariableChagelong(amountof_consumption,0);
	VariableChagelong(cardbalence_before,0);
	VariableChagelong(cardbalence_after,0);
	
	loop6:
  //初次验卡
  Order_Gpboc_ReadCard();
  temp = 0xff;
  temp = ManageReOrderType();
  if(temp == R_GpbocReadCard)
  {
  	flag0 = ManageGpbocReadCard();
  }
  if(flag0 == 0x01) //读卡成功
  {
    FristMoney = UpReadCardData.SztMoney;			
    //显示深圳通金额 UpReadCardData.SztMoney
	  VariableChagelong(cardbalence_before,UpReadCardData.SztMoney);
	  //EndMoney = FristMoney;
		if(UpReadCardData.SztMoney < money)// 余额不足
		{
			
		}
		else
		{
//			printf("深圳通初次扣款操作\r\n");
			Order_SztDeductOnce(money); //扣款
			delay_us(15);
				loop11:
			temp = 0xff;
			temp = ManageReOrderType(); 
			if(temp == R_SztDeductOnce) //初次扣款
			{
				flag1 = ManageSztDeductOnce();
				if( flag1 == 1) //扣款成功
				{
					flag3 = 1;
				}
				else if( flag1 == 0x52 )  //重操作扣款
				{
//					loop11:
//					printf("扣款重操作\r\n");
					Order_SztDeductAgain(money);
					delay_us(15);
					temp = 0xff;
					temp = ManageSztDeductAgain();
					if( temp == R_SztDeductAgain )
					{
						flag2 = ManageSztDeductAgain();
						if( flag2 == 1)
						{
							flag3 = 1;
						}
					}
				}
				
				if(flag3 ==1)  //扣款成功
				{
					VariableChagelong(amountof_consumption,money);  //显示扣款金额
					VariableChagelong(cardbalence_after,SztReductInf.EndMoney);  //显示扣款后余额
					delay_ms(1000);
					return 1;
				}
				else
				{
	//        tempcount++;
	//  			if(tempcount <10)
	//  			{
	//  				goto loop11;
	//  			}
				}
			}
			else			
			{
				tempcount++;
				if(tempcount <10)
				{
					goto loop11;
				}
			}
  	
      //
      
	  }
  }
	else
	{
		if(cardbalence_cancel_flag)
		{
			cardbalence_cancel_flag=false;
			return 0;
		}
    cnt_t++;
		DealSeriAceptData();
	  if(cnt_t<15) goto loop6;		
	}
  return endflag; 
}


/*深圳通上传数据，上传成功返回1，失败返回0*/
uint8_t SztAutoSend(void)
{
  uint8_t temp =0xff;
  uint8_t flag1 = 0x00;

  Order_SztAutoSendData();
  temp = ManageReOrderType();
  if( temp == R_SztCheck_In)
  	{
  	  flag1 = ManageSztAutoManage();	  
  	}

  return flag1;
}


/*读头总签到*/
void Szt_GpbocAutoCheckIn(void)
{
  uint8_t Szt_DiverInitFlag =0;
  uint8_t Gpboc_DiverInitFlag =0;
    
  if( Gpboc_DiverInitFlag ==0)
  {
    delay_us(500);
    Gpboc_DiverInitFlag = Gpboc_CheckIn();
  }
     
  
  if( Szt_DiverInitFlag ==0)
  {
    delay_us(500);
    Szt_DiverInitFlag = Szt_CheckIn();
  }
  

	if((Szt_DiverInitFlag == 1)&&(Gpboc_DiverInitFlag ==1))
	{
	  Flag_szt_gpboc_ok =1;
	}
		 
//	return true;
}

/*一个2位的BCD码转换成一个十进制码*/
uint8_t  BCDtoDec(uint8_t bcd)  
{  
     uint8_t i, tmp; 
   
     for(i=0; i<2; i++)  
     {  
        tmp = ((bcd>>4) & 0x0F)*10 + (bcd & 0x0F); 
     }  
   
     return tmp;  
}  
   


/*一个2位的十进制数转换成一个BCD码*/ 
uint8_t DectoBCD(uint8_t Dec)  
{  
     uint8_t temp;

	 temp = Dec/10*16 + Dec%10;
   
     return temp;  
}  






