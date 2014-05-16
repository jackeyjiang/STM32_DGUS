#include "bsp.h"


/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/

#define DATA_32                 ((uint32_t)0x12345678)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t StartSector = 0, EndSector = 0, Address = 0, SectorCounter = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;
  
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint32_t GetSector(uint32_t Address);

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int Flash_Write(uint32_t iAddress, uint8_t *buf, uint32_t iNbrToWrite) 
{
	int cnt_t=0;
	uint32_t iAddress_temp=0;
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */

	/* Get the number of the start and end sectors */
  StartSector = GetSector(iAddress);
  EndSector =   GetSector(iAddress+iNbrToWrite);
	iAddress_temp = iAddress; 
	
  /* Unlock the Flash to enable the flash control register access *************/ 	  	
  FLASH_Unlock();
    
  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 



  for (cnt_t = StartSector; cnt_t <= EndSector; cnt_t += 8)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (FLASH_EraseSector(cnt_t, VoltageRange_3) != FLASH_COMPLETE)
    { 
      /* Error occurred while sector erase. 
         User can add here some code to deal with this error  */
      while (1)
      {
      }
    }
  }
//  StartSector = GetSector(iAddress);

//  for (SectorCounter = StartSector; SectorCounter < (StartSector+iNbrToWrite); SectorCounter += 1)
//  {
//    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
//       be done by word */ 
//    if (FLASH_EraseSector(SectorCounter, VoltageRange_1) != FLASH_COMPLETE)
//    { 
//      /* Error occurred while sector erase. 
//         User can add here some code to deal with this error  */
//      while (1)
//      {
//      }
//    }
//  }

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  while (iAddress < (iAddress_temp+iNbrToWrite))
  {
    if (FLASH_ProgramByte(iAddress, *buf++) == FLASH_COMPLETE)
    {
      iAddress =iAddress + 1;
    }
    else
    { 
      /* Error occurred while writing data in Flash memory. 
         User can add here some code to deal with this error */
      while (1)
      {
      }
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock();	
	return cnt_t;
}


int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead) 
{
        int i = 0;
        while(i < iNbrToRead ) 
				{
           *(buf + i) = *(__IO uint8_t*) iAddress++;
           i++;
        }
        return i;
}


/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }

  return sector;
}

//void  WriteMeal(void) //在初始化时，flash的数据易丢失
//{
//	 Flash_Write(FLASH_USER_START_ADDR,FloorMealMessageWriteToFlash.FlashBuffer , FloorMealNum*6);
//}

//void ReadMeal(void)
//{
//   Flash_Read(FLASH_USER_START_ADDR,FloorMealMessageWriteToFlash.FlashBuffer , FloorMealNum*6); 
//}

//void WriteCoins(void)
//{
//  RTC_WriteBackupRegister(RTC_BKP_DR13,CoinsTotoalMessageWriteToFlash.CoinTotoal);
//	
//}
//void ReadCoins(void)
//{
//	//SPI_FLASH_BufferRead(CoinsTotoalMessageWriteToFlash.CoinsCnt, SPI_FLASH_Sector1, 2);
//	CoinsTotoalMessageWriteToFlash.CoinTotoal  = RTC_ReadBackupRegister(RTC_BKP_DR13);
//}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
