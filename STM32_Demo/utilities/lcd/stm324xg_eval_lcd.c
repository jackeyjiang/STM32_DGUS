/******************************************************************************
  * @file    stm324xg_eval_lcd.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *          and AM240320D5TOQW01H (LCD_ILI9325) Liquid Crystal Display Modules
  *          of STM324xG-EVAL evaluation board(MB786) RevB.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm324xg_eval_lcd.h"
#include "../Common/fonts.c"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */

/** @addtogroup STM324xG_EVAL
  * @{
  */

/** @defgroup STM324xG_EVAL_LCD
  * @brief This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *        and AM240320D5TOQW01H (LCD_ILI9325) Liquid Crystal Display Modules
  *        of STM324xG_EVAL board.
  * @{
  */

/** @defgroup STM324xG_EVAL_LCD_Private_TypesDefinitions
  * @{
  */
typedef struct
{
  __IO uint16_t LCD_REG;
  __IO uint16_t LCD_RAM;
} LCD_TypeDef;
/**
  * @}
  */


/** @defgroup STM324xG_EVAL_LCD_Private_Defines
  * @{
  */
/* LCD /CS is NE1 - Bank 3 of NOR/SRAM Bank 1~4                               */
#define LCD_BASE   (0x60000000UL)
#define LCD_REG16  (*((volatile unsigned short *)(LCD_BASE+0x00080000UL)))
#define LCD_DAT16  (*((volatile unsigned short *)(LCD_BASE+0x00100000UL)))


#define MAX_POLY_CORNERS   200
#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))
/**
  * @}
  */

/** @defgroup STM324xG_EVAL_LCD_Private_Macros
  * @{
  */
#define ABS(X)  ((X) > 0 ? (X) : -(X))
/**
  * @}
  */

static int nLcdType = 0;

__IO uint32_t lcdid = 0;

/** @defgroup STM324xG_EVAL_LCD_Private_Variables
  * @{
  */
static sFONT *LCD_Currentfonts;

  /* Global variables to set the written text color */
static __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;

/**
  * @}
  */


/** @defgroup STM324xG_EVAL_LCD_Private_FunctionPrototypes
  * @{
  */
#ifndef USE_Delay
static void delay(__IO uint32_t nCount);
#endif /* USE_Delay*/
static void PutPixel(int16_t x, int16_t y);
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed);


/**
  * @}
  */


/** @defgroup STM324xG_EVAL_LCD_Private_Functions
  * @{
  */

/**
  * @brief  DeInitializes the LCD.
  * @param  None
  * @retval None
  */
void LCD_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< LCD Display Off */
  LCD_DisplayOff();

  /* BANK 1 (of NOR/SRAM Bank 1~4) is disabled */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);

  /*!< LCD_SPI DeInit */
  FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM1);

