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
File        : GUIConf.h
Purpose     : Configures abilities, fonts etc.
----------------------------------------------------------------------
*/


#ifndef GUICONF_H
#define GUICONF_H

#if (defined LTK_FREERTOS || defined LTK_UCOS)
#define GUI_OS                    (1)  /* Compile with multitasking support */
#else
#define GUI_OS                    (0)  /* Compile without multitasking support */
#endif
#define GUI_SUPPORT_TOUCH         (1)  /* Support a touch screen (req. win-manager) */
#define GUI_SUPPORT_MOUSE         (0)  /* Support a mouse */
#define GUI_SUPPORT_UNICODE       (1)  /* Support mixed ASCII/UNICODE strings */

#ifdef LTK_ALL_IN_ONE
#define GUI_DEFAULT_FONT          &GUI_FontArialNarrow12
#define GUI_ALLOC_SIZE            (1 * 1024)  /* Size of dynamic memory ... For WM and memory devices*/
#else
#define GUI_DEFAULT_FONT          &GUI_Font6x8
#define GUI_ALLOC_SIZE            (5 * 1024)  /* Size of dynamic memory ... For WM and memory devices*/
#endif

#define GUI_DRAW_ALLOC_MEM        (4 * 1024)

/*********************************************************************
*
*         Configuration of available packages
*/

#define GUI_WINSUPPORT            1  /* Window manager package available */
#define GUI_SUPPORT_MEMDEV        1  /* Memory devices available */
#define GUI_SUPPORT_AA            1  /* Anti aliasing available */

#if (defined LTK_FREERTOS || defined LTK_UCOS)
#define GUI_X_WAIT_EVENT()      GUI_X_WaitEvent()
#define GUI_X_SIGNAL_EVENT()    GUI_X_SignalEvent()
#endif

#endif  /* Avoid multiple inclusion */

