#include "stm32f4xx.h"
#include "uart5.h"
#include "network_handle.h"
#include "printer.h"
#include "delay.h"
#include "rtc.h"
#include "stdio.h"
#include "string.h"
#include "MsgHandle.h"
#include "sd.h"
#include "uart6.h"
#include "serialscreen.h"



static long Batch = 0x00 ;//交易流水号

unsigned char  TID[7] = {0xa2,0x00,0x04,0x10,0x00,0x00,0x14}; /*终端TID码 10000006*/
unsigned char  BRWN[7+3] = {0xa6,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	 /*交易流水线*/
unsigned char  BNO[6] = {0xa7,0x00,0x03,0x00,0x00,0x00};               /*批次号*/
unsigned char  DeviceArea[3+3]={0xac,0x00,0x03,0x17,0x03,0x02};         /*终端所在区域编号*/
unsigned char  DeviceAreaNO[4+3]={0xad,0x00,0x04,0x17,0x03,0x02,0x07};   /*终端所在地域编号*/
unsigned char  DeviceStatus[2+3]={0xae,0x00,0x02,0xE0,0x10};	   /*终端状态*/
const unsigned char  DeviceTemperature[2+3]={0xdc,0x00,0x02,0x00,0x00};	   /*设备温度，最后的一个字节为温度*/
unsigned char  DealData[7]={0xa9,0x00,0x04,0x00,0x00,0x00,0x00};       /*交易日期*/
unsigned char  DealTime[6]={0xaa,0x00,0x03,0x00,0x00,0x00};       /*交易时间*/
unsigned char  MAC[8+3]={0xc9,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};   /*MAC*/
unsigned char  DealBalance[6+3]={0xb0,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};    /*交易金额*/
unsigned char  MealID[4+3] = {0xb1,0x00,0x04,0x10,0x00,0x00,0x20} ;                /*餐品ID  10000020*/
unsigned char  MealNO[1+3] = {0xb2,0x00,0x01,0x00};		   /*餐品购买数量*/
unsigned char  MealName[20+3] ={0xb3,0x00,0x14,'a','b','c','d','e','a','b','c','d','e','a','b','c','d','e','a','b','c','d','a'};  /*餐品名字*/
unsigned char  MealPrice[6+3]={0xbd,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};	  /*餐品价格*/
unsigned char  PayType[1+3]={0xbf,0x00,0x01,0x00};        /*支付方式*/
unsigned char  Change[6+3]={0xd7,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00} ;        /*找零金额*/
unsigned char  RemainMealNum[2+3]={0xd8,0x00,0x02,0x00,0x00};  /*剩余餐品数量*/
unsigned char  MName[20+3]={0xa4,0x00,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};             /*商户号MID*/
unsigned char  APPVersion[8+3]={ 0xc2,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};       /*应用程序版本*/
unsigned char  ParaFileVersion[8+3]={0xc3,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};  /*参数文件版本*/
unsigned char  BDataFileVersion[8+3]={0xc4,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};	/*业务数据文件版本*/
unsigned char  ChechStatus[2+3]={0xab,0x00,0x02,0x00,0x00};                                     /*自检状态*/
unsigned char  MID[3+3]={ 0xa3,0x00,0x03,0x10,0x00,0x01};                                       /*商户号MID*/
unsigned char  TTCount[3+2]     ={0xb7,0x00,0x02,0x00,0x00};	                                            /*交易总笔数 */
unsigned char  TNCT[6+3]        ={0xb8,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};                            /*交易总金额*/
unsigned char  TNtype[3+2]      ={0xd9,0x00,0x02,0x00,0x00};											    /*交易总产品数 d9*/
unsigned char  TotalChange[3+6] ={0xd7,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};							/*总找零金额 d7*/
unsigned char  TakeMealFlag[3+2]={0xdb,0x00,0x02,0x00,0x00};        /*取餐标志 0x01:成功 0x02:失败 */
unsigned char  UpdataFlag[4];
unsigned char  ACK[4];  //这个是什么?
unsigned char  WordKeyCipher[11];

Meal_struction Meal[MealKindTotoal]={
						    /*餐品数量*/	 /*餐品ID*/ 			     /*餐品名字*/				    /*餐品价格*/			     /*餐品类型*/
								   0x00,0x00,	 0x10,0x00,0x00,0x35,  {"秘制猪手饭      "},   0x00,0x00,0x16,0x00, 	{"C001"},
								   0x00,0x00,	 0x10,0x00,0x00,0x36,  {"潮氏卤鸡腿饭    "},   0x00,0x00,0x16,0x00, 	{"C001"},
								   0x00,0x00,	 0x10,0x00,0x00,0x37,  {"特色稻香肉饭    "},   0x00,0x00,0x16,0x00, 	{"C001"},
								   0x00,0x00,	 0x10,0x00,0x00,0x38,  {"黑脚猪扒饭      "},   0x00,0x00,0x16,0x00, 	{"C001"},
                   0x00,0x00,	 0x10,0x00,0x00,0x39,  {"蒲烧鲷鱼饭      "},   0x00,0x00,0x18,0x00, 	{"C001"},
								   0x00,0x00,	 0x10,0x00,0x00,0x40,  {"蒲烧秋刀鱼饭    "},   0x00,0x00,0x18,0x00, 	{"C001"},  
						   };
 unsigned char	Record_buffer[254] = {0} ;

 /*******************************************************************************
* Function Name  : GetBRWN
* Description    : 功能: 得到流水号
* Input          : CRC
* Output         : 数组以及大小
* Return         : CRC
*******************************************************************************/
void GetBRWN(void)
{
   /*时间作为流水号*/
  RTC_TimeShow();
	BRWN[3] =    20 /10 *16 + 20%10 ;	        /*年*/
	BRWN[4] =    TimeDate.Year /10 *16 +  TimeDate.Year%10 ;
	BRWN[5] =    TimeDate.Month /10 *16 + TimeDate.Month%10 ;
	BRWN[6] =    TimeDate.Date /10 *16 +  TimeDate.Date%10 ;
	BRWN[7] =    TimeDate.Hours /10 *16 + TimeDate.Hours%10 ;
	BRWN[8] =    TimeDate.Minutes /10 *16 + TimeDate.Minutes%10 ;
	BRWN[9] =    TimeDate.Senconds /10 *16 + TimeDate.Senconds%10 ;
}

/*******************************************************************************
* Function Name  : GetCrc16
* Description    : 进行CRC校验
* Input          : CRC
* Output         : 数组以及大小
* Return         : CRC
*******************************************************************************/
unsigned int GetCrc16(unsigned char *bufData,unsigned int sizeData)
{
  unsigned int Crc ,i = 0;
	unsigned char j = 0;
	if(sizeData == 0)
	  return 1 ;
	Crc = 0 ;
	for(i=0;i<sizeData;i++)
	{
	  Crc ^= bufData[i];
	  for(j=0;j<8;j++)
	  {
	    if(Crc&0x0001)
		  {
		    Crc >>=1 ;
		    Crc ^=0x08408 ;
		  }
		  else
		    Crc >>=1 ;
	  }
	}
	return Crc ;
}

/*---------------------------------------------------------------------------
void HL_IntToBuffer(const uint16_t int_v, unsigned char *ret_buf);
功能: 将一个 16bit 的整数转换成 Buffer 数据, 高位在前
高位在前
*/
void HL_IntToBuffer(const uint16_t int_v, unsigned char *ret_buf)
{
    ret_buf[0] = (unsigned char)(int_v >> 8);  // [0] 对应 --> 高位
    ret_buf[1] = (unsigned char)(int_v & 0xff);

}
/*---------------------------------------------------------------------------
 uint16_t HL_BufferToInt(const unsigned char *in_buf);
 功能: 将缓冲区数据转换成整数, 高位在前
 高位在前
*/
uint16_t HL_BufferToInit(const unsigned char *in_buf)
{
    uint16_t ret_n;
    ret_n = in_buf[0];           // 取高位
    ret_n = (ret_n << 8);
    ret_n += in_buf[1];          // 取低位
    return ret_n;
}

/*---------------------------------------------------------------------------
uint32_t HL_BufferToLong(const unsigned char *in_buf);
功能: 将 (unsigned char *) 数据转换成 Long 型, 高位在前
高位在前
*/
uint32_t HL_BufferToLong00(const unsigned char *in_buf)
{
    uint32_t ret_n;

    ret_n = in_buf[0];     // 最高位
    ret_n = (ret_n << 8);

    ret_n += in_buf[1];    // 次高位
    ret_n = (ret_n << 8);

    ret_n += in_buf[2];    // 高位
    ret_n = (ret_n << 8);

    ret_n += in_buf[3];    // 低位

    return ret_n;
}

/*---------------------------------------------------------------------------
void LH_LongToBuffer(uint32_t in_n, unsigned char *ret_buf);
功能: 将长整数转换成 unsigned char 型数据, 高位在前
*/
void HL_LongToBuffer00(const uint32_t in_n, unsigned char *ret_buf)
{
    ret_buf[0] = (unsigned char)((in_n >> 24) & 0xff);   // [0] 对应 --> 高位
    ret_buf[1] = (unsigned char)((in_n >> 16) & 0xff);
    ret_buf[2] = (unsigned char)((in_n >> 8) & 0xff);
    ret_buf[3] = (unsigned char)(in_n & 0xff);
}
 //---------------------------------------------------------------------------
// 功能: 将缓冲区清零
/*******************************************************************************
* Function Name  : mem_set_00
* Description    : 功能: 将缓冲区清零
* Input          : CRC
* Output         : 数组以及大小
* Return         : CRC
*******************************************************************************/
static void mem_set_00(unsigned char *dest, const long s_len)
{
    long j0;

    for(j0 = 0; j0 < s_len; j0++)  dest[j0] = 0;
}


//---------------------------------------------------------------------------
// 功能: 字符串拷贝
/*******************************************************************************
* Function Name  : mem_set_00
* Description    : 功能: 将缓冲区清零
* Input          : CRC
* Output         : 数组以及大小
* Return         : CRC
*******************************************************************************/
uint16_t mem_copy01(unsigned char *dest, const unsigned char *source, const long s_len)
{
  long j0;
  for(j0 = 0; j0 < s_len; j0++)  dest[j0] = source[j0];
  dest[j0++]=',';
  return (s_len+1);
}
uint16_t mem_copy00(unsigned char *dest, const unsigned char *source, const long s_len)
{
  long j0;
  for(j0 = 0; j0 < s_len; j0++)  dest[j0] = source[j0];
  return (s_len);
}

/*********************/
typedef struct
{
  unsigned char  Lenght[2] ;
  unsigned char  MealNO[35*4];    /*餐品具体数目*/

}ReturnData_Struction;
ReturnData_Struction  ReturnData;

  /*******************************************************************************
* Function Name  : CheckResponseCode
* Description    : 检查返回码看什么错误
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
unsigned char CheckResponseCode(unsigned char Cmd)
{
// unsigned char  i = 0 ;
  switch(Cmd)
  {
		case 0x00 : /*交易成功*/
			return 0 ;
		case 0x01 : /*包CRC错误*/
			return 1 ;
		case 0x02 : /*包CRC错误*/
				break;
		case 0x03 : /*无效的终端类型*/
				break  ;
		case 0x04 : /*协议版本错误*/
				break ;
		case 0x05 : /*无效的终端SN*/
				break;
		case 0x06 : /*无效的商户号(MID)*/
				break;
		case 0x07 :	/*无效文件偏移地址*/
				break ;
		case 0x08 : /*无效文件偏移地址*/
				break ;
		case 0x09 : /*其他包内容错误*/
				break;
		case 10 :	/*商户号与设备号对应关系错误*/
				break ;
		case 11 : /*餐品数据对比不一致*/
					 {
					 }
				break ;
		case 12 : /*后台不存在设备区域ID、所在点ID信息*/
				break;
		case 13 : /*设备上送的区域ID、所在点ID信息与后台不匹配*/
				break ;
		case 14 : /*设备上送餐品信息格式不正确*/
				break ;
		case 15 :  /*取餐交易（批次号、流水号）重复*/
				break;
		case 16 : /*终端上送的餐品数据里某餐品信息不存在于后台数据库*/
				break;
		case 17 : /*批上送日志格式不正确*/
				break;
		case 20 :  /*无效金额*/
				break;
		case 21 : /*原始金额错误*/
				break;
		case 22 : /*无效餐品ID号*/
				break;
		case 23 : /*购买数量无效*/
				break ;
		case 0x24 : Batch ++ ;
				break;
		case 30: /*密码错*/
				 break ;
		case 31 : /*账户内余额不足*/
				 break;
		case 40 : /*锁定终端*/
				 break;
		case 41 : /*解锁终端*/
				 break ;
		case 50 : /*后台文件版本不存在*/
				 break;
		case 51 :/*请求文件版本与后台发布的文件版本不一致*/
					break ;
		case 96 : /*后台系统错误*/
					break ;
		default :break;
  }
	return 3; /*后来加的防止有waring*/
}
 /*******************************************************************************
* Function Name  : GetData
* Description    : 得到相应命令的数据
* Input          : unsigned char *dest,unsigned char *souce,const long s_len,unsigned char Cmd
* Output         : void
* Return         : void
*******************************************************************************/
long  GetData(unsigned char *dest,unsigned char *souce, long s_len,unsigned char Cmd)
{
	long i = 0 ,j=0,Cmdlenght=0;
	for(i=0;i<s_len;i++)
	{
	  if(souce[i] == Cmd)	 /*得到命令*/
	  {
		  Cmdlenght= HL_BufferToInit(&souce[i+1]);    /*得到数据长度*/
//		printf("Cmdlenght=%d\r\n",Cmdlenght);
		  for(j=0;j<Cmdlenght;j++)				      /*得到数据  */
		  {
			  dest[j] = souce[i+j+3] ;
//			printf("dest[%d]=%x\r\n",j,dest[j]);
		  }
		  return Cmdlenght  ;
		}
	}
	return 0 ;
}
 /*******************************************************************************
* Function Name  : CmdDataSend
* Description    : 发送数据函数
* Input          : unsigned char *p  long Lenght
* Output         : void
* Return         : void       ok
*******************************************************************************/
static void CmdDataSend(unsigned char *p,long Lenght)
{
  uint16_t i=0;
	 /*初始化*/
 	F_RX1_SYNC = 0;
	rx1BufIndex = 0;
	F_RX1_VAILD = 1;
	CrcValue = 0;
	F_RX1_Right = 0 ;
	for(i=0;i<Lenght;i++)
	{
	  UART5->DR = (u8) p[i];
    while((UART5->SR&0X40)==0);//循环发送,直到发送完毕
	  //delay_ms(5);
	  //printf(" p[%d]=%x\r\n", i,p[i]);
	}
}