/*-- GPIO Configuration ------------------------------------------------------*/
  /* SRAM Data lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15 |
                                GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_MCO);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                GPIO_Pin_15;

  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_MCO);	//A19 as RS
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_MCO);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;

  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13, GPIO_AF_MCO);

  /* Blacklight configuration PB0 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_MCO);
  GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
void STM324xG_LCD_Init(void)
{
/* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();	 

/* Configure the FSMC Parallel interface -------------------------------------*/
  LCD_FSMCConfig();

  _delay_(5); /* delay 50 ms */

  /* Read the LCD ID */
  lcdid = LCD_ReadReg(0x00);

  if (lcdid == 0x7783)
  {
    LCD_WriteReg(0x0001, 0x0100);               /* Set SS bit                         */
	LCD_WriteReg(0x0002, 0x0700);
  #if (LANDSCAPE == 1)
    /* AM=1   (address is updated in vertical writing direction)              */
    LCD_WriteReg(0x03, 0x1038);
  #else
    /* AM=0   (address is updated in horizontal writing direction)            */
    LCD_WriteReg(0x03, 0x1030);
  #endif
	LCD_WriteReg(0x0008, 0x0807);
	LCD_WriteReg(0x0009, 0x0000);
	LCD_WriteReg(0x000a, 0x0008);
	LCD_WriteReg(0x0010, 0x0000);
	LCD_WriteReg(0x0011, 0x0005);
	LCD_WriteReg(0x0012, 0x0000);
	LCD_WriteReg(0x0013, 0x0000);
	_delay_(50);
	LCD_WriteReg(0x0010, 0x1290);//12b0
	_delay_(50);      
	LCD_WriteReg(0x0011, 0x0667);//0007
	_delay_(50);       
	LCD_WriteReg(0x0012, 0x008a);
	LCD_WriteReg(0x0013, 0x1900);//1700
	LCD_WriteReg(0x0029, 0x0034);//0020
	_delay_(50);    
	LCD_WriteReg(0x0030, 0x0301);
	LCD_WriteReg(0x0031, 0x0107);
	LCD_WriteReg(0x0032, 0x0301);
	LCD_WriteReg(0x0035, 0x0003);
	LCD_WriteReg(0x0036, 0x0000);
	LCD_WriteReg(0x0037, 0x0000);
	LCD_WriteReg(0x0038, 0x0706);
	LCD_WriteReg(0x0039, 0x0206);
	LCD_WriteReg(0x003c, 0x0004);
	LCD_WriteReg(0x003d, 0x0000);
	LCD_WriteReg(0x0050, 0x0000);
	//LCD_WriteReg(0x0051, (LCD_PIXEL_WIDTH - 1));//00ef
	LCD_WriteReg(0x0051, 239);//00ef
	LCD_WriteReg(0x0052, 0x0000);
	//LCD_WriteReg(0x0053, (LCD_PIXEL_HEIGHT - 1));//013f
	LCD_WriteReg(0x0053, 319);//013f
#if (LANDSCAPE == 1)
	LCD_WriteReg(0x0060, 0x2700);
#else
	LCD_WriteReg(0x0060, 0xa700);
#endif
	LCD_WriteReg(0x0061, 0x0001);
	LCD_WriteReg(0x0090, 0x0030);
	LCD_WriteReg(0x00ff, 0x0001);
	LCD_WriteReg(0x00f3, 0x0008);
	LCD_WriteReg(0x0007, 0x0133);
	LCD_REG16 = 0x22;
  }
  else if(lcdid == 0x47)
  {
  	nLcdType = 2;
	LCD_WriteReg(0x2E,0x79); //
	LCD_WriteReg(0xEE,0x0C); //

	//Driving ability Setting
	LCD_WriteReg(0xEA,0x00); //PTBA[15:8]
	LCD_WriteReg(0xEB,0x20); //PTBA[7:0]
	LCD_WriteReg(0xEC,0x08); //STBA[15:8]
	LCD_WriteReg(0xED,0xC4); //STBA[7:0]
	LCD_WriteReg(0xE8,0x40); //OPON[7:0]
	LCD_WriteReg(0xE9,0x38); //OPON1[7:0]
	LCD_WriteReg(0xF1,0x01); //OTPS1B
	LCD_WriteReg(0xF2,0x10); //GEN
	LCD_WriteReg(0x27,0xA3); //

	//Gamma 2.2 Setting
	LCD_WriteReg(0x40,0x01); //
	LCD_WriteReg(0x41,0x02); //
	LCD_WriteReg(0x42,0x00); //
	LCD_WriteReg(0x43,0x13); //
	LCD_WriteReg(0x44,0x12); //
	LCD_WriteReg(0x45,0x24); //
	LCD_WriteReg(0x46,0x08); //
	LCD_WriteReg(0x47,0x55); //
	LCD_WriteReg(0x48,0x02); //
	LCD_WriteReg(0x49,0x14); //
	LCD_WriteReg(0x4A,0x1A); //
	LCD_WriteReg(0x4B,0x19); //
	LCD_WriteReg(0x4C,0x16); //

	LCD_WriteReg(0x50,0x1B); //
	LCD_WriteReg(0x51,0x2D); //
	LCD_WriteReg(0x52,0x2C); //
	LCD_WriteReg(0x53,0x3F); //
	LCD_WriteReg(0x54,0x3D); //
	LCD_WriteReg(0x55,0x3E); //
	LCD_WriteReg(0x56,0x2A); //
	LCD_WriteReg(0x57,0x77); //
	LCD_WriteReg(0x58,0x09); //
	LCD_WriteReg(0x59,0x06); //
	LCD_WriteReg(0x5A,0x05); //
	LCD_WriteReg(0x5B,0x0B); //
	LCD_WriteReg(0x5C,0x1D); //
	LCD_WriteReg(0x5D,0xCC); //

	//Power Voltage Setting
	LCD_WriteReg(0x1B,0x1B); //VRH=4.65V
	LCD_WriteReg(0x1A,0x01); //BT (VGH~15V,VGL~-10V,DDVDH~5V)
	LCD_WriteReg(0x24,0x2F); //VMH(VCOM High voltage ~3.2V)
	LCD_WriteReg(0x25,0x57); //VML(VCOM Low voltage -1.2V)

	LCD_WriteReg(0x23,0x90); //for Flicker adjust //can reload from OTP

	//Power on Setting
	LCD_WriteReg(0x18,0x39); //I/P_RADJ,N/P_RADJ, Normal mode 75Hz
	LCD_WriteReg(0x19,0x01); //OSC_EN='1', start Osc
	LCD_WriteReg(0x01,0x00); //DP_STB='0', out deep sleep
	LCD_WriteReg(0x1F,0x88);// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
	_delay_(5);
	LCD_WriteReg(0x1F,0x80);// GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
	_delay_(5);
	LCD_WriteReg(0x1F,0x90);// GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
	_delay_(5);
	LCD_WriteReg(0x1F,0xD0);// GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
	_delay_(5);

#if (LANDSCAPE == 1)
	LCD_WriteReg (0x16, 0xA8);
#else
	LCD_WriteReg (0x16, 0x08);
#endif

	//262k/65k color selection
	LCD_WriteReg(0x17,0x05); //default 0x06 262k color // 0x05 65k color

	//SET PANEL
	LCD_WriteReg(0x36,0x00); //SS_P, GS_P,REV_P,BGR_P

	//Display ON Setting
	LCD_WriteReg(0x28,0x38); //GON=1, DTE=1, D=1000
	_delay_(40);
	LCD_WriteReg(0x28,0x3F); //GON=1, DTE=1, D=1100

	//Set GRAM Area
	LCD_WriteReg(0x02,0x00);
	LCD_WriteReg(0x03,0x00); //Column Start
	LCD_WriteReg(0x04,(LCD_PIXEL_WIDTH - 1) >> 8);
	LCD_WriteReg(0x05,(LCD_PIXEL_WIDTH - 1));
	LCD_WriteReg(0x06,0x00);
	LCD_WriteReg(0x07,0x00); //Row Start
	LCD_WriteReg(0x08,(LCD_PIXEL_HEIGHT - 1) >> 8);
	LCD_WriteReg(0x09,(LCD_PIXEL_HEIGHT - 1));
	LCD_REG16 = 0x22;
  }
  LCD_SetFont(&LCD_DEFAULT_FONT);
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

