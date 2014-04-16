#include "bsp.h"
/***************************************
Ó²¼þspi½Ó¿Ú
***************************************/
//
//void  SD_GPIO_Configuration(void)
//{
// 
//	GPIO_InitTypeDef GPIO_InitStructure;
//  	SPI_InitTypeDef  SPI_InitStructure;
////	DMA_InitTypeDef DMA_InitStructure; 
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	 
//
////	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 						 
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
// 
//	GPIO_PinAFConfig( GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
//	GPIO_PinAFConfig( GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
//	GPIO_PinAFConfig( GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);	
//
//    /* Configure SPI2 pins: SCK, MISO and MOSI */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
////	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
// //	GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);  //PB13/14/15ÉÏÀ­
//
//   GPIO_SetBits(GPIOA,GPIO_Pin_4);	      //ºóÃæÌí¼ÓµÄ
// //   GPIO_ResetBits(GPIOA,GPIO_Pin_4);	
//  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
//  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
//  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
//  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//  SPI_InitStructure.SPI_CRCPolynomial = 7;
//  SPI_Init(SPI2, &SPI_InitStructure);
//  SPI_Cmd(SPI2, ENABLE);
//
//}
//
//uint8_t SD_SPI_ReadWriteByte(uint8_t TxData)
//{	
//	   
//	uint8_t retry=0;				 	
//	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //¼ì²éÖ¸¶¨µÄSPI±êÖ¾Î»ÉèÖÃÓë·ñ:·¢ËÍ»º´æ¿Õ±êÖ¾Î»
//		{
//		retry++;
//		if(retry>200)return 0;
//		}			  
//	SPI_I2S_SendData(SPI2, TxData); //Í¨¹ýÍâÉèSPIx·¢ËÍÒ»¸öÊý¾Ý
//	retry=0;
//
//	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //¼ì²éÖ¸¶¨µÄSPI±êÖ¾Î»ÉèÖÃÓë·ñ:½ÓÊÜ»º´æ·Ç¿Õ±êÖ¾Î»
//		{
//		retry++;
//		if(retry>200)return ;
//		}	  						    
//	return SPI_I2S_ReceiveData(SPI2); //·µ»ØÍ¨¹ýSPIx×î½ü½ÓÊÕµÄÊý¾Ý					    
//}

/*************************************************************************
Ò»ÏÂÊÇÄ£ÄâSPI	 start
**************************************************************************/
void SD_GPIO_Configuration(void)
{
 GPIO_InitTypeDef GPIO_InitStructure;
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
 	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;	   //CS
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;	   //MISO
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//GPIO_Mode_OUT;
// GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 	;   //SCLK
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOA, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;	   //MOSI
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//GPIO_Mode_IN;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
}

 /******************************************************************
 - ¹¦ÄÜÃèÊö£ºÑÓÊ±º¯Êý
 - Á¥ÊôÄ£¿é£º¹«¿ªº¯ÊýÄ£¿é
 - º¯ÊýÊôÐÔ£ºÍâ²¿£¬ÓÃ»§¿Éµ÷ÓÃ
 - ²ÎÊýËµÃ÷£ºtime:timeÖµ¾ö¶¨ÁËÑÓÊ±µÄÊ±¼ä³¤¶Ì           
 - ·µ»ØËµÃ÷£ºÎÞ
 - ×¢£º.....
 ******************************************************************/
unsigned char is_init;  //ÔÚ³õÊ¼»¯µÄÊ±ºòÉèÖÃ´Ë±äÁ¿Îª1,Í¬²½Êý¾Ý´«Êä£¨SPI£©»á·ÅÂý

#define DELAY_TIME 1000 //SD¿¨µÄ¸´Î»Óë³õÊ¼»¯Ê±SPIµÄÑÓÊ±²ÎÊý£¬¸ù¾ÝÊµ¼ÊËÙÂÊÐÞ¸ÄÆäÖµ£¬·ñÔò»áÔì³ÉSD¿¨¸´Î»»ò³õÊ¼»¯Ê§°Ü

void delay(unsigned int time) 
{
 while(time--);
}

void SD_spi_write(unsigned char x) 
{
 unsigned char i;
 for(i=0;i<8;i++)
 { 
	 ResetSCLK ;
	 if(x&0x80)
	 {
     GPIOB->BSRRL = GPIO_Pin_5;
	 }
	 else
	 {
     GPIOB->BSRRH = GPIO_Pin_5;
	 }
	 if(is_init) delay(DELAY_TIME);
	 SetSCLK 	;
   x <<= 1 ;
 }
}

