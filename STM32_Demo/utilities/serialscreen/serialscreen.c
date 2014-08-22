 /**
  ******************************************************************************
  * 文件: serialscreen.c
  * 作者: jackey
  * 版本: V1.0.0
  * 描述: DWIN 串口屏通信程序，
  *  
  ******************************************************************************
	**/
#include <string.h>
#include "stm32f4xx.h"
#include "serialscreen.h"
#include "uart3.h"
#include "bsp.h"
#include "MsgHandle.h"

#define Null 0x00
typedef struct DispMeal
{
	char meal_id;
	char meal_cnt;
	char meal_cost;
}DispMealStuct;

DispMealStuct DispMeal[4]={0};

char price=0;
const char price_1st= 16;
const char price_2nd= 16;
const char price_3rd= 16;
const char price_4th= 16;
const char price_5th= 18;
const char price_6th= 18;
const char price_7th= 16;
const char price_8th= 16;

const char    mealname_1st[12]= {"秘制猪手饭"}; 
const char    mealname_2nd[14]= {"潮氏卤鸡腿饭"};
const char    mealname_3rd[14]= {"特色稻香肉饭"};
const char    mealname_4th[12]= {"黑椒猪扒饭"};
const char    mealname_5th[12]= {"蒲烧鲷鱼饭"};
const char    mealname_6th[14]= {"蒲烧秋刀鱼饭"};
const char    mealname_7th[12]= {"咖喱鸡扒饭"};
const char    mealname_8th[12]= {"梅菜扣肉饭"};

uint8_t Menu_interface= 0x00;
uint8_t MealSet_interface= 0x00;
uint8_t sell_type[4]={0};//存储四个菜品的的ID,在签到的时候需要获取的有当前需要显示售卖的哪一个界面和那几个餐品
uint8_t sell_type_1st[4]={0x01,0x02,0x03,0x04}; //第一个售餐菜单
uint8_t sell_type_2nd[4]={0x05,0x06,0x07,0x08}; //第二个售餐菜单
uint8_t sell_type_3rd[4]={0x03,0x04,0x05,0x06}; //第三个售餐菜单
uint8_t sell_type_4th[4]={0x05,0x06,0x01,0x02}; //第四个售餐菜单
uint8_t sell_type_5th[4]={0x05,0x06,0x01,0x02}; //第五个售餐菜单
/*将数据16位存储器地址分解为2个8位数据*/
union ScreenRam
{
		int16_t	 	adress ;
		int8_t  	adr[2] ;
}myunion;

/*将数据16位存储器地址分解为2个8位数据*/
union ScreenRamLong
{
		uint32_t	 	adress ;
		int8_t  	adr[4] ;
}myunion_long;


/*写指定寄存器帧*/
const unsigned char RegisterWrite[7]={ 
							FH0 , FH1 , /*帧头2字节 */
							0x04 , /*长度 包括命令和数据*/
              0x80 , /*指定地址写寄存器数据*/
							ADR  , /*寄存器地址*/
							0x00 ,0x00 /*数据*/
							//0x00 , 0x00	/*CRC校验侦尾*/
							};

/*读指定寄存器帧*/
const unsigned char RegisterRead[6]={
							FH0 , FH1 , /*帧头2字节 */
							0x03 , /*长度 包括命令和数据*/
              0x81 ,  /*指定地址读寄存器数据*/
							ADR  , /*寄存器地址*/
							RD_LEN , /*长度*/
							//0x00 , 0x00	/*CRC校验侦尾*/	
							};
	
/*写变量存储区帧*/							
const unsigned char VariableWrite[8]={
							FH0 , FH1 , /*帧头2字节 */
							0x00 , /*长度 包括命令和数据*/	
							0x82 , /*指定地址开始写入数据串(字数据)到变量存储区*/
							0x00 , 0x00 ,	/*变量存储空间2字节*/
							0x00 , 0x00   /*数据*/
							//0x00 , 0x00  	/*CRC校验侦尾*/
							}; 

/*读变量存储区帧*/	
const unsigned char VariableRead[7]={
							FH0 , FH1 , /*帧头2字节 */
							0x04 , /*长度 包括命令和数据*/
							0x83 , /*从变量存储区指定地址开始读入RD_LEN长度字数据*/
							0x00 , 0x00 ,	/*变量存储空间2字节*/
							RD_LEN , /*长度*/
							//0x00 , 0x00  	/*CRC校验侦尾*/
							};	

const char ABC[20]={
							FH0 , FH1 , /*帧头2字节 */
							0x05 , /*长度 包括命令和数据*/	
							0x82 , /*指定地址开始写入数据串(字数据)到变量存储区*/
							0x40 , 0x00 ,	/*变量存储空间2字节*/
							0xBA , 0xA0   /*数据*/
              };	
const unsigned char rtc_write[13]={
  						FH0 , FH1 , /*帧头2字节 */
							0x0A , /*长度 包括命令和数据*/	
							0x80 , /*指定地址开始写入数据串(字数据)到变量存储区*/
							0x1F , 0x5A ,	/*变量存储空间2字节*/
							0x00 , 0x00 ,0X00 ,   /*日期*/
              0X00 ,/*星期*/
              0x00 , 0x00 ,0x00 /*时间*/
              };
#include "stdio.h"

char *mystrcat(char *dest, const char *src, char length)
{
  int i=0;
	for(i=0;i<length;i++)
	{
		dest[6+i] = src[i];
	}
  return dest;
}


 /*******************************************************************************
 * 函数名称:VariableChage                                                                    
 * 描    述:改变指定变量的值                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void VariableChage(uint16_t Variable,uint16_t Value)							
{
	  unsigned char temp[8]={0};  //存放串口数据的临时指针
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 5;
		myunion.adress= Variable; 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		myunion.adress= Value; 
		temp[6]= myunion.adr[1];
		temp[7]= myunion.adr[0];		
		Uart3_Send(temp,sizeof(temp));	
}							

 /*******************************************************************************
 * 函数名称:VariableChagelong                                                                    
 * 描    述:改变指定变量的值                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void VariableChagelong (uint16_t Variable,uint32_t Value)							
{
	  unsigned char temp[10]={0};  //存放串口数据的临时指针
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 7;
		myunion.adress= Variable; 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		myunion_long.adress= Value;
		temp[6]= myunion_long.adr[3];
		temp[7]= myunion_long.adr[2];					
		temp[8]= myunion_long.adr[1];
		temp[9]= myunion_long.adr[0];		
		Uart3_Send(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:RegisterChage                                                                   
 * 描    述:改变指定变量的值                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void RegisterChage(uint8_t Register,uint16_t Value)							
{
	  unsigned char temp[8]={0};  //存放串口数据的临时指针
		memcpy(temp,RegisterWrite,sizeof(RegisterWrite));
		temp[3]= Register;
		myunion.adress=Value;
		temp[5]= myunion.adr[1];
		temp[6]= myunion.adr[0];		
		Uart3_Send(temp,sizeof(temp));	
}	

 /*******************************************************************************
 * 函数名称:RegisterChage                                                                   
 * 描    述:改变指定变量的值                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void ScreenControl(char cmd)							
{
	  unsigned char temp[8]={0};  //存放串口数据的临时指针
		memcpy(temp,RegisterWrite,sizeof(RegisterWrite));
		temp[3]= TPC_Enable;
		temp[5]= cmd ;	//0x00 关闭
		Uart3_Send(temp,sizeof(temp));	
}	
 /*******************************************************************************
 * 函数名称:MealNameDisp                                                                  
 * 描    述:显示餐品名称                                                          
 *           根据ID 显示名称 ，根据floor 改变现实变量的地址                                                                    
 * 输    入:meal_id ,floor                                                               
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void MealNameDisp(uint8_t meal_id,uint8_t floor)							
{
	  char temp[30]={0};  //存放串口数据的临时数组
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 17; //0x83 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= column1st_name+floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		switch(meal_id)
		{
			case 0x00:break;
			case 0x01:mystrcat(temp,mealname_1st,12);break;	
			case 0x02:mystrcat(temp,mealname_2nd,14);break;		
			case 0x03:mystrcat(temp,mealname_3rd,14);break;	
			case 0x04:mystrcat(temp,mealname_4th,12);break;
			case 0x05:mystrcat(temp,mealname_5th,12);break;
			case 0x06:mystrcat(temp,mealname_6th,14);break;      
			case 0x07:mystrcat(temp,mealname_7th,12);break;
			case 0x08:mystrcat(temp,mealname_8th,12);break;     
			default:break;			
		}
		Uart3_Sent(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:MealCntDisp                                                                 
 * 描    述:显示餐品数量                                                          
 *           更具floor 改变变量 ，meal_cnt 是要现实的数量                                                                    
 * 输    入:meal_id                                                                
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void MealCntDisp(uint8_t meal_cnt,uint8_t floor)							
{
	  char temp[20]={0};  //存放串口数据的临时数组
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 13; //0x83 0x00 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= column1st_count+floor*3; 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
    temp[7]= meal_cnt;
		Uart3_Sent(temp,sizeof(temp));	
}

 /*******************************************************************************
 * 函数名称:MealCostDisplay                                                                
 * 描    述:显示餐品数量                                                          
 *           更具floor 改变变量 ，meal_cnt 是要现实的数量                                                                    
 * 输    入:meal_id                                                                
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void MealCostDisplay(uint8_t meal_cost,uint8_t floor)							
{
	  char temp[20]={0};  //存放串口数据的临时数组
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 13; //0x83 0x00 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= column1st_cost+floor*3; 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
    temp[7]= meal_cost;
		Uart3_Sent(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:DisplayPassWord                                                                 
 * 描    述:显示密码长度以*表示                                                     
 *                                                                               
 * 输    入:PassWordLen                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void DisplayPassWord(char PassWordLen)
{
	char i=0;
	char temp[20]={0};  //存放串口数据的临时数组
	memcpy(temp,VariableWrite,sizeof(VariableWrite));
	temp[2]= PassWordLen+ 4; //  0x83 00 00 ****** 如果无密码显示则清空数据
	myunion.adress= password_show; 
  temp[4]= myunion.adr[1];
  temp[5]= myunion.adr[0];
	for(i=0;i<PassWordLen;i++) temp[i+6]='*';
	Uart3_Sent(temp,sizeof(temp)); 
}	
 /*******************************************************************************
 * 函数名称:DisplayAbnormal                                                               
 * 描    述:显示错误码                                                   
 *                                                                               
 * 输    入:abnomal_code 字符串                                                                  
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void DisplayAbnormal(char *abnomal_code)
{
	char i=0;
	char temp[20]={0};  //存放串口数据的临时数组
	memcpy(temp,VariableWrite,sizeof(VariableWrite));
	temp[2]= 7; //  数据长度为4位
	myunion.adress= erro_num; 
  temp[4]= myunion.adr[1];
  temp[5]= myunion.adr[0];
	for(i=0;i<4;i++) temp[i+6]=abnomal_code[i];
	Uart3_Sent(temp,sizeof(temp)); 	
}


 /*******************************************************************************
 * 函数名称:PageChange                                                                     
 * 描    述:发送切换图片命令                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 	
char pageunitil= 0;
void PageChange(char page)
{
		unsigned char temp[7]={0};
		memcpy(temp,RegisterWrite,sizeof(RegisterWrite));	
		temp[4]=	PIC_ID;	
	  temp[6]=  page;
		pageunitil = page;
		Uart3_Send(temp,sizeof(temp));
}
 /*******************************************************************************
 * 函数名称:ReadPage                                                                     
 * 描    述:读取当前页,数据处理在DealSeriAceptData中处理                                                           
 *                                                                               
 * 输    入:无                                                                    
 * 输    出:                                                                     
 * 返    回:                                                             
 * 修改日期:2014年6月24日                                                                    
 *******************************************************************************/ 	
