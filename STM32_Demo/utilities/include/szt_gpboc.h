#ifndef __szt_gpboc__h
#define __szt_gpboc__h


#define DataHead 0x00
#define BeginCode1 0x00
#define BeginCode2 0xff

#define EndCode 0x00


#define Year_Frist 0x20

typedef enum {
T_QueryStatus = 0x20, //状态查询
T_UserLogin = 0x40,	  //操作员登录
T_TimeCheck = 0x24,	  //时间同步
T_SztDeductOnce = 0x42,	 //初次发深圳通扣款
T_SztDeductAgain = 0x82, //再次发深圳通扣款
T_SztReadCardOnce = 0x68,  //初次读深圳通卡
T_SztReadCardAgain = 0x98, //再次读深圳通卡
T_SztUnlink = 0x22,		 //深圳通断开连接
T_SztCheck_In = 0x78,	  //深圳通自主签到、参数下载、数据上传
T_SztRechargeOnce = 0x54,	 //初次深圳通充值抵消
T_SztRechargeAgain = 0x94,	 //再次深圳通充值抵消
T_GpbocCheck_In = 0xC2,		 //银联签到
T_GpbocQueryReadCard = 0xC6,  //银联查询余额
T_GpbocDeduct = 0xC8,		  //银联扣款
T_GpbocSendData = 0xCA,		 //银联上送记录
T_GpbocCheck_Out = 0xCC,	 //银联批结、签退
T_M1Mutual =0x7A,		   //M1卡命令交互
T_Sleep = 0xAC,			   //休眠
T_Stop = 0xAE			   //关机
 
}SendOrderStatus;

typedef enum {
R_QueryStatus = 0x21,	  //状态查询
R_UserLogin = 0x41,		  //操作员登录
R_TimeCheck = 0x25,		  //时间同步
R_SztDeductOnce = 0x43,	   //初次发深圳通扣款
R_SztDeductAgain = 0x83,   //再次发深圳通扣款
R_SztReadCardOnce = 0x69,  //初次读深圳通卡
R_SztReadCardAgain = 0x99, //再次读深圳通卡
R_SztUnlink = 0x23,		  //深圳通断开连接
R_SztCheck_In = 0x79,	   //深圳通自主签到、参数下载、数据上传
R_SztRechargeOnce = 0x55,  //初次深圳通充值抵消
R_SztRechargeAgain = 0x95, //再次深圳通充值抵消
R_GpbocCheck_In = 0xC3,		//银联签到
R_GpbocReadCard = 0xC7,  //银联查询余额
R_GpbocDeduct = 0xC9,		 //银联扣款
R_GpbocSendData = 0xCB,		//银联上送记录
R_GpbocCheck_Out = 0xCD,   //银联批结、签退
R_M1Mutual =0x7B		     //M1卡命令交互
}RecOrderStatus;

typedef enum{
RxNo = 0,
RxOk = !RxNo
}RxStatus;

typedef enum{
NormalFlag = 0,
AckFlag =1,
DataFlag = 2
}Usart2DataStatus;

typedef enum{
IsNc = 0,
IsAck =1,
IsNack =2,
IsAckError =3
}AckReturnStatus;//Ack 返回状态

typedef enum{
DataNc = 0,	  //无返回数据
DataReOk =1,  //符合格式并完整
DataReErr =2   //返回数据不符合格式或不完整
}DataReStatus;// 返回数据状态


typedef struct {
uint8_t datahead;		//数据头
uint8_t startPacket[2]; //起始码
uint8_t dataLen[2];		//从saddr开始到Dcs之前所有的数据长度
uint8_t saddr[5];		//一般固定为06,FF,FF,FF,FF 
uint8_t taddr[5];		//一般以05开头，接下来的是sam卡上的卡号，先将十进制转换成十六进制，从低位开始读取
uint8_t startNum;		//开递增的值，一般为0
uint8_t SendCommand;	//发送的命令
uint8_t plen1[2];		//总共多少包数据，一般为1包，即00，01
uint8_t plen2[2];		//包编号,一般为00，01
uint8_t Dcs;			//校验码，其值加上从saddr开始到此值之前的所有值，结果为0
uint8_t dataend;		 //数据尾
}sendorderDef;


