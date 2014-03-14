/**
  ******************************************************************************
  * 文件: ltk_ucgui_ucos.c
  * 作者: LeiTek (leitek.taobao.com)
  * 版本: V1.0.0
  * 描述: 如果移植了操作系统，移植 uCGUI 需要定义的函数
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
#include "GUI_Private.H"

/**
  * 描述: 初始化 OS
  * 参数: 无
  * 返回: 无
  */
void  GUI_X_InitOS (void)
{
}

/**
  * 描述: 锁定 GUI，用于保护 GUI 临界资源
  * 参数: 无
  * 返回: 无
  */
void  GUI_X_Lock (void)
{
}

/**
  * 描述: 解锁 GUI，用于保护 GUI 临界资源
  * 参数: 无
  * 返回: 无
  */
void  GUI_X_Unlock (void)
{ 
}

/**
  * 描述: 得到 task 的 id
  * 参数: 无
  * 返回: task 的 id
  */
U32  GUI_X_GetTaskId (void) 
{
    OS_ERR err;
    return (U32)OSTaskRegGetID(&err);
}

/**
  * 描述: 等待事件发生，用于同步任务
  * 参数: 无
  * 返回: 无
  */
void GUI_X_WaitEvent(void)
{
}

/**
  * 描述: 通知事件已经发生，用于同步任务
  * 参数: 无
  * 返回: 无
  */
void GUI_X_SignalEvent(void)
{
}

/****************************** leitek.taobao.com *****************************/