void ReadPage(void)
{
		unsigned char temp[6]={0};
		memcpy(temp,RegisterRead,sizeof(RegisterRead));	
		temp[4]=	PIC_ID;	
		Uart3_Send(temp,sizeof(temp));
}

 /*******************************************************************************
 * 函数名称:MenuChange                                                                     
 * 描    述:根据返回的数据更新所选的菜单                                                         
 *                                                                               
 * 输    入:无                                                                    
 * 输    出:                                                                     
 * 返    回:                                                             
 * 修改日期:2014年6月24日                                                                    
 *******************************************************************************/ 	
void MenuChange(uint8_t MenuNO)
{
  switch(MenuNO)
  {
    case 0x00:
    {
      PageChange(Logo_interface);
      while(1);
    }break;
    case 0x01:
    {
      Menu_interface= Menu1st_interface;
      MealSet_interface= MealSet1st_interface;
      memcpy(sell_type,sell_type_1st,4);
    }break;
    case 0x02:
    {
      Menu_interface= Menu2nd_interface;
      MealSet_interface= MealSet2nd_interface;  
      memcpy(sell_type,sell_type_2nd,4);      
    }break;
    case 0x03:
    {
      Menu_interface= Menu3rd_interface;
      MealSet_interface= MealSet3rd_interface;   
      memcpy(sell_type,sell_type_3rd,4);        
    }break;
    case 0x04:
    {
      Menu_interface= Menu4th_interface;
      MealSet_interface= MealSet4th_interface;  
      memcpy(sell_type,sell_type_4th,4);         
    }break;
    case 0x05:
    {
      Menu_interface= Menu5th_interface;
      MealSet_interface= MealSet5th_interface;   
      memcpy(sell_type,sell_type_5th,4);     
    }break; 
    default:break;    
  }
}
 /*******************************************************************************
 * 函数名称:SetScreenRtc                                                                    
 * 描    述:读取当前页,数据处理在DealSeriAceptData中处理                                                           
 *                                                                               
 * 输    入:无                                                                    
 * 输    出:                                                                     
 * 返    回:                                                             
 * 修改日期:2014年6月24日                                                                    
 *******************************************************************************/ 	
void SetScreenRtc(void)
{
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_DateTypeDef RTC_DateStructure;
  unsigned char temp[13]={0};
  RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);
  memcpy(temp,rtc_write,13);
  temp[6]=  RTC_DateStructure.RTC_Year;
  temp[7]=  RTC_DateStructure.RTC_Month;
  temp[8]=  RTC_DateStructure.RTC_Date;
  temp[9]=  RTC_DateStructure.RTC_WeekDay;
  temp[10]= RTC_TimeStructure.RTC_Hours;
  temp[11]= RTC_TimeStructure.RTC_Minutes;
  temp[12]= RTC_TimeStructure.RTC_Seconds;
  Uart3_Send(temp,sizeof(temp));
}
 /*******************************************************************************
 * 函数名称:DispLeftMeal                                                                     
 * 描    述:在菜单选择界面显示各菜品所剩余的                                                           
 *                                                                               
 * 输    入:无                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 	

void DispLeftMeal(void)
{
		uint8_t i;
	  unsigned char temp[8]={0};  //存放串口数据的临时指针
		for(i=0;i<MealKindTotoal;i++)
    {		
      memcpy(temp,VariableWrite,sizeof(VariableWrite));
      temp[2]= 5;
      myunion.adress= meat+i; //在基地址推移位置
      temp[4]= myunion.adr[1];
      temp[5]= myunion.adr[0];
      temp[7]= DefineMeal[i].MealCount;//sell_type[i]存储的是四个菜品的ID
      Uart3_Send(temp,sizeof(temp));	
		}
}
 /*******************************************************************************
 * 函数名称:CutDownDisp                                                                     
 * 描    述:显示倒计时60-0                                                          
 *                                                                               
 * 输    入:time                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 	
void CutDownDisp(char time)
{
	  unsigned char temp[8]={0};
	  memcpy(temp,VariableWrite,sizeof(VariableWrite));
	  temp[2]= 5;
		myunion.adress= count_dowm; //在基地址推移位置
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
    temp[7]= time;	
		Uart3_Send(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:MealCostDisp                                                                     
 * 描    述:显示当前餐品应付的金额                                                          
 *                                                                               
 * 输    入:                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
unsigned char GetMealPrice(char meal_type,char count)
{
	char price= 0;
  switch(meal_type)
	{
		case 0x01:
		{
			price= price_1st*count;
		}break;
		case 0x02:
		{
      price= price_2nd*count;
		}break;
		case 0x03:
		{
      price= price_3rd*count;
		}break;
		case 0x04:
		{
      price= price_4th*count;
		}break;
		case 0x05:
		{
      price= price_5th*count;
		}break;
		case 0x06:
		{
      price= price_6th*count;
		}break;
		case 0x07:
		{
      price= price_7th*count;
		}break;
		case 0x08:
		{
      price= price_8th*count;
		}break;
    
		default:break;
	}
	return price;
}

 /*******************************************************************************
 * 函数名称:MealCostDisp                                                                     
 * 描    述:显示当前餐品应付的金额                                                          
 *                                                                               
 * 输    入:                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void MealCostDisp(char meal_id,char meal_count)
{
	  unsigned char temp[8]={0};
	  memcpy(temp,VariableWrite,sizeof(VariableWrite));
	  temp[2]= 5;
		temp[5]= (0x31+(meal_id-1)*2); //钱币变量地址
    temp[7]= GetMealPrice(meal_id,meal_count);	
		Uart3_Send(temp,sizeof(temp));		
}
 /*******************************************************************************
 * 函数名称:ClearUserBuffer                                                                     
 * 描    述:清除用户选择数据                                                          
 *                                                                               
 * 输    入:无                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void ClearUserBuffer(void)
{
	UserActMessageWriteToFlash.UserAct.MealCnt_1st=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_1st_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_2nd=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_3rd=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_4th=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_4th_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_5th=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_5th_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_6th=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_6th_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_7th=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_7th_t=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_8th=0;
	UserActMessageWriteToFlash.UserAct.MealCnt_8th_t=0;   
	UserActMessageWriteToFlash.UserAct.MealCost_1st=0;
	UserActMessageWriteToFlash.UserAct.MealCost_2nd=0;
	UserActMessageWriteToFlash.UserAct.MealCost_3rd=0;
	UserActMessageWriteToFlash.UserAct.MealCost_4th=0;
	UserActMessageWriteToFlash.UserAct.MealCost_5th=0;
	UserActMessageWriteToFlash.UserAct.MealCost_6th=0;  
  UserActMessageWriteToFlash.UserAct.MealCost_7th=0;  
  UserActMessageWriteToFlash.UserAct.MealCost_8th=0; 
	UserActMessageWriteToFlash.UserAct.MealID=0;
	UserActMessageWriteToFlash.UserAct.Meal_totoal=0;
	UserActMessageWriteToFlash.UserAct.Meal_takeout=0;
	UserActMessageWriteToFlash.UserAct.PayShould=0;
	UserActMessageWriteToFlash.UserAct.PayType=0;
  UserActMessageWriteToFlash.UserAct.PayForCoins=0;           //用户投入的硬币数	
	UserActMessageWriteToFlash.UserAct.PayForBills=0;           //用户投入的纸币数
	UserActMessageWriteToFlash.UserAct.PayForCards=0;           //用户应经刷卡的数
  UserActMessageWriteToFlash.UserAct.PayAlready=0;
  UserActMessageWriteToFlash.UserAct.MoneyBackShould=0;
  UserActMessageWriteToFlash.UserAct.MoneyBackAlready=0;
  UserActMessageWriteToFlash.UserAct.MoneyBack=0;
  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already=0;
  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_1st=0;
  UserActMessageWriteToFlash.UserAct.MoneyPayBack_Already_2nd=0;  
}
 /*******************************************************************************
 * 函数名称:PutIntoShopCart                                                                     
 * 描    述:在选产界面将点击放入购物车之后的程序响应                                                        
 *                                                                               
 * 输    入:                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 
void PutIntoShopCart(void)
{
	switch(UserActMessageWriteToFlash.UserAct.MealID)
	{
		case 0x01:
		{
			UserActMessageWriteToFlash.UserAct.MealCnt_1st= UserActMessageWriteToFlash.UserAct.MealCnt_1st_t;
			UserActMessageWriteToFlash.UserAct.MealCost_1st = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_1st);
		}break;
		case 0x02:
		{
			UserActMessageWriteToFlash.UserAct.MealCnt_2nd=UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t;
			UserActMessageWriteToFlash.UserAct.MealCost_2nd = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_2nd);
		}break;
		case 0x03:
		{
			UserActMessageWriteToFlash.UserAct.MealCnt_3rd=UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t;
			UserActMessageWriteToFlash.UserAct.MealCost_3rd = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_3rd);
		}break;
		case 0x04:
		{
			UserActMessageWriteToFlash.UserAct.MealCnt_4th=UserActMessageWriteToFlash.UserAct.MealCnt_4th_t;
			UserActMessageWriteToFlash.UserAct.MealCost_4th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_4th);
		}break;
		case 0x05:
		{
			UserActMessageWriteToFlash.UserAct.MealCnt_5th=UserActMessageWriteToFlash.UserAct.MealCnt_5th_t;
			UserActMessageWriteToFlash.UserAct.MealCost_5th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_5th);
		}break;
		case 0x06:
		{
			UserActMessageWriteToFlash.UserAct.MealCnt_6th=UserActMessageWriteToFlash.UserAct.MealCnt_6th_t;
			UserActMessageWriteToFlash.UserAct.MealCost_6th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_6th);
		}break;
		case 0x07:
		{
			UserActMessageWriteToFlash.UserAct.MealCnt_7th=UserActMessageWriteToFlash.UserAct.MealCnt_7th_t;
			UserActMessageWriteToFlash.UserAct.MealCost_7th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_7th);
		}break;
		case 0x08:
		{
			UserActMessageWriteToFlash.UserAct.MealCnt_8th=UserActMessageWriteToFlash.UserAct.MealCnt_8th_t;
			UserActMessageWriteToFlash.UserAct.MealCost_8th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_8th);
		}break;
		default:break;	
	}
	//PageChange((UserActMessageWriteToFlash.UserAct.MealID-1)*3+6); //不知道这个是否出问题???
	UserActMessageWriteToFlash.UserAct.Meal_totoal= UserActMessageWriteToFlash.UserAct.MealCost_8th+UserActMessageWriteToFlash.UserAct.MealCnt_7th+UserActMessageWriteToFlash.UserAct.MealCost_6th+UserActMessageWriteToFlash.UserAct.MealCnt_5th+
                                                  +UserActMessageWriteToFlash.UserAct.MealCost_4th+UserActMessageWriteToFlash.UserAct.MealCnt_3rd+UserActMessageWriteToFlash.UserAct.MealCnt_2nd+UserActMessageWriteToFlash.UserAct.MealCnt_1st;
}	

 /*******************************************************************************
 * 函数名称:SettleAccounts                                                                 
 * 描    述:当按下结算按钮后的程序流程                                                                                                                        
 * 输    入:无                                                                 
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                   
 *******************************************************************************/ 
