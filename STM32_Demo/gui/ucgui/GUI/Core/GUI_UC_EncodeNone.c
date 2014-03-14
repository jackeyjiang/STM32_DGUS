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
File        : GUI_UC_EncodeNone.c
Purpose     : Encoding routines for non unicode systems (default)
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI_Protected.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetCharCode
*
* Purpose:
*   Return the UNICODE character code of the current character.
*/
static U16 _GetCharCode(const char GUI_UNI_PTR * s) {
    U16 r;
    U8 Char = *(const U8*)s;
    if ((Char & 0x80) == 0) {                /* Single byte (ASCII)  */
      r = Char;
    }else{ /* Double byte sequence */
      r = (Char & 0xff) << 8;
      Char = *(++s);
      Char &= 0xff;
      r |= Char;
    } 
    return r;

}

/*********************************************************************
*
*       _GetCharSize
*
* Purpose:
*   Return the number of bytes of the current character.
*/
static int _GetCharSize(const char GUI_UNI_PTR * s) {
    U8 Char = *(const U8*)s;
    if ((Char & 0x80) == 0){
      return 1;
    }else{
      return 2;
    }
}

/*********************************************************************
*
*       _CalcSizeOfChar
*
* Purpose:
*   Return the number of bytes needed for the given character.
*/
static int _CalcSizeOfChar(U16 Char) {
    if (Char & 0xFF80)
    {
      return 2;
    }else{
      return 1;
    }

}

/*********************************************************************
*
*       _Encode
*
* Purpose:
*   Encode character into 1/2/3 bytes.
*/
static int _Encode(char *s, U16 Char) {
    int r;
    r = _CalcSizeOfChar(Char);
    switch (r) {
    case 1:
      *s = (char)Char;
      break;
    case 2:
      *s++ = 0xff | (Char >> 8);
      *s   = Char & 0xff;
      break;
    }
    return r;

}

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _API_Table
*/
const GUI_UC_ENC_APILIST GUI_UC_None = {
  _GetCharCode,     /*  return character code as U16 */
  _GetCharSize,     /*  return size of character: 1 */
  _CalcSizeOfChar,  /*  return size of character: 1 */
  _Encode           /*  Encode character */
};

/*********************************************************************
*
*       Exported code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_UC_SetEncodeNone
*/
void GUI_UC_SetEncodeNone(void) {
  #if GUI_SUPPORT_UNICODE
    GUI_LOCK();
    GUI_Context.pUC_API = &GUI_UC_None;
    GUI_UNLOCK();
  #endif
}

/*************************** End of file ****************************/
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