/******************************************************************
 - ¹¦ÄÜÃèÊö£ºIOÄ£ÄâSPI£¬¶ÁÈ¡Ò»¸ö×Ö½Ú
 - Á¥ÊôÄ£¿é£ºSD/SDHCÄ£¿é
 - º¯ÊýÊôÐÔ£ºÄÚ²¿
 - ²ÎÊýËµÃ÷£ºÎÞ
 - ·µ»ØËµÃ÷£º·µ»Ø¶Áµ½µÄ×Ö½Ú
 ******************************************************************/

unsigned char SD_spi_read() //SPI¶ÁÒ»¸ö×Ö½Ú
{  
  unsigned char i,temp=0;

  for(i=0;i<8;i++)
  {
    SetSCLK ;
    if(is_init) delay(DELAY_TIME);
    ResetSCLK ;
    if(is_init) delay(DELAY_TIME);
    if(GPIOA->IDR & GPIO_Pin_6)
    {
      temp|=(0x80>>i);
    }
  }
  return (temp);
}

/*************************************************************************
ÒÔÉÏÊÇÄ£ÄâSPI	 end
**************************************************************************/
//ÅÐ¶ÏÊÇ·ñÈòÄê
u8 isRunNian(u16 y)
{
  return (y % 4 == 0 && y % 100 != 0 || y % 400 == 0) ? 1 : 0;
}
//¸ù¾ÝÔÂÓëÄêµÃµ½ÔÂµÄÌìÊý
u8 getDays(u16 y, u8 m)
{
  u8 days = 0;
  switch(m)
  {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			days = 31; break;
		case 4:
		case 6:
		case 9:
		case 11:
			days = 30; break;
		case 2:
			days = isRunNian(y) ? 29 : 28; break;
		default:;
	}
	return days;
}


  /*******************************************************************************
 * º¯ÊýÃû³Æ:itoa                                                                   
 * Ãè    Êö:°Ñint×ª»»³É×Ö·ûÐÍ                                                              
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:ÎÞ                                                                  
 * ÐÞ¸ÄÈÕÆÚ:2013Äê4ÔÂ20ÈÕ                                                                    
 *******************************************************************************/
char 	f_name[12];
void itoa(char *f_name, Struct_TD  a)
{
	 uint8_t i,j=0;
	 f_name[j++] = 2               ;
	 f_name[j++] = 0			         ;
	 f_name[j++] = a.Year     /	10 ;
	 f_name[j++] = a.Year     %	10 ;
	 f_name[j++] = a.Month    / 10;
	 f_name[j++] = a.Month    % 10;
	 f_name[j++] = a.Date     / 10;
	 f_name[j++] = a.Date     % 10;
//	 f_name[j++] = a.Hours    / 10;
//	 f_name[j++] = a.Hours    % 10;
//	 f_name[j++] = a.Minutes  / 10;
//	 f_name[j++] = a.Minutes  % 10;
	
	 for(i=0;i<8;i++)
	 f_name[i] += '0'; //ÈÕÆÚÊý¾Ý×ª»»Îª×Ö·û
	 f_name[j++]  = '.';
	 f_name[j++]  = 't';
	 f_name[j++]	= 'x';
	 f_name[j++]	= 't';
}
  /*******************************************************************************
 * º¯ÊýÃû³Æ:itoa   °Ñint×ª»»³É×Ö·ûÐÍ                                                                 
 * Ãè    Êö:ÎÞ                                                                   
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:ÎÞ                                                                  
 * ÐÞ¸ÄÈÕÆÚ:2013Äê4ÔÂ20ÈÕ                                                                    
 *******************************************************************************/
FATFS fs;
FRESULT res;
FILINFO fileInfo;
UINT br,bw;
UINT r=1;
DIR dir;
FIL fsrc;
 
void Fwriter(unsigned char *p)
{
		uint8_t sd_time;   
	  uint32_t rd; 
	  res = f_mount(0, &fs); 
		if(res!=FR_OK)
		{
		  return ;
		}
		else
		{
		}
		for(sd_time=0;sd_time<20;sd_time++)
		{
    res = f_open(&fsrc,f_name,FA_OPEN_ALWAYS|FA_WRITE );
	//	res = f_open(&fsrc,"0:2013011209408.txt", FA_OPEN_ALWAYS|FA_WRITE);  //ÎÄ¼þ²»´æÔÚÔò´´½¨ÐÂÎÄ¼þ
		if(res==FR_OK) break;
	   }
		if(res!=FR_OK)
		{

		return ;
		}
		else
		{ 
		

		res = f_lseek(&fsrc,fsrc.fsize); 
   	res = f_write(&fsrc,p,2, &rd);
	//	res = f_write(&fsrc,"Ð¡ÍõÍ¯Ð¬ÄãºÃ£¡£¡£¡\r\n",512, &rd);
		f_sync(&fsrc)	;	
	  if(res!=FR_OK)
	{
	  f_close(&fsrc);
	  f_mount(0, NULL);
	  return;
	}
	else
	{
	   f_close(&fsrc);
	   f_mount(0, NULL);
	}  

}
}


  /*******************************************************************************
 * º¯ÊýÃû³Æ: Fread                                                                
 * Ãè    Êö: ´Ó¿¨ÖÐ¶ÁÈ¡Êý¾Ý                                                                
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:ÎÞ                                                                  
 * ÐÞ¸ÄÈÕÆÚ:2013Äê4ÔÂ20ÈÕ                                                                    
 *******************************************************************************/