char Floor= 0; //作为有商品种类的层数
void SettleAccounts(void)
{
	uint8_t tempcnt=0;
	//清除屏幕显示数据
  for(tempcnt=0;tempcnt<4;tempcnt++)
	{
		MealNameDisp(Null,tempcnt);
		MealCntDisp(Null,tempcnt);//显示数量
		MealCostDisplay(Null,tempcnt);//显示单总价 有两个重复		
	}
  for(Floor=0;Floor<4;Floor++)
	{
		DispMeal[Floor].meal_id= 0x00; //0x01为第一个餐品的ID
		DispMeal[Floor].meal_cnt= 0x00; //餐品的数量赋值
		DispMeal[Floor].meal_cost= 0x00; //餐品单总价的赋值
	}
	Floor= 0;
	tempcnt=0; /*计数清零*/
  PageChange(Acount_interface);//结算界面
	//在这里做标记 current = payformoney
  //显示硬币数
	VariableChage(payment_coin,UserActMessageWriteToFlash.UserAct.PayForCoins);
  //显示纸币数
	VariableChage(payment_bill,UserActMessageWriteToFlash.UserAct.PayForBills);
  //刷卡数
	VariableChage(payment_card,UserActMessageWriteToFlash.UserAct.PayForCards);
  //显示等待时间
	VariableChage(wait_payfor,60);
	OpenTIM7();
	//程序显示部分
	Floor= 0; //每次进一次结账界面就需要从新写入数据
	
	if(UserActMessageWriteToFlash.UserAct.MealCnt_1st>0)
	{
		DispMeal[Floor].meal_id= 0x01; //0x01为第一个餐品的ID
		DispMeal[Floor].meal_cnt= UserActMessageWriteToFlash.UserAct.MealCnt_1st; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserActMessageWriteToFlash.UserAct.MealCost_1st; //餐品单总价的赋值
		Floor++;
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_2nd>0)
	{
		DispMeal[Floor].meal_id= 0x02; //0x02为第二个餐品的ID
		DispMeal[Floor].meal_cnt= UserActMessageWriteToFlash.UserAct.MealCnt_2nd; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserActMessageWriteToFlash.UserAct.MealCost_2nd; //餐品单总价的赋值
		Floor++;
	}		
  if(UserActMessageWriteToFlash.UserAct.MealCnt_3rd>0)
	{
		DispMeal[Floor].meal_id= 0x03; //0x03为第三个餐品的ID
		DispMeal[Floor].meal_cnt= UserActMessageWriteToFlash.UserAct.MealCnt_3rd; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserActMessageWriteToFlash.UserAct.MealCost_3rd; //餐品单总价的赋值
		Floor++;		
	}		
	if(UserActMessageWriteToFlash.UserAct.MealCnt_4th>0)
	{
		DispMeal[Floor].meal_id= 0x04; //0x01为第四个餐品的ID
		DispMeal[Floor].meal_cnt= UserActMessageWriteToFlash.UserAct.MealCnt_4th; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserActMessageWriteToFlash.UserAct.MealCost_4th; //餐品单总价的赋值
		Floor++;	
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_5th>0)
	{
		DispMeal[Floor].meal_id= 0x05; //0x01为第四个餐品的ID
		DispMeal[Floor].meal_cnt= UserActMessageWriteToFlash.UserAct.MealCnt_5th; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserActMessageWriteToFlash.UserAct.MealCost_5th; //餐品单总价的赋值
		Floor++;	
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_6th>0)
	{
		DispMeal[Floor].meal_id= 0x06; //0x01为第四个餐品的ID
		DispMeal[Floor].meal_cnt= UserActMessageWriteToFlash.UserAct.MealCnt_6th; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserActMessageWriteToFlash.UserAct.MealCost_6th; //餐品单总价的赋值
		Floor++;	
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_7th>0)
	{
		DispMeal[Floor].meal_id= 0x07; //0x01为第四个餐品的ID
		DispMeal[Floor].meal_cnt= UserActMessageWriteToFlash.UserAct.MealCnt_7th; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserActMessageWriteToFlash.UserAct.MealCost_7th; //餐品单总价的赋值
		Floor++;	
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_8th>0)
	{
		DispMeal[Floor].meal_id= 0x08; //0x01为第四个餐品的ID
		DispMeal[Floor].meal_cnt= UserActMessageWriteToFlash.UserAct.MealCnt_8th; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserActMessageWriteToFlash.UserAct.MealCost_8th; //餐品单总价的赋值
		Floor++;	
	}
	//加入显示的部分：从第一栏开始到第四栏
	for(tempcnt=0;tempcnt<Floor;tempcnt++)
	{
		MealNameDisp(DispMeal[tempcnt].meal_id,tempcnt);//显示菜名  
		MealCntDisp(DispMeal[tempcnt].meal_cnt,tempcnt);//显示数量
		MealCostDisplay(DispMeal[tempcnt].meal_cost,tempcnt);//显示单总价 有两个重复
	}
  if(Floor>4) while(1);/*如果用户选择的餐品大于4个直接报错*/
	UserActMessageWriteToFlash.UserAct.PayShould= (UserActMessageWriteToFlash.UserAct.MealCost_1st+UserActMessageWriteToFlash.UserAct.MealCost_2nd+UserActMessageWriteToFlash.UserAct.MealCost_3rd+UserActMessageWriteToFlash.UserAct.MealCost_4th
                                                +UserActMessageWriteToFlash.UserAct.MealCost_5th+UserActMessageWriteToFlash.UserAct.MealCost_6th+UserActMessageWriteToFlash.UserAct.MealCost_7th+UserActMessageWriteToFlash.UserAct.MealCost_8th);
	VariableChage(mealtotoal_cost,UserActMessageWriteToFlash.UserAct.PayShould);
}

 /*******************************************************************************
 * 函数名称:SyncMealNameDisp                                                                  
 * 描    述:数据同步时显示餐品名称                                                          
 *           根据ID 显示名称 ，根据floor 改变现实变量的地址                                                                    
 * 输    入:meal_id ,floor                                                               
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void SyncMealNameDisp(uint8_t meal_id,uint8_t floor)							
{
	  char temp[30]={0};  //存放串口数据的临时数组
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 17; //0x83 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= sync_column1st_name+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		switch(meal_id)
		{
			case 0x00:break;
			case 0x01:mystrcat(temp,mealname_1st,12);break;	
			case 0x02:mystrcat(temp,mealname_2nd,14);break;		
			case 0x03:mystrcat(temp,mealname_3rd,14);break;	
			case 0x04:mystrcat(temp,mealname_4th,12);break;
			case 0x05:mystrcat(temp,mealname_5th,12);break;
			case 0x06:mystrcat(temp,mealname_6th,14);break;
			case 0x07:mystrcat(temp,mealname_7th,12);break;
			case 0x08:mystrcat(temp,mealname_8th,12);break;      
			default:break;			
		}
		Uart3_Sent(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:SyncMealCntDisp                                                                 
 * 描    述:数据同步时显示餐品数量  ，以文本方式显示                                                        
 *           更具floor 改变变量 ，meal_cnt 是要现实的数量                                                                    
 * 输    入:meal_id                                                                
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void SyncMealCntDisp(uint8_t meal_cnt,uint8_t floor)							
{
	  char temp[20]={0};  //存放串口数据的临时数组
		char buffer[5]={0};
		int cnt_t=0,i=0;
		cnt_t= sprintf(buffer,"%d",meal_cnt);
		if(meal_cnt==0xff) cnt_t=0;
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 4+cnt_t; //0x83 0x00 0x41 0x00 0x00 0x00 0x00 
		myunion.adress= sync_column1st_number+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		for(i=0;i<cnt_t;i++) temp[i+6]=buffer[i]; 
		Uart3_Sent(temp,sizeof(temp));	
}

 /*******************************************************************************
 * 函数名称:AbnomalMealNameDisp                                                                  
 * 描    述:错误记录时显示餐品名称                                                          
 *           根据ID 显示名称 ，根据floor 改变现实变量的地址                                                                    
 * 输    入:meal_id ,floor                                                               
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月9日                                                                    
 *******************************************************************************/ 							
void AbnomalMealNameDisp(uint8_t meal_id,uint8_t floor)							
{
	  char temp[30]={0};  //存放串口数据的临时数组
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 17; //0x83 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= record_column1st_name+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		switch(meal_id)
		{
			case 0x00:break;
			case 0x01:mystrcat(temp,mealname_1st,12);break;	
			case 0x02:mystrcat(temp,mealname_2nd,14);break;		
			case 0x03:mystrcat(temp,mealname_3rd,14);break;	
			case 0x04:mystrcat(temp,mealname_4th,12);break;
			case 0x05:mystrcat(temp,mealname_5th,12);break;
			case 0x06:mystrcat(temp,mealname_6th,14);break;
			case 0x07:mystrcat(temp,mealname_7th,12);break;
			case 0x08:mystrcat(temp,mealname_8th,12);break;      
			default:break;			
		}
		Uart3_Sent(temp,sizeof(temp));	
}

