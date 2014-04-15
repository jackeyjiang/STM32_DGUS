/******************** (C) COPYRIGHT 2010 www.armjishu.com ********************
* File Name          : spi_flash.h
* Author             : www.armjishu.com
* Version            : V1.0
* Library            : Using STM32F2XX_STDPERIPH_VERSION V3.3.0
* Date               : 10/16/2010
* Description        : Header for spi_flash.c file.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define SPI_FLASH_SectorSize         4096
#define SPI_FLASH_PageSize           256
#define SPI_FLASH_PerWritePageSize   256



#define SPI_FLASH_Sector0            0		  /*这个地址用来保存*/
#define SPI_FLASH_Sector1            1*4096


/**
  * @brief  SPI_FLASH SPI Interface pins
  */

#define SPI_FLASH_SPI                           SPI2


#define SPI_FLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define SPI_FLASH_SPI_SCK_PIN                   GPIO_Pin_10                  /* PA.05 */



#define SPI_FLASH_SPI_SCK_GPIO_PORT             GPIOB                       /* GPIOA */
#define SPI_FLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define SPI_FLASH_SPI_SCK_SOURCE                GPIO_PinSource10

#define SPI_FLASH_SPI_MISO_PIN                  GPIO_Pin_2                  /* PA.06 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT            GPIOC                       /* GPIOA */
#define SPI_FLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SPI_FLASH_SPI_MISO_SOURCE               GPIO_PinSource2

#define SPI_FLASH_SPI_MOSI_PIN                  GPIO_Pin_3                  /* PA.07 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT            GPIOC                       /* GPIOA */
#define SPI_FLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SPI_FLASH_SPI_MOSI_SOURCE               GPIO_PinSource3

#define SPI_FLASH_CS_PIN                        GPIO_Pin_9                 /* PC.04 */
#define SPI_FLASH_CS_GPIO_PORT                  GPIOB                       /* GPIOC */
#define SPI_FLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOB


/* Exported macro ------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low  */
#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)

/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u32 SPI_FLASH_ReadID(void);
u32 SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);

/*----- Low layer function -----*/
u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
void SPI_FLASH_TEST(void);


 void  WriteMeal(void);
 void  ReadMeal(void);
 void ClearMealInfo(void);
 void  ReadCoins(void);
 void  WriteCoins(void);
#endif /* __SPI_FLASH_H */

/******************* (C) COPYRIGHT 2010 www.armjishu.com *****END OF FILE****/
