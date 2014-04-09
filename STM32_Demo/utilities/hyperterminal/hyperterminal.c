#include "stm32f4xx_usart.h"
#include "stdio.h"
#define LINE_LENGTH 20  /*行缓冲区大小，决定每行最多输入的字符数*/
/*标准终端设备中，特殊ACSII码定义，请勿修改*/
#define In_BACKSP  0x08		       /*ASCII <--(退格键)*/
#define In_DELETE  0x7F			   /*ASCII <DEL> (DEL键)*/
#define In_EOL  '\r'			   /*ASCII <CR> (回车键)*/
#define In_SKIP '\3'			   /*ASCII control-C*/
#define In_EOF  '\x1A'			   /*ASCII control-Z*/
#define Out_DELETE  "\x8 \x8"	   /*VT100 backspace and clear*/
#define Out_SKIP  "^C\n"		   /*C and new line*/
#define Out_EOF  "^Z" 			   /*^Z and return EOF*/

/****************************************************
*名    称:putchar()
*功    能:向标准终端设备发送一个字节数据
*入口参数:ch:待发送的字符
*出口函数:发出的字符
说     明:printf函数会调用该函数作为底层输出。这里从串口输出字符到PC的
          超级终端，printf的结果将打印到超级终端上
****************************************************/
//int putchar(int ch)
int fputc(int ch, FILE *f)
{
  if(ch=='\n')  //'\n'
  {
    USART_SendData(USART3,0x0d);    //'\r'
  }
  USART_SendData(USART3,ch);   //从串口发出数据
  return(ch);     //将发送的数据原样返回表述成功
}
/****************************************************
*名    称:put_message()
*功    能:向标准终端设备发送一个字符串
*入口参数:无
*出口函数:无
*说    明:无
****************************************************/
static void put_message(char *s)
{
  while(*s)			 //当前字符不为空(字符串以0x00结尾)
  {
	 putchar(*s++);  //输出一个字符，指针指向下一个字符
  }
}

/****************************************************
*名    称:getchar()
*功    能:向标准终端设备发送一个字节数据
*入口参数:ch:待发送的字符
*出口函数:发出的字符
*说    明:printf函数会调用该函数作为底层输出。这里从串口输出字符到PC的
          超级终端，printf的结果将打印到超级终端上
****************************************************/
//int getchar(void)
//{
//  char ch;
//  ch= USART_ReceiveData(USART3);
//  putchar(ch);
//  return(ch);
//}

//int getchar(void)
int fgetc(FILE *f)
{
  static char io_buffer[LINE_LENGTH+2];   /*Where to put chars*/
  static int ptr;                         /*point to buffer*/
  char c;
  while(1)
  {
    if(io_buffer[ptr])
	  return (io_buffer[ptr++]);
	ptr= 0;
	while(1)
	{
	  c= USART_ReceiveData(USART3); 
	  if(c== In_EOF&&!ptr)			     //---EOF键(Ctrl+Z)---
	  {									 //EOF符只能在未输入其他字符时才有效
	    put_message(Out_EOF);			 //让终端显示EOF符
		return EOF;
	  }
	  if((c==In_DELETE)||(c==In_BACKSP))  //---退格或DEL键---
	  {
	    if(ptr)
		{
		  ptr--;						  //将缓冲区删掉一个字符
		  put_message(Out_DELETE);		  //让终端显示也删掉一个字符
		}
	  }
	  else if(c==In_SKIP)				  //---取消键Ctrl+C---
	  {
	    put_message(Out_SKIP);			  //让终端显示放弃本行调到下一行
		ptr =LINE_LENGTH+1;				  /*这里永远是0(结束符)*/
		break;
	  }
	  else if(c==In_EOL)				  //---遇到回车键---
	  {
	    putchar(io_buffer[ptr++]='\n');	  //让终端换行
		io_buffer[ptr]=	0;				  //末尾添加字符结束字符NULL
		ptr =0;							  //指针清空
		break;							  //跳出后开始返回数据
	  }
	  else if(c<LINE_LENGTH)			  //---正常ACSII码字符---
	  {
	    if(c>=' ')						  //删除0x20以下的其他ACSII码
		{
		  putchar(io_buffer[ptr++]=c);	  //存入缓冲区
		}
	  }
	  else
	  {									  //---缓冲区已满---
	    putchar('\7'); //想终端发送鸣响符0x07,PC会响一声，提示已满
	  }
	}
  }
}





