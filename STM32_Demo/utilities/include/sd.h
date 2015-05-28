#ifndef __SD_H
#define __SD_H
#include "rtc.h"

extern unsigned char is_init;
extern char 	f_name[12];
#define SetSCLK 	 	      GPIOA->BSRRL = GPIO_Pin_5;//GPIO_SetBits(GPIOC,GPIO_Pin_12)
#define ResetSCLK 	 	    GPIOA->BSRRH = GPIO_Pin_5;// GPIO_ResetBits(GPIOC,GPIO_Pin_12)

uint8_t SD_SPI_ReadWriteByte(uint8_t TxData);
unsigned char SD_spi_read(void); //SPI¶ÁÒ»¸ö×Ö½Ú
void SD_spi_write(unsigned char x);
void SD_GPIO_Configuration(void);
unsigned char MessageWriteToSD(unsigned char Cuttern);
void WriteDatatoSD(unsigned char *);
bool ReadDatatoBuffer(void);
void File_delete(void);
void SendtoServce(void);
void DataRecord(void);
void Sd_Write(char erro_flag,char takeout_flag);
uint32_t SearchSeparator(uint8_t *dest,uint8_t *souce,int Separator);
#endif