/*******************************************************************************
 * 函数名称:AbnomalMealCnttDisp                                                                 
 * 描    述:错误记录时显示餐品数量  ，以文本方式显示                                                        
 *           更具floor 改变变量 ，meal_cnt 是要现实的数量                                                                    
 * 输    入:meal_id                                                                
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void AbnomalMealCnttDisp(uint8_t meal_cnt,uint8_t floor)							
{
	  char temp[20]={0};  //存放串口数据的临时数组
		char buffer[5]={0};
		int cnt_t=0,i=0;
		cnt_t= sprintf(buffer,"%d",meal_cnt);
		if(meal_cnt==0xff) cnt_t=0;
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 4+cnt_t; //0x83 0x00 0x41 0x00 0x00 0x00 0x00 
		myunion.adress= record_column1st_cnt_t+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		for(i=0;i<cnt_t;i++) temp[i+6]=buffer[i]; 
		Uart3_Sent(temp,sizeof(temp));	
}

/*******************************************************************************
 * 函数名称:AbnomalMealCntDisp                                                                 
 * 描    述:错误记录时显示餐品数量  ，以文本方式显示                                                        
 *           更具floor 改变变量 ，meal_cnt 是要现实的数量                                                                    
 * 输    入:meal_id                                                                
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void AbnomalMealCntDisp(uint8_t meal_cnt,uint8_t floor)							
{
	  char temp[20]={0};  //存放串口数据的临时数组
		char buffer[5]={0};
		int cnt_t=0,i=0;
		cnt_t= sprintf(buffer,"%d",meal_cnt);
		if(meal_cnt==0xff) cnt_t=0;
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 4+cnt_t; //0x83 0x00 0x41 0x00 0x00 0x00 0x00 
		myunion.adress= record_column1st_cnt+ floor*(0x0100); 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		for(i=0;i<cnt_t;i++) temp[i+6]=buffer[i]; 
		Uart3_Sent(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:DisplayTimeCutDown                                                                    
 * 描    述:显示离售餐时间的倒计时                                                            
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年7月22日                                                                    
 *******************************************************************************/ 
uint32_t sellsecond_remain=0;
void DisplayTimeCutDown(void)
{
  uint32_t sellsecond_totoal=0,realsecond_totoal=0;
	RTC_TimeShow();	//获取当前的时间
  sellsecond_totoal= selltime_hour*3600+selltime_minute*60; //设置售餐的时间换位秒
  realsecond_totoal= TimeDate.Hours*3600+TimeDate.Minutes*60+TimeDate.Senconds; //当前时间转换为秒
  if(sellsecond_totoal>realsecond_totoal)  
  {
    sellmeal_flag= false;
    sellsecond_remain = sellsecond_totoal-realsecond_totoal;
    selltime_hour_r= sellsecond_remain/3600;
    selltime_minute_r= (sellsecond_remain%3600)/60;
    selltime_second_r= (sellsecond_remain%3600)%60;
    VariableChage(wait_sellmeal_hour,selltime_hour_r);
    VariableChage(wait_sellmeal_minute,selltime_minute_r);
    VariableChage(wait_sellmeal_second,selltime_second_r);
    PageChange(SellMeal_TimeWait_interface); 
    OpenTIM4(); 
  }
  else
  {
    sellsecond_remain=0;
    CloseTIM4();
    sellmeal_flag= true;
    PageChange(Menu_interface); 
  }
}
 /*******************************************************************************
 * 函数名称:DisplayRecordTime                                                                     
 * 描    述:显示出错时的错误记录                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月23日                                                                    
 *******************************************************************************/  
char record_time[20]={"20yy-mm-dd hh:mm:ss"};
void DisplayRecordTime(void)
{ 
	  char temp[30]={0};  //存放串口数据的临时数组  
		RTC_TimeShow();     /*时间作为错误记录时间*/
		record_time[0] =    20/10+'0'; 
		record_time[1] =    20%10 +'0'; 
		record_time[2] =    TimeDate.Year/10+ '0'; 
		record_time[3] =    TimeDate.Year%10+ '0' ;
		record_time[5] =    TimeDate.Month/10+ '0';
		record_time[6] =    TimeDate.Month%10+ '0';
		record_time[8] =    TimeDate.Date/10+ '0'; 
		record_time[9] =    TimeDate.Date%10+ '0';
		record_time[11] =   TimeDate.Hours/10+ '0';
		record_time[12] =   TimeDate.Hours%10+ '0';
		record_time[14] =   TimeDate.Minutes/10+ '0';
		record_time[15] =   TimeDate.Minutes%10+ '0' ;
		record_time[17] =   TimeDate.Senconds/10+ '0'; 
		record_time[18] =   TimeDate.Senconds%10+ '0';	
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 23; //22 0x83 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= record_UserDataTime;
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		mystrcat(temp,record_time,20);
		Uart3_Sent(temp,sizeof(temp));	
}
 /*******************************************************************************
 * 函数名称:DisplayUserRecord                                                                     
 * 描    述:显示出错时的错误记录                                                              
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年5月9日                                                                    
 *******************************************************************************/  
void DisplayUserRecord(void)
{
	uint8_t cnt_t=0,floor=0;
	for(floor=0;floor<4;floor++)		//清楚屏幕数据
	{
		AbnomalMealNameDisp(0,floor);
		AbnomalMealCnttDisp(0xff,floor);
		AbnomalMealCntDisp(0xff,floor);		
	}
	floor=0; //复原
	if(UserActMessageWriteToFlash.UserAct.MealCnt_1st_t>0)  
	{
		AbnomalMealNameDisp(sell_type[cnt_t],floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_1st_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_1st,floor);		
		floor++;
	}
	cnt_t++;
	if(UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t>0)
	{
		AbnomalMealNameDisp(sell_type[cnt_t],floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_2nd,floor);	
		floor++;
	}
	cnt_t++;
	if(UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t>0)
	{
		AbnomalMealNameDisp(sell_type[cnt_t],floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_3rd,floor);	
		floor++;
	}
	cnt_t++;
	if(UserActMessageWriteToFlash.UserAct.MealCnt_4th_t>0)
	{
		AbnomalMealNameDisp(sell_type[cnt_t],floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_4th_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_4th,floor);	
		floor++;
	}
	cnt_t++;
	if(UserActMessageWriteToFlash.UserAct.MealCnt_5th_t>0)
	{
		AbnomalMealNameDisp(sell_type[cnt_t],floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_5th_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_5th,floor);	
		floor++;
	}
	cnt_t++;
	if(UserActMessageWriteToFlash.UserAct.MealCnt_6th_t>0)
	{
		AbnomalMealNameDisp(sell_type[cnt_t],floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_6th_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_6th,floor);	
		floor++;
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_7th_t>0)
	{
		AbnomalMealNameDisp(sell_type[cnt_t],floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_7th_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_7th,floor);	
		floor++;
	}
	if(UserActMessageWriteToFlash.UserAct.MealCnt_8th_t>0)
	{
		AbnomalMealNameDisp(sell_type[cnt_t],floor);
		AbnomalMealCnttDisp(UserActMessageWriteToFlash.UserAct.MealCnt_8th_t,floor);
		AbnomalMealCntDisp(UserActMessageWriteToFlash.UserAct.MealCnt_8th,floor);	
		floor++;
	}
  cnt_t=0;  
		//显示用户已付 和  应退  和 已退
	VariableChage(record_UserActPayAlready,UserActMessageWriteToFlash.UserAct.PayAlready);
		//应退的钱 = 总的应该退币的钱
	VariableChage(record_UserActPayBack,UserActMessageWriteToFlash.UserAct.MoneyBackShould);
	  //已退的钱 = 总的应该退币的钱- 还未退的钱
	VariableChage(record_UserActPayBackAlready,UserActMessageWriteToFlash.UserAct.MoneyBackShould-UserActMessageWriteToFlash.UserAct.MoneyBack);
}
	
 /*******************************************************************************
 * 函数名称:GetPassWord                                                                     
 * 描    述:放餐密码                                                            
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年4月9日                                                                    
 *******************************************************************************/  
void GetPassWord(unsigned char *PassWord)
{
  PassWord[0] = 5;
	PassWord[1] = 1;
	PassWord[2] = 8;
	PassWord[3] = 5;
	PassWord[4] = 1;
	PassWord[5] = 8;
}
 /*******************************************************************************
 * 函数名称:GetAdminPassWord                                                                     
 * 描    述:硬币退币与装填密码                                                           
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年8月13日                                                                    
 *******************************************************************************/  
void GetAdminPassWord(unsigned char *PassWord)
{
  PassWord[0] = 1;
	PassWord[1] = 2;
	PassWord[2] = 3;
	PassWord[3] = 4;
	PassWord[4] = 5;
	PassWord[5] = 6;
}
 /*******************************************************************************
 * 函数名称:GetUserPassword                                                                     
 * 描    述:错误记录清楚密码                                                         
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年8月13日                                                                    
 *******************************************************************************/  
void GetUserPassWord(unsigned char *PassWord)
{
  PassWord[0] = 6;
	PassWord[1] = 6;
	PassWord[2] = 6;
	PassWord[3] = 6;
	PassWord[4] = 6;
	PassWord[5] = 6;  
}
 /*******************************************************************************
 * 函数名称:VerifyPassword                                                                   
 * 描    述:数组比较                                                                 
 *                                                                               
 * 输    入:数组指针1，数组指针2，需比较的长度                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年4月9日                                                                     
 *******************************************************************************/  
uint8_t VerifyPassword( uint8_t *Src1, uint8_t *Src2, uint8_t Length)
{
  uint8_t i = 0; 
	for(i = 0; i < Length; i++)
	{
	  if(*(Src1+i) != *(Src2+i))
		{
		    return 0;
		}
	}
	return 1;
}

 /*******************************************************************************
 * 函数名称:RecRegisterValues                                                                   
 * 描    述:获取寄存器的值                                                                                                                          
 * 输    入:寄存器地址,寄存器数据,寄存器数据长度                                                                  
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年6月24日                                                                   
 *******************************************************************************/ 
char current_page =0;//当前页面
void RecRegisterValues(char VariableAdress,char *VariableData,char length)
{
	if(VariableAdress==PIC_ID)//读取判断当前的页面的ID
	{
		current_page =VariableData[length-1];
		if(current_page!=pageunitil)
		{
			//PageChange(cmd_page);
    }
  }
}

 /*******************************************************************************
 * 函数名称:ChangeVariableValues                                                                   
 * 描    述:改变数据变量的值                                                                                                                          
 * 输    入:变量地址,变量数据,变量长度                                                                  
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                   
 *******************************************************************************/ 
