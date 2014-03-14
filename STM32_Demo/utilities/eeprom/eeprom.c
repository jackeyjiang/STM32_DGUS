#include "bsp.h"
#include "eeprom.h"



 /*******************************************************************************
 * 函数名称:SetReceUsersMoney                                                                     
 * 描    述:设置接受10  20 5  的钱                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/  
void GetPassWord(unsigned char *PassWord)
{
    PassWord[0] = 5;
	PassWord[1] = 1;
	PassWord[2] = 8;
	PassWord[3] = 5;
	PassWord[4] = 1;
	PassWord[5] = 8;

}

 /*******************************************************************************
 * 函数名称:SetReceUsersMoney                                                                     
 * 描    述:设置接受10  20 5  的钱                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/  
uint8_t VerifyPassword( uint8_t *Src1, uint8_t *Src2, uint8_t Length)
{
    uint8_t i = 0; 
	for(i = 0; i < Length; i++)
	{
	    if(*(Src1+i) != *(Src2+i))
		{
		    return 0;
		}
	}
	return 1;
	
}


  /*******************************************************************************
 * 函数名称:FLASH_If_Init                                                                     
 * 描    述:解锁                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void FLASH_If_Init(void)
{ 
  FLASH_Unlock(); 

  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
}

/*******************************************************************************
 * 函数名称:FLASH_If_Write                                                                     
 * 描    述:写数据                                                                  
 *                                                                               
 * 输    入:有                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
  uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength)
{
  uint32_t i = 0;

  for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i)) == FLASH_COMPLETE)
    {
     /* Check the written value */
      if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
      {
        /* Flash content doesn't match SRAM content */
        return(2);
      }
      /* Increment FLASH destination address */
      *FlashAddress += 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
      return (1);
    }
  }

  return (0);
}
/**
  * @brief  Gets the sector of a given address
  * @param  Address: Flash address
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
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

 /*******************************************************************************
 * 函数名称:FLASH_If_Erase                                                                     
 * 描    述:擦出哪个扇区                                                                 
 *                                                                               
 * 输    入:有                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
  uint32_t UserStartSector = FLASH_Sector_1, i = 0;

  /* Get the sector where start the user flash area */
  UserStartSector = GetSector(StartSector);
  for(i = UserStartSector; i <= FLASH_Sector_11; i += 8)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
    {
      /* Error occurred while page erase */
      return (1);
    }
  }
  
  return (0);
}

/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval 0: No write protected sectors inside the user flash area
  *         1: Some sectors inside the user flash area are write protected
  */
uint16_t FLASH_If_GetWriteProtectionStatus(uint32_t Application_Address)
{
  uint32_t UserStartSector = FLASH_Sector_1;

  /* Get the sector where start the user flash area */
  UserStartSector = GetSector(Application_Address);

  /* Check if there are write protected sectors inside the user flash area */
  if ((FLASH_OB_GetWRP() >> (UserStartSector/8)) == (0xFFF >> (UserStartSector/8)))
  { /* No write protected sectors inside the user flash area */
    return 1;
  }
  else
  { /* Some sectors inside the user flash area are write protected */
    return 0;
  }
}


/**
  * @brief  Disables the write protection of user flash area.
  * @param  None
  * @retval 1: Write Protection successfully disabled
  *         2: Error: Flash write unprotection failed
  */
