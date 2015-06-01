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
#include "szt_gpboc.h"

static long Batch = 0x00 ;//交易流水号
//__attribute__ ((aligned (4)))
uint8_t  TID[7] = {0xa2,0x00,0x04,0x10,0x00,0x00,0x66}; /*终端TID码：可修改*/
uint8_t  BRWN[7+3] = {0xa6,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00};  /*交易流水线*/
uint8_t  BNO[6] = {0xa7,0x00,0x03,0x00,0x00,0x00};               /*批次号：可修改*/
uint8_t  DeviceArea[3+3]={0xac,0x00,0x03,0x17,0x03,0x02};         /*终端所在区域编号：可修改*/
uint8_t  DeviceAreaNO[4+3]={0xad,0x00,0x04,0x17,0x03,0x02,0x07};   /*终端所在地域编号：可修改*/
uint8_t  DeviceStatus[2+3]={0xae,0x00,0x02,0xE0,0x10};     /*终端状态*/
uint8_t  ErroDelect[2+3]={0xae,0x00,0x02,0xE0,0x10};     /*加入数组，防止数据指针超出界限错误炒作*/
uint8_t  DealData[7]={0xa9,0x00,0x04,0x00,0x00,0x00,0x00};     /*交易日期*/
uint8_t  DealTime[6]={0xaa,0x00,0x03,0x00,0x00,0x00};       /*交易时间*/
uint8_t  MAC[8+3]={0xc9,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};   /*MAC*/
uint8_t  DealBalance[6+3]={0xb0,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};    /*交易金额*/
uint8_t  MealID[4+3] = {0xb1,0x00,0x04,0x10,0x00,0x00,0x20} ;                /*餐品ID  10000020*/
uint8_t  MealNO[1+3] = {0xb2,0x00,0x01,0x00};      /*餐品购买数量*/
uint8_t  MealName[20+3] ={0xb3,0x00,0x14,'a','b','c','d','e','a','b','c','d','e','a','b','c','d','e','a','b','c','d','a'};  /*餐品名字*/
uint8_t  MealPrice[6+3]={0xbd,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};   /*餐品价格*/
uint8_t  PayType[1+3]={0xbf,0x00,0x01,0x00};        /*支付方式*/
uint8_t  Change[6+3]={0xd7,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00} ;        /*找零金额*/
uint8_t  RemainMealNum[2+3]={0xd8,0x00,0x02,0x00,0x00};  /*剩余餐品数量*/
uint8_t  MName[20+3]={0xa4,0x00,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};             /*商户号MID*/
uint8_t  APPVersion[8+3]={ 0xc2,0x00,0x08,'0','4','0','0','0','4',0x0D,0x0A};       /*应用程序版本  V4.0.3  */ 
uint8_t  ParaFileVersion[8+3]={0xc3,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};   /*参数文件版本*/
uint8_t  BDataFileVersion[8+3]={0xc4,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};  /*业务数据文件版本*/
uint8_t  ChechStatus[2+3]={0xab,0x00,0x02,0x00,0x00};                                     /*自检状态*/
uint8_t  MID[3+3]={ 0xa3,0x00,0x03,0x10,0x00,0x01};                                       /*商户号MID*/
uint8_t  TTCount[3+2]     ={0xb7,0x00,0x02,0x00,0x00};                                    /*交易总笔数 */
uint8_t  TNCT[6+3]        ={0xb8,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};                /*交易总金额*/
uint8_t  TNtype[3+2]      ={0xd9,0x00,0x02,0x00,0x00};                              /*交易总产品数 d9*/
uint8_t  TotalChange[3+6] ={0xd7,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00};          /*总找零金额 d7*/
uint8_t  TakeMealFlag[3+2]={0xdb,0x00,0x02,0x00,0x00};                              /*取餐标志 0x01:成功 0x02:失败 */
uint8_t  PosDevNum[10+3]=   {0xe0,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};                  /*刷卡器终端号*/
uint8_t  PosTenantNum[15+3]={0xe1,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; /*刷卡器商户号*/
uint8_t  PosBatchNum[10+3]= {0xe2,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};      /*交易流水号*/
uint8_t  PosUserNum[21+3]=  {0xe3,0x00,0x15,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};  /*用户银行卡号*/
uint8_t  UpdataFlag[3+1]=  {0xc1,0x00,0x01,0x00};                                     //更新标识
uint8_t  MenuNo[3+1]= {0xdd,0x00,0x01,0x01};                                        /*菜单编号,0x01,0x02,0x03,0x04,0x05（初步）*/
uint8_t  MenuDetail[3+MealKindTotoal*47]={0xdf,0x00,0x00};                          /*菜单的详细信息*/ 
uint8_t  ACK[3+1]={0xc0,0x00,0x02,0x00};                                            //应答码
uint8_t  WordKeyCipher[11]={0xc7,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//工作密钥
//__attribute__ ((aligned (1)))
union _Meal_Union Meal_Union;

uint8_t Record_buffer[254] = {0} ;

const uint8_t  DeviceTemperature[2+3]={0xdc,0x00,0x02,0x00,0x00};    /*设备温度，最后的一个字节为温度*/
const uint8_t  DeviceCoinsTotal[2+3] ={0xde,0x00,0x02,0x00,0x00};    /*机内硬币数，后两个字节存储的是机内*/
const uint8_t  Discount =10;  //定义折扣10折(不打折),5折(半价),0折(不要钱)
/*餐品的ID*/
uint16_t MealID_1st[4]={0};
uint16_t MealID_2nd[4]={0};
uint16_t MealID_3rd[4]={0};
uint16_t MealID_4th[4]={0};

/*餐品的数量*/
uint16_t MealNo_1st=0;
uint16_t MealNo_2nd=0;
uint16_t MealNo_3rd=0;
uint16_t MealNo_4th=0;

/*餐品的价格信息*/
uint16_t price_1st= 0; 
uint16_t price_2nd= 0;
uint16_t price_3rd= 0;
uint16_t price_4th= 0;

/*餐品的现金折扣信息*/
uint8_t cashcut_1st=0;
uint8_t cashcut_2nd=0;
uint8_t cashcut_3rd=0;
uint8_t cashcut_4th=0;

/*餐品的深圳通折扣信息*/
uint8_t sztcut_1st=0;
uint8_t sztcut_2nd=0;
uint8_t sztcut_3rd=0;
uint8_t sztcut_4th=0;

/*餐品的银联折扣信息*/
uint8_t gboccut_1st=0;
uint8_t gboccut_2nd=0;
uint8_t gboccut_3rd=0;
uint8_t gboccut_4th=0;

/*餐品的会员卡折扣信息*/
uint8_t vipcut_1st=0;
uint8_t vipcut_2nd=0;
uint8_t vipcut_3rd=0;
uint8_t vipcut_4th=0;

/*微信折扣*/
uint8_t vchatcut_1st=0;
uint8_t vchatcut_2nd=0;
uint8_t vchatcut_3rd=0;
uint8_t vchatcut_4th=0;

/*餐品的位置信息*/
uint8_t Order_1st=0;
uint8_t Order_2nd=0;
uint8_t Order_3rd=0;
uint8_t Order_4th=0;

/*餐品类型*/
uint8_t MealType_1st[4]={0};
uint8_t MealType_2nd[4]={0};
uint8_t MealType_3rd[4]={0};
uint8_t MealType_4th[4]={0};

/*屏幕显示的餐品名称*/
uint8_t  mealname_1st[20]= {'\0'}; 
uint8_t  mealname_2nd[20]= {'\0'};
uint8_t  mealname_3rd[20]= {'\0'};
uint8_t  mealname_4th[20]= {'\0'};

uint8_t Menu_interface= Menu1st_interface;
uint8_t MealSet_interface= MealSet1st_interface;
uint8_t sell_type[4]={0x01,0x02,0x03,0x04};

 /*******************************************************************************
 * 函数名称:GetMealDetail                                                                   
 * 描    述:通过结构体获取结构体中的变量                                                                                                                         
 * 输    入:无                                                                
 * 输    出:无                                                                    
 * 返    回:void                                                               
 * 修改日期:2014年11月6日                                                                   
 *******************************************************************************/ 
void GetMealDetail(void)
{
	/*获取餐品数量*/
	MealNo_1st= Meal_Union.Meal[0].MealCnt[0]*16 + Meal_Union.Meal[0].MealCnt[1];
	MealNo_2nd= Meal_Union.Meal[1].MealCnt[0]*16 + Meal_Union.Meal[1].MealCnt[1];
	MealNo_3rd= Meal_Union.Meal[2].MealCnt[0]*16 + Meal_Union.Meal[2].MealCnt[1];
	MealNo_4th= Meal_Union.Meal[3].MealCnt[0]*16 + Meal_Union.Meal[3].MealCnt[1];

	/*获取价格信息*/
	price_1st=((Meal_Union.Meal[0].MealPrice[4]&0xf0)>>4)*10;
	price_1st+=Meal_Union.Meal[0].MealPrice[4]&0x0f;
	price_2nd= ((Meal_Union.Meal[1].MealPrice[4]&0xf0)>>4)*10;
	price_2nd+=Meal_Union.Meal[1].MealPrice[4]&0x0f;
	price_3rd= ((Meal_Union.Meal[2].MealPrice[4]&0xf0)>>4)*10;
	price_3rd+=Meal_Union.Meal[2].MealPrice[4]&0x0f;
	price_4th= ((Meal_Union.Meal[3].MealPrice[4]&0xf0)>>4)*10;
	price_4th+=Meal_Union.Meal[3].MealPrice[4]&0x0f;
	
	/*获取普通折扣信息*/
	cashcut_1st= ((Meal_Union.Meal[0].MealCut[0]-'0')*10+Meal_Union.Meal[0].MealCut[1]-'0');
	cashcut_2nd= ((Meal_Union.Meal[1].MealCut[0]-'0')*10+Meal_Union.Meal[1].MealCut[1]-'0');
	cashcut_3rd= ((Meal_Union.Meal[2].MealCut[0]-'0')*10+Meal_Union.Meal[2].MealCut[1]-'0');
	cashcut_4th= ((Meal_Union.Meal[3].MealCut[0]-'0')*10+Meal_Union.Meal[3].MealCut[1]-'0');
	
	if(cashcut_1st==0x00) cashcut_1st=100;
	if(cashcut_2nd==0x00) cashcut_2nd=100;
	if(cashcut_3rd==0x00) cashcut_3rd=100;
	if(cashcut_4th==0x00) cashcut_4th=100;
	
	/*获取深圳通折扣信息*/
	sztcut_1st= ((Meal_Union.Meal[0].SztCut[0]-'0')*10+Meal_Union.Meal[0].SztCut[1]-'0');
	sztcut_2nd= ((Meal_Union.Meal[1].SztCut[0]-'0')*10+Meal_Union.Meal[1].SztCut[1]-'0');
	sztcut_3rd= ((Meal_Union.Meal[2].SztCut[0]-'0')*10+Meal_Union.Meal[2].SztCut[1]-'0');
	sztcut_4th= ((Meal_Union.Meal[3].SztCut[0]-'0')*10+Meal_Union.Meal[3].SztCut[1]-'0');
	if(sztcut_1st==0x00) sztcut_1st=100;
	if(sztcut_2nd==0x00) sztcut_2nd=100;
	if(sztcut_3rd==0x00) sztcut_3rd=100;
	if(sztcut_4th==0x00) sztcut_4th=100;
	
	/*获取银联折扣信息*/
	gboccut_1st= ((Meal_Union.Meal[0].GbocCut[0]-'0')*10+Meal_Union.Meal[0].GbocCut[1]-'0');
	gboccut_2nd= ((Meal_Union.Meal[1].GbocCut[0]-'0')*10+Meal_Union.Meal[1].GbocCut[1]-'0');
	gboccut_3rd= ((Meal_Union.Meal[2].GbocCut[0]-'0')*10+Meal_Union.Meal[2].GbocCut[1]-'0');
	gboccut_4th= ((Meal_Union.Meal[3].GbocCut[0]-'0')*10+Meal_Union.Meal[3].GbocCut[1]-'0');  
	if(gboccut_1st==0x00) gboccut_1st=100;
	if(gboccut_2nd==0x00) gboccut_2nd=100;
	if(gboccut_3rd==0x00) gboccut_3rd=100;
	if(gboccut_4th==0x00) gboccut_4th=100; 
	
	/*获取会员卡折扣信息*/
	vipcut_1st= ((Meal_Union.Meal[0].VipCut[0]-'0')*10+Meal_Union.Meal[0].VipCut[1]-'0');
	vipcut_2nd= ((Meal_Union.Meal[1].VipCut[0]-'0')*10+Meal_Union.Meal[1].VipCut[1]-'0');
	vipcut_3rd= ((Meal_Union.Meal[2].VipCut[0]-'0')*10+Meal_Union.Meal[2].VipCut[1]-'0');
	vipcut_4th= ((Meal_Union.Meal[3].VipCut[0]-'0')*10+Meal_Union.Meal[3].VipCut[1]-'0');
	if(vipcut_1st==0x00) vipcut_1st=100;
	if(vipcut_2nd==0x00) vipcut_2nd=100;
	if(vipcut_3rd==0x00) vipcut_3rd=100;
	if(vipcut_4th==0x00) vipcut_4th=100; 
	
	/*获取餐品的位置信息*/
	Order_1st= (Meal_Union.Meal[0].MealOrder[0]);
	Order_2nd= (Meal_Union.Meal[1].MealOrder[0]);
	Order_3rd= (Meal_Union.Meal[2].MealOrder[0]);
	Order_4th= (Meal_Union.Meal[3].MealOrder[0]);
	
	/*获取餐品ID信息*/
	memcpy(MealID_1st,Meal_Union.Meal[0].MealID,sizeof(Meal_Union.Meal[0].MealID));
	memcpy(MealID_2nd,Meal_Union.Meal[1].MealID,sizeof(Meal_Union.Meal[1].MealID));
	memcpy(MealID_3rd,Meal_Union.Meal[2].MealID,sizeof(Meal_Union.Meal[2].MealID));
	memcpy(MealID_4th,Meal_Union.Meal[3].MealID,sizeof(Meal_Union.Meal[3].MealID)); 
	
	/*获取餐品名*/
	memcpy(mealname_1st,Meal_Union.Meal[0].MealName,sizeof(Meal_Union.Meal[0].MealName));
	memcpy(mealname_2nd,Meal_Union.Meal[1].MealName,sizeof(Meal_Union.Meal[1].MealName));
	memcpy(mealname_3rd,Meal_Union.Meal[2].MealName,sizeof(Meal_Union.Meal[2].MealName));
	memcpy(mealname_4th,Meal_Union.Meal[3].MealName,sizeof(Meal_Union.Meal[3].MealName));
	
	/*餐品的类型*/
	memcpy(MealType_1st,Meal_Union.Meal[0].MealType,sizeof(Meal_Union.Meal[0].MealType));
	memcpy(MealType_2nd,Meal_Union.Meal[1].MealType,sizeof(Meal_Union.Meal[1].MealType));
	memcpy(MealType_3rd,Meal_Union.Meal[2].MealType,sizeof(Meal_Union.Meal[2].MealType));
	memcpy(MealType_4th,Meal_Union.Meal[3].MealType,sizeof(Meal_Union.Meal[3].MealType));  
	
}
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
	BRWN[3] =    20 /10 *16 + 20%10 ;         /*年*/
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
uint32_t GetCrc16(uint8_t *bufData,uint32_t sizeData)
{
	uint32_t Crc ,i = 0;
	uint8_t j = 0;
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
void HL_IntToBuffer(const uint16_t int_v, uint8_t *ret_buf);
功能: 将一个 16bit 的整数转换成 Buffer 数据, 高位在前
高位在前
*/
void HL_IntToBuffer(const uint16_t int_v, uint8_t *ret_buf)
{
	ret_buf[0] = (uint8_t)(int_v >> 8);  // [0] 对应 --> 高位
	ret_buf[1] = (uint8_t)(int_v & 0xff);

}
/*---------------------------------------------------------------------------
 uint16_t HL_BufferToInt(const uint8_t *in_buf);
 功能: 将缓冲区数据转换成整数, 高位在前
 高位在前
*/
uint16_t HL_BufferToInit(const uint8_t *in_buf)
{
	uint16_t ret_n;
	ret_n = in_buf[0];           // 取高位
	ret_n = (ret_n << 8);
	ret_n += in_buf[1];          // 取低位
	return ret_n;
}

/*---------------------------------------------------------------------------
uint32_t HL_BufferToLong(const uint8_t *in_buf);
功能: 将 (uint8_t *) 数据转换成 Long 型, 高位在前
高位在前
*/
uint32_t HL_BufferToLong00(const uint8_t *in_buf)
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
void LH_LongToBuffer(uint32_t in_n, uint8_t *ret_buf);
功能: 将长整数转换成 uint8_t 型数据, 高位在前
*/
void HL_LongToBuffer00(const uint32_t in_n, uint8_t *ret_buf)
{
	ret_buf[0] = (uint8_t)((in_n >> 24) & 0xff);   // [0] 对应 --> 高位
	ret_buf[1] = (uint8_t)((in_n >> 16) & 0xff);
	ret_buf[2] = (uint8_t)((in_n >> 8) & 0xff);
	ret_buf[3] = (uint8_t)(in_n & 0xff);
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
static void mem_set_00(uint8_t *dest, const long s_len)
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
uint16_t mem_copy01(uint8_t *dest, const uint8_t *source, const long s_len)
{
	long j0;
	for(j0 = 0; j0 < s_len; j0++)  dest[j0] = source[j0];
	dest[j0++]=',';
	return (s_len+1);
}
uint16_t mem_copy00(uint8_t *dest, const uint8_t *source, const long s_len)
{
	long j0;
	for(j0 = 0; j0 < s_len; j0++)  dest[j0] = source[j0];
	return (s_len);
}

/*********************/
typedef struct
{
	uint8_t  Lenght[2] ;
	uint8_t  MealNO[35*4];    /*餐品具体数目*/

}ReturnData_Struction;
ReturnData_Struction  ReturnData;

/*******************************************************************************
* Function Name  : CheckResponseCode
* Description    : 检查返回码看什么错误
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
uint8_t CheckResponseCode(uint8_t Cmd)
{
// uint8_t  i = 0 ;
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
		case 0x07 : /*无效文件偏移地址*/
				break ;
		case 0x08 : /*无效文件偏移地址*/
				break ;
		case 0x09 : /*其他包内容错误*/
				break;
		case 10 : /*商户号与设备号对应关系错误*/
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
* Input          : uint8_t *dest,uint8_t *souce,const long s_len,uint8_t Cmd
* Output         : void
* Return         : void
*******************************************************************************/
long  GetData(uint8_t *dest,uint8_t *souce, long s_len,uint8_t Cmd)
{
	long i = 0 ,j=0,Cmdlenght=0;
	for(i=0;i<s_len;i++)
	{
		if(souce[i] == Cmd)  /*得到命令*/
		{
			Cmdlenght= HL_BufferToInit(&souce[i+1]);    /*得到数据长度*/
//    printf("Cmdlenght=%d\r\n",Cmdlenght);
			for(j=0;j<Cmdlenght;j++)              /*得到数据  */
			{
				dest[j] = souce[i+j+3] ;
//      printf("dest[%d]=%x\r\n",j,dest[j]);
			}
			return Cmdlenght  ;
		}
	}
	return 0 ;
}
 /*******************************************************************************
* Function Name  : CmdDataSend
* Description    : 发送数据函数
* Input          : uint8_t *p  long Lenght
* Output         : void
* Return         : void       ok
*******************************************************************************/
static void CmdDataSend(uint8_t *p,long Lenght)
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

uint8_t  MealComparefunDemo(long Cmd ,uint8_t *p,unsigned long lenght)
{
	uint16_t  CRCValue= 0 ;
// static  int Bno = 0 ;
	uint8_t Waittimeout = 250 ;
	/*给数组赋值命令*/
	HL_IntToBuffer(Cmd,&p[1]);
	/*数组长度*/
	HL_IntToBuffer(lenght-8,&p[3]);
	/*得到数据的CRC*/
	CRCValue=GetCrc16(&p[1],lenght-4);
	/*把CRC赋值到数组中*/
	HL_IntToBuffer(CRCValue,&p[lenght-2]);
	/*发送命令*/
	CmdDataSend(p,lenght);  //发送数据
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
 * 函数名称:SignInFun
 * 描    述:终端签到                                                      
 *                                                                               
 * 输    入:无                                                                   
 * 输    出:无                                                                     
 * 返    回:   0 --- 签到失败
 *             1 --- 无菜单
							 2 --- 无餐品数据
							 3 --- 成功
 * 修改日期:2014年11月16日                                                                    
 *******************************************************************************/ 
uint8_t SignInFun(void)
{
	long i = 0 ;
	long  Command = 0x0100 ;
	long  Lenght = 0,j = 0  ;
	long  CmdLenght = 0 ;  //命令长度
	long  data_detail_length =0; //餐品数据的长度
	uint8_t  Send_Buf[400]={0};
	memset(rx1Buf,0,sizeof(rx1Buf));  //数据清零

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
	GetBRWN();                             /*得到流水号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN));   /*交易流水号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO));     /*批次号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceArea,sizeof(DeviceArea));  /*终端所在区域编号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceAreaNO,sizeof(DeviceAreaNO)); /*终端所在地域编号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],APPVersion,sizeof(APPVersion)); /*应用程序版本号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],ParaFileVersion,sizeof(ParaFileVersion)); /*参数文件版本号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BDataFileVersion,sizeof(BDataFileVersion)); /*业务数据文件版本号*/
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],ChechStatus,sizeof(ChechStatus)); /*自检状态*/
	Send_Buf[CmdLenght] = 0x03  ;
	CmdLenght+=0x03;
	 /*发送命令*/
		 i = MealComparefunDemo(Command,Send_Buf,CmdLenght);
		/***************************************************************************/
	if(i == 0x01)  //
		return  0;
	Lenght = HL_BufferToInit(&rx1Buf[2]);  //判断数据长度是否
	if(Lenght == 0x00)
		return  0 ;
	 for(i=0;i<23;i++)
	 {
		 MName[i] = rx1Buf[i+4];//得到商幻
	 }
	 for(i=0;i<3;i++)
	 {
		 BNO[i+3] = rx1Buf[i+27+3];//得到批次号
	 }
	 for(i=0;i<11;i++)
	 {
		 APPVersion[i] = rx1Buf[i+0x21];//得到程序版本号
	 }
	 for(i=0;i<11;i++)
	 {
		 ParaFileVersion[i] = rx1Buf[i+0x2c];//得到参数文件版本
	 }
	 for(i=0;i<11;i++)
	 {
		 BDataFileVersion[i] = rx1Buf[i+0x37];//业务数据版本文件
	 }
	 for(i=0;i<4;i++)
	 {
		 UpdataFlag[i] = rx1Buf[i+0x42];//得到批次号
	 }
	 for(i=0;i<4;i++)
	 {
			MenuNo[i]= rx1Buf[i+0x46];
			//printf("%X ",MenuNo[i]);
	 }
	 if(MenuNo[0]!=0xdd)  return 1; 
	 if(MenuNo[2]==0x00)  return 2; 
	 for(i=0;i<3;i++)
	 {
			MenuDetail[i]= rx1Buf[i+0x4A];
	 }    
	 if(MenuDetail[2]!=0)
	 {     
		 memcpy(Meal_Union.MealBuf,&rx1Buf[77],MealKindTotoal*47);
	 }
	 else
	 {
		 return 0;
	 }
	 for(i=0;i<4;i++)
	 {
		 ACK[i] = rx1Buf[i+265];//得到
	 }
	 for(i=0;i<11;i++)
	 {
		 WordKeyCipher[i] = rx1Buf[i+269];//得到工作密文
	 }
	 for(i=0;i<11;i++)
	 {
		 MAC[i] = rx1Buf[i+280];
	 }
	 memset(rx1Buf,0,sizeof(rx1Buf));
	 if(ACK[3]==0x24)
	 {
		 return 3 ;
	 }
	 return 3 ;
}

void StructCopyToBuffer(uint8_t *dest)
{
	long j0=0,i=0,k=0;
//上传4份餐品的数据 4*(4+20+2+4+4)
	for(j0 = 0; j0 < 4; j0++)
	{
		for(i=0;i<4;i++)
		dest[k++]=Meal_Union.Meal[j0].MealID[i];
		
		for(i=0;i<20;i++)
		dest[k++]=Meal_Union.Meal[j0].MealName[i];

		dest[k++]= 0; //第一个数为0
		dest[k++]= DefineMeal[j0].MealCount; //第二个数为餐品的数量

		for(i=0;i<6;i++) 
		dest[k++]=Meal_Union.Meal[j0].MealPrice[i];

		for(i=0;i<4;i++)
		dest[k++]=Meal_Union.Meal[j0].MealType[i];
	}
}

/*******************************************************************************
* Function Name  : 餐品数据对比    0X0200
* Description    :餐品数据对比
* Input          : void
* Output         : void
* Return         : char
* Time          :2014-4-7  MrLao   数据域采用TVL格式
*******************************************************************************/
uint8_t k = 0 ;
MealCompareDataStruct MealCompareData;
uint32_t  MealDataCompareFun(void)
{
	
	uint8_t i= 0 ;
	long  Lenght = 0 ,j;
//  uint8_t MealID = 0 ;
	long  CmdLenght = 0 ;
	uint8_t status = 0 ;
	uint8_t Send_Buf[300]={0};
	uint8_t TempBuffer[37*4]={0};
	uint8_t Buffer[36*4]={0};
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
	Send_Buf[CmdLenght] =  0xbc ;  //T  餐品数据对比， 数据域采用TVL格式
	Send_Buf[CmdLenght+1] =  0x00 ;  //L 数据长度
	Send_Buf[CmdLenght+2] =  36*4 ;   //V 数据内容  
	CmdLenght += 3;
	StructCopyToBuffer(Buffer);
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Buffer,sizeof(Buffer));

	Send_Buf[CmdLenght] = 0x03  ;
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
	
	Lenght = HL_BufferToInit(&rx1Buf[2]);   //得到数据长度
	GetData(&ReturnData.Lenght[0],rx1Buf,Lenght,0xc0);    /*返回状态*/
	
	//printf("Status=%x\r\n",ReturnData.Lenght[0]);

	if(ReturnData.Lenght[0] == 0x00 )
	{
		MealCompareData.MealCompareTotoal= 0xFFFFFFFF;  
			return MealCompareData.MealCompareTotoal;  /*数据正确*/
	} 
	if(ReturnData.Lenght[0] == 0x24 )
	{
		MealCompareData.MealCompareTotoal= 0xFFFFFFFF;  
			return MealCompareData.MealCompareTotoal;  /*数据正确*/
	}
	CmdLenght = GetData(TempBuffer,rx1Buf,Lenght,0xBC);/*餐品对比*/
	//printf("StatusCmdLenght=%x\r\n",CmdLenght);
	if(CmdLenght>37)
	{
		//status  = CmdLenght / 37  ;
		for(i=0;i<4;i++)
		{
			if(rx1Buf[47+i*37]==0x04)   //餐品对比标志
			{
				MealCompareData.MealComparePart[i]=0xFF;  //144-36/3  108/3=36
			}         
			else
			{
				MealCompareData.MealComparePart[i]=rx1Buf[36+i*37];
			}       
		}
		return 1 ;  //返回对比正确
	}
	else
	{
		MealCompareData.MealCompareTotoal=0;
		return 0;    //返回对比错误
	}
}


/*******************************************************************************
* Function Name  : 2.3.3 签退    0X0300
* Description    :退签函数
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
uint8_t SignOutFun()
{

	uint8_t i = 0 ;
	long  Lenght = 0 ,j;
	long  CmdLenght = 0 ;
	uint8_t     Send_Buf[400];

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
	Send_Buf[CmdLenght] = 0x03  ;
	CmdLenght+=0x03;
	i = MealComparefunDemo(0x0300,Send_Buf,CmdLenght);

	if(i == 0x01)
	return 1 ;
	 Lenght = HL_BufferToInit(&rx1Buf[2]);
	 for(j=0;j<Lenght+7;j++)
	 {
//   printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	 }
	 return 0 ;
}

 /*******************************************************************************
* Function Name  : 状态上传函数    0X0400
* Description    :状态函数
* Input          : void
* Output         : void
* Return         : void         ok
*******************************************************************************/
uint8_t StatusUploadingFun(uint16_t erro_status)
{

	uint8_t i = 0 ;
	long  Lenght = 0 ,j;
	long  CmdLenght = 0 ;
	uint8_t Temperature_t=0;
	uint16_t CoinsTotoal_t=0;
	uint8_t  Temperature_tt[5]={0};
	uint8_t  CoinsTotoal_tt[5]={0};
	uint8_t  Send_Buf[100]={0};
	uint8_t  state_temp[2]={0};  
	//sprintf(state_temp,"%x",erro_status); 
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

	state_temp[1]=(erro_status&0x00ff);
	state_temp[0]=(erro_status>>8)&0x00ff;
	for(i=0;i<2;i++) 
	{
		DeviceStatus[3+i]= state_temp[i];
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceStatus,sizeof(DeviceStatus));  /*终端的状态*/
	Temperature_t= Temperature;
	state_temp[1]=(Temperature_t&0x00ff);
	state_temp[0]=(Temperature_t>>8)&0x00ff;
	memcpy(Temperature_tt,DeviceTemperature,sizeof(DeviceTemperature));
	for(i=0;i<2;i++) 
	{
		Temperature_tt[3+i]= state_temp[i];
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Temperature_tt,sizeof(Temperature_tt));  /*终端的温度*/
	CoinsTotoal_t =CoinsTotoalMessageWriteToFlash.CoinTotoal;
	state_temp[1]=(CoinsTotoal_t&0x00ff);
	state_temp[0]=(CoinsTotoal_t>>8)&0x00ff;   
	memcpy(CoinsTotoal_tt,DeviceCoinsTotal,sizeof(DeviceCoinsTotal));  
	for(i=0;i<2;i++) 
	{
		CoinsTotoal_tt[3+i]= state_temp[i];
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],CoinsTotoal_tt,sizeof(CoinsTotoal_tt));  /*终端的硬币数*/   
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
* Function Name  : 回响测试  0X0700
* Description    :回响测试
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
uint8_t EchoFun(void)
{

	uint8_t i = 0 ;
	long  Lenght = 0 ,j;
	long  CmdLenght = 0 ;
	uint8_t     Send_Buf[100];
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
	Send_Buf[CmdLenght] = 0x03  ;
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
//  printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	}
	if(rx1Buf[0]==0x07 && rx1Buf[1]==0x10)  //表示正确
	{
		//  tmp_yy  = rx1Buf[7]/16*10+rx1Buf[7]%16 ;/*年*/
		tmp_yy  = rx1Buf[8]/16*10+rx1Buf[8]%16 ;/*年*/
		if(tmp_yy<14) return 1 ;// 以免年份出错
//       printf("tmp_yy=%d\r\n",tmp_yy);
		tmp_m2  = rx1Buf[9]/16*10+rx1Buf[9]%16 ;/*月*/
//       printf("tmp_yy=%d\r\n",tmp_m2);
		tmp_dd  = rx1Buf[10]/16*10+rx1Buf[10]%16 ;/*日*/
//       printf("tmp_dd=%d\r\n",tmp_dd);

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
//       for(i=0;i<4;i++)
//       printf("rx1Buf[%d]=%d\r\n",i,rx1Buf[i+7]);

//       for(i=0;i<3;i++)
//       printf("rx1Buf[%d]=%d\r\n",i,rx1Buf[i+14]);
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
uint8_t TakeMealsFun(uint8_t *SendBuffer,uint8_t takeout_flag)
{
	uint8_t i = 0 ;
	long  Lenght = 0 ,j=0;
	long  CmdLenght = 0 ;
	uint8_t  Send_Buf[512];  //要发送的数据啦
	memset(rx1Buf,0,sizeof(rx1Buf));
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
	{
		MealName[3+j]=CustomerSel.MealName[j];
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealName,sizeof(MealName));        /*餐品名字*/

	/*这里赋值餐品的价格*/
	for(i=0;i<6;i++)
		MealPrice[3+i] = CustomerSel.MealPrice[i] ;

	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealPrice,sizeof(MealPrice));      /*餐品价格*/
	/*付钱的方式*/
	PayType[3] = CustomerSel.PayType ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PayType,sizeof(PayType));        /*支付方式*/
	/*找零金额*/
	for(i=0;i<6;i++)
		Change[3+i] = CustomerSel.Change[i] ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Change,sizeof(Change));         /*找零金额*/
	/*剩余餐品数量*/
	for(i=0;i<2;i++)
	{
		RemainMealNum[3+i] = CustomerSel.RemainMealNum[i] ;
		//printf("CustomerSel.RemainMealNum[i]=%d\r\n",CustomerSel.RemainMealNum[i]);
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],RemainMealNum,sizeof(RemainMealNum));  /*剩余餐品数量*/
	TakeMealFlag[4]= takeout_flag;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TakeMealFlag,sizeof(TakeMealFlag)); /*取餐标记*/ 
	if(UserActMessageWriteToFlash.UserAct.PayType != '1')
	{     
		CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PosDevNum,PosDevNum[2]+3); /*刷卡器终端号*/
		CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PosTenantNum,PosTenantNum[2]+3); /*刷卡器商户号*/   
		CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PosBatchNum,PosBatchNum[2]+3); /*刷卡器交易流水号*/
		CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PosUserNum,PosUserNum[2]+3); /*用户银行卡号*/
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MAC,sizeof(MAC));            /*MAC*/
	Send_Buf[CmdLenght] = 0x03  ;  //包含数据包包尾标识和CRC校验码
	CmdLenght+=0x03;
	i = MealComparefunDemo(0x0800,Send_Buf,CmdLenght); //发送数据(取餐交易)
	mem_copy00(SendBuffer, Send_Buf,CmdLenght);  //把发送的参数传入数组写入SD卡
	/***************************************************************************/
	if(i == 0x01) /*判断是否超时*/
	return 1 ;
	Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	if(Lenght == 0x00)/*判断数据长度*/
	return 1 ;
	for(j=0;j<Lenght+7;j++)
	{
		//printf("rx1Buf[%d]=%x\r\n",j,rx1Buf[j]);
	}
	if(rx1Buf[0]==0x08 && rx1Buf[1]==0x10)  //表示正确
	{
		if(rx1Buf[7]==0x00)/*是否应答命令*/
		{
			return 0 ;
		}
		if(rx1Buf[7]==0x24)/*交易流水号（终端流水）重复*/
		{
		//   Batch ++ ;  /*流水号自加*/
			return 1 ;
		}
	}
	return 1 ;
}


 /*******************************************************************************
* Function Name  : SignInFunction
* Description    : 签到函数
* Input          : void
* Output         : void
* Return         : void
*******************************************************************************/
uint8_t signin_state=0;
bool SignInFunction(void)
{
	uint8_t i = 80 ;
	do
	{
		if((signin_state=SignInFun())>2)
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
 * 修改日期:2014年4月4日    ok
 *******************************************************************************/
uint32_t statesend_record=0;//用来防止多次传输
uint8_t old_hours=0;
uint8_t old_minutes=0;
extern uint8_t Flag_szt_gpboc_ok;
void StateSend(void)
{
	RTC_TimeShow();//获得时间
	if(old_minutes!=TimeDate.Minutes)
	{
		old_minutes= TimeDate.Minutes;
		switch(TimeDate.Minutes)
		{
			case 5:
			case 10:
			case 15:
			case 20:
			case 25:
			case 30:
			case 35:
			case 40:
			case 45:       
			case 50:
			case 55:
			case 0:       
					StatusUploadingFun(0xE800); //状态上送
					if(sellmeal_flag== false)
					{
						if(Flag_szt_gpboc_ok != 1)
						{
							Szt_GpbocAutoCheckIn();
						}
					}
				break;
				default : break;
			} 
	}
	if(old_hours!= TimeDate.Hours)
	{
		old_hours= TimeDate.Hours;
		switch(old_hours)
		{
			case 13:
			case 14:
			case 15:
			case 16:
			SendtoServce();break;
			default:break;			
		}
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
	uint8_t i = 100 ;
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
uint8_t MealDataCompareFunction(void)
{
	uint8_t i = 100;
	while(i)
	{
		if(MealDataCompareFun()==0x00)
		return 0 ;
		--i ;
		delay_ms(100);
	}
	return 1 ;
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
void DataUpload(uint8_t takemeal_flag)
{
	uint8_t cnt_t=0;
	itoa(f_name,TimeDate);    //把时间转换成字符
	if(UserActMessageWriteToFlash.UserAct.MealID==0x00)
	{
		for(cnt_t=0;cnt_t<10;cnt_t++)
		{
			printf("upload/UserAct.MealID == %d\r\n",UserActMessageWriteToFlash.UserAct.MealID);
			delay_ms(200);
		}
		while(1);
	}
	MealArr(UserActMessageWriteToFlash.UserAct.MealID);
	/*发送取餐数据给服务器*/
	memset(Record_buffer,0,254);
	if(TakeMealsFun(Record_buffer,takemeal_flag) == 0x01) //表示发送失败
	{
		Sd_Write('n',takemeal_flag);//发送失败
	}
	else
	{
		Sd_Write('y',takemeal_flag);//改变当前最后两位为N0
	}
	UserActMessageWriteToFlash.UserAct.MealID= 0x00;//数据上传还一次对ID清零，这样就可以知道数据是否上传了
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

uint8_t ReadBuf[20]={0};
uint8_t TakeMealsFun1(uint8_t *SendBuffer)
{
	uint8_t temp[10]={0};
	uint8_t i = 0 ;
	long  Lenght = 0 ,j=0;
	long  CmdLenght = 0 ;
	uint8_t     Send_Buf[256]={0}; //要发送的数据啦
	mem_set_00(rx1Buf,sizeof(rx1Buf));
	SearchSeparator(ReadBuf,SendBuffer,1); //读取包头
	StringToHexGroup1(temp,ReadBuf,10);   
	/*水流号++*/
	for(i=0;i<5;i++)
	{
		Send_Buf[i] = temp[i] ;
	}
	CmdLenght = 5 ;
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TID,sizeof(TID));  /*终端的TID*/

	SearchSeparator(ReadBuf,SendBuffer,3); //读取流水号
	StringToHexGroup1(temp,ReadBuf,14);
	for(i=0;i<7;i++)
	{
		BRWN[3+i]=temp[i];
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BRWN,sizeof(BRWN)); /*流水号*/

	SearchSeparator(ReadBuf,SendBuffer,4);	/*批次号*/
	StringToHexGroup1(temp,ReadBuf,6);
	for(i=0;i<3;i++)
	{
		BNO[3+i]=temp[i];
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],BNO,sizeof(BNO)); /*批次号*/

	SearchSeparator(ReadBuf,SendBuffer,5);	/*终端所在区域编号*/
	StringToHexGroup1(temp,ReadBuf,6);
	for(i=0;i<3;i++)
	{
		DeviceArea[3+i]=temp[i];
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceArea,sizeof(DeviceArea)); /*终端所在区域编号*/

	SearchSeparator(ReadBuf,SendBuffer,6);	/*终端所在地域编号*/
	StringToHexGroup1(temp,ReadBuf,8);
	for(i=0;i<4;i++)
	{
		DeviceAreaNO[3+i]=temp[i];
	}
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DeviceAreaNO,sizeof(DeviceAreaNO)); /*终端所在地域编号*/

	SearchSeparator(ReadBuf,SendBuffer,7); /*交易金额*/
	StringToHexGroup1(temp,ReadBuf,12); 
	for(i=0;i<6;i++)
	{
		DealBalance[3+i]=temp[i];
	}   
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],DealBalance,sizeof(DealBalance)); /*交易金额(支付金额) */

	SearchSeparator(ReadBuf,SendBuffer,8); //餐品ID
	StringToHexGroup1(temp,ReadBuf,8); 
	for(i=0;i<4;i++)
	{
		MealID[3+i]=temp[i];
	}   
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealID,sizeof(MealID)); /*餐品ID*/

	SearchSeparator(ReadBuf,SendBuffer,9); //餐品数量
	StringToHexGroup1(temp,ReadBuf,2);
	MealNO[3]=temp[0];
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealNO,sizeof(MealNO)); /*餐品购买数量*/

	SearchSeparator(ReadBuf,SendBuffer,10); //餐品名字
	for(i=0;i<20;i++)
	{
		MealName[3+i]=ReadBuf[i];
	}  
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealName,sizeof(MealName)); /*餐品名字*/

	SearchSeparator(ReadBuf,SendBuffer,11); //餐品价格
	StringToHexGroup1(temp,ReadBuf,12); 
	for(i=0;i<6;i++)
	{
		MealPrice[3+i]=temp[i];
	}   
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MealPrice,sizeof(MealPrice));      /*餐品价格*/

	SearchSeparator(ReadBuf,SendBuffer,12); //付款方式
	PayType[3]=ReadBuf[0];  
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PayType,PayType[2]+3);       /*支付方式*/

	SearchSeparator(ReadBuf,SendBuffer,13); //找零金额
	StringToHexGroup1(temp,ReadBuf,12); 
	for(i=0;i<6;i++)
	{
		Change[3+i]=temp[i];
	}     
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],Change,sizeof(Change));         /*找零金额*/

	SearchSeparator(ReadBuf,SendBuffer,14); //剩余餐品数量
	StringToHexGroup1(temp,ReadBuf,4); 
	for(i=0;i<4;i++)
	{
		RemainMealNum[3+i]=temp[i];
	}   
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],RemainMealNum,sizeof(RemainMealNum));  /*剩余餐品数量*/

	SearchSeparator(ReadBuf,SendBuffer,15); //取餐标记
	StringToHexGroup1(temp,ReadBuf,4);    
	for(i=0;i<2;i++)
	{
		TakeMealFlag[3+i]=temp[i];
	}   
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],TakeMealFlag,sizeof(TakeMealFlag));  /*取餐标记*/

	if(PayType[3]!= '1')//现金的时候不上传下面数据
	{
		memset(ReadBuf,0,sizeof(ReadBuf));
		SearchSeparator(ReadBuf,SendBuffer,16); //刷卡器终端号
		PosDevNum[2]= strlen((char*)ReadBuf);
		for(i=0;i<PosDevNum[2];i++)
			PosDevNum[3+i]=ReadBuf[i];  //终端号
		CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PosDevNum,PosDevNum[2]+3); 

		memset(ReadBuf,0,sizeof(ReadBuf));
		SearchSeparator(ReadBuf,SendBuffer,17); //刷卡器商号号
		PosTenantNum[2]= strlen((char*)ReadBuf);   
		for(i=0;i<PosTenantNum[2];i++)
			PosTenantNum[3+i]=ReadBuf[i];  //商户号
		CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PosTenantNum,PosTenantNum[2]+3);

		memset(ReadBuf,0,sizeof(ReadBuf));
		SearchSeparator(ReadBuf,SendBuffer,18); //刷卡器交易流水号    
		PosBatchNum[2]= strlen((char*)ReadBuf); 
		for(i=0;i<PosBatchNum[2];i++)
			PosBatchNum[3+i]=ReadBuf[i];  //流水号
		CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PosBatchNum,PosBatchNum[2]+3);

		memset(ReadBuf,0,sizeof(ReadBuf));     
		SearchSeparator(ReadBuf,SendBuffer,19); //用户卡号
		PosUserNum[2]= strlen((char*)ReadBuf);  
		for(i=0;i<PosUserNum[2];i++)
			PosUserNum[3+i]= ReadBuf[i];   //卡号  
		CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],PosUserNum,PosUserNum[2]+3);    
	}  
	CmdLenght +=mem_copy00(&Send_Buf[CmdLenght],MAC,sizeof(MAC));            /*MAC*/  
	Send_Buf[CmdLenght] = 0x03  ;  //包含数据包包尾标识和CRC校验码
	CmdLenght+=0x03;
	i = MealComparefunDemo(0x0800,Send_Buf,CmdLenght); //发送数据(取餐交易)
	/***************************************************************************/

	if(i == 0x01) /*判断是否超时*/
		return 1 ;
	Lenght = HL_BufferToInit(&rx1Buf[2]) ;
	if(Lenght == 0x00)/*判断数据长度*/
		return 1 ;
	if(rx1Buf[0]==0x08 && rx1Buf[1]==0x10)  //表示正确
	{
		if(rx1Buf[7]==0x00)/*是否应答命令*/
		{
			return 0 ;
		}
		if(rx1Buf[7]==0x24)/*交易流水号（终端流水）重复*/
		{
			return 1 ;
		}
	}
	return 1 ;
}