/**
  * @brief  Sets the LCD Text and Background colors.
  * @param  _TextColor: specifies the Text Color.
  * @param  _BackColor: specifies the Background Color.
  * @retval None
  */
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
  TextColor = _TextColor;
  BackColor = _BackColor;
}

/**
  * @brief  Gets the LCD Text and Background colors.
  * @param  _TextColor: pointer to the variable that will contain the Text
            Color.
  * @param  _BackColor: pointer to the variable that will contain the Background
            Color.
  * @retval None
  */
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
  *_TextColor = TextColor; *_BackColor = BackColor;
}

/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(__IO uint16_t Color)
{
  TextColor = Color;
}


/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(__IO uint16_t Color)
{
  BackColor = Color;
}

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}

/**
  * @brief  Clears the selected line.
  * @param  Line: the Line to be cleared.
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..n
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
  uint16_t refcolumn = 0;
  /* Send the string character by character on lCD */
  while ((refcolumn + LCD_Currentfonts->Width) <= LCD_PIXEL_WIDTH)
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, ' ');
    /* Decrement the column position by 16 */
    refcolumn += LCD_Currentfonts->Width;
  }
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t Color)
{
  uint32_t index = 0;

	if (nLcdType == 0x02)
	{
		LCD_SetCursor(0x00, 0x00);
	}
	else
	{
	  	LCD_SetCursor(0x00, 0x00);
	}
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  for(index = 0; index < 240 * 320; /*76800; */index++)
  {
   	LCD_DAT16 = Color;
  }
}