typedef struct{
uint8_t datahead;		  //数据头
uint8_t startParcket[2];  //起始码
uint8_t dataLen[2];		  //从saddr开始到Dcs之前所有的数据长度
uint8_t saddr[5];		  //终端读到的Sam卡号，以05开头，紧接Sam卡号
uint8_t taddr[5];		  //上位机传入的Sam卡号，以06开头，紧接Sam卡号
uint8_t startNum;		  //开始递增的数值，与接到的命令中的值相同，一般为00
uint8_t RevCommand;		  //该数据是什么命令返回的值
uint8_t plen1[2];		  //总共多少包数据，一般为1包，即00，01
uint8_t plen2[2];		  //包编号,一般为00，01
uint8_t resultNum;		  //错误代码
uint8_t Dcs;			  //校验码，其值加上从saddr开始到此值之前的所有值，结果为0
uint8_t dataend;		  //数据尾
}revDataDef;

typedef struct {
	uint8_t Device_ID[4];				//设备ID   4 BYTE BIN
	uint8_t Terminal_ID;			//深圳通终端编号  4 BYTE BIN
	uint8_t SoftWare_Ver[10];					//设备软件版本号  10 BYTE ASCII
	uint8_t Device_Status[4];			//设备状态字   4 BYTE BIN
	int32_t Curr_RemainMoney;		//当前剩余额度   4 BYTE BIN
	uint8_t Curr_ParFileName[28];				//当前额度参数名  28 BYTE ASCII
	uint16_t Curr_OKRecNum;			//当前未上传交易记录数  2 BYTE BIN
	uint16_t Curr_BadRecNum;		//当前未上传灰记录数  2 BYTE BIN
	uint8_t Curr_DeviceTime[8];		//当前设备时钟信息  8 BYTE BCD
}DeviceInf;	   //状态查询返回数据内容信息



typedef struct{
uint8_t Year_H;
uint8_t Year_L;
uint8_t Month;
uint8_t Day;
uint8_t Hours;
uint8_t Minutes;
uint8_t Senconds;
uint8_t WeekDay;
}MachineTimeDef;   //设备返回同步时间值


typedef struct{
uint8_t CardStatus;   //卡状态，正常卡为0x00
uint32_t CardNum;     //卡号
uint8_t CardType;	  //卡类型
int32_t SztMoney;	   //卡当前余额
int32_t MachinMoney;   //设备当前剩余额度
int32_t SztDeposit;	   //卡押金
uint8_t SellCardDay[4];	  //卡出售日期
uint8_t GetRentsMonth;	  //收租月份
int32_t RentsNum;		  //应收租金
int32_t ReturnableMoney;  //可退实际金额
int32_t Poundage;		  //退卡手续费
int32_t NoSellCardDeposit;	//未售卡押金
int32_t NoSellCardMoney;	//未售卡金额
}SztReadCardInfStr;   //深圳通读卡返回内容值



typedef struct{
int32_t BeginMoney;   //深圳通扣款前金额
int32_t EndMoney;     //深圳通扣款后金额
uint32_t CardNum;     //深圳通卡号
uint32_t TradeNum;    //交易号
uint32_t TACNum;       //TAC号
int32_t MachineAmount;   //设备授权额度
uint32_t machineNum;    //设备终端号
}SztReductInfStr;


typedef struct  {
        uint8_t   ucLogInLogVersion; //本结构体的版本号，可能以后会有不同版本、不同定义
        uint8_t   ucAcqBankNum; 	   //该签到对应的收单行号，自1计数 **
        uint8_t   ucLogInOK;         //该收单行签到成功了
        uint8_t   ucOper;            //操作员号
	    uint8_t   ucKeyMode;         //签到所用密钥类型，0x30, 代表双倍长；0x31,代表单倍长
        uint32_t   lBatchNumber;      /* 批次号*/
        uint8_t   szPosId[9];        /*  POS 终端号         ANS8    */
        uint8_t   szMerchantId[16];   //存储15字符长的商户号 ANS15
} GpbocInitInf;	 //银联初始化签到返回数据内容信息

typedef struct {
	uint8_t SztStatus;  //深圳通卡状态，0x00表示不有存在深圳通数据，后继8个字节全为0
	uint32_t SztNum;  //深圳通卡号
	int32_t SztMoney;  //深圳通卡余额
	uint8_t SztReserve[64];  //深圳通保留字
	uint8_t GpbocStatus;  //银联卡状态，0x00表示不有存在银联数据，后继24个字节全为0
	uint8_t GpbocNum[20];  //银联卡号
	int32_t GpbocMoney;  //银联卡余额
	uint8_t GpbocReserve[64];	//银联保留字
}GpbocReadCardInf;//读银联卡时，返回的数据内容，不包括m1卡内容