uint8_t ReadSdBuff[512]={0};
//uint8_t SendHostBuff[128]={0};
//void DataUpload(void)
//{
//  //读取SD卡中当天的数据
//  //读取第16个','后面的内容
//  //判断是否为'N',是则发送数据
//  //读取第16个','后面的内容
//  uint8_t DelteFlag = 0 ;//用来标记是否全部上传了数据的函数
//  uint8_t Times  = 0  ;
//  RTC_TimeShow();
//  itoa(f_name,TimeDate);    //把时间转换成字符

//    do
//    {
//       if(Fread(ReadSdBuff)== 0x00)               //出错记录，然后跳出显示错误
//       break ;
//    }while(1);
//  SearchSeparator(ReadBuf,ReadSdBuff,17);
//  if(ReadBuf[0]=='N')
//  {
//     if(TakeMealsFun1(ReadSdBuff)==0);
//     //在第十六个逗号后写Y
//     
////    SearchSeparator(ReadBuf,ReadSdBuff,FH); //读取包头
////    memcpy_02(SendHostBuff,ReadBuf,6);
////    SearchSeparator(SendHostBuff,ReadSdBuff,TID); //读取TID
////    //读取Buffer
////    SendDataToHost();
//  }
//  if(ReadBuf[0]=='Y')
//  {
//        
//  }
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
bool StringToHexGroup1(uint8_t *OutHexBuffer, uint8_t *InStrBuffer, unsigned int strLength)
{
	unsigned int i, k=0;
	uint8_t HByte,LByte;
//  if(InStrBuffer[strLength-1]!=0x0A)
//    return false;
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