/**
  * @brief  Sets the cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval None
  */
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	if (nLcdType == 0x02)
	{
		LCD_WriteReg(0x02,Xpos >> 8);
		LCD_WriteReg(0x03,Xpos);
		LCD_WriteReg(0x06,Ypos >> 8);
		LCD_WriteReg(0x07,Ypos);
	}
	else
	{
		//Xpos = 20;
		//Ypos = 40;
#if (LANDSCAPE == 1)
		LCD_WriteReg(0x20, Ypos);
		LCD_WriteReg(0x21, Xpos);
#else
		LCD_WriteReg(0x20, Xpos);
		LCD_WriteReg(0x21, Ypos);
#endif
	}
}

/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, i = 0;

  //LCD_SetCursor(Xpos, Ypos);
  LCD_SetDisplayWindow(Xpos, Ypos, LCD_Currentfonts->Width, LCD_Currentfonts->Height);

  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(i = 0; i < LCD_Currentfonts->Width; i++)
    {

      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> i)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
        (((c[index] & (0x1 << i)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))

      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      }
    }
    Ypos++;
    LCD_SetCursor(Xpos, Ypos);
  }
}

/**
  * @brief  Displays one character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_DrawChar(Column, Line, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}


//取得汉字字模所在位置
static unsigned short GetHanziPos(const char *hz)
{
    int i;
    for (i = 0; i < sizeof(hzIndex)/sizeof(hzIndex[0]); i += 2)
    {
        if (hzIndex[i] == (unsigned char)hz[0] && hzIndex[i + 1] == (unsigned char)hz[1])
        {
            return (i >> 1);   //就是除以2
        }
    }
    return 0xffff;
}

//显示汉字
void LCD_DisplayHanzi(uint16_t Xpos, uint16_t Ypos, const char *hz)
{
    int i;
    unsigned char *p;
    unsigned short index;
    
    index = GetHanziPos(hz);
    if (index == 0xffff) return;
    
    p = hzfont + index * 32;
    
    LCD_SetDisplayWindow(Xpos, Ypos, 16, 16);
    for(index = 0; index < 16; index++)
    {
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        for(i = 0; i < 16; i++)
        {
            if (i == 8) p++;
            if ((*p & (0x80 >> (i % 8))) == 0x00)
            {
                LCD_WriteRAM(BackColor);
            }
            else
            {
                LCD_WriteRAM(TextColor);
            }
        }
        p++;
        Ypos++;
        LCD_SetCursor(Xpos, Ypos);
    }
}

/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  *ptr: pointer to string to display on LCD.
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{
  uint16_t refcolumn = 0;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & ((refcolumn + LCD_Currentfonts->Width) <= LCD_PIXEL_WIDTH))
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, *ptr);
    /* Decrement the column position by 16 */
    refcolumn += LCD_Currentfonts->Width;
    /* Point on the next character */
    ptr++;
  }
}

/**
  * @brief  Sets a display window
  * @param  Xpos: specifies the X bottom left position.
  * @param  Ypos: specifies the Y bottom left position.
  * @param  Height: display window height.
  * @param  Width: display window width.
  * @retval None
  */
