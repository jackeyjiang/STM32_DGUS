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
File        : GUI_TOUCH_DriverAnalog.c
Purpose     : Touch screen manager
----------------------------------------------------------------------
This module handles the touch screen. It is configured in the file
GUITouch.conf.h (Should be located in the Config\ directory).
----------------------------------------------------------------------
*/

#include "LCD_Private.h"      /* private modul definitions & config */
#include "ltk_touchscreen.h"

/* Generate code only if configuration says so ! */
#if GUI_SUPPORT_TOUCH

/*********************************************************************
*
*       _StoreUnstable
*/
static void _StoreUnstable(int x, int y) {
  static int _xLast = -1;
  static int _yLast = -1;
  int xOut, yOut;

  if ((x != -1) && (y != -1) && (_xLast != -1) && (_yLast != -1)) {
    xOut = _xLast;    
    yOut = _yLast;    
  } else {
    xOut = -1;
    yOut = -1;    
  }
  _xLast = x;
  _yLast = y;
  GUI_TOUCH_StoreUnstable(xOut, yOut);
}

void StoreUnstable_Invalid(void)
{
    _StoreUnstable(-1, -1);
}

/*********************************************************************
*
*       GUI_TOUCH_Exec
*/
void GUI_TOUCH_Exec(void) 
{
    uint8_t touchscreen_error = 0;
    pen_state_t *pen_st;
    pen_st = ltk_get_penstate();
    if(!LTK_PEN_STATE())
    {
        touchscreen_error = ltk_touchscreen_rd_lcd_xy();
        if(touchscreen_error == 0)
        {
              _StoreUnstable(pen_st->x_converted, pen_st->y_converted);
        }
        else if(touchscreen_error == 2)
        {
            _StoreUnstable(-1, -1);
        }
    }
}

#endif    /* defined(GUI_SUPPORT_TOUCH) && GUI_SUPPORT_TOUCH */

/*************************** End of file ****************************/

