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

typedef struct DispMeal
{
	char meal_id;
	char meal_cnt;
	char meal_cost;
}DispMealStuct;

DispMealStuct DispMeal[4]={0};

const char price_1st= 15;
const char price_2nd= 20;
const char price_3rd= 25;
const char price_4th= 30;

const char meat_name[12]={"红萝卜炒肉"};
const char chicken_name[10]={"香菇滑鸡"};
const char duck_name[10]={"脆皮烤鸭"};
const char fish_name[10]={"红烧鱼块"};

/*将数据16位存储器地址分解为2个8位数据*/
union ScreenRam
{
		int16_t	 	adress ;
		int8_t  	adr[2] ;
}myunion;


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
		temp[2]= 4;
		myunion.adress= Variable; 
		temp[4]= myunion.adr[0];
		temp[5]= myunion.adr[1];
		myunion.adress= Value; 
		temp[6]= myunion.adr[0];
		temp[7]= myunion.adr[1];		
		Uart3_Send(temp,sizeof(temp));	
}							

 /*******************************************************************************
 * 函数名称:TextDisp                                                                  
 * 描    述:显示字符串函数,                                                          
 *                                                                               
 * 输    入:meal_name                                                                   
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                    
 *******************************************************************************/ 							
void TextDisp(uint16_t meal_name)							
{
	  char temp[20]={0};  //存放串口数据的临时数组
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 13; //0x83 0x00 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
		myunion.adress= column1st_name; 
		temp[4]= myunion.adr[1];
		temp[5]= myunion.adr[0];
		switch(meal_name)
		{
			case column1st_name:strcat(temp,meat_name);break;	
			case column2nd_name:strcat(temp,chicken_name);break;		
			case column3rd_name:strcat(temp,duck_name);break;	
			case column4th_name:strcat(temp,fish_name);break;
			default:break;			
		}
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
	temp[2]= PassWordLen+ 3; //  0x83 00 00 ******
	myunion.adress= password_show; 
  temp[4]= myunion.adr[1];
  temp[5]= myunion.adr[0];
	for(i=0;i<PassWordLen;i++) temp[i+6]='*';
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
void PageChange(char page)
{
		unsigned char temp[7]={0};
		memcpy(temp,RegisterWrite,sizeof(RegisterWrite));	
		temp[4]=	PIC_ID;	
	  temp[6]=  page;
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
		for(i=0;i<4;i++){		
		memcpy(temp,VariableWrite,sizeof(VariableWrite));
		temp[2]= 4;
		myunion.adress= meat+i; //在基地址推移位置
		temp[4]= myunion.adr[0];
		temp[5]= myunion.adr[1];
		//将数据进行填充，需要用到flash
		temp[7]= DefineMeal[i].MealCount;
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
	  temp[2]= 4;
		myunion.adress= count_dowm; //在基地址推移位置
		temp[4]= myunion.adr[0];
		temp[5]= myunion.adr[1];
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
char GetMealPrice(char meal_type,char count)
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
	  temp[2]= 4;
		temp[5]= (0x31+(meal_id-1)*2); //钱币变量地址
    temp[7]= GetMealPrice(meal_id,meal_count);	
		Uart3_Send(temp,sizeof(temp));		
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

	switch(UserAct.MealID)
	{
		case 0x01:
		{
			UserAct.MealCnt_1st=UserAct.MealCnt_1st_t;
		}break;
		case 0x02:
		{
			UserAct.MealCnt_2nd=UserAct.MealCnt_2nd_t;
		}break;
		case 0x03:
		{
			UserAct.MealCnt_3rd=UserAct.MealCnt_3rd_t;
		}break;
		case 0x04:
		{
			UserAct.MealCnt_4th=UserAct.MealCnt_4th_t;
		}break;
		default:break;	
	}
	PageChange(UserAct.MealID*3+6);
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
  PageChange(Acount_interface);//结算界面
	//在这里做标记 current = payformoney
  //显示硬币数
	VariableChage(payment_coin,UserAct.PayForCoins);
  //显示纸币数
	VariableChage(payment_bill,UserAct.PayForBills);
  //刷卡数
	VariableChage(payment_card,UserAct.PayForCards);
  //显示等待时间
	VariableChage(wait_payfor,60);
	//程序显示部分
	Floor= 0; //每次进一次结账界面就需要从新写入数据
	if(UserAct.MealCnt_1st>0)
	{
		DispMeal[Floor].meal_id= 0x01; //0x01为第一个餐品的ID
		DispMeal[Floor].meal_cnt= UserAct.MealCnt_1st; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserAct.MealCost_1st; //餐品单总价的赋值
		Floor++;
	}
	if(UserAct.MealCnt_2nd>0)
	{
		DispMeal[Floor].meal_id= 0x02; //0x02为第二个餐品的ID
		DispMeal[Floor].meal_cnt= UserAct.MealCnt_2nd; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserAct.MealCost_2nd; //餐品单总价的赋值
		Floor++;
	}		
  if(UserAct.MealCnt_3rd>0)
	{
		DispMeal[Floor].meal_id= 0x03; //0x03为第三个餐品的ID
		DispMeal[Floor].meal_cnt= UserAct.MealCnt_3rd; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserAct.MealCost_3rd; //餐品单总价的赋值
		Floor++;		
	}		
	if(UserAct.MealCnt_4th>0)
	{
		DispMeal[Floor].meal_id= 0x04; //0x01为第四个餐品的ID
		DispMeal[Floor].meal_cnt= UserAct.MealCnt_4th; //餐品的数量赋值
		DispMeal[Floor].meal_cost= UserAct.MealCost_4th; //餐品单总价的赋值
	}
	//加入显示的部分
	UserAct.PayShould= (UserAct.MealCost_1st+UserAct.MealCost_2nd+UserAct.MealCost_3rd+UserAct.MealCost_4th);
	VariableChage(mealtotoal_cost,UserAct.PayShould);
}

 /*******************************************************************************
 * 函数名称:GetPassWord                                                                     
 * 描    述:原始密码 可以加入修改密码函数                                                               
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
 * 函数名称:ChangeVariableValues                                                                   
 * 描    述:改变数据变量的值                                                                                                                          
 * 输    入:变量地址,变量数据,变量长度                                                                  
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2014年3月13日                                                                   
 *******************************************************************************/ 
char meat_cnt_t= 0,chicken_cnt_t= 0,duck_cnt_t= 0,fish_cnt_t= 0; /*临时数量*/
uint8_t  PassWordLen=0;	//密码的长度为0
uint8_t  PassWord[6]={0};
uint8_t  InputPassWord[6]={0};
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
							UserAct.MealID= VariableData[1]; //当前用户选餐的ID
							UserAct.MealCnt_1st= 1;//设置默认分数为 1
							PageChange(Meal1st_interface);//显示相应界面
							CutDownDisp(60);//显示倒计时时间
							MealCostDisp(UserAct.MealID,UserAct.MealCnt_1st);//根据用户所选餐品ID号显示合计钱数
						}break;						
					case 0x02:
						if(DefineMeal[1].MealCount > 0)	   //判断餐品是否大于0
						{
							UserAct.MealID= VariableData[1];
							UserAct.MealCnt_2nd= 1;//设置默认分数为 1
							PageChange(Meal2rd_interface);//显示相应界面	
							CutDownDisp(60);//显示倒计时时间
							MealCostDisp(UserAct.MealID,UserAct.MealCnt_2nd);//根据用户所选餐品ID号显示合计钱数
						}break;							
					case 0x03:
						if(DefineMeal[2].MealCount > 0)	   //判断餐品是否大于0
						{
							UserAct.MealID= VariableData[1];
							UserAct.MealCnt_3rd= 1;//设置默认分数为 1
							PageChange(Meal3ns_interface);//显示相应界面	
							CutDownDisp(60);//显示倒计时时间
							MealCostDisp(UserAct.MealID,UserAct.MealCnt_3rd);//根据用户所选餐品ID号显示合计钱数
						}break;						
					case 0x04:
					{
						if(DefineMeal[3].MealCount > 0)	   //判断餐品是否大于0
						{
							UserAct.MealID= VariableData[1];
							UserAct.MealCnt_4th= 1;//设置默认分数为 1
							PageChange(Meal4th_interface);//显示相应界面	
							CutDownDisp(60);//显示倒计时时间
							MealCostDisp(UserAct.MealID,UserAct.MealCnt_4th);//根据用户所选餐品ID号显示合计钱数
						}			
					}break;
					case 0x05:  /*管理用户键*/
					{
						PageChange(Password_interface);
					}break;
					default:break;
				}
			}break;
        				
			case meat_cnt: /*胡萝卜炒肉分数*/
			{
				if(VariableData[1]<= DefineMeal[0].MealCount)	//设置餐品选择的上限
				{
					UserAct.MealCnt_1st_t= VariableData[1];
					MealCostDisp(UserAct.MealID,UserAct.MealCnt_1st_t);
				}
				else
				{
					UserAct.MealCnt_1st_t= DefineMeal[0].MealCount;
					MealCostDisp(UserAct.MealID,UserAct.MealCnt_1st_t);
					VariableChage(meat_cnt,UserAct.MealCnt_1st_t);	//改变变量地址数据
				}	 						 
			}break;
			case chicken_cnt:/*香菇滑鸡分数*/
			{
				if(VariableData[1]<= DefineMeal[1].MealCount)	//设置餐品选择的上限
				{
					UserAct.MealCnt_2nd_t= VariableData[1];
					MealCostDisp(UserAct.MealID,UserAct.MealCnt_2nd_t);
				}
				else
				{
					UserAct.MealCnt_2nd_t= DefineMeal[1].MealCount;
					MealCostDisp(UserAct.MealID,UserAct.MealCnt_2nd_t);
					VariableChage(meat_cnt,UserAct.MealCnt_2nd_t);	//改变变量地址数据
				}	 						 
			}break;
			case duck_cnt:/*脆皮烤鸭*/
			{
				if(VariableData[1]<= DefineMeal[2].MealCount)	//设置餐品选择的上限
				{
					UserAct.MealCnt_3rd_t= VariableData[1];
					MealCostDisp(UserAct.MealID,UserAct.MealCnt_3rd_t);
				}
				else
				{
					UserAct.MealCnt_3rd_t= DefineMeal[2].MealCount;
					MealCostDisp(UserAct.MealID,UserAct.MealCnt_3rd_t);
					VariableChage(meat_cnt,UserAct.MealCnt_3rd_t);	//改变变量地址数据			
				}	 
			}break;	
			case fish_cnt:/*红烧鱼块*/
			{
				if(VariableData[1]<= DefineMeal[3].MealCount)	//设置餐品选择的上限
				{
					UserAct.MealCnt_4th_t= VariableData[1];
					MealCostDisp(UserAct.MealID,UserAct.MealCnt_4th_t);
				}
				else
				{
					UserAct.MealCnt_4th_t= DefineMeal[3].MealCount;
					MealCostDisp(UserAct.MealID,UserAct.MealCnt_4th_t);
					VariableChage(meat_cnt,UserAct.MealCnt_4th_t);	//改变变量地址数据
				}
			}break;	
   
      case mealcnt_choose:/*单页选择按钮*/
      {
			  switch(VariableData[1])
				{
					case 0x01:  /*放入购物车*/
					{
						PutIntoShopCart();		
					}
					case 0x02:  /*继续选餐*/
					{
						PageChange(Menu_interface);
					}
 					case 0x03:  /*结算*/
					{
						SettleAccounts();
					}
					case 0x04:  /*取消*/
					{
						//清空所有的用户数据
						UserAct.MealCnt_1st_t=0;
						UserAct.MealCnt_2nd_t=0;
						UserAct.MealCnt_3rd_t=0;
						UserAct.MealCnt_4th_t=0;
						PageChange(Menu_interface);
					}
					default:break;
				}
			}break;
			/*改变四栏中餐品的数量*/
			/*需要餐品的ID和行号:column1st_count, MealID*/
			case column1st_count:MealID= DispMeal[0].meal_id;
			case column2nd_count:MealID= DispMeal[1].meal_id;
			case column3rd_count:MealID= DispMeal[2].meal_id;
			case column4th_count:MealID= DispMeal[3].meal_id;
			{
				switch(MealID)
				{
					case 0x01:
					{
					  if(VariableData[1]<=DefineMeal[MealID].MealCount)//当用户选择数量小于或等于存货时
				    {
					    UserAct.MealCnt_1st =VariableData[1]; //将改变的值返回到结构体中
					    UserAct.MealCost_1st =UserAct.MealCnt_1st *price_1st; //计算单总价
					    VariableChage(VariableAdress,UserAct.MealCnt_1st); //改变数量
					    VariableChage(VariableAdress+1,UserAct.MealCost_1st); //改变单总价，地址偏移1
				    }
				    else
				    {	
							VariableChage(VariableAdress,UserAct.MealCnt_1st); //维持原来所选的数量
				    }					
					}break;
					case 0x02:
					{
					  if(VariableData[1]<=DefineMeal[MealID].MealCount)//还是要判断数量
				    {
					    UserAct.MealCnt_2nd =VariableData[1]; //将改变的值返回到结构体中
					    UserAct.MealCost_2nd =UserAct.MealCnt_2nd *price_2nd; //计算单总价
					    VariableChage(VariableAdress,UserAct.MealCnt_2nd); //改变数量
					    VariableChage(VariableAdress+1,UserAct.MealCost_2nd); //改变单总价
				    }
				   else
				    {
					    VariableChage(VariableAdress,UserAct.MealCnt_2nd); //维持原来所选的数量
				    }									
					}break;
					case 0x03:
					{
					  if(VariableData[1]<=DefineMeal[MealID].MealCount)//还是要判断数量
				    {
					    UserAct.MealCnt_3rd =VariableData[1]; //将改变的值返回到结构体中
					    UserAct.MealCost_3rd =UserAct.MealCnt_3rd *price_3rd; //计算单总价
					    VariableChage(VariableAdress,UserAct.MealCnt_3rd); //改变数量
					    VariableChage(VariableAdress+1,UserAct.MealCost_3rd); //改变单总价
				    }
				   else
				    {
					     VariableChage(VariableAdress,UserAct.MealCnt_3rd); //维持原来所选的数量
				    }							
					}break;
					case 0x04:
					{
					  if(VariableData[1]<=DefineMeal[MealID].MealCount)//还是要判断数量
				    {
					    UserAct.MealCnt_4th =VariableData[1]; //将改变的值返回到结构体中
					    UserAct.MealCost_4th =UserAct.MealCnt_4th *price_4th; //计算单总价
					    VariableChage(VariableAdress,UserAct.MealCnt_4th); //改变数量
					    VariableChage(VariableAdress+1,UserAct.MealCost_4th); //改变单总价
				    }
				   else
				    {
					     VariableChage(VariableAdress,UserAct.MealCnt_4th); //维持原来所选的数量
				    }							
					}break;
					default:break;						
				}
				UserAct.PayShould= (UserAct.MealCost_1st+UserAct.MealCost_2nd+UserAct.MealCost_3rd+UserAct.MealCost_4th);
	      VariableChage(mealtotoal_cost,UserAct.PayShould);
			}break;
			case payment_method: /*付款方式*/
			{
				switch(VariableData[1])
				{
					case 0x01:   /*现金支付*/
					{
						//OpenCashSystem();
					}break;
					case 0x02:   /*银行预付卡*/
					{
						//CloseCashSystem();
					}break;
					case 0x03:   /*深圳通支付*/
					{
						//CloseCashSystem();
					}break;
					case 0x04:   /*取消*/
					{
						//CloseCashSystem();
					}break;
					default:break;		
				}					
			}break;
			case bill_print:
			{
				switch(VariableData[1])
					case 0x01:   /*打印小票*/
					case 0x02:   /*不打印小票*/
					  default:break;
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
					case 0x11:/*确认件*/
					{
	          GetPassWord(PassWord);
    	      if(VerifyPassword(PassWord, InputPassWord,6) == 0)
		        {
		          /*密码验证错误，进入密码输入界面*/
		           PageChange(Password_interface);
							 DisplayPassWord(0);
			         PassWordLen = 0;
		        }
		        else
		        {
		           /*进入 管理员界面*/
		           PageChange(MealSet_interface);
							 DisplayPassWord(0);//清楚密码显示
			         PassWordLen = 0;
		        }
	        }break;						
					case 0x12:/*返回*/
					{
		        PageChange(Menu_interface);
						DisplayPassWord(0);//清楚密码显示
		        PassWordLen = 0;	
					}break;
          default:break;					
        } 					
			}
			case admin_set:
			{
				switch(VariableData[1])
				{
					case 0x01:  /*温度设置按钮*/
					case 0x02:  /*餐品设置按钮*/
					case 0x03:	/*取消键*/
          default:break;
				}					
			}break;
			case meal_num:
			{
				//VariableData[1]; 需要对当前餐号进行记录,然后再处理
			  CurFloor.MealID= VariableData[1];	
        InitSetting();//清空第一二三列的数据 
			}break;
      case floor_num:
			{
			  CurFloor.FloorNum= VariableData[1];
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
						for(j = 0; j < 4; j++)	 //4代表餐ID号
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
							{														 /*餐的ID是从1-9*/
								if(FloorMealMessageWriteToFlash.FloorMeal[i].MealID == j + 1)
								{
									DefineMeal[j].MealCount      = DefineMeal[j].MealCount + FloorMealMessageWriteToFlash.FloorMeal[i].MealCount;
									DefineMeal[j].Position[i][0] = FloorMealMessageWriteToFlash.FloorMeal[i].FCount;
									DefineMeal[j].Position[i][1] = FloorMealMessageWriteToFlash.FloorMeal[i].SCount;
									DefineMeal[j].Position[i][2] = FloorMealMessageWriteToFlash.FloorMeal[i].TCount;
								}
							}
						}
						WriteMeal();
					}break;
					case 0x04:  /*取消*/
					{
						CurFloor.FCount    = 0;
						CurFloor.SCount    = 0;
						CurFloor.TCount    = 0;
						CurFloor.MealID    = 0;
						CurFloor.MealCount = 0;
						CurFloor.FloorNum  = 0;	
						PageChange(Menu_interface);
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
					default:break;	
				}
			}break;
			case temprature_set:
			{
			//VariableData[1]; 温度变量=VariableData[1];
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
	char VariableLength= 0;   //变量数据的长度
	while(UART3_GetCharsInRxBuf()>0) //获取缓冲区大小，直至缓冲区无数据
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