unsigned char  MealComparefunDemo(long Cmd ,unsigned char *p,unsigned long lenght)
{
  uint16_t  CRCValue= 0 ;
// static  int Bno = 0 ;
  unsigned char Waittimeout = 250 ;
  /*给数组赋值命令*/
  HL_IntToBuffer(Cmd,&p[1]);
  /*数组长度*/
  HL_IntToBuffer(lenght-8,&p[3]);
  /*得到数据的CRC*/
  CRCValue=GetCrc16(&p[1],lenght-4);
  /*把CRC赋值到数组中*/
  HL_IntToBuffer(CRCValue,&p[lenght-2]);
  /*发送命令*/
  CmdDataSend(p,lenght);	//发送数据
  /*等待数据返回*/
  while(--Waittimeout)
	{
		delay_ms(10);
		if(F_RX1_Right) //接收到有限数据CRC正确
		{
		  F_RX1_Right = 0 ;
		  break;
		}
  }
  if(Waittimeout == 0 )
		return 1 ;//超时返回1
  return 0 ;
}


/*******************************************************************************
发送签到后，返回给终端的结构体数据
*******************************************************************************/
/*******************************************************************************
* Function Name  : 签到 	 0X0100
* Description	 :签到
* Input 		 : void
* Output		 : void
* Return		 : char
* Time	       :2014-4-7  MrLao  ok
*******************************************************************************/