static uint32_t  Index = 0; 
unsigned char    Buffer[1024] = {0};
unsigned char    Fread(unsigned char *p)
{
		uint8_t   sd_time;   
    uint32_t  rd; 
	  uint16_t  Lenght = 0 ;	  
	  uint32_t  indexflag = 0; 
	
	  res = f_mount(0, &fs); 
		if(res!=FR_OK)
		{
			return 0;
		}
		for(sd_time=0;sd_time<20;sd_time++) //ÖØ¸´´ò¿ª20´Î
		{
      res = f_open(&fsrc,f_name,FA_OPEN_EXISTING|FA_READ |FA_WRITE);  //Ä¬ÈÏ´ò¿ªÒ»¸öÎÄ¼þ
		//res = f_open(&fsrc,"0:123456789.txt", FA_OPEN_EXISTING|FA_READ );  //ÎÄ¼þ²»´æÔÚÔò´´½¨ÐÂÎÄ¼þ
		  if(res==FR_OK) 	
		     break;
	  }
		if(res!=FR_OK) 
		{
		  return  1; //·µ»Ø´íÎó
		}
		else
		{ 	
		  // res = f_lseek(&fsrc,fsrc.fsize); 
		  res = f_lseek(&fsrc,Index); //Ö¸ÕëÖ¸ÏòµÚIndex¸ö×Ö½Ú
			//	printf("Index = %d \r\n",res);
		  res = f_read(&fsrc,Buffer,512, &rd);//¶ÁÈ¡512×Ö½ÚµÄÊý¾Ýµ½Buffer,rd ´æ´¢µÄÊÇ¶Áµ½µÄ×Ö½ÚÊý
			//	printf("Index = %d \r\n",res);
		  res = f_lseek(&fsrc,Index+512); //Ö¸ÕëÆ«ÒÆIndex+512¸ö×Ö½Ú
			//	printf("Index = %d \r\n",res);
			res = f_read(&fsrc,&Buffer[512],512, &rd);//´«µÝµÄÊÇBuffer[512]µÄµØÖ·£¬¶ÁÈ¡ÁË512×Ö½ÚµÄÊý¾Ý
//				printf("Index = %d \r\n",res);
//			  printf("Buffer = %c \r\n",Buffer[1021]);
//			  printf("Buffer = %c \r\n",Buffer[1022]);
//				printf("Buffer = %c \r\n",Buffer[1023]);
			if(Buffer[1022]== 'N' && Buffer[1023]== 'O') //±íÊ¾Ã»ÓÐ·¢ËÍ³É¹¦
			{
//				  printf("Buffer = %c \r\n",Buffer[1021]);
//				  printf("Buffer = %c \r\n",Buffer[1022]);
//				  printf("Buffer = %c \r\n",Buffer[1023]);
				Lenght = Buffer[1019] << 8 | Buffer[1020];
				if(1)
				//if(Resend(Buffer,Lenght) == 0x00)
				{   		
					indexflag = Index+1021 ;
					res = f_lseek(&fsrc,indexflag);//Æ«ÒÆIndex+1021  	  
          res = f_write(&fsrc,"Yes",3, &rd); //Ð´"Yes"
				  f_close(&fsrc);
        } 
				else 
				{
					*p =  0x01 ; 
					res = f_lseek(&fsrc,1000);
					res = f_write(&fsrc,"Uncomplete",10, &rd);
					f_close(&fsrc);		      						
        }
      }	 
		  Index += 1024 ; //Æ«ÒÆ1024
		  // printf("Index = %d \r\n",Index);
	    if(res!=FR_OK ||fsrc.fsize <= Index ) //fsrc.fsizeÊÇÎÄ¼þ´óÐ¡£¬ÕâÀïÅÐ¶ÏÎÄ¼þ´óÐ¡
	    {
	      f_close(&fsrc);
//      f_mount(0, NULL);
	      return 1;
	    }
	    else
	    {
	      f_close(&fsrc);
//	    f_mount(0, NULL);
		    return 0 ;
	    }  
   }
}
  /*******************************************************************************
 * º¯ÊýÃû³Æ:WriteDatatoSD                                                                  
 * Ãè    Êö:°ÑÐÅÏ¢Ð´½øsd¿¨                                                                 
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:ÎÞ                                                                  
 * ÐÞ¸ÄÈÕÆÚ:2013Äê4ÔÂ20ÈÕ                                                                    
 *******************************************************************************/
