#include "stm32f4xx.h"
#include "delay.h"
__IO u16 ntime;								    
//使用SysTick进行精确延时
//ms延时
void delay_ms(u16 nms)
{	 		  	  
	ntime=nms;
	SysTick_Config(168000);//1ms产生一次中断,并对ntime减1
	while(ntime);//等待时间到达
	SysTick->CTRL=0x00;			  	    
}   
//us延时		    								   
void delay_us(u32 nus)
{		
	ntime=nus;
	SysTick_Config(168);//1us产生一次中断,并对ntime减1
	while(ntime);
	SysTick->CTRL=0x00;
}
//粗略延时
void Delay(u32 count)
{
	while(count--);
}




