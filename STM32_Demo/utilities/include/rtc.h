#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_conf.h"


extern	u32 tmp_hh , tmp_mm , tmp_ss ;
extern	u32 tmp_yy , tmp_m2 , tmp_dd ,tmp_ww ;


typedef struct 	TD
{
    uint8_t Hours;    /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */

   uint8_t Minutes;  /*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */
  
   uint8_t Senconds;  /*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */
   uint8_t WeekDay; /*!< Specifies the RTC Date WeekDay.
                        This parameter can be a value of @ref RTC_WeekDay_Definitions */
  
   uint8_t Month;   /*!< Specifies the RTC Date Month (in BCD format).
                       This parameter can be a value of @ref RTC_Month_Date_Definitions */

   uint8_t Date;     /*!< Specifies the RTC Date.
                        This parameter must be set to a value in the 1-31 range. */
  
   uint8_t Year;     /*!< Specifies the RTC Date Year.
                        This parameter must be set to a value in the 0-99 range. */

}Struct_TD;

extern Struct_TD TimeDate ;


//void RTC_Config(void);
//void RTC_AlarmCfg(void);
u8 MyRTC_Init(void);
void RTC_Config(void);
void RTC_TimeShow(void);
void RTC_AlarmShow(void);
void RTC_TimeRegulate(void);
void RTC_GetTime_HM(uint16_t *);

//void Set_RTC();
#endif

