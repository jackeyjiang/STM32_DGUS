/**
  ******************************************************************************
  * 文件: gui_demo.c
  * 作者: LeiTek (leitek.taobao.com)
  * 版本: V1.0.0
  * 描述: gui demo
  ******************************************************************************
  *
  *                  版权所有 (C), LeiTek (leitek.taobao.com)
  *                                www.leitek.com
  *
  ******************************************************************************
  */


#include <stddef.h>
#include "GUI.h"
#if GUI_WINSUPPORT
#include "PROGBAR.h"
#include "LISTBOX.h"
#include "FRAMEWIN.h"
#include "BUTTON.h"
#endif
#include "gui_demo.h"
#include "stm32f10x.h"
#include "ltk_systick.h"
#include "ltk_touchscreen.h"
#include "ltk_lcd.h"
#include "ltk_debug.h"
#include "ltk_freertos.h"
#include <assert.h>
#include <stdio.h>
#include "ff.h"

#define    MAINPAGE_ID_TIMER        0
#define    MAINPAGE_ID_SETTINGS     1
#define    MAINPAGE_ID_BOOK         2
#define    MAINPAGE_ID_PHOTO        3
#define    MAINPAGE_ID_FOLDER       4
#define    MAINPAGE_ID_CALC         5
#define    MAINPAGE_ID_TASK         6
#define    MAINPAGE_ID_CONTACT      7
#define    MAINPAGE_ID_TODO         8
#define    MAINPAGE_ID_MUSIC        9
#define    MAINPAGE_ID_VIDEO        10
#define    MAINPAGE_ID_WHEATHER     11
#define    MAINPAGE_ID_TXT          12
#define    MAINPAGE_ID_PAINT        13
#define    MAINPAGE_ID_GAME         14
#define    MAINPAGE_ID_ABOUT        15

#define    GUI_ID_CLEAR             (GUI_ID_USER + 1)

WM_HWIN hAboutWM, hPaintWM, hPhotoPage;

/**
  * 描述: 通过触摸屏上的坐标，转换为屏幕上图标的位置，低四位为图标的 x，
  *       高四位为图标的 y，如，时间 设置 返回为 0x12。
  * 参数: x, 触摸屏的 x 坐标
  *       y, 触摸屏的 y 坐标
  * 返回: 图标位置
  */
static uint16_t ltk_get_icon_range(uint16_t x, uint16_t y)
{
    uint8_t i, j;
    
    /* 触摸不在有效图标内 */
    if(x <= 8 || y <= 24)
        return 0xff;
    
    i = (x - 8) / 58;
    /* 触摸不在有效图标内 */
    if(x >= 58 * (i + 1))
        return 0xff;
    
    j = (y - 24) / 74;
    /* 触摸不在有效图标内 */
    if(y >= 74 * (j + 1))
        return 0xff;
    /* 返回图标位置 */
    return (i | (j << 8));
}

/**
  * 描述: 从 SD 卡内得到数据
  * 参数: *p, SD 卡内需要读取的文件的文件名
  *       **sd_data, 返回从 SD 卡读取的数据
  *       byte_num, 一次读取的数据的大小
  *       file_seek, 是否为第一次读取
  * 返回: 无
  */
int ltk_get_data(void *p, const U8 **sd_data, int byte_num, U32 file_seek)
{
    static unsigned char buffer[GUI_DRAW_ALLOC_MEM];
    uint32_t re;
    FIL *ph_file;
    
    ph_file = (FIL *)p;
    /* 首先找到该文件 */
    if(file_seek == 1)
        f_lseek(ph_file, 0);
    
    /* 调用 fatfs 函数，读取该文件内的内容 */
    f_read(ph_file, buffer, byte_num, &re);

    /* 将缓存返回 */
    *sd_data = buffer;
    
    /* 返回还有多少字节需要读取 */
    return re;
}

/**
  * 描述: 关于页面的回调函数
  * 参数: *pMsg，传递给回调函数的消息
  * 返回: 无
  */