uint32_t FLASH_If_DisableWriteProtection(uint32_t Application_Address)
{
  __IO uint32_t UserStartSector = FLASH_Sector_1, UserWrpSectors = OB_WRP_Sector_1;

  /* Get the sector where start the user flash area */
  UserStartSector = GetSector(Application_Address);

  /* Mark all sectors inside the user flash area as non protected */  
  UserWrpSectors = 0xFFF-((1 << (UserStartSector/8))-1);
   
  /* Unlock the Option Bytes */
  FLASH_OB_Unlock();

  /* Disable the write protection for all sectors inside the user flash area */
  FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);

  /* Start the Option Bytes programming process. */  
  if (FLASH_OB_Launch() != FLASH_COMPLETE)
  {
    /* Error: Flash write unprotection failed */
    return (2);
  }

  /* Write Protection successfully disabled */
  return (1);
}
  /*******************************************************************************
 * 函数名称:STMFLASH_ReadHalfWord                                                                     
 * 描    述:读                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 

}
  /*******************************************************************************
 * 函数名称:EraseSector                                                                     
 * 描    述:擦出扇区                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
uint8_t EraseSector(uint32_t Application_Address)
{
   uint32_t res;
   if(FLASH_If_GetWriteProtectionStatus(Application_Address)==0)
   {
	  FLASH_If_DisableWriteProtection(Application_Address);
   }
	if(FLASH_If_Erase(Application_Address)!=0)
	 {
	 	 res=FLASH_GetStatus();
		 switch(res)
		 {
		  case 	    FLASH_BUSY :            	printf("FLASH_BUSY\r\n");  break;
		  case      FLASH_ERROR_PGS:			printf("FLASH_ERROR_PGS\r\n");  break;
		  case      FLASH_ERROR_PGP:			printf("FLASH_ERROR_PGP\r\n");  break;
		  case      FLASH_ERROR_PGA:			printf("FLASH_ERROR_PGA\r\n");  break;
		  case      FLASH_ERROR_WRP:			printf("FLASH_ERROR_WRP\r\n");  break;
		  case      FLASH_ERROR_PROGRAM:		printf("FLASH_ERROR_PROGRAM\r\n");  break;
		  case      FLASH_ERROR_OPERATION:		printf("FLASH_ERROR_OPERATION\r\n");  break;
		  case      FLASH_COMPLETE 	  :			printf("FLASH_COMPLETE\r\n");  break;
		  default: break;
		 }
	 }
 return res;
 
}

  /*******************************************************************************
 * 函数名称:STMFLASH_WriteHalfWord                                                                     
 * 描    述:写数据                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
uint8_t  STMFLASH_WriteHalfWord(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{
    uint32_t res,i;
	 res=FLASH_GetStatus();
	 	 while(res != FLASH_COMPLETE)
		 {
		  printf("error\r\n") ;
		 }
		 switch(res)
		 {
		  case 	    FLASH_BUSY :            	printf("FLASH_BUSY\r\n");  break;
		  case      FLASH_ERROR_PGS:			printf("FLASH_ERROR_PGS\r\n");  break;
		  case      FLASH_ERROR_PGP:			printf("FLASH_ERROR_PGP\r\n");  break;
		  case      FLASH_ERROR_PGA:			printf("FLASH_ERROR_PGA\r\n");  break;
		  case      FLASH_ERROR_WRP:			printf("FLASH_ERROR_WRP\r\n");  break;
		  case      FLASH_ERROR_PROGRAM:		printf("FLASH_ERROR_PROGRAM\r\n");  break;
		  case      FLASH_ERROR_OPERATION:		printf("FLASH_ERROR_OPERATION\r\n");  break;
		  case      FLASH_COMPLETE 	  :		//	printf("FLASH_COMPLETE\r\n"); 
		    break;
		  default: break;
		 }
	 
   	for(i=0;i<NumToWrite;i++)
	{
	     res= FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	     WriteAddr+=2;//地址增加2.	
	//	 printf("pBuffer[%d]=%d\r\n",i,pBuffer[i]);	
	}  
	 if(res!=FLASH_COMPLETE)
	 {
	 	 res=FLASH_GetStatus();
	
		 switch(res)
		 {
		  case 	    FLASH_BUSY :            	printf("FLASH_BUSY\r\n");  break;
		  case      FLASH_ERROR_PGS:			printf("FLASH_ERROR_PGS\r\n");  break;
		  case      FLASH_ERROR_PGP:			printf("FLASH_ERROR_PGP\r\n");  break;
		  case      FLASH_ERROR_PGA:			printf("FLASH_ERROR_PGA\r\n");  break;
		  case      FLASH_ERROR_WRP:			printf("FLASH_ERROR_WRP\r\n");  break;
		  case      FLASH_ERROR_PROGRAM:		printf("FLASH_ERROR_PROGRAM\r\n");  break;
		  case      FLASH_ERROR_OPERATION:		printf("FLASH_ERROR_OPERATION\r\n");  break;
		  case      FLASH_COMPLETE :			printf("FLASH_COMPLETE\r\n");  break;
		  default: break;
		 }
	 }
 return res;
 
}

 /*******************************************************************************
 * 函数名称:STMFLASH_Read                                                                
 * 描    述:读flash地址的数据,从指定地址开始读出指定长度的数据                                                                      
 *                                                                               
 * 输    入:有  ReadAddr:起始地址 , pBuffer:数据指针 , NumToWrite:半字(16位)数                                                               
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	STMFLASH_Unlock();	

	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
//		printf("pBuffer[%d]=%d\r\n",i,pBuffer[i]);
	} 
	STMFLASH_Lock();//上锁
}
 /*******************************************************************************
 * 函数名称:STMFLASH_Unlock                                                                     
 * 描    述:解除flash锁功能，用户可以操作flash                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void STMFLASH_Unlock(void)
{
    FLASH_Unlock();
}
 /*******************************************************************************
 * 函数名称:STMFLASH_Lock                                                                     
 * 描    述:锁住flash，用户不可以操作flash                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void STMFLASH_Lock(void)
{
  FLASH_Lock();//上锁
}
 /*******************************************************************************
 * 函数名称:STMFLASH_Lock                                                                     
 * 描    述:锁住flash，用户不可以操作flash                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
uint8_t STMFLASH_GetStatus(void)
{	
	u32 res;		
    res=FLASH_GetStatus();
	return res;						//操作完成
}

 /*******************************************************************************
 * 函数名称:STM32FLASH_WaitDone                                                                     
 * 描    述:锁住flash，用户不可以操作flash                                                                  
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:状态                                                                     
 * 返    回:uint8_t                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
 uint8_t STMFLASH_WaitDone(void)
 {
  	u32 res; 
    res=FLASH_WaitForLastOperation();
	return res;
 }

 /*******************************************************************************
 * 函数名称:DataWriteStmFlash                                                                     
 * 描    述:把数据写进flash                                                                  
 *                                                                               
 * 输    入:Application_Address：地址     *pBuffer：数据          NumToWrite：数量                                                        
 * 输    出:状态                                                                     
 * 返    回:uint8_t                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/
uint8_t DataWriteStmFlash(uint32_t Application_Address,u16 *pBuffer,u16 NumToWrite)
{
        
         uint32_t res = 0 ;
   		 FLASH_If_Init();
         res =EraseSector(Application_Address);
	     if(res == 0)
		 {
		    return 1 ;
		 }
         STMFLASH_WriteHalfWord(Application_Address,pBuffer,NumToWrite);  
         return 0 ;      
}