unsigned char  SignInFun(void)
{
	unsigned char i = 0 ;
	 long  Command = 0x0100 ;
	 long  Lenght = 0,j = 0  ;
	 long  CmdLenght = 0 ;  //命令长度
	 unsigned char  Send_Buf[400]={0};
	 mem_set_00(rx1Buf,sizeof(rx1Buf));	 //数据清零

	/* 命令包 ++*/
	Send_Buf[0] =  0x02 ;
	Send_Buf[1] =  0x00 ;
	Send_Buf[2] =  0x00 ;
	Send_Buf[3] =  0x00 ;
	Send_Buf[4] =  0x00 ;
	CmdLenght = 5 ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));      /*终端的TID*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MID,sizeof(MID));  /*商户号TID 命令(1)+数据长度(2)+数据     100001*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MName,sizeof(MName));  /*商户名称*/
	GetBRWN();														 /*得到流水号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));   /*交易流水号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));     /*批次号*/
  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceArea,sizeof(DeviceArea));  /*终端所在区域编号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceAreaNO,sizeof(DeviceAreaNO)); /*终端所在地域编号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],APPVersion,sizeof(APPVersion)); /*应用程序版本号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],ParaFileVersion,sizeof(ParaFileVersion)); /*参数文件版本号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BDataFileVersion,sizeof(BDataFileVersion)); /*业务数据文件版本号*/
  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],ChechStatus,sizeof(ChechStatus)); /*自检状态*/
 	Send_Buf[CmdLenght] = 0x03 	;
 	CmdLenght+=0x03;
	 /*发送命令*/
     i = MealComparefunDemo(Command,Send_Buf,CmdLenght);
	  /***************************************************************************/
 	if(i == 0x01)	 //
 	 return 1;
	Lenght = HL_BufferToInit(&rx1Buf[2]);	 //判断数据长度是否
	if(Lenght == 0x00)
	 return  1 ;
//	 if(rx1Buf[7]!=0x00)/*是否应答命令*/
//	 return 1 ;
//     for(j=0;j<Lenght;j++)
//	 {
//	  CheckInitReturnUnion.CheckInitReturnBuffer[j]= rx1Buf[j];
//	  printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
//	 }
//	 printf("\r\nACK: ");
//	 for(i=0;i<4;i++)
//	 {
//	 ACK[i] = rx1Buf[i+0x46];//得到
//	 printf("%x ",ACK[i]);
//	 }
//	 if(ACK[3]!=0x00)
//	 {
//	   return 1 ;
//	 }


//	 printf("\r\nMName: ");
	 for(i=0;i<23;i++)
	 {
	   MName[i] = rx1Buf[i+4];//得到商户
//	 printf("%x ",MName[i]);
	 }
//	 printf("\r\nBNO: ");
	 for(i=0;i<3;i++)
	 {
	   BNO[i+3] = rx1Buf[i+27+3];//得到批次号
//	 printf("%x ",BNO[i+3]);
	 }
//	 printf("\r\nAPPVersion: ");
	 for(i=0;i<11;i++)
	 {
	   APPVersion[i] = rx1Buf[i+0x21];//得到批次号
//	 printf("%x ",APPVersion[i]);
	 }
//	 printf("\r\nParaFileVersion: ");
	 for(i=0;i<11;i++)
	 {
	   ParaFileVersion[i] = rx1Buf[i+0x2c];//得到批次号
//	 printf("%x ",ParaFileVersion[i]);
	 }
	 //printf("\r\nBDataFileVersion: ");
	 for(i=0;i<11;i++)
	 {
	   BDataFileVersion[i] = rx1Buf[i+0x37];//得到批次号
//	 printf("%x ",BDataFileVersion[i]);
	 }
//	 printf("\r\nUpdataFlag: ");
	 for(i=0;i<4;i++)
	 {
	   UpdataFlag[i] = rx1Buf[i+0x42];//得到批次号
//	 printf("%x ",UpdataFlag[i]);
	 }

//	 printf("\r\nACK: ");
	 for(i=0;i<4;i++)
	 {
	   ACK[i] = rx1Buf[i+0x46];//得到
//	 printf("%x ",ACK[i]);
	 }
//
 //    printf("\r\nWordKeyCipher: ");
	 for(i=0;i<11;i++)
	 {
	   WordKeyCipher[i] = rx1Buf[i+0x4a];//得到批次号
//	 printf("%x ",WordKeyCipher[i]);
	 }

//	 printf("\r\nMAC: ");
   for(i=0;i<11;i++)
	 {
	   MAC[i] = rx1Buf[i+0x55];//得到工作密文
//	 printf("%x ",MAC[i]);
	 }
   if(ACK[3]==0x24)
	 {
	   return 1 ;
	 }
   return 0 ;
}

void StructCopyToBuffer(unsigned char *dest)
{
  long j0=0,i=0,k=0;
//上传4份餐品的数据
  for(j0 = 0; j0 < 4; j0++)
	{
	  for(i=0;i<4;i++)
	  dest[k++]=Meal[sell_type[j0]].MealID[i];
		
	  for(i=0;i<20;i++)
	  dest[k++]=Meal[sell_type[j0]].MaelName[i];

		dest[k++]= Meal[sell_type[j0]].MealNum[0]; //第一个数为0
	  dest[k++]= DefineMeal[sell_type[j0]].MealCount; //第二个数为餐品的数量

	  for(i=0;i<4;i++)
	  dest[k++]=Meal[sell_type[j0]].MealPreace[i];

	  for(i=0;i<4;i++)
	  dest[k++]=Meal[sell_type[j0]].MealType[i];
	}
}

/*******************************************************************************
* Function Name  : 餐品数据对比		 0X0200
* Description    :餐品数据对比
* Input          : void
* Output         : void
* Return         : char
* Time          :2014-4-7  MrLao   数据域采用TVL格式
*******************************************************************************/
unsigned char k = 0 ;
MealCompareDataStruct MealCompareData;
uint32_t  MealDataCompareFun(void)
{
  
 	unsigned char i= 0 ;
  long  Lenght = 0 ,j;
//	unsigned char MealID = 0 ;
	long  CmdLenght = 0 ;
	unsigned char status = 0 ;
	unsigned char Send_Buf[400];
	unsigned char TempBuffer[35*4]={0};
	unsigned char Buffer[306]={0};
	mem_set_00(rx1Buf,sizeof(rx1Buf));

	Send_Buf[0] =  0x02 ;
	Send_Buf[1] =  0x00 ;
	Send_Buf[2] =  0x00 ;
	Send_Buf[3] =  0x00 ;
	Send_Buf[4] =  0x00 ;
	CmdLenght = 5 ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));  /*终端的TID*/
	GetBRWN(); /*得到水流号*/
  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));                   /*交易流水线*/
  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));                     /*批次号*/
  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceArea,sizeof(DeviceArea));       /*终端所在区域编号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceAreaNO,sizeof(DeviceAreaNO));   /*终端所在地域编号*/
 	Send_Buf[CmdLenght] =  0xbc ;	 //T  餐品数据对比， 数据域采用TVL格式
	Send_Buf[CmdLenght+1] =  0x01 ;	 //L 数据长度
	Send_Buf[CmdLenght+2] =  0x32 ;	  //V 数据内容
	CmdLenght += 3;
	StructCopyToBuffer(Buffer);
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Buffer,sizeof(Buffer));

	Send_Buf[CmdLenght] = 0x03 	;
	CmdLenght+=0x03;

	/*发送命令*/
  i = MealComparefunDemo(0x0200,Send_Buf,CmdLenght);