void LCD_SetDisplayWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	if (nLcdType == 0x02)
	{
#if (LANDSCAPE == 1)
		LCD_WriteReg(0x02, (x >> 8) & 0xff);
		LCD_WriteReg(0x03, x & 0xff); //Column Start
		LCD_WriteReg(0x04, (x + w - 1) >> 8);
		LCD_WriteReg(0x05, (x + w - 1) & 0xff);
		LCD_WriteReg(0x06, (y >> 8) & 0xff);
		LCD_WriteReg(0x07, y & 0xff); //Row Start
		LCD_WriteReg(0x08, (y + h - 1) >> 8);
		LCD_WriteReg(0x09, (y + h - 1) & 0xff);
#else
		LCD_WriteReg(0x02, x >> 8);
		LCD_WriteReg(0x03, x); //Column Start
		LCD_WriteReg(0x04, (x + w - 1) >> 8);
		LCD_WriteReg(0x05, (x + w - 1));
		LCD_WriteReg(0x06, y >> 8);
		LCD_WriteReg(0x07, y); //Row Start
		LCD_WriteReg(0x08, (y + h - 1) >> 8);
		LCD_WriteReg(0x09, (y + h - 1));
#endif
	}
	else
	{
#if (LANDSCAPE == 1)
		LCD_WriteReg(0x50, y);                    /* Vertical   GRAM Start Address      */
		LCD_WriteReg(0x51, y+h-1);                /* Vertical   GRAM End   Address (-1) */
		LCD_WriteReg(0x52, x);                    /* Horizontal GRAM Start Address      */
		LCD_WriteReg(0x53, x+w-1);                /* Horizontal GRAM End   Address (-1) */
		LCD_WriteReg(0x20, y);
		LCD_WriteReg(0x21, x);
#else
		LCD_WriteReg(0x50, x);                    /* Horizontal GRAM Start Address      */
		LCD_WriteReg(0x51, x+w-1);                /* Horizontal GRAM End   Address (-1) */
		LCD_WriteReg(0x52, y);                    /* Vertical   GRAM Start Address      */
		LCD_WriteReg(0x53, y+h-1);                /* Vertical   GRAM End   Address (-1) */
		LCD_WriteReg(0x20, x);
		LCD_WriteReg(0x21, y);
#endif
	}
}

/**
  * @brief  Disables LCD Window mode.
  * @param  None
  * @retval None
  */
void LCD_WindowModeDisable(void)
{
  LCD_SetDisplayWindow(239, 0x13F, 240, 320);
  LCD_WriteReg(LCD_REG_3, 0x1018);
}

/**
  * @brief  Displays a line.
  * @param Xpos: specifies the X position.
  * @param Ypos: specifies the Y position.
  * @param Length: line length.
  * @param Direction: line direction.
  *   This parameter can be one of the following values: Vertical or Horizontal.
  * @retval None
  */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
  uint32_t i = 0;

  LCD_SetCursor(Xpos, Ypos);
  if(Direction == LCD_DIR_HORIZONTAL)
  {
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
      LCD_WriteRAM(TextColor);
      if (LANDSCAPE) Xpos++;
      else Ypos++;
      //Xpos++;
      LCD_SetCursor(Xpos, Ypos);
    }
  }
  else
  {
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
      LCD_WriteRAM(TextColor);
      if (LANDSCAPE) Ypos++;
      else Xpos++;
      //Xpos++;
      LCD_SetCursor(Xpos, Ypos);
    }
  }
}

/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: display rectangle height.
  * @param  Width: display rectangle width.
  * @retval None
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
  LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
  LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);

  LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
  LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);
}