typedef struct  __attribute__ ((__packed__)) {
        uint8_t   iIccDataLen;            //长度内容两变量顺序与VA交易记录定义相反
        uint8_t   sIccData[255];          //55域内容
        uint8_t   bPanSeqNoOk;            // ADVT case 43 [3/31/2006 Tommy]
        uint8_t   ucPanSeqNo;             // App. PAN sequence number
        uint8_t   sAppCrypto[8];          // app. cryptogram, 9F26, already in F55
        uint8_t   sAuthRspCode[2];        //"Y1"
        uint8_t   sTVR[5];
        uint8_t   szAID[32+1];
        uint8_t   szAppLable[16+1];
        uint8_t   sTSI[2];
        uint8_t   sATC[2];
        uint8_t   sAIP[2];
        uint8_t   sCVR[4];
        uint8_t   szAppPreferName[16+1];
        uint8_t   szCardNoFull[21];         //较VAPOSLOG新加，未用*屏蔽前6后4之外的位置的全卡号
		uint8_t   szMerchantId[16];       //2013-04-23新加，方便上位机记录完整交易信息，存储15字符长的商户号 ANS15
} CRYPT_LOG_STRC;


//规划采用192字节(48个双字节块)RSA公钥对下面交易记录后加密部分CRYPT_LOG_STRC进行加密。每块实际可以PAYLOAD负载比密钥长度少1
#define LOG_RSAKey_BYTES        192
#define LOG_RSAKey_DWORDS       (192/4)
#define LOG_RSA_BLOCKLOAD_BYTES (LOG_RSAKey_BYTES-1)
#define LOGPUR_CRYPTDATA_BLOCKS ((sizeof(CRYPT_LOG_STRC)+LOG_RSA_BLOCKLOAD_BYTES-1)/LOG_RSA_BLOCKLOAD_BYTES)
#define LOGPUR_CRYPTDATA_BYTES  (LOGPUR_CRYPTDATA_BLOCKS * LOG_RSAKey_BYTES)

typedef struct  __attribute__ ((__packed__))   {
       // uint16_t  TransLogLen;      //本结构体所有数据的长度（含本域）
        uint8_t   ucLogVersion;      //本结构体的版本号，可能以后会有不同版本、不同定义 **
        uint8_t   ucAcqBankNum;      //该交易对应的收单行号，自1计数 
        uint32_t  iTransNo;       //交易号,实为交易类型，VA将EC固定为66
        uint8_t   ucBlklistFail;    //黑名单卡
        uint8_t   ucqPBOCSaleFail;  //非接交易失败
        uint8_t   ucqPBOCLastRd;    //读最后一条记录失败
        uint8_t   ucOfflineSaleFail;  //IC脱机拒绝

       // uint8_t   sAmount[6];   //较VAPOSLOG减少，用下面lTransAmount变量报告
        uint32_t  lTransAmount;   //较VAPOSLOG新加，用于向上位机报告交易金额
        uint32_t  lCardBalance;   //较VAPOSLOG新加，用于向上位机报告卡EC金额
        uint32_t  lTraceNo;       // 流水号
        uint32_t  lBatchNumber;   // 批次号

        uint8_t   szPosId[9];     //  POS 终端号          N(8)    
        uint8_t   szCardNo[21];   //传递给上位机时，需要用*屏蔽前6后4之外的位置
        uint8_t   szDate[9];
        uint8_t   szTime[7];
        uint8_t   szCardUnit[4];  //卡组织：CUP VIS MAS
        uint8_t   szExpDate[5];
        uint8_t   szTellerNo[4];  //操作员

        uint32_t  BkDataLen;      //备份交易数据的长度
        //下面为加密后灾备用的交易数据，上位机使用 ，下位机使用结构体StrucBkData
        
//		union {
//             CRYPT_LOG_STRC  StrucBkData;  //__attribute__ ((__packed__)) 
//             uint8_t   CryptBkData[LOGPUR_CRYPTDATA_BYTES];
//        } ;
		
		/*CRYPT_LOG_STRC  StrucBkData; //备份数据包*/
} TRANSLOG_TO_HOST;   //QPBOC消费记录

/*
SLIM_LOG_STRC qpbocslimlog;
Memcpy(&qpbocslimlog, DeductOutData + 4 , sizeof(TRANSLOG_TO_HOST));
*/