/***************************************************************************/
  MealCompareData.MealCompareTotoal= 0; 
    /*单片是否成功*/
	if(i==0x01) //超时
	{		
		return MealCompareData.MealCompareTotoal; 
	}
	
	Lenght = HL_BufferToInit(&rx1Buf[2]);		//得到数据长度
	GetData(&ReturnData.Lenght[0],rx1Buf,Lenght,0xc0);	  /*返回状态*/
	
 	//printf("Status=%x\r\n",ReturnData.Lenght[0]);

	if(ReturnData.Lenght[0] == 0x00 )
	{
		MealCompareData.MealCompareTotoal= 0xFFFFFFFF;	
	    return MealCompareData.MealCompareTotoal;	 /*数据正确*/
	} 
	if(ReturnData.Lenght[0] == 0x24 )
	{
		MealCompareData.MealCompareTotoal= 0xFFFFFFFF;	
	    return MealCompareData.MealCompareTotoal;	 /*数据正确*/
	}
	CmdLenght = GetData(TempBuffer,rx1Buf,Lenght,0xBC);/*餐品对比*/
  //printf("StatusCmdLenght=%x\r\n",CmdLenght);
	if(CmdLenght>34)
	{
	  status  = CmdLenght / 35  ;
		for(i=0;i<4;i++)
		{
			if(rx1Buf[45+i*35]==0x04)   //餐品对比标志
			{
				MealCompareData.MealComparePart[i]=0xFF;
			}					
			else
			{
				MealCompareData.MealComparePart[i]=rx1Buf[36+i*35];
		  }				
		}
		return MealCompareData.MealCompareTotoal ;/*餐品对比信息*/
	}
  MealCompareData.MealCompareTotoal=0;
  return MealCompareData.MealCompareTotoal;
}



/*******************************************************************************
* Function Name  : 2.3.3 签退		 0X0300
* Description    :退签函数
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
unsigned char SignOutFun()
{

 	unsigned char i = 0 ;
  long  Lenght = 0 ,j;
	long 	CmdLenght = 0 ;
	unsigned char 	  Send_Buf[400];

  mem_set_00(rx1Buf,sizeof(rx1Buf));
  /*水流号++*/
	Send_Buf[0] =  0x02 ;
	Send_Buf[1] =  0x00 ;
	Send_Buf[2] =  0x00 ;
	Send_Buf[3] =  0x00 ;
	Send_Buf[4] =  0x00 ;
	CmdLenght = 5 ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));  /*终端的TID*/
	GetBRWN(); /*得到水流号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));  /*流水号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));  /*批次号*/
	Send_Buf[CmdLenght] = 0x03 	;
	CmdLenght+=0x03;
  i = MealComparefunDemo(0x0300,Send_Buf,CmdLenght);

	if(i == 0x01)
	return 1 ;
	 Lenght = HL_BufferToInit(&rx1Buf[2]);
	 for(j=0;j<Lenght+7;j++)
	 {
//	 printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	 }
   return 0 ;
}

 /*******************************************************************************
* Function Name  : 状态上传函数		 0X0400
* Description    :状态函数
* Input          : void
* Output         : void
* Return         : void         ok
*******************************************************************************/
unsigned char StatusUploadingFun(uint16_t erro_status)
 {

	 unsigned char i = 0 ;
	 long  Lenght = 0 ,j;
	 long	 CmdLenght = 0 ;
   char Temperature_t=0;
   char  Temperature_tt[5]={0};
	 unsigned char	Send_Buf[400]={0};
	 char  state_temp[2]={0};  
	 //sprintf(state_temp,"%x",erro_status); 
	 mem_set_00(rx1Buf,sizeof(rx1Buf));
	 /*水流号++*/
	 Send_Buf[0] =	0x02 ;
	 Send_Buf[1] =	0x00 ;
	 Send_Buf[2] =	0x00 ;
	 Send_Buf[3] =	0x00 ;
	 Send_Buf[4] =	0x00 ;
	 CmdLenght = 5 ;
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));	/*终端的TID*/
	 GetBRWN(); /*得到水流号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));  /*流水号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));	/*批次号*/
	 
   state_temp[1]=(erro_status&0x00ff);
	 state_temp[0]=(erro_status>>8)&0x00ff;
	 for(i=0;i<3;i++) 
	 {
		 DeviceStatus[3+i]= state_temp[i];
	 }
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceStatus,sizeof(DeviceStatus));  /*终端的状态*/
   Temperature_t= Temperature;
   state_temp[1]=(Temperature_t&0x00ff);
	 state_temp[0]=(Temperature_t>>8)&0x00ff;
   memcpy(Temperature_tt,DeviceTemperature,sizeof(DeviceTemperature));
	 for(i=0;i<3;i++) 
	 {
		 Temperature_tt[3+i]= state_temp[i];
	 }
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Temperature_tt,sizeof(Temperature_tt));  /*终端的状态*/
	 Send_Buf[CmdLenght] = 0x03  ;
	 CmdLenght+=0x03;
	 i = MealComparefunDemo(0x0400,Send_Buf,CmdLenght);//0x0400 状态上送
	 if(i == 0x01)
	 return 1 ;
	 Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	 for(j=0;j<Lenght+7;j++)
	 {
  
 //   printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);

	 }
   return 0 ;

 }

 /*******************************************************************************
* Function Name  : 状态上传函数		 0X0400
* Description    :状态函数
* Input          : void
* Output         : void
* Return         : void         ok
*******************************************************************************/
unsigned char TemperatureUploadingFun(uint8_t Temperature_t)
 {

	 unsigned char i = 0 ;
	 long  Lenght = 0 ,j;
	 long	 CmdLenght = 0 ;
	 unsigned char	Send_Buf[400]={0};
	 char  state_temp[2]={0};
   char  Temperature_tt[5]={0};
	 //sprintf(state_temp,"%x",erro_status); 
	 mem_set_00(rx1Buf,sizeof(rx1Buf));
	 /*水流号++*/
	 Send_Buf[0] =	0x02 ;
	 Send_Buf[1] =	0x00 ;
	 Send_Buf[2] =	0x00 ;
	 Send_Buf[3] =	0x00 ;
	 Send_Buf[4] =	0x00 ;
	 CmdLenght = 5 ;
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));	/*终端的TID*/
	 GetBRWN(); /*得到水流号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));  /*流水号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));	/*批次号*/ 
   state_temp[1]=(Temperature_t&0x00ff);
	 state_temp[0]=(Temperature_t>>8)&0x00ff;
   memcpy(Temperature_tt,DeviceTemperature,sizeof(DeviceTemperature));
	 for(i=0;i<3;i++) 
	 {
		 Temperature_tt[3+i]= state_temp[i];
	 }
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Temperature_tt,sizeof(Temperature_tt));  /*终端的状态*/
	 Send_Buf[CmdLenght] = 0x03  ;
	 CmdLenght+=0x03;
	 i = MealComparefunDemo(0x0400,Send_Buf,CmdLenght);//0x0400 状态上送
	 if(i == 0x01)
	 return 1 ;
	 Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	 for(j=0;j<Lenght+7;j++)
	 {
  
 //   printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);

	 }
   return 0 ;

 }
 /*******************************************************************************
* Function Name  : 回响测试	 0X0700
* Description    :回响测试
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
unsigned char EchoFun(void)
{

 	unsigned char i = 0 ;
	long  Lenght = 0 ,j;
	long 	CmdLenght = 0 ;
	unsigned char 	  Send_Buf[400];
	mem_set_00(rx1Buf,sizeof(rx1Buf));
    /*水流号++*/
	Send_Buf[0] =  0x02 ;
	Send_Buf[1] =  0x00 ;
	Send_Buf[2] =  0x00 ;
	Send_Buf[3] =  0x00 ;
	Send_Buf[4] =  0x00 ;
	CmdLenght = 5 ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));  /*终端的TID*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DealData,sizeof(DealData));  /*终端的TID*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DealTime,sizeof(DealTime));  /*终端的TID*/
	Send_Buf[CmdLenght] = 0x03 	;
	CmdLenght+=0x03;
  i = MealComparefunDemo(0x0700,Send_Buf,CmdLenght);
	if(i == 0x01)  // 返回数据超时
	  return 1 ;

	USART_ITConfig(UART5,USART_IT_RXNE,DISABLE);
  Lenght = HL_BufferToInit(&rx1Buf[2]) ;

	if(Lenght == 0x00)
	{
	  USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
	  return 1 ;
	}
	for(j=0;j<Lenght+7;j++)
	{
//	printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	}
	 if(rx1Buf[0]==0x07 && rx1Buf[1]==0x10)	 //表示正确
	 {
		 //  tmp_yy  = rx1Buf[7]/16*10+rx1Buf[7]%16 ;/*年*/
		   tmp_yy  = rx1Buf[8]/16*10+rx1Buf[8]%16 ;/*年*/
//		   printf("tmp_yy=%d\r\n",tmp_yy);
		   tmp_m2  = rx1Buf[9]/16*10+rx1Buf[9]%16 ;/*月*/
//		   printf("tmp_yy=%d\r\n",tmp_m2);
		   tmp_dd  = rx1Buf[10]/16*10+rx1Buf[10]%16 ;/*日*/
//		   printf("tmp_dd=%d\r\n",tmp_dd);

		   tmp_hh  = rx1Buf[14]/16*10+rx1Buf[14]%16 ;/*时*/
		   tmp_mm  = rx1Buf[15]/16*10+rx1Buf[15]%16 ;/*分*/
		   tmp_ss  = rx1Buf[16]/16*10+rx1Buf[16]%16 ;/*秒*/

		   rx1Buf[7]  = rx1Buf[7]/16*10+rx1Buf[7]%16 ;/*年*/
		   rx1Buf[8]  = rx1Buf[8]/16*10+rx1Buf[8]%16 ;/*年*/
		   rx1Buf[9]  = rx1Buf[9]/16*10+rx1Buf[9]%16 ;/*月*/
		   rx1Buf[10]  = rx1Buf[10]/16*10+rx1Buf[10]%16 ;/*日*/

		   rx1Buf[14]  = rx1Buf[14]/16*10+rx1Buf[14]%16 ;/*时*/
		   rx1Buf[15]  = rx1Buf[15]/16*10+rx1Buf[15]%16 ;/*分*/
		   rx1Buf[16]  = rx1Buf[16]/16*10+rx1Buf[16]%16 ;/*秒*/
//		   for(i=0;i<4;i++)
//		   printf("rx1Buf[%d]=%d\r\n",i,rx1Buf[i+7]);

//		   for(i=0;i<3;i++)
//		   printf("rx1Buf[%d]=%d\r\n",i,rx1Buf[i+14]);

		 USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
	  return 0 ;
	 }

	 	 USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
    return 1 ;

}


 /*******************************************************************************
* Function Name  : 取餐发送数据  OK
* Description    : 取餐发送数据
* Input          : void
* Output         : void
* Return         : void
    08 10 00 0f c0 00 01 24 c9 00 08 00 00 00 00 00 00 00 00 03 e8 47
   //应答码 2位          MAC 8位
*******************************************************************************/

