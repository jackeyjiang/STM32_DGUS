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

/*帧的一些字节定义*/
#define FH0 0xA5 
#define FH1 0x5A
#define RD_LEN 0x00
#define ADR 0x00

/*寄存器空间定义*/
#define Version 0x00	/*DGUS版本号*/
#define LED_NOW 0X01  /*LED亮度控制寄存器，0x00~0x40*/
#define BZ_IME  0X02  /*蜂鸣器鸣叫控制寄存器，单位10ms*/
#define PIC_ID  0x03  /*2字节 读：当前显示页面ID 写：切换到指定页面(页面控制)*/
#define TP_Flag 0x05  /*0x5a=触摸屏坐标有更新；其他触摸屏坐标未更新*/
#define TP_Status 0x06  /*0x01=第一次按下 0x03一直按压中 0x02=抬起 其他=无效*/
#define TP_Position 0x07 /*4字节 触摸屏按压坐标位置：X_H:L Y_H:L*/
#define TPC_Enable 0x0B /*0x00=触控不起用 其他=启用*/

/*剩余菜品变量地址*/
#define carrot     0x0000 /*红萝卜炒肉*/
#define egg 	   0x0001 /*菜脯煎蛋*/
#define potato     0x0002 /*土豆牛腩*/
#define mushroom   0x0003 /*香菇滑鸡*/
#define fish       0x0004 /*红烧鱼块*/
#define chicken    0x0005 /*香菇滑鸡*/
#define duck       0x0006 /*脆皮烤鸭*/
#define pork       0x0007 /*梅菜扣肉*/
#define roast      0x0008 /*秘制叉烧*/

/*将数据16位存储器地址分解为2个8位数据*/
union ScreenRam
{
		int16_t	 	adress ;
		int8_t  	adr[2] ;
}myunion;


/*写指定寄存器帧*/
const unsigned char RegisterWrite[]={ 
							FH0 , FH1 , /*帧头2字节 */
							0x00 , /*长度 包括命令和数据*/
              0x80 , /*指定地址写寄存器数据*/
							ADR  , /*寄存器地址*/
							0x00 , /*数据*/
							0x00 , 0x00	/*CRC校验侦尾*/
							};

/*读指定寄存器帧*/
const unsigned char RegisterRead[8]={
							FH0 , FH1 , /*帧头2字节 */
							0x03 , /*长度 包括命令和数据*/
              0x81 ,  /*指定地址读寄存器数据*/
							ADR  , /*寄存器地址*/
							RD_LEN , /*长度*/
							0x00 , 0x00	/*CRC校验侦尾*/	
							};
	
/*写变量存储区帧*/							
const unsigned char VariableWrite[]={
							FH0 , FH1 , /*帧头2字节 */
							0x00 , /*长度 包括命令和数据*/	
							0x82 , /*指定地址开始写入数据串(字数据)到变量存储区*/
							0x00 , 0x00 ,	/*变量存储空间2字节*/
							0x00 , /*数据*/
							0x00 , 0x00  	/*CRC校验侦尾*/
							}; 

/*读变量存储区帧*/	
const unsigned char VariableRead[9]={
							FH0 , FH1 , /*帧头2字节 */
							0x04 , /*长度 包括命令和数据*/
							0x83 , /*从变量存储区指定地址开始读入RD_LEN长度字数据*/
							0x00 , 0x00 ,	/*变量存储空间2字节*/
							RD_LEN , /*长度*/
							0x00 , 0x00  	/*CRC校验侦尾*/
							};										

 /*******************************************************************************
 * 函数名称:PageChange                                                                     
 * 描    述:发送切换图片命令                                                              
 *                                                                               
 * 输    入:page                                                                    
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void PageChange(char page)
{
		unsigned char *temp;
		memcpy(temp,RegisterWrite,sizeof(RegisterWrite));
		temp[2]=	3;		
		temp[4]=	PIC_ID;	
	    temp[5]=  page;
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
	    unsigned char *temp;
		for(i=0;i<9;i++){		
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 4;
		myunion.adress= carrot+i; //在基地址推移位置
		temp[4]= myunion.adr[0];
		temp[5]= myunion.adr[1];
		Uart3_Send(temp,sizeof(temp));	
		}

}

 /*******************************************************************************
 * 函数名称:DealSeriAceptData                                                                   
 * 描    述:处理从迪文屏幕返回的数据                                                        
 *                                                                               
 * 输    入:无                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                   
 *******************************************************************************/ 	
void DealSeriAceptData(void)
{
		
}