typedef struct  __attribute__ ((__packed__))   {
       uint8_t   ucUpLogVersion;    //本结构体的版本号，可能以后会有不同版本、不同定义
       uint8_t   ucAcqBankNum; 	   //该签到对应的收单行号，自1计数 **
       uint8_t   ucUploadOK;        //该收单行签到成功了
       uint32_t  lBatchNumber;      //批次号
       uint32_t  lTotalNum;         //需要上送文件中的总笔数
       uint32_t  lToUploadNum;      //真正需要上送的总笔数（有些失败交易等不能上送）
       uint32_t  lSuccNum;          //成功上送的笔数
       uint32_t  lSuccTotalAmt;     //成功上送的总金额
       uint32_t  lFailNum;          //上送失败的笔数
       uint32_t  lFailTotalAmt;     //上送失败的总金额
       uint8_t   szPosId[9];        //POS 终端号  ANS8   
       uint8_t   szMerchantId[16];   //存储15字符长的商户号 ANS15
} UPLOADLOG_TO_HOS;	 //银联上传数据返回内容解析包



typedef struct  __attribute__ ((__packed__))   {
      uint8_t   ucLogInLogVersion; //本结构体的版本号，可能以后会有不同版本、不同定义
      uint8_t   ucAcqBankNum; 	   //该签到对应的收单行号，自1计数 **
      uint8_t   ucLogInOK;         //该收单行签到成功了
      uint8_t   ucOper;            //操作员号
	    uint8_t   ucKeyMode;         //签到所用密钥类型，0x30, 代表双倍长；0x31,代表单倍长
      uint32_t  lBatchNumber;      /* 批次号*/
      uint8_t   szPosId[9];        /*  POS 终端号         ANS8    */
      uint8_t   szMerchantId[16];   //存储15字符长的商户号 ANS15
} LOGINLOG_TO_HOST;  //银联批结、签退返回内容解析包

extern SztReductInfStr SztReductInf;	//深圳通扣款返回值
extern TRANSLOG_TO_HOST UpDeductData;	//银联扣款交易内容
extern SztReductInfStr SztReductInf;	//深圳通扣款返回值
extern GpbocInitInf UpInitData[10];
void Uart2_Configuration(void);
void Open_Szt_Gpboc(void);
void Close_Szt_Gpboc(void);
void return_ACK(void);
void return_NACK(void);
void SztFixData(uint8_t LenH,uint8_t LenL,uint8_t OrderNum);
void GpbocFixData(uint8_t LenH,uint8_t LenL,uint8_t OrderNum);
uint8_t ManageACK(void);
uint8_t ManageRxData(void);
uint8_t ManageReOrderType(void);

void Order_Ifn_QueryStatus(void);
void Order_SztUserLogin(void);
void Order_SztTimeCheck(void);
void Order_SztDeductOnce(int32_t money);
void Order_SztDeductAgain(int32_t money);
void Order_SztReadCardOnce(void);
void Order_SztReadCardAgain(void);
void Order_SztUnlink(void);
void Order_SztAutoCheckIn(void);
void Order_SztAutoDownload(void);
void Order_SztAutoSendData(void);
void Order_SztRechargeOnce(void);
void Order_SztRechargeAgain(void);

void Order_Gpboc_Init(void);
void Order_Gpboc_ReadCard (void);
void Order_Gpboc_Deduct(uint32_t submoney);
void Order_Gpboc_SendData(void);
void Order_Gpboc_Check_Out(void);

uint8_t ManageQueryStatus(void);
uint8_t ManageSztUserLogin(void);
uint8_t ManageSztTimeCheck(void);
uint8_t ManageSztUnlink(void);
uint8_t ManageSztAutoManage(void);
uint8_t ManageSztReadCardOnce(void);
uint8_t ManageSztReadCardAgain(void);
uint8_t ManageSztDeductOnce(void);
uint8_t ManageSztDeductAgain(void);
uint8_t ManageSztRechargeOnce(void);
uint8_t ManageSztRechargeAgain(void);

uint8_t ManageGpbocInit(void);
uint8_t ManageGpbocDeduct(void);
uint8_t ManageGpbocReadCard(void);
uint8_t ManageGpbocSendData(void);
uint8_t ManageGpbocCheckOut(void);

void ManageGpbocAll(uint8_t PutInNum);

uint8_t Gpboc_CheckIn (void);
uint8_t OnlyReadGpbocCard(void);
uint8_t GpbocDeduct(uint32_t money_deduct);
uint8_t UpperGpboc(void);
uint8_t GpbocVout(void);

void TestSzt(uint8_t PutInNum);

uint8_t Szt_CheckIn(void);
uint8_t SztDeduct(int32_t money);
uint8_t SztAutoSend(void);

void Szt_GpbocAutoCheckIn(void);

uint8_t  BCDtoDec(uint8_t bcd);
uint8_t DectoBCD(uint8_t Dec); 







#endif