CustomerSel__struction CustomerSel;
unsigned char TakeMealsFun(unsigned char *SendBuffer,unsigned char takeout_flag)
 {
	 unsigned char i = 0 ;
	 long  Lenght = 0 ,j=0;
	 long  CmdLenght = 0 ;
	 unsigned char	   Send_Buf[1024];	//要发送的数据啦
	 mem_set_00(rx1Buf,sizeof(rx1Buf));
	 /*水流号++*/
	 Send_Buf[0] =	0x02 ;
	 Send_Buf[1] =	0x00 ;
	 Send_Buf[2] =	0x00 ;
	 Send_Buf[3] =	0x00 ;
	 Send_Buf[4] =	0x00 ;
	 CmdLenght = 5 ;
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));	/*终端的TID*/
	 GetBRWN();
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));  /*流水号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));	/*批次号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceArea,sizeof(DeviceArea));  /*终端所在区域编号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceAreaNO,sizeof(DeviceAreaNO)); /*终端所在地域编号*/
	 for(i=0;i<6;i++)
	 {
	   DealBalance[3+i] = CustomerSel.DealBalance[i] ;
	 }
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DealBalance,sizeof(DealBalance)); /*交易金额(支付金额) */

	 for(i=0;i<4;i++)
	 {
	  //这里赋值餐品的ID
	   MealID[3+i] = CustomerSel.MealID[i] ; //(10000020)
	 }
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealID,sizeof(MealID)); /*餐品ID*/

	 /*这里赋值餐品的ID*/
	 MealNO[3] = CustomerSel.MealNo;
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealNO,sizeof(MealNO)); /*餐品购买数量*/

	  //这里赋值餐品的名字
	  for(j=0;j<20;j++)
	  MealName[3+j]=Meal[CustomerSel.MealName-1].MaelName[j];

	  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealName,sizeof(MealName));			  /*餐品名字*/

	 /*这里赋值餐品的价格*/
	 for(i=0;i<6;i++)
	 MealPrice[3+i] = CustomerSel.MealPrice[i] ;

	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealPrice,sizeof(MealPrice));			/*餐品价格*/
	/*付钱的方式*/
	 PayType[3] = CustomerSel.PayType ;
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PayType,sizeof(PayType));				/*支付方式*/
	 /*找零金额*/
	 for(i=0;i<6;i++)
	 Change[3+i] = CustomerSel.Change[i] ;
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Change,sizeof(Change)); 			   /*找零金额*/
	 /*剩余餐品数量*/
	 for(i=0;i<2;i++)
	 {
	  RemainMealNum[3+i] = CustomerSel.RemainMealNum[i] ;
    //printf("CustomerSel.RemainMealNum[i]=%d\r\n",CustomerSel.RemainMealNum[i]);
	 }
	   CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],RemainMealNum,sizeof(RemainMealNum));  /*剩余餐品数量*/
	   TakeMealFlag[4]= takeout_flag;
	   CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TakeMealFlag,sizeof(TakeMealFlag)); /*取餐标记*/
	   CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MAC,sizeof(MAC));					  /*MAC*/
 //  if(UserAct.PayType == '2' )																		/* 表示如果是刷卡*/
 //  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],STATUS.PacketData,STATUS.DataLength-17);				 /*记录刷卡信息*/
 //  if(UserAct.PayType == '3')
 //  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],STATUS.PacketData,STATUS.DataLength-17);				 /*记录刷卡信息*/

	 Send_Buf[CmdLenght] = 0x03  ;  //包含数据包包尾标识和CRC校验码
	 CmdLenght+=0x03;
   i = MealComparefunDemo(0x0800,Send_Buf,CmdLenght); //发送数据(取餐交易)
   mem_copy00(SendBuffer, Send_Buf,CmdLenght);	//把发送的参数传入数组写入SD卡
 /***************************************************************************/
	 HL_IntToBuffer(CmdLenght,&SendBuffer[1019]);	/*保存数据的长度*/

	 if(i == 0x01) /*判断是否超时*/
	 return 1 ;
	 Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	 if(Lenght == 0x00)/*判断数据长度*/
	 return 1 ;
	 for(j=0;j<Lenght+7;j++)
	 {
     //printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	 }
	 if(rx1Buf[0]==0x07 && rx1Buf[1]==0x10)  //表示正确
	 {
		 if(rx1Buf[7]==0x00)/*是否应答命令*/
		 {
			  return 0 ;
		 }
		 if(rx1Buf[7]==0x24)/*交易流水号（终端流水）重复*/
		 {
		 //   Batch ++ ;	/*流水号自加*/
			 return 1 ;
		 }
	 }
   return 0 ;
}

 /*******************************************************************************
* Function Name  : MealUploadingFun		 0X0300
* Description    : 餐品上送
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
unsigned char MealUploadingFun(void)
{
  unsigned char i = 0 ;
	long  Lenght = 0 ,j=0;
	unsigned char 	  Send_Buf[400];
	long  CmdLenght = 0 ;
	/*接受缓冲区清0*/
	mem_set_00(rx1Buf,sizeof(rx1Buf));
	/*水流号++*/
	Send_Buf[0] =  0x02 ;
	Send_Buf[1] =  0x00 ;
	Send_Buf[2] =  0x00 ;
	Send_Buf[3] =  0x00 ;
	Send_Buf[4] =  0x00 ;
	CmdLenght = 5 ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));  /*终端的TID*/

	  GetBRWN();
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));  /*流水号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));  /*批次号*/
  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceArea,sizeof(DeviceArea));  /*终端所在区域编号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceAreaNO,sizeof(DeviceAreaNO)); /*终端所在地域编号*/
	 /*
	  这里添加没有上传的产品名字
	 */
  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MAC,sizeof(MAC)); /*MAC*/
	Send_Buf[CmdLenght] = 0x03 	;
	CmdLenght+=0x03;
	i = MealComparefunDemo(0x0400,Send_Buf,CmdLenght);
	if(i == 0x01)
	  return 1 ;
  Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	for(j=0;j<Lenght+7;j++)
	{
//	printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	}
	return 0 ;
 }



 /*******************************************************************************
* Function Name  : MealGroupUploadingFun		 披上送
* Description    : 餐品披上送
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/

typedef struct
{
  unsigned char  BRWN[3];           /*交易流水线*/
  unsigned char  MealID[4];         /*餐品ID*/
  unsigned char  DealBalance[6];    /*交易金额*/
  unsigned char  MealNO;		    /*餐品购买数量*/
  unsigned char	 RemainMealNum[2];  /*剩余餐品数量*/
  unsigned char  MealPrice[6];	    /*餐品价格*/
  unsigned char  PayType;           /*支付方式*/
  unsigned char  Change[6];         /*找零金额*/
  unsigned char  TimeData[7];		/*购买的时间*/
}MealGroup_struction;
 MealGroup_struction 	 MealGroup[9]={		/*交易流水线*/ /*餐品ID*/		  /*交易金额*/            /*餐品购买数量*/    /*剩余餐品数量*/		/*餐品价格*/	  /*支付方式*/			/*找零金额*/			  /*购买的时间*/
                                         {0x00,0x00,0x01, 0x10,0x00,0x00,0x20, 0x00,0x00,0x00,0x00,0x00,0x00,  0x00,   0x00,0x00,   0x00,0x00,0x00,0x00,0x15,0x00,  0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00},
                                       	 {0x00,0x00,0x02, 0x10,0x00,0x00,0x21, 0x00,0x00,0x00,0x00,0x00,0x00,  0x00,    0x00,0x00,  0x00,0x00,0x00,0x00,0x15,0x00,  0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00},
										 {0x00,0x00,0x03, 0x10,0x00,0x00,0x22, 0x00,0x00,0x00,0x00,0x00,0x00,  0x00,    0x00,0x00,  0x00,0x00,0x00,0x00,0x15,0x00,  0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00},
										 {0x00,0x00,0x04, 0x10,0x00,0x00,0x23, 0x00,0x00,0x00,0x00,0x00,0x00,  0x00,    0x00,0x00,  0x00,0x00,0x00,0x00,0x20,0x00,  0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00},
										 {0x00,0x00,0x05, 0x10,0x00,0x00,0x24, 0x00,0x00,0x00,0x00,0x00,0x00,  0x00,    0x00,0x00,  0x00,0x00,0x00,0x00,0x20,0x00,  0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00},
										 {0x00,0x00,0x06, 0x10,0x00,0x00,0x25, 0x00,0x00,0x00,0x00,0x00,0x00,  0x00,    0x00,0x00,  0x00,0x00,0x00,0x00,0x20,0x00,  0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00},
										 {0x00,0x00,0x07, 0x10,0x00,0x00,0x26, 0x00,0x00,0x00,0x00,0x00,0x00,  0x00,    0x00,0x00,  0x00,0x00,0x00,0x00,0x25,0x00,  0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00},
										 {0x00,0x00,0x08, 0x10,0x00,0x00,0x27, 0x00,0x00,0x00,0x00,0x00,0x00,  0x00,    0x00,0x00,  0x00,0x00,0x00,0x00,0x25,0x00,  0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00},
										 {0x00,0x00,0x09, 0x10,0x00,0x00,0x28, 0x00,0x00,0x00,0x00,0x00,0x00,  0x00,    0x00,0x00,  0x00,0x00,0x00,0x00,0x25,0x00,  0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,0x00},
									    };