char meal1st_cnt_t= 0,chicken_cnt_t= 0,duck_cnt_t= 0,fish_cnt_t= 0; /*临时数量*/
uint8_t  PassWordLen=0;	//密码的长度为0
uint8_t  PassWord[6]={0};
uint8_t  InputPassWord[6]={0};
bool cardbalence_cancel_flag= false;
bool mealneed_sync = false;  //餐品同步标记
int16_t CoinTotoal_t=0; //作为基数 		
int8_t	selltime_hour= 11,selltime_hour_t=11,selltime_hour_r=0;
int8_t	selltime_minute=30, selltime_minute_t=30,selltime_minute_r=0;
int8_t  selltime_second_r=0;
bool sellmeal_flag = true;
void ChangeVariableValues(int16_t VariableAdress,char *VariableData,char length)
{
	char MealID =0;
	uint8_t i= 0,j=0;
	  switch (VariableAdress)
		{
			case meal_choose: /*主角面用户选择*/
			{
				switch(VariableData[1])
				{
					case 0x01:
						if(DefineMeal[0].MealCount > 0)	   //判断餐品是否大于0
						{
							PlayMusic(VOICE_1);
							UserActMessageWriteToFlash.UserAct.MealID= VariableData[1]; //当前用户选餐的ID
							UserActMessageWriteToFlash.UserAct.MealCnt_1st_t= 1;//设置默认分数为 1							
							WaitTimeInit(&WaitTime);
							VariableChage(count_dowm,WaitTime);
							OpenTIM3();
							PageChange(Meal1st_interface);//显示相应界面
              VariableChage(meal1st_cnt,0x01);
 							MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_1st_t);//根据用户所选餐品ID号显示合计钱数
						}break;						
					case 0x02:
						if(DefineMeal[1].MealCount > 0)	   //判断餐品是否大于0
						{
							PlayMusic(VOICE_1);
							UserActMessageWriteToFlash.UserAct.MealID= VariableData[1];
							UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t= 1;//设置默认分数为 1
							WaitTimeInit(&WaitTime);
							VariableChage(count_dowm,WaitTime);
							OpenTIM3();							
							PageChange(Meal2nd_interface);//显示相应界面	
							VariableChage(meal2nd_cnt,0x01);
							MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t);//根据用户所选餐品ID号显示合计钱数
						}break;							
					case 0x03:
						if(DefineMeal[2].MealCount > 0)	   //判断餐品是否大于0
						{
							PlayMusic(VOICE_1);
							UserActMessageWriteToFlash.UserAct.MealID= VariableData[1];
							UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t= 1;//设置默认分数为 1
							WaitTimeInit(&WaitTime);
							VariableChage(count_dowm,WaitTime);
							OpenTIM3();							
							PageChange(Meal3rd_interface);//显示相应界面	
							VariableChage(meal3rd_cnt,0x01);
							MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t);//根据用户所选餐品ID号显示合计钱数
						}break;						
					case 0x04:
					{					
						if(DefineMeal[3].MealCount > 0)	   //判断餐品是否大于0
						{
							PlayMusic(VOICE_1);
							UserActMessageWriteToFlash.UserAct.MealID= VariableData[1];
							UserActMessageWriteToFlash.UserAct.MealCnt_4th_t= 1;//设置默认分数为 1
							WaitTimeInit(&WaitTime);							
							OpenTIM3();	
						  VariableChage(count_dowm,WaitTime);
							PageChange(Meal4th_interface);//显示相应界面	
							VariableChage(meal4th_cnt,0x01);
							MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_4th_t);//根据用户所选餐品ID号显示合计钱数
						}			
					}break;
					case 0x05:
						if(DefineMeal[4].MealCount > 0)	   //判断餐品是否大于0
						{
							PlayMusic(VOICE_1);
							UserActMessageWriteToFlash.UserAct.MealID= VariableData[1];
							UserActMessageWriteToFlash.UserAct.MealCnt_5th_t= 1;//设置默认分数为 1
							WaitTimeInit(&WaitTime);
							VariableChage(count_dowm,WaitTime);
							OpenTIM3();							
							PageChange(Meal5th_interface);//显示相应界面	
							VariableChage(meal5th_cnt,0x01);
							MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_5th_t);//根据用户所选餐品ID号显示合计钱数
						}break;						
					case 0x06:
					{					
						if(DefineMeal[5].MealCount > 0)	   //判断餐品是否大于0
						{
							PlayMusic(VOICE_1);
							UserActMessageWriteToFlash.UserAct.MealID= VariableData[1];
							UserActMessageWriteToFlash.UserAct.MealCnt_6th_t= 1;//设置默认分数为 1
							WaitTimeInit(&WaitTime);							
							OpenTIM3();	
						  VariableChage(count_dowm,WaitTime);
							PageChange(Meal6th_interface);//显示相应界面	
							VariableChage(meal6th_cnt,0x01);
							MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_6th_t);//根据用户所选餐品ID号显示合计钱数
						}			
					}break;   
					case 0x07:
					{					
						if(DefineMeal[6].MealCount > 0)	   //判断餐品是否大于0
						{
							PlayMusic(VOICE_1);
							UserActMessageWriteToFlash.UserAct.MealID= VariableData[1];
							UserActMessageWriteToFlash.UserAct.MealCnt_7th_t= 1;//设置默认分数为 1
							WaitTimeInit(&WaitTime);							
							OpenTIM3();	
						  VariableChage(count_dowm,WaitTime);
							PageChange(Meal7th_interface);//显示相应界面	
							VariableChage(meal7th_cnt,0x01);
							MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_7th_t);//根据用户所选餐品ID号显示合计钱数
						}			
					}break;  
					case 0x08:
					{					
						if(DefineMeal[7].MealCount > 0)	   //判断餐品是否大于0
						{
							PlayMusic(VOICE_1);
							UserActMessageWriteToFlash.UserAct.MealID= VariableData[1];
							UserActMessageWriteToFlash.UserAct.MealCnt_8th_t= 1;//设置默认分数为 1
							WaitTimeInit(&WaitTime);							
							OpenTIM3();	
						  VariableChage(count_dowm,WaitTime);
							PageChange(Meal8th_interface);//显示相应界面	
							VariableChage(meal8th_cnt,0x01);
							MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_8th_t);//根据用户所选餐品ID号显示合计钱数
						}			
					}break;            
					case 0x0F:  /*管理用户键*/
					{
						PageChange(Password_interface);
					}break;
					default:break;
				}
			}break;      				
			case meal1st_cnt: /*胡萝卜炒肉分数*/
			{
				if(VariableData[1]<= DefineMeal[0].MealCount)	//设置餐品选择的上限
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_1st_t= VariableData[1];
					UserActMessageWriteToFlash.UserAct.MealCost_1st = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_1st_t);//有一些重复计算
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_1st_t);
				}
				else
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_1st_t= DefineMeal[0].MealCount;
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_1st_t);
					VariableChage(meal1st_cnt,UserActMessageWriteToFlash.UserAct.MealCnt_1st_t);	//改变变量地址数据
				}	 						 
			}break;
			case meal2nd_cnt:/*香菇滑鸡分数*/
			{
				if(VariableData[1]<= DefineMeal[1].MealCount)	//设置餐品选择的上限
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t= VariableData[1];
					UserActMessageWriteToFlash.UserAct.MealCost_2nd = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t);//有一些重复计算
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t);
				}
				else
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t= DefineMeal[1].MealCount;
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t);
					VariableChage(meal2nd_cnt,UserActMessageWriteToFlash.UserAct.MealCnt_2nd_t);	//改变变量地址数据
				}	 						 
			}break;
			case meal3rd_cnt:/*脆皮烤鸭*/
			{
				if(VariableData[1]<= DefineMeal[2].MealCount)	//设置餐品选择的上限
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t= VariableData[1];
					UserActMessageWriteToFlash.UserAct.MealCost_3rd = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t);//有一些重复计算
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t);
				}
				else
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t= DefineMeal[2].MealCount;
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t);
					VariableChage(meal3rd_cnt,UserActMessageWriteToFlash.UserAct.MealCnt_3rd_t);	//改变变量地址数据			
				}	 
			}break;	
			case meal4th_cnt:/*红烧鱼块*/
			{
				if(VariableData[1]<= DefineMeal[3].MealCount)	//设置餐品选择的上限
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_4th_t= VariableData[1];
					UserActMessageWriteToFlash.UserAct.MealCost_4th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_4th_t);//有一些重复计算
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_4th_t);
				}
				else
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_4th_t= DefineMeal[3].MealCount;
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_4th_t);
					VariableChage(meal4th_cnt,UserActMessageWriteToFlash.UserAct.MealCnt_4th_t);	//改变变量地址数据
				}
			}break;	
 			case meal5th_cnt:/*梅菜扣肉*/
			{
				if(VariableData[1]<= DefineMeal[4].MealCount)	//设置餐品选择的上限
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_5th_t= VariableData[1];
					UserActMessageWriteToFlash.UserAct.MealCost_5th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_5th_t);//有一些重复计算
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_5th_t);
				}
				else
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_5th_t= DefineMeal[4].MealCount;
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_5th_t);
					VariableChage(meal5th_cnt,UserActMessageWriteToFlash.UserAct.MealCnt_5th_t);	//改变变量地址数据
				}
			}break;	 
 			case meal6th_cnt:/*土豆牛腩*/
			{
				if(VariableData[1]<= DefineMeal[5].MealCount)	//设置餐品选择的上限
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_6th_t= VariableData[1];
					UserActMessageWriteToFlash.UserAct.MealCost_6th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_6th_t);//有一些重复计算
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_6th_t);
				}
				else
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_6th_t= DefineMeal[5].MealCount;
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_6th_t);
					VariableChage(meal6th_cnt,UserActMessageWriteToFlash.UserAct.MealCnt_6th_t);	//改变变量地址数据
				}
			}break;	   
 			case meal7th_cnt:/*土豆牛腩*/
			{
				if(VariableData[1]<= DefineMeal[6].MealCount)	//设置餐品选择的上限
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_7th_t= VariableData[1];
					UserActMessageWriteToFlash.UserAct.MealCost_7th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_7th_t);//有一些重复计算
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_7th_t);
				}
				else
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_7th_t= DefineMeal[6].MealCount;
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_7th_t);
					VariableChage(meal7th_cnt,UserActMessageWriteToFlash.UserAct.MealCnt_7th_t);	//改变变量地址数据
				}
			}break;	 
 			case meal8th_cnt:/*土豆牛腩*/
			{
				if(VariableData[1]<= DefineMeal[7].MealCount)	//设置餐品选择的上限
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_8th_t= VariableData[1];
					UserActMessageWriteToFlash.UserAct.MealCost_8th = GetMealPrice(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_8th_t);//有一些重复计算
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_8th_t);
				}
				else
				{
					UserActMessageWriteToFlash.UserAct.MealCnt_8th_t= DefineMeal[7].MealCount;
					MealCostDisp(UserActMessageWriteToFlash.UserAct.MealID,UserActMessageWriteToFlash.UserAct.MealCnt_8th_t);
					VariableChage(meal8th_cnt,UserActMessageWriteToFlash.UserAct.MealCnt_8th_t);	//改变变量地址数据
				}
			}break;	       
      case mealcnt_choose:/*单页选择按钮*/
      {
			  switch(VariableData[1])
				{
					case 0x01:  /*放入购物车*/
					{
								
					}break;
					case 0x02:  /*继续选餐*/
					{
            PutIntoShopCart();
						PageChange(Menu_interface);
						WaitTimeInit(&WaitTime);
					}break;
 					case 0x03:  /*进入购物车*/
					{
            if(sellmeal_flag)
            {
              PutIntoShopCart();
              SettleAccounts();
              if(UserActMessageWriteToFlash.UserAct.Meal_totoal>0)
              {  
                CloseTIM3();
                WaitTimeInit(&WaitTime);
                OpenTIM7();
                Current= waitfor_money;//进入读钱界面
                CurrentPoint= 1;       //倒计时60s，打开现金接受
                PlayMusic(VOICE_2);
              }
            }
            else
            {
              PageChange(SellMeal_TimeWait_interface);
            }
					}break;
					case 0x04:  /*取消*/
					{
						//清空所有的用户数据???
						ClearUserBuffer();
            SaveUserData();
						PageChange(Menu_interface);
						CloseTIM3();
						Current= current_temperature;//取消进入空闲程序
						
					}break;
					default:break;
				}
			}break;
			/*改变四栏中餐品的数量*/
			/*需要餐品的ID和行号:column1st_count, MealID*/
			case column1st_count:MealID= DispMeal[0].meal_id;goto loop1;
			case column2nd_count:MealID= DispMeal[1].meal_id;goto loop1;
			case column3rd_count:MealID= DispMeal[2].meal_id;goto loop1;
			case column4th_count:MealID= DispMeal[3].meal_id;goto loop1;
			{
loop1:	switch(MealID)
				{
					case 0x00:
					{
						VariableChage(VariableAdress,0); //维持原来所选的数量
					}break;
					case 0x01:
					{
					  if(VariableData[1]<=DefineMeal[MealID-1].MealCount)//当用户选择数量小于或等于存货时
				    {
					    UserActMessageWriteToFlash.UserAct.MealCnt_1st =VariableData[1]; //将改变的值返回到结构体中
					    UserActMessageWriteToFlash.UserAct.MealCost_1st =UserActMessageWriteToFlash.UserAct.MealCnt_1st *price_1st; //计算单总价
					    VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_1st); //改变数量
					    VariableChage(VariableAdress+1,UserActMessageWriteToFlash.UserAct.MealCost_1st); //改变单总价，地址偏移1
				    }
				    else
				    {	
							VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_1st); //维持原来所选的数量
				    }					
					}break;
					case 0x02:
					{
					  if(VariableData[1]<=DefineMeal[MealID-1].MealCount)//还是要判断数量
				    {
					    UserActMessageWriteToFlash.UserAct.MealCnt_2nd =VariableData[1]; //将改变的值返回到结构体中
					    UserActMessageWriteToFlash.UserAct.MealCost_2nd =UserActMessageWriteToFlash.UserAct.MealCnt_2nd *price_2nd; //计算单总价
					    VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_2nd); //改变数量
					    VariableChage(VariableAdress+1,UserActMessageWriteToFlash.UserAct.MealCost_2nd); //改变单总价
				    }
				    else
				    {
					    VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_2nd); //维持原来所选的数量
				    }									
					}break;
					case 0x03:
					{
					  if(VariableData[1]<=DefineMeal[MealID-1].MealCount)//还是要判断数量
				    {
					    UserActMessageWriteToFlash.UserAct.MealCnt_3rd =VariableData[1]; //将改变的值返回到结构体中
					    UserActMessageWriteToFlash.UserAct.MealCost_3rd =UserActMessageWriteToFlash.UserAct.MealCnt_3rd *price_3rd; //计算单总价
					    VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_3rd); //改变数量
					    VariableChage(VariableAdress+1,UserActMessageWriteToFlash.UserAct.MealCost_3rd); //改变单总价
				    }
				    else
				    {
					     VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_3rd); //维持原来所选的数量
				    }							
					}break;
					case 0x04:
					{
					  if(VariableData[1]<=DefineMeal[MealID-1].MealCount)//还是要判断数量
				    {
					    UserActMessageWriteToFlash.UserAct.MealCnt_4th =VariableData[1]; //将改变的值返回到结构体中
					    UserActMessageWriteToFlash.UserAct.MealCost_4th =UserActMessageWriteToFlash.UserAct.MealCnt_4th *price_4th; //计算单总价
					    VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_4th); //改变数量
					    VariableChage(VariableAdress+1,UserActMessageWriteToFlash.UserAct.MealCost_4th); //改变单总价
				    }
				    else
				    {
					     VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_4th); //维持原来所选的数量
				    }							
					}break;
					case 0x05:
					{
					  if(VariableData[1]<=DefineMeal[MealID-1].MealCount)//还是要判断数量
				    {
					    UserActMessageWriteToFlash.UserAct.MealCnt_5th =VariableData[1]; //将改变的值返回到结构体中
					    UserActMessageWriteToFlash.UserAct.MealCost_5th =UserActMessageWriteToFlash.UserAct.MealCnt_5th *price_5th; //计算单总价
					    VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_5th); //改变数量
					    VariableChage(VariableAdress+1,UserActMessageWriteToFlash.UserAct.MealCost_5th); //改变单总价
				    }
				    else
				    {
					     VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_5th); //维持原来所选的数量
				    }							
					}break;
					case 0x06:
					{
					  if(VariableData[1]<=DefineMeal[MealID-1].MealCount)//还是要判断数量
				    {
					    UserActMessageWriteToFlash.UserAct.MealCnt_6th =VariableData[1]; //将改变的值返回到结构体中
					    UserActMessageWriteToFlash.UserAct.MealCost_6th =UserActMessageWriteToFlash.UserAct.MealCnt_6th *price_6th; //计算单总价
					    VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_6th); //改变数量
					    VariableChage(VariableAdress+1,UserActMessageWriteToFlash.UserAct.MealCost_6th); //改变单总价
				    }
				    else
				    {
					     VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_6th); //维持原来所选的数量
				    }							
					}break; 
					case 0x07:
					{
					  if(VariableData[1]<=DefineMeal[MealID-1].MealCount)//还是要判断数量
				    {
					    UserActMessageWriteToFlash.UserAct.MealCnt_7th =VariableData[1]; //将改变的值返回到结构体中
					    UserActMessageWriteToFlash.UserAct.MealCost_7th =UserActMessageWriteToFlash.UserAct.MealCnt_7th *price_7th; //计算单总价
					    VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_7th); //改变数量
					    VariableChage(VariableAdress+1,UserActMessageWriteToFlash.UserAct.MealCost_7th); //改变单总价
				    }
				    else
				    {
					     VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_7th); //维持原来所选的数量
				    }							
					}break;  
					case 0x08:
					{
					  if(VariableData[1]<=DefineMeal[MealID-1].MealCount)//还是要判断数量
				    {
					    UserActMessageWriteToFlash.UserAct.MealCnt_8th =VariableData[1]; //将改变的值返回到结构体中
					    UserActMessageWriteToFlash.UserAct.MealCost_8th =UserActMessageWriteToFlash.UserAct.MealCnt_8th *price_8th; //计算单总价
					    VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_8th); //改变数量
					    VariableChage(VariableAdress+1,UserActMessageWriteToFlash.UserAct.MealCost_8th); //改变单总价
				    }
				    else
				    {
					     VariableChage(VariableAdress,UserActMessageWriteToFlash.UserAct.MealCnt_8th); //维持原来所选的数量
				    }							
					}break;            
					default:break;						
				}
				UserActMessageWriteToFlash.UserAct.PayShould= (UserActMessageWriteToFlash.UserAct.MealCost_1st+UserActMessageWriteToFlash.UserAct.MealCost_2nd+UserActMessageWriteToFlash.UserAct.MealCost_3rd+UserActMessageWriteToFlash.UserAct.MealCost_4th
                                                      +UserActMessageWriteToFlash.UserAct.MealCost_5th+UserActMessageWriteToFlash.UserAct.MealCost_6th+UserActMessageWriteToFlash.UserAct.MealCost_7th+UserActMessageWriteToFlash.UserAct.MealCost_8th);
	      VariableChage(mealtotoal_cost,UserActMessageWriteToFlash.UserAct.PayShould);
			}break;
			case payment_method: /*付款方式*/ 
			{
        uint32_t temp1= 0,temp2= 0;
				AcountCopy();
				if(UserActMessageWriteToFlash.UserAct.PayShould==0) goto loop7;
				PageChange(Acount_interface+2); //当按下付款后，跳转到另一个页面禁止分数加减
				//MoveToFisrtMeal();  //当选择付款方式后可以查询当前的用户选餐的ID,直接发送坐标,需要加入待机命令不然不行
				switch(VariableData[1])
				{
					case 0x01:   /*现金支付*/
					{
            if(cash_limit_flag==true) break; //如果零钱找完，无法进入
						CurrentPoint =2;
            PlayMusic(VOICE_3);
						if(!OpenCashSystem()){OpenCashSystem();};// printf("cash system is erro2");  //关闭现金接受
					}break;
					case 0x02:   /*银行预付卡*/
					{
						CurrentPoint =7;
						PlayMusic(VOICE_5);
						if(!CloseCashSystem()){CloseCashSystem();};// printf("cash system is erro3");  //关闭现金接受
					}break;
					case 0x03:   /*深圳通支付*/
					{
						CurrentPoint =8;
						PlayMusic(VOICE_5);
						if(!CloseCashSystem()){CloseCashSystem();};//printf("cash system is erro4");  //关闭现金接受			
					}break;
					case 0x04:   /*取消*/
					{
	 loop7:		temp1= UserActMessageWriteToFlash.UserAct.MoneyBack= UserActMessageWriteToFlash.UserAct.PayAlready; //超时将收到的钱以硬币的形式返还
	          temp2= UserActMessageWriteToFlash.UserAct.MoneyBackShould = UserActMessageWriteToFlash.UserAct.PayAlready; //总的应该退的钱
            ClearUserBuffer();
            UserActMessageWriteToFlash.UserAct.MoneyBack= temp1;
            UserActMessageWriteToFlash.UserAct.MoneyBackShould= temp2;
            SaveUserData();
			      if(!CloseCashSystem()){CloseCashSystem();};//printf("cash system is erro5");  //关闭现金接受
            PageChange(Menu_interface);
						CloseTIM3();
						CloseTIM7();
						CurrentPoint = 0 ;
            if(UserActMessageWriteToFlash.UserAct.MoneyBack>0) //当有钱需要退的时候，标记用户取消购买，无需退币的时候直接进入温度显示状态
            {
              UserActMessageWriteToFlash.UserAct.Cancle= 0x01;
						  Current= hpper_out; 
            }
            else
            {              
              Current= current_temperature; 
            }
					}break;
					default:break;		
				}					
			}break;
			case caedbalence_cancel:/*刷卡取消*/
			{
				PageChange(Acount_interface+2);
				cardbalence_cancel_flag=true;
			}break;			
			case bill_print:
			{
				switch(VariableData[1])
				{
					case 0x01:   /*打印小票*/
          {
            UserActMessageWriteToFlash.UserAct.PrintTick= 0x00000001;
            PrintTickFun(&UserActMessageWriteToFlash.UserAct.PrintTick);
            CloseTIM4();
            if(!erro_record) //当有错误的时候不进入出餐界面						
						PageChange(Mealout_interface);            
          }break;
					case 0x02:   /*不打印小票*/
					{
						UserActMessageWriteToFlash.UserAct.PrintTick= 0x00000002;
							 /*判断是否打印小票*/ 			
            PrintTickFun(&UserActMessageWriteToFlash.UserAct.PrintTick);
            CloseTIM4();
            if(!erro_record) //当有错误的时候不进入出餐界面						
						PageChange(Mealout_interface);
					}break;  
					default:break;
				}
			}break;
			case password:
			{
				switch(VariableData[1])
				{
					case 0x00:
					case 0x01:
					case 0x02:
					case 0x03:
					case 0x04:
					case 0x05:
					case 0x06:
					case 0x07:
					case 0x08:
					case 0x09:
					case 0x0A:						
					case 0x0B:
					case 0x0C:
					case 0x0D:
					case 0x0E:
					case 0x0F:
					{
	          if(PassWordLen < 6)
		        {
		          InputPassWord[PassWordLen] = (VariableData[1]);
		          PassWordLen++;
			        DisplayPassWord(PassWordLen);//显示一个字符串******
		        }
	        }break;					
					case 0x10:/*退格键*/
					{
	          if(PassWordLen > 0)
		        {
	            InputPassWord[PassWordLen] = 0;
	            PassWordLen--;
		          DisplayPassWord(PassWordLen);
		        }
					}break;
					case 0x11:/*确认件*/ //需要加入超级密码
					{
	          GetPassWord(PassWord);
    	      if(VerifyPassword(PassWord, InputPassWord,6) == 1) //放餐密码正确
		        {
							PassWordLen = 0;		
							DisplayPassWord(0);//清楚密码显示
							memset(InputPassWord,0,6);
						  if(Current== 0x00) 
						  {
							  break;
						  }              
						  if(erro_record!=0) 
						  {
							  break;
						  }							
							//如果有错误标记则break;
		           /*进入餐品放置界面*/
		           PageChange(MealSet_interface);					 
							 InitSetting();//清空第一二三列的数据 //对放餐的数据进行初始化
			         break;
		        }
						GetAdminPassWord(PassWord);//硬币设置密码
    	      if(VerifyPassword(PassWord, InputPassWord,6) == 1) 
		        {
							//进入管理员功能设置，设置硬币设置界面
		           /*进入管理员界面*/
							memset(InputPassWord,0,6);	
              DisplayPassWord(0);//清楚密码显示
              PassWordLen = 0;
						  if(Current== 0x00) 
						  {
							  break;
						  }
						  if(erro_record!=0) 
						  {
							  break;
						  }              
               VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal);//显示机内硬币总数
               VariableChage(coins_input,0);//显示放币数
               VariableChage(coins_retain,coins_remain);//保留硬币数
               VariableChage(coins_back,0); //显示退币数
               PageChange(Coinset_interface);
               //当前退币数
               break;
		        }
            GetUserPassWord(PassWord);   //出错数据显示密码
            if(VerifyPassword(PassWord, InputPassWord,6) == 1) 
            {
               memset(InputPassWord,0,6);
               DisplayPassWord(0);//清楚密码显示
               PassWordLen = 0;	
               if(erro_record!=0)
               {
                 if(!((erro_record&1<<arm_limit)||(erro_record&1<<signin_erro)||erro_record&1<<network_erro))
                   DisplayUserRecord();
                 PageChange(UserAbonamalRecord_interface);
               }                 
               break;              
            }
		        else
		        {
		          /*密码验证错误，进入密码输入界面*/   
							 DisplayPassWord(0);	
               PageChange(Password_interface);						
			         PassWordLen = 0;
		        }						
	        }break;						
					case 0x12:/*返回*/
					{
						if(erro_record!=0) //有错误的时候只能返回到错误界面
						{
						  PageChange(Err_interface);
							break;
						}								
		        PageChange(Menu_interface);
						DisplayPassWord(0);//清楚密码显示
		        PassWordLen = 0;	
					}break;
          default:break;					
        } 					
			}break;
			case record_clear:
			{
				switch(VariableData[1])
			  {
					case 0x01: //清除数据
					{
						erro_flag=0; //清除数据标记,数据清除在AbnormalHandle函数里
						if(Current==current_temperature)//限定在初始化状态
						{
							UserActMessageWriteToFlash.UserAct.MoneyBack=0;
							ClearUserBuffer();
					    SaveUserData();
							PageChange(Logo_interface);
						}
   				}break;
					case 0x02: //返回
					{
						PageChange(Err_interface);
					}break;
          default:break;
				}	
			}break;
			case admin_set:
			{
				switch(VariableData[1])
				{ 
					case 0x01:  /*温度设置按钮*/
					{
						PageChange(TemperatureSet_interface);
					}break;
					case 0x02:  /*餐品设置按钮 应该改为一键清空所有数据 */
					{
							for(i=0;i<90;i++)
	            {
	              FloorMealMessageWriteToFlash.FlashBuffer[i] = 0 ;
	            }
	            WriteMeal();  //写入餐品数据
							StatisticsTotal(); 
							DispLeftMeal();
					}break;
					case 0x03:	/*取消键*/
					{
						if(mealneed_sync == false)
						{
						  PageChange(Menu_interface);
						}
					}
          default:break;
				}					
			}break;
			case meal_num: //当改变餐品的编号的时候，需要查找当前的ID
			{
				uint8_t cnt_t=0;
        uint8_t meal_id_t=0,find_flag=0;
        for(meal_id_t=0;meal_id_t<4;meal_id_t++)
        {
          if(sell_type[meal_id_t]==VariableData[1])
          {
            find_flag=1;
            break;
          }
        }
        if(find_flag==1)
			    CurFloor.MealID= VariableData[1];	//当前的就是餐品的ID
        else
        {
          CurFloor.MealID= sell_type[0]; //当查不到输入的ID与本地的id不匹配则选最小号的ID
        }
        VariableChage(meal_num,CurFloor.MealID);		
				InitSetting();
				for(cnt_t = 0; cnt_t < FloorMealNum; cnt_t++)  //查找那一层有是空的
				{
					if(FloorMealMessageWriteToFlash.FloorMeal[cnt_t].MealCount ==0)
					{
						 CurFloor.FloorNum = cnt_t+1;
						 break;
					}			
				}
				VariableChage(floor_num,CurFloor.FloorNum);				
			}break;
      case floor_num:
			{
			  CurFloor.FloorNum= VariableData[1];
				InitSetting();
			}break;
      case row_1st:
      {
				CurFloor.FCount= VariableData[1];
			}break;	
      case row_2nd:
      {
				CurFloor.SCount= VariableData[1];
			}break;	
      case row_3rd:
      {
				CurFloor.TCount= VariableData[1];
			}break;	
      case mealinput_choose:
			{
				switch(VariableData[1])
				{
					case 0x01:  /*放满*/
					{
						CurFloor.FCount = 3;
		        CurFloor.SCount = 3;
		        CurFloor.TCount = 3; 
		        CurFloor.MealCount = 9;
					  VariableChage(row_1st,CurFloor.FCount);
		        VariableChage(row_2nd,CurFloor.SCount);
		        VariableChage(row_3rd,CurFloor.TCount);	
					}break;
					case 0x02:  /*清空*/
					{
						CurFloor.FCount = 0;
		        CurFloor.SCount = 0;
		        CurFloor.TCount = 0; 
		        CurFloor.MealCount = 0;
					  VariableChage(row_1st,CurFloor.FCount);
		        VariableChage(row_2nd,CurFloor.SCount);
		        VariableChage(row_3rd,CurFloor.TCount);							
					}break;
					case 0x03:  /*确认*/
					{
						/*计算总共的餐品数*/
						CurFloor.MealCount=  CurFloor.FCount + CurFloor.SCount + CurFloor.TCount;
						/*把当前第一列设置的参数保存起来*/
						FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].FCount    = CurFloor.FCount;
						/*把当前第2列设置的参数保存起来*/
						FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].SCount    = CurFloor.SCount;
						/*把当前第3列设置的参数保存起来*/
						FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].TCount    = CurFloor.TCount;
						/*把当前设置餐的ID号保存起来*/
						FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].MealID    = CurFloor.MealID;
						/*把当前设置的餐的总数保存起来*/
						FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].MealCount = CurFloor.MealCount;
						/*把设置餐的层数保存起来*/
						FloorMealMessageWriteToFlash.FloorMeal[CurFloor.FloorNum - 1].FloorNum  = CurFloor.FloorNum;
						/*九种菜品*/
						for(j = 0; j < MealKindTotoal; j++)	 //统计每个餐品的数据
						{
							/*各个餐的数目清零*/
							DefineMeal[j].MealCount = 0;
							/*各个餐的列对应的数目清零*/
							for(i = 0; i < FloorMealNum; i++)
							{
								DefineMeal[j].Position[i][0] = 0;
								DefineMeal[j].Position[i][1] = 0;
								DefineMeal[j].Position[i][2] = 0;
							}
							 /*统计各个餐的数目的总和*/
							for(i = 0; i < FloorMealNum; i++)
							{														 /*餐的ID是从1-6*/
								if(FloorMealMessageWriteToFlash.FloorMeal[i].MealID == j + 1)
								{
									DefineMeal[j].MealCount      = DefineMeal[j].MealCount + FloorMealMessageWriteToFlash.FloorMeal[i].MealCount;
									DefineMeal[j].Position[i][0] = FloorMealMessageWriteToFlash.FloorMeal[i].FCount;
									DefineMeal[j].Position[i][1] = FloorMealMessageWriteToFlash.FloorMeal[i].SCount;
									DefineMeal[j].Position[i][2] = FloorMealMessageWriteToFlash.FloorMeal[i].TCount;
									//printf("DefineMeal[%d].Position[%d][0]=%d\r\n",j,i,DefineMeal[j].Position[i][0]);
								}
							}
						}
						StatisticsTotal(); //先统计数目在写入
						WriteMeal(); //写数据到flash
						DispLeftMeal(); //更新餐品数据
						mealneed_sync = true;
						//printf("DefineMeal[%d].MealCount=%d\r\n",i,DefineMeal[i].MealCount);
					}break;
					case 0x04:  /*数据同步*/
					{
						uint8_t cnt_t=0,floor=0;
						for(cnt_t=0x00;cnt_t<0x04;cnt_t++)
						{
							/*显示餐品,数量*/									
              SyncMealNameDisp(0,cnt_t);
              SyncMealCntDisp(0xFF,cnt_t); 									 
						}	
						//禁止屏幕点击*/
            ScreenControl(ScreenDisable);
						/*餐品同步时尽量避免与服务器断开的情况*/
						/*不能加入签到函数，直接死掉*/
						//if(!SignInFunction())  AbnormalHandle(signin_erro);
						/*上传断网后的数据，供后台实时更新数据*/
            // SendtoServce(); //不要，因为没有用， 
						//数据同步子程序
						MealDataCompareFun();
						if(MealCompareData.MealCompareTotoal==0xFFFFFFFF) //正确
						 {
							 mealneed_sync = false;
							 PageChange(Data_synchronization);
							 for(cnt_t=0x00;cnt_t<0x04;cnt_t++)
							 {
									 /*显示餐品,数量*/									
                  SyncMealNameDisp(sell_type[cnt_t],floor);
                  SyncMealCntDisp(DefineMeal[sell_type[cnt_t]-1].MealCount,floor); 
                  floor++;									 
							 }							 
						 }
						 /*可以加入对比的返回值进行判断*/
             else
             {
							 mealneed_sync = true;
							 PageChange(Data_synchronization+2);
							 for(cnt_t=0x00;cnt_t<0x04;cnt_t++)
							 {
								 if(MealCompareData.MealComparePart[cnt_t]==0xff)
								 {
									//餐品同步成功不做任何事情 
								 }
								 else
								 {
									 /*显示餐品,数量*/									
                  SyncMealNameDisp(sell_type[cnt_t],floor);
                  SyncMealCntDisp(MealCompareData.MealComparePart[cnt_t],floor); 
                  floor++;									 
								 }
               }						 
						 }
						//超时时退出，错误退出
						ScreenControl(ScreenEnable);
						DispLeftMeal();//改变剩余餐品数量显示
					}break;
					case 0x05:  /*返回*/
					{
						CurFloor.FCount    = 0;
						CurFloor.SCount    = 0;
						CurFloor.TCount    = 0;
						CurFloor.MealID    = 0;
						CurFloor.MealCount = 0;
						CurFloor.FloorNum  = 0;	 
	          PageChange(MealSet_interface);
					}break;
					case 0x06: /*售餐设置*/
					{
            VariableChage(set_sellmeal_hour,selltime_hour);
            VariableChage(set_sellmeal_minute,selltime_minute);  
						PageChange(SellMeal_TimeSet_interface); 
					}break;
          case 0x07: /*餐品对比成功返回*/
          {
            PageChange(MealInput_interface);
          }break;
					default:break;	
				}
			}break;
			case temprature_set:
			{
        if(VariableData[1]==0xFF)
        {
          PageChange(MealSet_interface);
          VariableChage(temprature_set,0);
        }
        else
        {
          SetTemper(VariableData[1]);
        }
				
			}break;
			case coins_key:  //按一次退一次
      {
				int16_t cnt_t=0,i=0;
			  int16_t coins_time=0;
				int16_t MoneyBack_cnt=0;
				/*当按下任意一个按键的时候需要读硬币数据进行处理*/
				CoinsTotoalMessageWriteToFlash.CoinTotoal+= CoinTotoal_t;
				CoinTotoal_t=0;
				WriteCoins();
				if(VariableData[1]==0x01) /*部分退币*/
				{
					if(CoinsTotoalMessageWriteToFlash.CoinTotoal-coins_remain>0)
					{
						Coins_cnt=0;
						coins_time= ((CoinsTotoalMessageWriteToFlash.CoinTotoal-coins_remain)/10);
						cnt_t= ((CoinsTotoalMessageWriteToFlash.CoinTotoal-coins_remain)%10);
						VariableChage(coins_back,Coins_cnt);
						for(i=0;i<coins_time+1;i++) //一次退币10个，
						{
							if(i!=coins_time)
							{
								UserActMessageWriteToFlash.UserAct.MoneyBack+=SendOutN_Coin(10);		
							}
							else
							{
								if(cnt_t>0)
									UserActMessageWriteToFlash.UserAct.MoneyBack+=SendOutN_Coin(cnt_t);	
								else
									break;
							}
							delay_ms(1000); 
							VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal);//显示机内硬币数
							VariableChage(coins_back,Coins_cnt);						
							if(ErrorType ==1)  //退币机无币错误,直接进入错误状态
							{
								break;
							}														
						}
					}	
					//采用能发送几个是几个的方法是不可行的，//CoinsTotoalMessageWriteToFlash.CoinTotoal = SendOutN_Coin(CoinsTotoalMessageWriteToFlash.CoinTotoal);			
				}
				else if(VariableData[1] == 0x02) /*全部退币*/
				{
					if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_9)!=0)//加入限定
					{
						Coins_cnt=0;
						coins_time= (CoinsTotoalMessageWriteToFlash.CoinTotoal/10);
						cnt_t= (CoinsTotoalMessageWriteToFlash.CoinTotoal%10);
						VariableChage(coins_back,Coins_cnt);
						for(i=0;i<coins_time+1;i++) //一次退币10个，
						{
							if(i!=coins_time)
							{
								MoneyBack_cnt+=SendOutN_Coin(10);		
							}
							else
							{
								if(cnt_t>0)
									MoneyBack_cnt+=SendOutN_Coin(cnt_t);	
								else
									break;
							}
							delay_ms(1500); 
							VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal);//显示机内硬币数
							VariableChage(coins_back,Coins_cnt);												
						}							
						if(ErrorType ==1)  //退币机无币错误,再次发送退币
						{
							MoneyBack_cnt= SendOutN_Coin(MoneyBack_cnt);
							//RefundButton(UserAct.MoneyBack);如果使用这个会造成退币机错误
							//已完全退币，显示退币出错界面，请重启
						}	
					}						
				}
				else if(VariableData[1] == 0x03) /*返回*/
				{
					StopRefond();
					PageChange(Menu_interface);
				}
			}break;	
      case 	coins_input: /*将要将放置的硬币与总数联合起来*/
      {
        myunion.adr[0] =	VariableData[1];
				myunion.adr[1] =	VariableData[0];
				CoinTotoal_t   =  myunion.adress;
				VariableChage(coins_in,CoinsTotoalMessageWriteToFlash.CoinTotoal+ CoinTotoal_t);//显示机内硬币数			
			}break;
      case set_sellmeal_hour: /*售餐设置的小时*/	
			{
				selltime_hour_t= VariableData[1];		
			}break;				
			case set_sellmeal_minute:/*售餐设置的分*/	
			{
				selltime_minute_t= VariableData[1];				
      }break;				
			case set_sellmeal_key: /*售餐设置的按键*/
			{
				if(VariableData[1]==0x01) /*确认*/
				{
					selltime_hour= selltime_hour_t;
					selltime_minute= selltime_minute_t;
					DisplayTimeCutDown();
        }
				else if(VariableData[1]==0x02) /*返回*/
				{
          PageChange(Data_synchronization);				
        }
      }break;
			  default:break;		
		}
}
 /*******************************************************************************
 * 函数名称:DealSeriAceptData                                                                   
 * 描    述:处理从迪文屏幕返回的数据                                                        
 *          如何实现三个连续的数据：0xA5 0XA5 0X5A                                                                           
 * 输    入:无                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                   
 *******************************************************************************/ 	