static void ltk_cb_about(WM_MESSAGE* pMsg)
{
    BUTTON_Handle hButton = WM_HWIN_NULL;
    switch (pMsg->MsgId)
    {
        /* 更新窗口消息 */
        case WM_PAINT:
            /* 设置背景为蓝色 */
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
            
            /* 在屏幕上输出一段字符串，颜色为黑色，中间对齐，位置为 (120, 130) */
            GUI_SetTextAlign(GUI_TA_CENTER);
            GUI_SetColor(GUI_BLACK);
            GUI_DispStringAt("leitek all in one demo", 120, 160 - 30);
            
            /* 在屏幕上输出一段字符串，颜色为黑色，中间对齐，位置为 (120, 160) */
            GUI_SetTextAlign(GUI_TA_CENTER);
            GUI_DispStringAt("www.leitek.com", 120, 160);
            
            /* 在屏幕上输出一段字符串，颜色为红色，中间对齐，位置为 (120, 190) */
            GUI_SetColor(GUI_RED);
            GUI_SetTextAlign(GUI_TA_CENTER);
            GUI_DispStringAt("leitek.taobao.com", 120, 160 + 30);
            
            /* 创建 OK 按钮 */
            hButton = BUTTON_CreateEx(180, 280, 40, 20, hAboutWM, WM_CF_SHOW, 0, GUI_ID_OK);
            
            /* 按钮上的字符串为 OK，颜色为红色 */
            BUTTON_SetText (hButton, "OK");
            BUTTON_SetBkColor(hButton, 0, GUI_RED);
            break;
            
        /* 通知父窗口消息 */
        case WM_NOTIFY_PARENT:
            {
                /* 得到通知消息代码 */
                int n_code = pMsg->Data.v;             /* Get notification code */
                switch (n_code)
                {
                    /* 如果按钮松开 */
                    case WM_NOTIFICATION_RELEASED:      /* React only if released */
                        /* 删除按键控件 */
                        BUTTON_Delete(hButton);
                        /* 删除 about 窗口 */
                        WM_DeleteWindow(hAboutWM);
                        /* 返回背景页面 */
                        ltk_ucgui_background();
                        break;
                }
            }
            break;
            
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}

/**
  * 描述: 关于页面，回调函数为 ltk_cb_about
  * 参数: 无
  * 返回: 无
  */
static void ltk_about_page(void)
{
    /* 创建关于页面 */
    hAboutWM = WM_CreateWindowAsChild(0, 0, 240, 320, NULL, WM_CF_SHOW,
                                      &ltk_cb_about, 0);
}

/**
  * 描述: 画图页面的回调函数
  * 参数: *pMsg，传递给回调函数的消息
  * 返回: 无
  */
static void ltk_cb_paint(WM_MESSAGE* pMsg)
{
    GUI_PID_STATE *touch_state;
    BUTTON_Handle button_back = WM_HWIN_NULL;
    BUTTON_Handle button_clear = WM_HWIN_NULL;
    switch (pMsg->MsgId)
    {
        /* 更新窗口消息 */
        case WM_PAINT:
            /* 设置背景为蓝色 */
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
            
            /* 创建 返回 按钮 */
            button_back = BUTTON_CreateEx(190, 290, 40, 20, hPaintWM,
                                          WM_CF_SHOW, 0, GUI_ID_OK);
            /* 设置该按钮上的文字为 back */
            BUTTON_SetText (button_back, "back");
            /* 设置该按钮的背景为绿色 */
            BUTTON_SetBkColor(button_back, 0, GUI_GREEN);
            
            /* 创建 清屏 按钮 */
            button_clear = BUTTON_CreateEx(140, 290, 40, 20, hPaintWM,
                                           WM_CF_SHOW, 0, GUI_ID_CLEAR);
            /* 设置该按钮上的文字为 clear */
            BUTTON_SetText (button_clear, "clear");
            /* 设置该按钮的背景为绿色 */
            BUTTON_SetBkColor(button_clear, 0, GUI_GREEN);
            break;
            
        /* 通知父窗口消息 */
        case WM_NOTIFY_PARENT:
            {
                /* 如果按下的是 返回 按键，并且按下释放 */
                if((WM_GetId(pMsg->hWinSrc) == GUI_ID_OK) &&
                   (pMsg->Data.v == WM_NOTIFICATION_RELEASED))
                {
                    /* 删除该窗口内的所有按键及该窗口 */
                    BUTTON_Delete(button_back);
                    BUTTON_Delete(button_clear);
                    WM_DeleteWindow(hPaintWM);
                    /* 返回到背景页面 */
                    ltk_ucgui_background();
                }
                /* 如果按下的是 清屏 按键，并且按下释放 */
                if((WM_GetId(pMsg->hWinSrc) == GUI_ID_CLEAR) &&
                   (pMsg->Data.v == WM_NOTIFICATION_RELEASED ))
                {
                    /* 标记父窗口需要重新更新，并通知父窗口 */
                    WM_InvalidateWindow(hPaintWM);
                    WM_NotifyParent(hPaintWM, WM_PAINT);
                }
            }
            break;

        /* 触摸消息 */
        case WM_TOUCH:
            /* 得到触摸的状态 */
            touch_state = (GUI_PID_STATE *) pMsg->Data.p;
            /* 设置笔颜色为红色，并在触摸位置划线 */
            GUI_SetColor(GUI_RED);
            GL_FillCircle(touch_state->x, touch_state->y, 2);
            break;
            
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}

/**
  * 描述: 画图页面，回调函数为 ltk_cb_paint
  * 参数: 无
  * 返回: 无
  */
static void ltk_paint_page(void)
{
    /* 创建画图页面 */
    hPaintWM = WM_CreateWindowAsChild(0, 0, 240, 320, NULL, WM_CF_SHOW,
                                      &ltk_cb_paint, 0);
}

/**
  * 描述: 图片页面的回调函数
  * 参数: *pMsg，传递给回调函数的消息
  * 返回: 无
  */
static void ltk_cb_photo(WM_MESSAGE* pMsg)
{
    FATFS fs;
    
    FIL file;
    FRESULT res;
    switch (pMsg->MsgId)
    {
        /* 更新窗口消息 */
        case WM_PAINT:
            /* 图片是从 SPI FLASH 内读取的，访问之前先锁定 spi 总线，防止
               spi 总线上的其它器件访问，如 touchscreen */
            ltk_spi_lock();
            
            /* 初始化 SD 卡，并打开卡内的 2.bmp */
            res = f_mount(&fs, "", 0);
            res = f_open(&file, "/ltk/picture/2.bmp", FA_OPEN_EXISTING | FA_READ);
            if (res == FR_OK)
            {
                
                /* 在 LCD 屏幕上显示图片 */
                GUI_BMP_DrawEx(ltk_get_data, &file, 0, 0);
                /* 使用完后关闭文件 file */
                f_close(&file); 
            }
            /* SPI SD 卡使用完毕，卸载 SD 卡 */
            res = f_mount(NULL, "", 0);
            /* 解锁 SPI 总线，以便其它器件访问 */
            ltk_spi_unlock();
            break;
            
        /* 触摸消息 */
        case WM_TOUCH:
            /* 删除当前窗口 */
            WM_DeleteWindow(hPhotoPage);

            /* 返回到背景页面 */
            ltk_ucgui_background();
            break;
            
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}

/**
  * 描述: 图片页面，回调函数为 ltk_cb_photo
  * 参数: 无
  * 返回: 无
  */
static void ltk_photo_page(void)
{
    /* 创建图片页面 */
    hPhotoPage = WM_CreateWindowAsChild(0, 0, 240, 320, NULL, WM_CF_SHOW,
                                        &ltk_cb_photo, 0);
}

/**
  * 描述: 背景页面的回调函数
  * 参数: *pMsg，传递给回调函数的消息
  * 返回: 无
  */
static void ltk_cb_bk_window(WM_MESSAGE *pMsg) 
{
    
    /* 定义背景页面图标下面的字 */
    static char *icon_text[] = {"时间",   "设置",   "图书",     "图片",
                                "目录",   "计算器", "任务管理", "通讯录",
                                "备忘录", "音乐",   "视频",     "天气",
                                "记事本", "画图",    "游戏",    "关于"
                               };
    GUI_PID_STATE *touch_state;
    FATFS fs;
    FIL file;
    FRESULT res;
    uint8_t background_icon, i, j;
    uint16_t area;

    switch (pMsg->MsgId) 
    {
        /* 更新窗口消息 */
        case WM_PAINT:
            /* 背景图片是从 SPI FLASH 内读取的，访问之前先锁定 spi 总线，防止
               spi 总线上的其它器件访问，如 touchscreen */
            ltk_spi_lock();
            
            /* 初始化 SD 卡，并打开卡内的 background.bmp */
            res = f_mount(&fs, "", 0);
            res = f_open(&file, "/ltk/theme/mainpage/background.bmp", 
                         FA_OPEN_EXISTING | FA_READ);
            if (res == FR_OK)
            {
                /* 在 LCD 屏幕上画图 */
                GUI_BMP_DrawEx(ltk_get_data, &file, 0, 0);
                /* 使用完后关闭文件 file */
                f_close(&file); 
            }
            
            /* 设置字体，改字体包含中文 */
            GUI_SetFont(&GUI_FontArialNarrow12);
            /* 设置字体显示的模式 */
            GUI_SetTextMode(GUI_TM_TRANS);
            for(j = 0; j < 4; j++)
            {
                for( i = 0; i < 4; i++)
                {
                    /* 字体中间靠齐 */
                    GUI_SetTextAlign(GUI_TA_CENTER);
                    /* 在不同的位置显示 icon_text[] 内的字 */
                    GUI_DispStringAt(icon_text[i + 4 * j], 33 + 58 * i, 76 + 74 * j);
                }
            }
            /* SPI SD 卡使用完毕，卸载 SD 卡 */
            res = f_mount(NULL, "", 0);
            /* 解锁 SPI 总线，以便其它器件访问 */
            ltk_spi_unlock();
            break;
            
        /* 触摸消息 */
        case WM_TOUCH:
            /* 得到触摸的状态 */
            touch_state = (GUI_PID_STATE *) pMsg->Data.p;
            /* 如果触摸屏上有触摸 */
            if ((touch_state) && (!touch_state->Pressed))
            {
                /* 将触摸坐标转换为页面上的图标位置，低四位为 x，高四位为 y */
                area = ltk_get_icon_range(touch_state->x, touch_state->y);
                if(area != 0xff)
                {
                    /* 将图标位置转换为图标 ID，每行有 4 个图标 */
                    background_icon = (area & 0xff) + ((area >> 8) & 0xff) * 4;
                    switch (background_icon)
                    {
                        /* 如果按下的是 关于 图标 */
                        case MAINPAGE_ID_ABOUT:
                            ltk_about_page();
                            break;
                            
                        /* 如果按下的是 画图 图标 */
                        case MAINPAGE_ID_PAINT:
                            ltk_paint_page();
                            break;

                        /* 如果按下的是 图片 图标 */
                        case MAINPAGE_ID_PHOTO:
                            ltk_photo_page();
                            break;
                    }
                }
            }
            break;
            
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}

/**
  * 描述: 背景页面，回调函数为 ltk_cb_bk_window
  * 参数: 无
  * 返回: 无
  */
void ltk_ucgui_background(void) 
{
    /* 背景清为黑色 */
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
    /* 设置背景页面的回调函数 */
    WM_SetCallback(WM_HBKWIN, ltk_cb_bk_window);
}

/****************************** leitek.taobao.com *****************************/