void StructCopyToBuffer00(unsigned char *dest,unsigned char index)
{

  long i=0,k=0;
  for(i=0;i<3;i++)
	{
	  dest[k++]=MealGroup[index].BRWN[i];
	}
	for(i=0;i<4;i++)
	{
	  dest[k++]=MealGroup[index].MealID[i];
	}
	for(i=0;i<6;i++)
	{
	  dest[k++]=MealGroup[index].DealBalance[i];
	}
	dest[k++]=MealGroup[index].MealNO;
	for(i=0;i<2;i++)
	{
	   dest[k++]=MealGroup[index].RemainMealNum[i];
	}
	for(i=0;i<6;i++)
	dest[k++]=MealGroup[index].MealPrice[i];
	dest[k++]=MealGroup[index].PayType;
	if(MealGroup[index].PayType==0x01)
	{
	  for(i=0;i<4;i++)
	  dest[k++]=MealGroup[index].Change[i];
	}
 	for(i=0;i<6;i++)
	dest[k++]=MealGroup[index].TimeData[i];
}

unsigned char MealGroupUploadingFun(void)
{
  unsigned char i = 0 ;
	long  Lenght = 0 ,j=0;
	long  CmdLenght = 0 ;
	unsigned char 	 Send_Buf[500];
	unsigned char    Buffer[327]={0x00,0x00,0x0a};
	/*接受缓冲区清0*/
	mem_set_00(rx1Buf,sizeof(rx1Buf));
	/*水流号++*/
	Send_Buf[0] =  0x02 ;
	Send_Buf[1] =  0x00 ;
	Send_Buf[2] =  0x00 ;
	Send_Buf[3] =  0x00 ;
	Send_Buf[4] =  0x00 ;
	CmdLenght = 5 ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));  /*终端的TID*/
	  GetBRWN();

	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));  /*流水号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));  /*批次号*/
 	Send_Buf[CmdLenght]   =  0xbb ;	   //T
	Send_Buf[CmdLenght+1] =  0x01 ;	   //L
	Send_Buf[CmdLenght+2] =  0x44 ;	   //V
	CmdLenght += 3;

	 StructCopyToBuffer00(&Buffer[3],0);
	 StructCopyToBuffer00(&Buffer[3+0x24],1);
	 StructCopyToBuffer00(&Buffer[3+0x24*2],2);
	 StructCopyToBuffer00(&Buffer[3+0x24*3],3);
	 StructCopyToBuffer00(&Buffer[3+0x24*4],4);
	 StructCopyToBuffer00(&Buffer[3+0x24*5],5);
	 StructCopyToBuffer00(&Buffer[3+0x24*6],6);
	 StructCopyToBuffer00(&Buffer[3+0x24*7],7);
	 StructCopyToBuffer00(&Buffer[3+0x24*8],8);

   printf("%x\r\n",sizeof(MealGroup_struction));
 	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));  /*批次号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Buffer,sizeof(Buffer));  /*批次号*/
	 Send_Buf[CmdLenght] = 0x03 	;
	 CmdLenght+=0x03;
	/*
	 这里添加没有上传的产品名字
	*/
	 i = MealComparefunDemo(0x0500,Send_Buf,CmdLenght);
	 if(i == 0x01)
	 return 1 ;
     Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	 for(j=0;j<Lenght+7;j++)
	 {
//	  printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	 }
	 return 0 ;
}
  /*******************************************************************************
* Function Name  : ClearingFun		 披上送
* Description    :
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
unsigned char    ClearingFun(void)
{
  unsigned char i = 0 ;
	long  Lenght = 0 ,j=0;
	unsigned char 	  Send_Buf[400];
	long  CmdLenght = 0 ;
	/*接受缓冲区清0*/
	mem_set_00(rx1Buf,sizeof(rx1Buf));
	/*水流号++*/
	Send_Buf[0] =  0x02 ;
	Send_Buf[1] =  0x00 ;
	Send_Buf[2] =  0x00 ;
	Send_Buf[3] =  0x00 ;
	Send_Buf[4] =  0x00 ;
	CmdLenght = 5 ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));  /*终端的TID*/

  GetBRWN();
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));  /*流水号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));  /*批次号*/
  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MAC,sizeof(MAC)); /*MAC*/
	Send_Buf[CmdLenght] = 0x03 	;
	CmdLenght+=0x03;
	i = MealComparefunDemo(0x0500,Send_Buf,CmdLenght);
	if(i == 0x01)
	  return 1 ;
  Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	for(j=0;j<Lenght+7;j++)
	{
//	printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	}
	return 0 ;
}

 /*******************************************************************************
* Function Name  : SignInFunction
* Description    : 签到函数
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
bool SignInFunction(void)
{
   
  unsigned char i = 50 ;
    do
	{
	  if(SignInFun()==0x00)
	  break;
	  delay_ms(100);
	}while(--i);

	if(i == 0x00)
	  return false;
	else
		return true;
}	
    

/*******************************************************************************
   * 函数名称:StateSend
   * 描    述:状态上送
   *
   * 输    入:无
   * 输    出:无
   * 返    回:void
   * 修改日期:2014年4月4日		ok
   *******************************************************************************/
  void StateSend(void)
  {
	  RTC_TimeShow();//获得时间
	  switch(TimeDate.Minutes)
		{
      case 5:
			case 10:
//			case 12:
//			case 14:
			case 15:						
//	    case 18:
//		  case 19:								
			case 20:
//			case 22:
			case 25:
//			case 26:
//			case 28:
			case 30:
//			case 32:
			case 35:
//			case 36:
//			case 38:
			case 40:
//			case 42:
			case 45:
//			case 46:
//			case 48:				
			case 50:
//			case 52:
			case 55:
//			case 56:
			case 0:				
			{
			  if(TimeDate.Senconds==10) //控制多次传输
			  {
					//TemperatureUploadingFun(Temperature); //温度上传
				  StatusUploadingFun(0xE800); //状态上送	
			  }
			  break;
			}
		  default : break;
		}
  }

  /*******************************************************************************
* Function Name  : SignInFunction
* Description    : 数据回响
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
bool EchoFuntion(void (*fptr)(void))
 {
   unsigned char i = 100 ;
   do
	 {
	   if(EchoFun()==0x00)
	   break;
	   delay_ms(200);
	 }while(--i);
	 if(i == 0x00)
	 return false;
	 (*fptr)() ;
	 return true;

 }
   /*******************************************************************************
* Function Name  :
* Description    : 餐品数据对比
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
unsigned char MealDataCompareFunction(void)
 {
   unsigned char i = 100;
   while(i)
   {
    if(MealDataCompareFun()==0x00)
	  return 0 ;
	  --i ;
	  delay_ms(100);
   }
    return 1 ;
 }


unsigned char 	Resend(unsigned char *p,long lenght)
{
	unsigned char Waittimeout = 100 ;
	long Lenght,j;
	mem_set_00(rx1Buf,sizeof(rx1Buf));
	CmdDataSend(p,lenght);	//发送数据
  /*等待数据返回*/
  while(--Waittimeout)
	{
		delay_ms(10);
		if(F_RX1_Right)
		{
		  F_RX1_Right = 0 ;
		  break;
		}
  }
  if(Waittimeout == 0)
    return 1 ;
	Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	if(Lenght == 0x00)/*判断数据长度*/
	  return 1 ;
	for(j=0;j<Lenght+7;j++)
	{
//	printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	}
	if(rx1Buf[0]==0x07 && rx1Buf[1]==0x10)	 //表示正确
	{
		if(rx1Buf[7]==0x00)/*是否应答命令*/
		{
		  return 0 ;
		}
		else
		  return 1 ;
//  if(rx1Buf[7]==0x24)/*交易流水号（终端流水）重复*/
//	{
//     return 1 ;
//	}
	}
  return 0 ;
}

 /*******************************************************************************
* 函数名称:HexToChar 		assert_hex_u8print
* 描	述:写入SD卡数据显示转换
* 输	入:char
* 输	出:char
* 返	回:char
* 修改日期:2014年4月14日			MrLAO
*******************************************************************************/