/**
  * @brief  Displays a circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;/* Decision Variable */
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */

  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;

  while (CurX <= CurY)
  {
    LCD_SetCursor(Xpos + CurX, Ypos + CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurX, Ypos - CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos + CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos - CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos + CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos - CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos + CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos - CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

/**
  * @brief  Displays a mono-color picture.
  * @param  Pict: pointer to the picture array.
  * @retval None
  */
void LCD_DrawMonoPict(const uint32_t *Pict)
{
  uint32_t index = 0, i = 0;
  LCD_SetCursor(0, (LCD_PIXEL_WIDTH - 1));
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  for(index = 0; index < 2400; index++)
  {
    for(i = 0; i < 32; i++)
    {
      if((Pict[index] & (1 << i)) == 0x00)
      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      }
    }
  }
}

/**
  * @brief  Displays a bitmap picture loaded in the internal Flash.
  * @param  BmpAddress: Bmp picture address in the internal Flash.
  * @retval None
  */
void LCD_WriteBMP(uint32_t BmpAddress)
{
  uint32_t index = 0, size = 0;
  /* Read bitmap size */
  size = *(__IO uint16_t *) (BmpAddress + 2);
  size |= (*(__IO uint16_t *) (BmpAddress + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(__IO uint16_t *) (BmpAddress + 10);
  index |= (*(__IO uint16_t *) (BmpAddress + 12)) << 16;
  size = (size - index)/2;
  BmpAddress += index;
  /* Set GRAM write direction and BGR = 1 */
  /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
  /* AM=1 (address is updated in vertical writing direction) */
  if (nLcdType == 0x02) LCD_WriteReg(0x16,0x88);
  else
  {
  #if (LANDSCAPE == 1)
    /* AM=1   (address is updated in vertical writing direction)              */
    LCD_WriteReg(0x03, 0x1028);
  #else
    /* AM=0   (address is updated in horizontal writing direction)            */
    LCD_WriteReg(0x03, 0x1020);
  #endif
  }

  LCD_WriteRAM_Prepare();

  for(index = 0; index < size; index++)
  {
    LCD_WriteRAM(*(__IO uint16_t *)BmpAddress);
    BmpAddress += 2;
  }

  /* Set GRAM write direction and BGR = 1 */
  /* I/D = 01 (Horizontal : increment, Vertical : decrement) */
  /* AM = 1 (address is updated in vertical writing direction) */
  if (nLcdType == 0x02) LCD_WriteReg(0x16,0x00);
  else
  {
  #if (LANDSCAPE == 1)
    /* AM=1   (address is updated in vertical writing direction)              */
    LCD_WriteReg(0x03, 0x1038);
  #else
    /* AM=0   (address is updated in horizontal writing direction)            */
    LCD_WriteReg(0x03, 0x1030);
  #endif
  }
}

/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: rectangle height.
  * @param  Width: rectangle width.
  * @retval None
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  LCD_SetTextColor(TextColor);

  LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
  LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);

  LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
  LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);

  Width -= 2;
  Height--;
  Ypos--;

  LCD_SetTextColor(BackColor);

  while(Height--)
  {
    LCD_DrawLine(++Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
  }

  LCD_SetTextColor(TextColor);
}

/**
  * @brief  Displays a full circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;/* Decision Variable */
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */

  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;

  while (CurX <= CurY)
  {
    LCD_DrawLine(Xpos - CurX, Ypos - CurY, CurY * 2, LCD_DIR_VERTICAL);
    LCD_DrawLine(Xpos + CurX, Ypos - CurY, CurY * 2, LCD_DIR_VERTICAL);
    LCD_DrawLine(Xpos - CurY, Ypos + CurX, CurY * 2, LCD_DIR_HORIZONTAL);
    LCD_DrawLine(Xpos - CurY, Ypos - CurX, CurY * 2, LCD_DIR_HORIZONTAL);
    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

/**
  * @brief  Displays an uni-line (between two points).
  * @param  x1: specifies the point 1 x position.
  * @param  y1: specifies the point 1 y position.
  * @param  x2: specifies the point 2 x position.
  * @param  y2: specifies the point 2 y position.
  * @retval None
  */
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
  curpixel = 0;

  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */

  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    PutPixel(x, y);             /* Draw the current pixel */
    num += numadd;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
    {
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
    }
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
  }
}

/**
  * @brief  Displays an poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLine(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0;

  if(PointCount < 2)
  {
    return;
  }

  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    LCD_DrawUniLine(X, Y, Points->X, Points->Y);
  }
}

/**
  * @brief  Displays an relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @param  Closed: specifies if the draw is closed or not.
  *           1: closed, 0 : not closed.
  * @retval None
  */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed)
{
  int16_t X = 0, Y = 0;
  pPoint First = Points;

  if(PointCount < 2)
  {
    return;
  }
  X = Points->X;
  Y = Points->Y;
  while(--PointCount)
  {
    Points++;
    LCD_DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
    X = X + Points->X;
    Y = Y + Points->Y;
  }
  if(Closed)
  {
    LCD_DrawUniLine(First->X, First->Y, X, Y);
  }
}

