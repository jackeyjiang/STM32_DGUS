/*-----------------------------------------------------------------------------
 * Name:    TSC.h
 * Purpose: Touchscreen controller definitions
 *-----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef __STM324XG_EVAL_TSC_H__
#define __STM324XG_EVAL_TSC_H__

#ifdef __cplusplus
 extern "C" {
#endif   

#include "stm324xg_eval.h"

extern void TSC_Init      (void);
extern uint32_t TSC_TouchDet  (void);
extern void TP_GetAdXY(int *x,int *y);
int Read_X(void);
int Read_Y(void);

#ifdef __cplusplus
}
#endif   

#endif /* __TSC_H */