unsigned char HexToChar(unsigned char byTemp)
{
    byTemp &= 0x0f;
    if(byTemp >= 10)     // show 'A' - 'F'
   {
       byTemp = byTemp - 0xa + 0x41;
   }
   else        // show '0' - '9'
   {
       byTemp = byTemp + 0x30;
   }
   return(byTemp);
}


   /*******************************************************************************
 * 函数名称:DataUpload
 * 描    述:数据上传
 *
 * 输    入:无
 * 输    出:无
 * 返    回:void
 * 修改日期:2014年4月4日        ok
 *******************************************************************************/
extern uint8_t Current;
char mealvariety=0;
void DataUpload(char takemeal_flag)
{
  uint8_t cnt_t=0;
	itoa(f_name,TimeDate);	  //把时间转换成字符
	if(UserAct.MealID==0x00)
	{
		for(cnt_t=0;cnt_t<10;cnt_t++)
		{
	    printf("upload/UserAct.MealID == %d\r\n",UserAct.MealID);
			delay_ms(200);
		}
		while(1);
	}
	MealArr(UserAct.MealID);
	/*发送取餐数据给服务器*/
  memset(Record_buffer,0,254);
	if(TakeMealsFun(Record_buffer,takemeal_flag) == 0x01) //表示发送失败
	{
    UserAct.MealID= 0x00;//数据上传还一次对ID清零，这样就可以知道数据是否上传了
    Sd_Write('n',takemeal_flag);//发送失败
	}
	else
  {
    UserAct.MealID= 0x00;//数据上传还一次对ID清零，这样就可以知道数据是否上传了    
		Sd_Write('y',takemeal_flag);//改变当前最后两位为N0
  }
}	

 /*******************************************************************************
* Function Name  : 取餐发送数据  OK
* Description    : 取餐发送数据
* Input          : void
* Output         : void
* Return         : void
    08 10 00 0f c0 00 01 24 c9 00 08 00 00 00 00 00 00 00 00 03 e8 47
   //应答码 2位          MAC 8位
*******************************************************************************/

CustomerSel__struction CustomerSel;

char ReadBuf[20]={0};
unsigned char TakeMealsFun1(unsigned char *SendBuffer)
 {
	 char temp[10]={0};
	 
	 unsigned char i = 0 ;
	 long  Lenght = 0 ,j=0;
	 long  CmdLenght = 0 ;
   unsigned char	   Send_Buf[256]={0};	//要发送的数据啦
	 mem_set_00(rx1Buf,sizeof(rx1Buf));
	 SearchSeparator(ReadBuf,SendBuffer,1); //读取包头
	 StringToHexGroup1(temp,ReadBuf,10);	 
	 /*水流号++*/
	 for(i=0;i<5;i++)
	 {
	  Send_Buf[i] =	temp[i] ;
	 }
	 CmdLenght = 5 ;
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));	/*终端的TID*/
	 //GetBRWN();
	 SearchSeparator(ReadBuf,SendBuffer,3); //读取流水号
	 StringToHexGroup1(temp,ReadBuf,14);
	 for(i=0;i<7;i++)
	 {
		 BRWN[3+i]=temp[i];
	 }
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));  /*流水号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));	/*批次号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceArea,sizeof(DeviceArea));  /*终端所在区域编号*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceAreaNO,sizeof(DeviceAreaNO)); /*终端所在地域编号*/
//	 for(i=0;i<6;i++)
//	 {
//	   DealBalance[3+i] = CustomerSel.DealBalance[i] ;
//	 }
	 SearchSeparator(ReadBuf,SendBuffer,7); /*交易金额*/
	 StringToHexGroup1(temp,ReadBuf,12); 
   for(i=0;i<6;i++)
   {
		 DealBalance[3+i]=temp[i];
	 } 	 
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DealBalance,sizeof(DealBalance)); /*交易金额(支付金额) */