void DealSeriAceptData(void)
 {
	unsigned char i;
	unsigned char temp,temp1,length,checkout;
	char RegisterAdress=0;
	int16_t VariableAdress=0;
	
	unsigned char Rx3DataBuff[10]={0};/*设置一个数组大小，?以免越界(out of bounds),?可变长度数组*/
	char RegisterData[5]={0};  //寄存器数据数组
	char VariableData[5]={0};  //变量数据数组
	char RegisterLength= 0;   //寄存器数据的长度
	char VariableLength= 0;   //变量数据的长度
	while(UART3_GetCharsInRxBuf()>=9) //获取缓冲区大小，直至缓冲区无数据
	{	
		if(USART3_GetChar(&temp)==1)
		{	
			if(temp==FH0)	//匹配帧头第一个字节
			{ 
loop:	  if(USART3_GetChar(&temp1)==1)  
				{
					if(temp1==FH1)  //匹配帧头第二个字节：
					{
						if(USART3_GetChar(&length)==1)  //获取命令字节长度
						{
							checkout =1;//置位开始读取命令与数据标志	
						}	
					}
					else if(temp1==FH0)	goto loop; //如果持续出现0xA5则继续判断下一个字节是否为0x5A
				}
			}
		}
	  if(checkout==1) //直到获取数据长度，然后读缓存
	  {
	 	  checkout =0;  //复位标志
		  for(i=0;i<length;i++) USART3_GetChar(Rx3DataBuff+i);
		  if(Rx3DataBuff[0]==0x81)  //读寄存器返回命令
		  {
			  RegisterAdress =Rx3DataBuff[1];
			  for(i=0;i<(length-2);i++)
			  RegisterData[i]=Rx3DataBuff[2+i];
				//加入修改相关数据的功能
				RecRegisterValues(RegisterAdress,RegisterData,RegisterLength);
		  }
		  else if(Rx3DataBuff[0]==0x83) //读数据存储器返回命令
		  {
			  myunion.adr[1] =Rx3DataBuff[1]; //有可能是小端模式引起的
			  myunion.adr[0] =Rx3DataBuff[2];
			  VariableAdress =myunion.adress;
			  VariableLength =Rx3DataBuff[3];
			  for(i=0;i<(length-4);i++)
			  VariableData[i]=Rx3DataBuff[4+i];
			  //加入修改变量地址数据的功能
			  ChangeVariableValues(VariableAdress,VariableData,VariableLength);
		   }
	   } 
	 }		
}