/**
  * @brief  Displays a closed poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLine(Points, PointCount);
  LCD_DrawUniLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
}

/**
  * @brief  Displays a relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

/**
  * @brief  Displays a closed relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}


/**
  * @brief  Displays a  full poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount)
{
  /*  public-domain code by Darel Rex Finley, 2007 */
  uint16_t  nodes = 0, nodeX[MAX_POLY_CORNERS], pixelX = 0, pixelY = 0, i = 0,
  j = 0, swap = 0;
  uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

  IMAGE_LEFT = IMAGE_RIGHT = Points->X;
  IMAGE_TOP= IMAGE_BOTTOM = Points->Y;

  for(i = 1; i < PointCount; i++)
  {
    pixelX = POLY_X(i);
    if(pixelX < IMAGE_LEFT)
    {
      IMAGE_LEFT = pixelX;
    }
    if(pixelX > IMAGE_RIGHT)
    {
      IMAGE_RIGHT = pixelX;
    }

    pixelY = POLY_Y(i);
    if(pixelY < IMAGE_TOP)
    {
      IMAGE_TOP = pixelY;
    }
    if(pixelY > IMAGE_BOTTOM)
    {
      IMAGE_BOTTOM = pixelY;
    }
  }

  LCD_SetTextColor(BackColor);

  /*  Loop through the rows of the image. */
  for (pixelY = IMAGE_TOP; pixelY < IMAGE_BOTTOM; pixelY++)
  {
    /* Build a list of nodes. */
    nodes = 0; j = PointCount-1;

    for (i = 0; i < PointCount; i++)
    {
      if (((POLY_Y(i)<(double) pixelY) && (POLY_Y(j)>=(double) pixelY)) || \
          ((POLY_Y(j)<(double) pixelY) && (POLY_Y(i)>=(double) pixelY)))
      {
        nodeX[nodes++]=(int) (POLY_X(i)+((pixelY-POLY_Y(i))*(POLY_X(j)-POLY_X(i)))/(POLY_Y(j)-POLY_Y(i)));
      }
      j = i;
    }

    /* Sort the nodes, via a simple "Bubble" sort. */
    i = 0;
    while (i < nodes-1)
    {
      if (nodeX[i]>nodeX[i+1])
      {
        swap = nodeX[i];
        nodeX[i] = nodeX[i+1];
        nodeX[i+1] = swap;
        if(i)
        {
          i--;
        }
      }
      else
      {
        i++;
      }
    }

    /*  Fill the pixels between node pairs. */
    for (i = 0; i < nodes; i+=2)
    {
      if(nodeX[i] >= IMAGE_RIGHT)
      {
        break;
      }
      if(nodeX[i+1] > IMAGE_LEFT)
      {
        if (nodeX[i] < IMAGE_LEFT)
        {
          nodeX[i]=IMAGE_LEFT;
        }
        if(nodeX[i+1] > IMAGE_RIGHT)
        {
          nodeX[i+1] = IMAGE_RIGHT;
        }
        LCD_SetTextColor(BackColor);
        LCD_DrawLine(pixelY, nodeX[i+1], nodeX[i+1] - nodeX[i], LCD_DIR_HORIZONTAL);
        LCD_SetTextColor(TextColor);
        PutPixel(pixelY, nodeX[i+1]);
        PutPixel(pixelY, nodeX[i]);
        /* for (j=nodeX[i]; j<nodeX[i+1]; j++) PutPixel(j,pixelY); */
      }
    }
  }

  /* draw the edges */
  LCD_SetTextColor(TextColor);
}

/**
  * @brief  Writes to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @param  LCD_RegValue: value to write to the selected register.
  * @retval None
  */
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
  LCD_REG16 = LCD_Reg;
  /* Write 16-bit Reg */
  LCD_DAT16 = LCD_RegValue;
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  LCD_Reg: address of the selected register.
  * @retval LCD Register Value.
  */
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD_REG16 = LCD_Reg;
  /* Read 16-bit Reg */
  return (LCD_DAT16);
}

/**
  * @brief  Prepare to write to the LCD RAM.
  * @param  None
  * @retval None
  */
void LCD_WriteRAM_Prepare(void)
{
	LCD_REG16 = 0x22;
}

/**
  * @brief  Writes to the LCD RAM.
  * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
  * @retval None
  */
void LCD_WriteRAM(uint16_t RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  LCD_DAT16 = RGB_Code;
}

/**
  * @brief  Reads the LCD RAM.
  * @param  None
  * @retval LCD RAM Value.
  */
uint16_t LCD_ReadRAM(void)
{
	uint32_t v;
	/* Write 16-bit Index (then Read Reg) */
	LCD_REG16 = 0x22; /* Select GRAM Reg */
	_delay_(1);
	/* Read 16-bit Reg */
	v = LCD_DAT16;
	//_delay_(1);
	v <<= 16;
	v |= LCD_DAT16;
	
	return v;
}

