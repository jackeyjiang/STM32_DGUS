/**
  ******************************************************************************
  * 文件: ltk_ucgui_freertos.c
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
#include "ltk_ucgui_freertos.h"


/* GUI mutex, 用于保护临界资源 */
static xSemaphoreHandle disp_sem;
/* GUI semaphore, 用于同步任务 */
static xSemaphoreHandle disp_binary_sem;

xSemaphoreHandle binary_sem_ts;

/**
  * 描述: 初始化 OS，创建了 GUI mutex 和触摸屏 binary semaphore
  * 参数: 无
  * 返回: 无
  */
void  GUI_X_InitOS (void)
{
    disp_sem = xSemaphoreCreateMutex();
    vSemaphoreCreateBinary( disp_binary_sem );
    
    vSemaphoreCreateBinary(binary_sem_ts);
}

/**
  * 描述: 锁定 GUI，用于保护 GUI 临界资源
  * 参数: 无
  * 返回: 无
  */
void  GUI_X_Lock (void)
{
    xSemaphoreTake(disp_sem, portMAX_DELAY);
}

/**
  * 描述: 解锁 GUI，用于保护 GUI 临界资源
  * 参数: 无
  * 返回: 无
  */
void  GUI_X_Unlock (void)
{ 
    xSemaphoreGive(disp_sem);
}

/**
  * 描述: 得到 task 的 id
  * 参数: 无
  * 返回: task 的 id
  */
U32  GUI_X_GetTaskId (void) 
{
    return (uint32_t)uxTaskGetTaskNumber(NULL);
}

/**
  * 描述: 等待事件发生，用于同步任务
  * 参数: 无
  * 返回: 无
  */
void GUI_X_WaitEvent(void)
{
    xSemaphoreTake(disp_binary_sem, portMAX_DELAY);
}

/**
  * 描述: 通知事件已经发生，用于同步任务
  * 参数: 无
  * 返回: 无
  */
void GUI_X_SignalEvent(void)
{
    xSemaphoreGive(disp_binary_sem);
}

/****************************** leitek.taobao.com *****************************/
