#include "stm324xg_eval_tsc.h"

//=========================================================================================================================================

//兼容ADS7843 XPT2046

// A/D 通道选择命令字和工作寄存器
#define	CHX 	0x90 	//通道Y+的选择控制字	
#define	CHY 	0xd0	//通道X+的选择控制字 

//硬件相关的子函数
#define PIN_TP_DCLK 	4		//SCK	O
#define PORT_TP_DCLK 	GPIOB
#define PIN_TP_CS 		15		//NSS	O
#define PORT_TP_CS 		GPIOA
#define PIN_TP_DIN 		1		//MOSI	O
#define PORT_TP_DIN 	GPIOB
#define PIN_TP_DOUT 	5		//MISO	I
#define PORT_TP_DOUT 	GPIOB
#define PIN_TP_INT		15		//IRQ	I
#define PORT_TP_INT		GPIOB
#define PIN_TP_BUSY		2		//BUSY	I
#define PORT_TP_BUSY	GPIOC

#define TP_DCLK(a)	\
						if (!a)	\
						PORT_TP_DCLK->BSRRH |= 1UL << PIN_TP_DCLK;	\
						else		\
						PORT_TP_DCLK->BSRRL |= 1UL << PIN_TP_DCLK
#define TP_CS(a)	\
						if (!a)	\
						PORT_TP_CS->BSRRH |= 1UL << PIN_TP_CS;	\
						else		\
						PORT_TP_CS->BSRRL |= 1UL << PIN_TP_CS
#define TP_DIN(a)	\
						if (!a)	\
						PORT_TP_DIN->BSRRH |= 1UL << PIN_TP_DIN;	\
						else		\
						PORT_TP_DIN->BSRRL |= 1UL << PIN_TP_DIN
													
#define TP_DOUT		(PORT_TP_DOUT->IDR & (1UL << PIN_TP_DOUT)) >> PIN_TP_DOUT
#define TP_BUSY		(PORT_TP_BUSY->IDR & (1UL << PIN_TP_BUSY)) >> PIN_TP_BUSY
#define TP_INT_IN   (PORT_TP_INT->IDR & (1UL << PIN_TP_INT)) >> PIN_TP_INT


void TP_GetAdXY(int *x,int *y);

//====================================================================================
void Delayus( int k)
{
    int j;
    for(j=k << 8;j > 0;j--);
}

//====================================================================================
void TSC_Init(void)
{
	/* Enable SPI1 and GPIO clocks */
	RCC->AHB1ENR    |=((1UL <<  0) |      /* Enable GPIOA clock                 */
	                 (1UL <<  1) |      /* Enable GPIOB clock                 */
	                 (1UL <<  2));      /* Enable GPIOC clock                 */

	//PA.15 as CS OUTPUT
	GPIOA->MODER    &= ~0xC0000000;       /* Clear Bits                         */
	GPIOA->MODER    |=  0x40000000;       /* Alternate Function mode            */
	GPIOA->OSPEEDR  &= ~0xC0000000;       /* Clear Bits                         */
	GPIOA->OSPEEDR  |=  0x80000000;       /* 50 MHz Fast speed                  */
	GPIOA->AFR[0]   &= ~0x00000000;       /* Clear Bits                         */
	GPIOA->AFR[0]   |=  0x00000000;       /* Alternate Function mode AF12       */
	GPIOA->AFR[1]   &= ~0xF0000000;       /* Clear Bits                         */
	GPIOA->AFR[1]   |=  0x00000000;       /* Alternate Function mode AF12       */

	//PB.15 as IRQ[I], PB.04 as SCK[O], PB.05 as MISO[I], PB.01 as MOSI[O]
	GPIOB->MODER    &= ~0xC0000F0C;       /* Clear Bits                         */
	GPIOB->MODER    |=  0x00000104;       /* Alternate Function mode            */
	GPIOB->OSPEEDR  &= ~0xC0000F0C;       /* Clear Bits                         */
	GPIOB->OSPEEDR  |=  0x80000A08;       /* 50 MHz Fast speed                  */
	GPIOB->AFR[0]   &= ~0x00FF00F0;       /* Clear Bits                         */
	GPIOB->AFR[0]   |=  0x00000000;       /* Alternate Function mode AF12       */
	GPIOB->AFR[1]   &= ~0xF0000000;       /* Clear Bits                         */
	GPIOB->AFR[1]   |=  0x00000000;       /* Alternate Function mode AF12       */

	//PC.2 as BUSY[I]
	GPIOC->MODER    &= ~0x00000030;       /* Clear Bits                         */
	GPIOC->MODER    |=  0x00000000;       /* Alternate Function mode            */
	GPIOC->OSPEEDR  &= ~0x00000030;       /* Clear Bits                         */
	GPIOC->OSPEEDR  |=  0x00000020;       /* 50 MHz Fast speed                  */
	GPIOC->AFR[0]   &= ~0x00000F00;       /* Clear Bits                         */
	GPIOC->AFR[0]   |=  0x00000000;       /* Alternate Function mode AF12       */
	GPIOC->AFR[1]   &= ~0x00000000;       /* Clear Bits                         */
	GPIOC->AFR[1]   |=  0x00000000;       /* Alternate Function mode AF12       */

	TP_CS(1);
	TP_DCLK(0);
	TP_DIN(0);
}
//====================================================================================
static void WR_CMD (unsigned char cmd) 
{
    unsigned char buf;
    unsigned char i;
    TP_CS(1);
    TP_DIN(0);
    TP_DCLK(0);
    TP_CS(0);
    for(i=0;i<8;i++) 
    {
        buf=(cmd>>(7-i))&0x1;
        TP_DIN(buf);
        Delayus(5);
        TP_DCLK(1);
        Delayus(5);
        TP_DCLK(0);
    }
}
//====================================================================================
static unsigned short RD_AD(void) 
{
    unsigned short buf=0,temp;
    unsigned char i;
    TP_DIN(0);
    TP_DCLK(1);
    for(i=0;i<12;i++) 
    {
        Delayus(5);
        TP_DCLK(0);         
        Delayus(5);   
        temp= (TP_DOUT) ? 1:0;
        buf|=(temp<<(11-i));
        
        Delayus(5);
        TP_DCLK(1);
    }
    TP_CS(1);
    buf&=0x0fff;
    return(buf);
}
//====================================================================================
int Read_X(void) 
{ 
    int i;
    WR_CMD(CHX);
   // while(TP_BUSY);
    Delayus(5);
    i=RD_AD();
    return i;   
}
//====================================================================================
int Read_Y(void) 
{ 
    int i;
    WR_CMD(CHY);
    //while(TP_BUSY);
    Delayus(5);
    i=RD_AD();
    return i;    
}
//====================================================================================
void TP_GetAdXY(int *x,int *y) 
{
    int adx,ady;
    adx=Read_X();
    ady=Read_Y();
    *x=adx;
    *y=ady;
}

uint32_t TSC_TouchDet (void) 
{
	return !(TP_INT_IN);
}