/**
  * @brief  Power on the LCD.
  * @param  None
  * @retval None
  */
void LCD_PowerOn(void)
{
	/* Power On sequence ---------------------------------------------------------*/
  LCD_WriteReg(LCD_REG_16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(LCD_REG_17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
  LCD_WriteReg(LCD_REG_18, 0x0000); /* VREG1OUT voltage */
  LCD_WriteReg(LCD_REG_19, 0x0000); /* VDV[4:0] for VCOM amplitude*/
  _delay_(20);                 /* Dis-charge capacitor power voltage (200ms) */
  LCD_WriteReg(LCD_REG_16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(LCD_REG_17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
  _delay_(5);                  /* _delay_ 50 ms */
  LCD_WriteReg(LCD_REG_18, 0x0139); /* VREG1OUT voltage */
  _delay_(5);                  /* _delay_ 50 ms */
  LCD_WriteReg(LCD_REG_19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
  LCD_WriteReg(LCD_REG_41, 0x0013); /* VCM[4:0] for VCOMH */
  _delay_(5);                  /* _delay_ 50 ms */
  LCD_WriteReg(LCD_REG_7, 0x0173);  /* 262K color and display ON */
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void LCD_DisplayOn(void)
{
  /* Display On */
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);
  LCD_WriteReg(LCD_REG_7, 0x0173); /* 262K color and display ON */
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void LCD_DisplayOff(void)
{
  /* Display Off */
  LCD_WriteReg(LCD_REG_7, 0x0);
  GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

/**
  * @brief  Configures LCD Control lines (FSMC Pins) in alternate function mode.
  * @param  None
  * @retval None
  */
void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOB, GPIOD, GPIOE AFIO clocks */
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
  //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG | 
   //                      RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, ENABLE);

/*-- GPIO Configuration ------------------------------------------------------*/
  /* PDs */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15 |
                                GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

  /* PEs */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

  /* Blacklight configuration PB0 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_MCO);
  GPIO_SetBits(GPIOB, GPIO_Pin_0);
  //GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

/**
  * @brief  Configures the Parallel interface (FSMC) for LCD(Parallel mode)
  * @param  None
  * @retval None
  */
void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;

  /* Enable FSMC clock */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

/*-- FSMC Configuration ------------------------------------------------------*/
/*----------------------- SRAM Bank 1 ----------------------------------------*/
  /* FSMC_Bank1_NORSRAM4 configuration */
  p.FSMC_AddressSetupTime = 5;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 10;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;
  /* Color LCD configuration ------------------------------------
     LCD configured as follow:
        - Data/Address MUX = Disable
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable
        - Extended Mode = Enable
        - Asynchronous Wait = Disable */

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

  /* Enable FSMC NOR/SRAM Bank3 */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/**
  * @brief  Displays a pixel.
  * @param  x: pixel x.
  * @param  y: pixel y.
  * @retval None
  */
static void PutPixel(int16_t x, int16_t y)
{
  if(x < 0 || x > 239 || y < 0 || y > 319)
  {
    return;
  }
  LCD_DrawLine(x, y, 1, LCD_DIR_HORIZONTAL);
}

void ReverseLCD(void)
{
	if (nLcdType == 0x02)
	{
#if (LANDSCAPE == 1)
	LCD_WriteReg (0x16, 0xe8);
#else
	LCD_WriteReg (0x16, 0xC8);
#endif
	}
	else
	{
		LCD_WriteReg(0x0001, 0x0000);
#if (LANDSCAPE == 1)
//    	LCD_WriteReg(0x03, 0x1028);
#else
//    	LCD_WriteReg(0x03, 0x1020);
#endif
	}
}

void NormalLCD(void)
{
	if (nLcdType == 0x02)
	{
#if (LANDSCAPE == 1)
	LCD_WriteReg (0x16, 0xA8);
#else
	LCD_WriteReg (0x16, 0x08);
#endif
	}
	else
	{
		LCD_WriteReg(0x0001, 0x0100);
#if (LANDSCAPE == 1)
//    	LCD_WriteReg(0x03, 0x1038);
#else
//    	LCD_WriteReg(0x03, 0x1030);
#endif
	}
}

#ifndef USE_Delay
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0;
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}
#endif /* USE_Delay*/
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
