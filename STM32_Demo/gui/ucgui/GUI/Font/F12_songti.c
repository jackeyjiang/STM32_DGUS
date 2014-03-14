/*
*********************************************************************************************************
*                                             uC/GUI V3.98
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : F12_songti.c
Purpose     : Chinese font GB2312
Height      : 12
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI_FontIntern.h"

extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font8x12ASCII_CharInfo[96];
GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ12_CharInfo[1] = 
{    
  {  12,  12,  2, (void *)"/ltk/font/songti_12.FON"},   
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontArialNarrow12_Prop2 = 
{
   0xA1A1,
   0xF7FE,
   &GUI_FontHZ12_CharInfo[0], 
   (GUI_CONST_STORAGE GUI_FONT_PROP*)0
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontArialNarrow12_Prop1 = 
{
   32,
   126,
   &GUI_Font8x12ASCII_CharInfo[0],
   (void GUI_CONST_STORAGE *)&GUI_FontArialNarrow12_Prop2
};

GUI_CONST_STORAGE GUI_FONT GUI_FontArialNarrow12 = 
{
   GUI_FONTTYPE_PROP,
   12,
   12,
   1,
   1,
   (void GUI_CONST_STORAGE *)&GUI_FontArialNarrow12_Prop1,
   10, /* Baseline */
   6,  /* LHeight */
   9   /* CHeight */
};