//	 for(i=0;i<4;i++)
//	 {
//	  //这里赋值餐品的ID
//	   MealID[3+i] = CustomerSel.MealID[i] ; //(10000020)
//	 }
	 SearchSeparator(ReadBuf,SendBuffer,8); //餐品ID
	 StringToHexGroup1(temp,ReadBuf,8); 
   for(i=0;i<4;i++)
   {
		 MealID[3+i]=temp[i];
	 }	 
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealID,sizeof(MealID)); /*餐品ID*/

	 /*这里赋值餐品的ID*/
//	 MealNO[3] = CustomerSel.MealNo;
	 SearchSeparator(ReadBuf,SendBuffer,9); //餐品数量
	 StringToHexGroup1(temp,ReadBuf,2);
 	 MealNO[3]=temp[0];
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealNO,sizeof(MealNO)); /*餐品购买数量*/

	  //这里赋值餐品的名字
//	  for(j=0;j<20;j++)
//	  MealName[3+j]=Meal[CustomerSel.MealName-1].MaelName[j];
	 SearchSeparator(ReadBuf,SendBuffer,10); //餐品名字
   for(i=0;i<20;i++)
   {
		 MealName[3+i]=ReadBuf[i];
	 }	
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealName,sizeof(MealName));			  /*餐品名字*/

	 /*这里赋值餐品的价格*/
	 
//	 for(i=0;i<6;i++)
//	 MealPrice[3+i] = CustomerSel.MealPrice[i] ;
	 SearchSeparator(ReadBuf,SendBuffer,11); //餐品价格
	 StringToHexGroup1(temp,ReadBuf,12); 
   for(i=0;i<6;i++)
   {
		 MealPrice[3+i]=temp[i];
	 }	 
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealPrice,sizeof(MealPrice));			/*餐品价格*/
	/*付钱的方式*/
	 //PayType[3] = CustomerSel.PayType ;
	 SearchSeparator(ReadBuf,SendBuffer,12); //付款方式
	 StringToHexGroup1(temp,ReadBuf,2);
 	 PayType[3]=temp[0];	 
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PayType,sizeof(PayType));				/*支付方式*/
	 /*找零金额*/
//	 for(i=0;i<6;i++)
//	 Change[3+i] = CustomerSel.Change[i] ;
	 SearchSeparator(ReadBuf,SendBuffer,13); //找零金额
	 StringToHexGroup1(temp,ReadBuf,12); 
   for(i=0;i<6;i++)
   {
		 Change[3+i]=temp[i];
	 }		 
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Change,sizeof(Change)); 			   /*找零金额*/
	 /*剩余餐品数量*/
//	 for(i=0;i<2;i++)
//	 {
//	  RemainMealNum[3+i] = CustomerSel.RemainMealNum[i] ;
//    //printf("CustomerSel.RemainMealNum[i]=%d\r\n",CustomerSel.RemainMealNum[i]);
//	 }
	 SearchSeparator(ReadBuf,SendBuffer,14); //剩余餐品数量
	 StringToHexGroup1(temp,ReadBuf,4); 
   for(i=0;i<4;i++)
   {
		 RemainMealNum[3+i]=temp[i];
	 }	 
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],RemainMealNum,sizeof(RemainMealNum));  /*剩余餐品数量*/
	 SearchSeparator(ReadBuf,SendBuffer,15); //取餐标记
	 StringToHexGroup1(temp,ReadBuf,4); 	 
   for(i=0;i<4;i++)
   {
		 TakeMealFlag[3+i]=temp[i];
	 }	 
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TakeMealFlag,sizeof(TakeMealFlag));  /*取餐标记*/
	 CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MAC,sizeof(MAC));					  /*MAC*/
 //  if(UserAct.PayType == '2' )																		/* 表示如果是刷卡*/
 //  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],STATUS.PacketData,STATUS.DataLength-17);				 /*记录刷卡信息*/
 //  if(UserAct.PayType == '3')
 //  CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],STATUS.PacketData,STATUS.DataLength-17);				 /*记录刷卡信息*/
	 
	 Send_Buf[CmdLenght] = 0x03  ;  //包含数据包包尾标识和CRC校验码
	 CmdLenght+=0x03;
   i = MealComparefunDemo(0x0800,Send_Buf,CmdLenght); //发送数据(取餐交易)
 //mem_copy00(SendBuffer, Send_Buf,CmdLenght);	//把发送的参数传入数组写入SD卡
 /***************************************************************************/
	 HL_IntToBuffer(CmdLenght,&SendBuffer[1019]);	/*保存数据的长度*/

	 if(i == 0x01) /*判断是否超时*/
	 return 1 ;
	 Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	 if(Lenght == 0x00)/*判断数据长度*/
	 return 1 ;
	 for(j=0;j<Lenght+7;j++)
	 {

 //   printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);

	  }
	  if(rx1Buf[0]==0x07 && rx1Buf[1]==0x10)  //表示正确
	  {

		 if(rx1Buf[7]==0x00)/*是否应答命令*/
		 {
			return 0 ;
		 }
		 if(rx1Buf[7]==0x24)/*交易流水号（终端流水）重复*/
		 {
		  //   Batch ++ ;	/*流水号自加*/
			return 1 ;
		 }

	  }
   return 0 ;

 }

char ReadSdBuff[512]={0};
char SendHostBuff[128]={0};
//void DataUpload(void)
//{
//	//读取SD卡中当天的数据
//	//读取第16个','后面的内容
//	//判断是否为'N',是则发送数据
//	//读取第16个','后面的内容
//  unsigned char DelteFlag = 0 ;//用来标记是否全部上传了数据的函数
//	unsigned char Times  = 0  ;
//	RTC_TimeShow();
//	itoa(f_name,TimeDate);	  //把时间转换成字符

//	  do
//	  {
//	     if(Fread(ReadSdBuff)== 0x00)					      //出错记录，然后跳出显示错误
//		   break ;
//    }while(1);
//	SearchSeparator(ReadBuf,ReadSdBuff,17);
//  if(ReadBuf[0]=='N')
//	{
//		 if(TakeMealsFun1(ReadSdBuff)==0);
//		 //在第十六个逗号后写Y
//		 
////		SearchSeparator(ReadBuf,ReadSdBuff,FH); //读取包头
////		memcpy_02(SendHostBuff,ReadBuf,6);
////		SearchSeparator(SendHostBuff,ReadSdBuff,TID); //读取TID
////		//读取Buffer
////		SendDataToHost();
//	}
//	if(ReadBuf[0]=='Y')
//	{
//				
//	}
//}



 /*******************************************************************************
 * 函数名称:StringToHexGroup1                                                                    
 * 描    述:将字符数组转换为hex数组,功能有所修改                                                                  
 *                                                                               
 * 输    入:                                                                   
 * 输    出:bool                                                                      
 * 返    回:void                                                               
 * 修改日期:2013年8月28日  
 *******************************************************************************/
bool StringToHexGroup1(unsigned char *OutHexBuffer, char *InStrBuffer, unsigned int strLength)
{
  unsigned int i, k=0;
  unsigned char HByte,LByte;
//	if(InStrBuffer[strLength-1]!=0x0A)
//		return false;
  for(i=0; i<strLength; i=i+2)
  {
    if(InStrBuffer[i]>='0' && InStrBuffer[i]<='9')
    {
      HByte=InStrBuffer[i]-'0';
    }
    else if(InStrBuffer[i]>='A' && InStrBuffer[i]<='F')
    {
      HByte=InStrBuffer[i]-'A' +10;
    }
    else
    {
      HByte=InStrBuffer[i];
      return false;
    }
    HByte=HByte <<4;
    HByte = HByte & 0xF0;
    if(InStrBuffer[i+1]>='0' && InStrBuffer[i+1]<='9')
    {
       LByte=InStrBuffer[i+1]-'0';
    }
    else if(InStrBuffer[i+1]>='A' && InStrBuffer[i+1]<='F')
    {
       LByte=InStrBuffer[i+1]-'A' +10;
    }
    else
    {
       LByte=InStrBuffer[i];
       return false;
    }
		OutHexBuffer[k++]=HByte |LByte;		
 }
 return true;
}