void WriteDatatoSD(unsigned char *data)
{
	RTC_TimeShow();
	itoa(f_name,TimeDate);	  //°ÑÊ±¼ä×ª»»³É×Ö·û
	Fwriter(data);					      //¼ÙÈç³ö´íÁË¡£	
}


  /*******************************************************************************
 * º¯ÊýÃû³Æ:ReadDatatoBuffer                                                                  
 * Ãè    Êö:´ÓSD¿¨ÖÐ¶ÁÈ¡Êý¾Ý                                                               
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:ÎÞ                                                                  
 * ÐÞ¸ÄÈÕÆÚ:2013Äê4ÔÂ20ÈÕ                                                                    
 *******************************************************************************/
void ReadDatatoBuffer(void)
{
  unsigned char DelteFlag = 0 ;//ÓÃÀ´±ê¼ÇÊÇ·ñÈ«²¿ÉÏ´«ÁËÊý¾ÝµÄº¯Êý
	unsigned char Times  = 0  ;
	Index = 0 ;
	for(Times = 0 ;Times <10 ;Times--)
	{
	  do
	  { 
	     if(Fread(&DelteFlag)== 0x01)					      //¼ÙÈç³ö´íÁË¡???
		   break ;
    }while(1);
  }
}


  /*******************************************************************************
 * º¯ÊýÃû³Æ:SendtoServce                                                                  
 * Ãè    Êö:ÉÏµçÊ±¿Ì¼ì²éÊý¾Ý     ¿ÉÒÔÍÑ»ú½»Ò×7Ìì¡£¡£¡£                                                             
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:ÎÞ                                                                  
 * ÐÞ¸ÄÈÕÆÚ:2013Äê4ÔÂ20ÈÕ                                                                    
 *******************************************************************************/
void SendtoServce(void)
{
//20140211.txt	 
	unsigned char i    = 0;
	RTC_TimeShow();
	for(i=0;i<7;i++)
	{
		itoa(f_name,TimeDate);	  //ÎÄ¼þÃûÎªTimeDate
//	printf("%s\r\n",f_name);
	 	if(TimeDate.Date == 1)
		{
			TimeDate.Date  = getDays(2000+TimeDate.Year,TimeDate.Month);
			if(TimeDate.Month == 1)
		  TimeDate.Month   = 12;
    }
		 //·¢ËÍÊý¾Ý ???
		ReadDatatoBuffer();
		TimeDate.Date -- ;	
  }	 
}

  /*******************************************************************************
 * º¯ÊýÃû³Æ:File_delete                                                                  
 * Ãè    Êö:ÉÏµçÊ±¿ÌÉ¾³ýÇ°2¸öÔÂµÄ¼ÇÂ¼                                                                
 *                                                                               
 * Êä    Èë:ÎÞ                                                                     
 * Êä    ³ö:ÎÞ                                                                     
 * ·µ    »Ø:ÎÞ                                                                  
 * ÐÞ¸ÄÈÕÆÚ:2013Äê4ÔÂ20ÈÕ                                                                    
 *******************************************************************************/
void File_delete(void)
{
	 unsigned char i = 0 ,j=0,days = 0;
	 RTC_TimeShow();
 //  TimeDate.Month  = TimeDate.Month % 10 - 2 ;  //É¾³ýÇ°¶þ¸öÔÂµÄ¼ÇÂ¼
	 itoa(f_name,TimeDate);	  //°ÑÊ±¼ä×ª»»³É×Ö·û 
//	 printf("%s\r\n",f_name);
	 if(TimeDate.Month == 0x01)
	 {
		 TimeDate.Month = 11;
		 TimeDate.Year -=1 ; 
	 }
	 if(TimeDate.Month == 0x02)
	 {
		 TimeDate.Month = 12;
	   TimeDate.Year -=1 ; 
	 }
// TimeDate.Month =TimeDate.Month>2 ? TimeDate.Month-2: 12-TimeDate.Month;        
	 for(i=0;i<2;i++)
	 { 
			res = f_mount(0, &fs); 
			if(res!=FR_OK)
			{
			return ;
			}
//	  TimeDate.Month= GetageMonth(TimeDate.Month);
	    itoa(f_name,TimeDate);	  //°ÑÊ±¼ä×ª»»³É×Ö·û 	  
		  TimeDate.Date  = getDays(2000+TimeDate.Year,TimeDate.Month);
		  days = TimeDate.Date ;
		  for(j=0;j<days;j++)
		  {
				itoa(f_name,TimeDate);	  //°ÑÊ±¼ä×ª»»³É×Ö·û 
//				printf("%s\r\n",f_name);
				f_unlink(f_name);  //
				TimeDate.Date -- ;
      }
			TimeDate.Month-- ;
   }
}


