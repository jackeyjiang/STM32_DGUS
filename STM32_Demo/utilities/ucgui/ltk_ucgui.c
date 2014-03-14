/**
  ******************************************************************************
  * 文件: ltk_ucgui.c
  * 作者: LeiTek (leitek.taobao.com)
  * 版本: V1.0.0
  * 描述: 移植 uCGUI 需要定义的函数
  ******************************************************************************
  *
  *                  版权所有 (C), LeiTek (leitek.taobao.com)
  *                                www.leitek.com
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "GUI_Private.h"
#include "ltk_systick.h"
#ifdef LTK_FREERTOS
#include "FreeRTOS.h"
#endif

/**
  * 描述: 空函数
  * 参数: 无
  * 返回: 无
  */
void GUI_X_Init (void) 
{
}

/**
  * 描述: 延迟一段时间
  * 参数: 需要延迟的时间
  * 返回: 无
  */
void  GUI_X_Delay (int period) 
{
    ltk_systick_delay(period);
}

/**
  * 描述: 空函数
  * 参数: 无
  * 返回: 无
  */
void GUI_X_ExecIdle (void) 
{
}

/**
  * 描述: 得到当前的时间
  * 参数: 无
  * 返回: 当前时间
  */
int  GUI_X_GetTime (void) 
{
    #if defined LTK_FREERTOS
    return ((int)xTaskGetTickCount());
    #elif (defined LTK_UCOS)
    OS_ERR  *p_err = OS_ERR_NONE;
    return ((int)OSTimeGet(p_err));
    #else
    return ((int)ltk_get_systick_timer());
    #endif
}

/**
  * 描述: 空函数
  * 参数: *s
  * 返回: 无
  */
void GUI_X_Log(const char *s) 
{
    /* 只是为了去除某些编译器的警告信息，没有实际意义 */
    GUI_USE_PARA(s); 
}

/**
  * 描述: 空函数
  * 参数: *s
  * 返回: 无
  */
void GUI_X_Warn(const char *s) 
{
    /* 只是为了去除某些编译器的警告信息，没有实际意义 */
    GUI_USE_PARA(s); 
}

/**
  * 描述: 空函数
  * 参数: *s
  * 返回: 无
  */
void GUI_X_ErrorOut(const char *s)
{
    /* 只是为了去除某些编译器的警告信息，没有实际意义 */
    GUI_USE_PARA(s); 
}

/****************************** leitek.taobao.com *****************************/

