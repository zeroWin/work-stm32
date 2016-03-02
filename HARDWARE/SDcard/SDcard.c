#include	"SDcard.h"
#include	"SPI.h"
#include	"stm32l1xx_conf.h"
//////////////////////////////////////////////////////////////////////////////////

//´ıÌîËµÃ÷

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------Ïà¹Ø±äÁ¿ÉùÃ÷-------------------------- 
uint8_t  SD_Type; 	//SD¿¨µÄÀàĞÍ

//---------------------------ÄÚ²¿µ÷ÓÃº¯ÊıÉùÃ÷--------------------------
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t SD_Select(void);
uint8_t SD_TypeJudge(void);
uint8_t SD_RecvData(uint8_t*buf,uint16_t len);
uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd);
uint8_t SD_GetResponse(uint8_t Response);


void SD_DisSelect(void);
void SD_SPI_Init(void);

//=============== º¯ÊıÊµÏÖ =====================
/*
 * º¯ÊıÃû£ºSD_Initialize()
 * ÊäÈë£ºvoid
 * Êä³ö£ºuint8_t ·µ»Ø0£ºÕıÈ·£»·µ»ØR1£º¸ù¾İR1ÅĞ¶Ï´íÎó£»·µ»ØUNKNOW_ERROR:Î´Öª´íÎó
 * ¹¦ÄÜ£ºSPIÓ²¼ş²ã³õÊ¼»¯£¬ÄÚ²¿µ÷ÓÃ£¬¹©SD_Initialize()º¯Êıµ÷ÓÃ
 */
uint8_t SD_Initialize(void)							
{
	uint8_t r1;      // ´æ·ÅSD¿¨µÄ·µ»ØÖµ
	uint16_t i;		 // ¸÷ÀàÑ­»·¼ÆÊı
	
	SD_SPI_Init();
	SD_SPI_SpeedLow();	//ÉèÖÃµ½µÍËÙÄ£Ê½ 
	
	for(i=0;i<10;i++)SD_SPI_ReadWriteByte(DUMMY_DATA);		//SD²Ù×÷Ç°ÖÁÉÙ·¢Å¶ÊÇÄÇ¸ö74¸öÂö³åĞÅºÅ£¬·¢80¸ö
	
	r1 = SD_TypeJudge();	//SD¿¨ÀàĞÍÅĞ¶Ï
	
	SD_DisSelect();//È¡ÏûÆ¬Ñ¡
	SD_SPI_SpeedHigh();//¸ßËÙ

	if(SD_Type)return 0;	//SD_TYPE²»Îª0£¬±íÊ¾ÅĞ¶Ï³öSD¿¨ÀàĞÍ£¬·µ»Ø0ÕıÈ·
	else if(r1)return r1; 	//R1²»Îª0£¬±íÊ¾¶ÔSD²Ù×÷¹ı³ÌÖĞÓĞÎÊÌâ£¬·µ»ØR1¸ù¾İR1½øĞĞÅĞ¶Ï
	return UNKNOW_ERROR;	//0xAAÎªÎ´Öª´íÎó	
}

/*
 * º¯ÊıÃû£ºSD_ReadDisk()
 * ÊäÈë£ºuint8_t*buf,uint32_t sector,uint8_t cnt
 * 		buf£ºÊı¾İ»º´æÇø
 *		sector£ºÒª¶ÁµÄÉÈÇø¿ªÊ¼±àºÅ
 * 		cnt:ÒªÁ¬Ğø¶ÁÈ¡µÄÉÈÇøÊı
 * Êä³ö£ºuint8_t ·µ»ØÖµ:0,ok;ÆäËû,Ê§°Ü.
 * ¹¦ÄÜ£º¶ÁSD¿¨
 */
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t r1;
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;//×ª»»Îª×Ö½ÚµØÖ·
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD17,sector,0X01);//¶ÁÃüÁî
		if(r1==0)//Ö¸Áî·¢ËÍ³É¹¦
		{
			r1=SD_RecvData(buf,512);//½ÓÊÕ512¸ö×Ö½Ú	   
		}
	}else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//Á¬Ğø¶ÁÃüÁî
		do
		{
			r1=SD_RecvData(buf,512);//½ÓÊÕ512¸ö×Ö½Ú	 
			buf+=512;  
		}while(--cnt && r1==0); 	
		SD_SendCmd(CMD12,0,0X01);	//·¢ËÍÍ£Ö¹ÃüÁî
	}   
	SD_DisSelect();//È¡ÏûÆ¬Ñ¡
	return r1;
}

/*
 * º¯ÊıÃû£ºSD_WriteDisk()
 * ÊäÈë£ºuint8_t*buf,uint32_t sector,uint8_t cnt
 * 		buf£ºÊı¾İ»º´æÇø
 *		sector£ºÒªĞ´µÄÉÈÇø¿ªÊ¼±àºÅ
 * 		cnt:ÒªÁ¬ĞøĞ´ÈëµÄÉÈÇøÊı
 * Êä³ö£ºuint8_t ·µ»ØÖµ:0,ok;ÆäËû,Ê§°Ü.
 * ¹¦ÄÜ£ºĞ´SD¿¨
 */
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t r1;
	if(SD_Type!=SD_TYPE_V2HC)sector *= 512;//×ª»»Îª×Ö½ÚµØÖ·
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01);//Ğ´ÃüÁî
		if(r1==0)//Ö¸Áî·¢ËÍ³É¹¦
		{
			r1=SD_SendBlock(buf,0xFE);//Ğ´512¸ö×Ö½Ú	   
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//·¢ËÍÖ¸Áî	
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);//Á¬ĞøĞ´ÃüÁî
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);//Ğ´Èë512¸ö×Ö½Ú	 
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);//·¢ËÍÍ£Ö¹ÃüÁî
		}
	}   
	SD_DisSelect();//È¡ÏûÆ¬Ñ¡
	return r1;//
}	   

/*
 * º¯ÊıÃû£ºSD_GetSectorCount()
 * ÊäÈë£ºvoid
 * Êä³ö£ºuint32_t
 * 		0£º È¡ÈİÁ¿³ö´í
 *		ÆäËû:SD¿¨µÄÈİÁ¿(ÉÈÇøÊı*512×Ö½Ú)
 * ¹¦ÄÜ£º»ñÈ¡SD¿¨µÄ×ÜÈİÁ¿£¨»òÉÈÇøÊı£©
 * PS£º//Ã¿ÉÈÇøµÄ×Ö½ÚÊı±ØÎª512£¬ÒòÎªÈç¹û²»ÊÇ512£¬Ôò³õÊ¼»¯²»ÄÜÍ¨¹ı.	
 */  										  
uint32_t SD_GetSectorCount(void)
{
    uint8_t csd[16];
    uint32_t Capacity;  
    uint8_t n;
	uint16_t csize;  					    
	//È¡CSDĞÅÏ¢£¬Èç¹ûÆÚ¼ä³ö´í£¬·µ»Ø0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //Èç¹ûÎªSDHC¿¨£¬°´ÕÕÏÂÃæ·½Ê½¼ÆËã
    if((csd[0]&0xC0)==0x40)	 //V2.00HC¿¨
    {	
		csize = csd[9] + ((uint16_t)csd[8] << 8) + 1; //µÃµ½ÉÈÇøÊı
		Capacity = (uint32_t)csize << 10;//ÉÈÇøÊı*512KB£¬µÃµ½ÓĞ¶àÉÙ¸öKB×Ö½Ú,Ö®ËùÒÔÒÆ¶¯10Î»£¬ÊÇÎªÁËºÍÏÂÃæµÄÍ¨ÓÃ
    }else//V2.0»òV1.XX»òMMC¿¨
    {	
		//READ_BL_LENÎªCSD[83:80]   csd[5]µÍ4Î»
		//C_SIZE_MULTÎªCSD[59:47]	csd[9]µÄµÍÁ½Î»ºÍcsd[10]×î¸ßÎ»×é³É
		//C_SIZEÎª	   CSD[73:62]	csd[6]µÄµÍÁ½Î»¡¢csd[7]ËùÓĞÎ»¡¢csd[8]µÄ×î¸ßÁ½Î»×é³É
		//ÈİÁ¿ = 2^(READ_BL_LEN + C_SIZE_MULT + 2) *  £¨C_SIZE + 1£©
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
		Capacity= (uint32_t)csize << (n - 9);//µÃµ½ÉÈÇøÊı   
    }
    return Capacity;
}

/*
 * º¯ÊıÃû£ºSD_GetCSD()
 * ÊäÈë£ºuint8_t *:´æ·ÅCIDµÄÄÚ´æ£¬ÖÁÉÙ16Byte
 * Êä³ö£ºuint8_t ·µ»ØÖµ:0£ºNO_ERR 1£º´íÎó	
 * ¹¦ÄÜ£º»ñÈ¡SD¿¨µÄCSDĞÅÏ¢£¬°üÀ¨ÈİÁ¿ºÍËÙ¶ÈĞÅÏ¢
 */  			   
uint8_t SD_GetCSD(uint8_t *csd_data)
{
    uint8_t r1;	 
    r1=SD_SendCmd(CMD9,0,0x01);//·¢CMD9ÃüÁî£¬¶ÁCSD
    if(r1==0)
	{
    	r1=SD_RecvData(csd_data, 16);//½ÓÊÕ16¸ö×Ö½ÚµÄÊı¾İ 
    }
	SD_DisSelect();//È¡ÏûÆ¬Ñ¡
	if(r1)return 1;
	else return 0;
}  

/*
 * º¯ÊıÃû£ºSD_GetCID()
 * ÊäÈë£ºuint8_t *:´æ·ÅCIDµÄÄÚ´æ£¬ÖÁÉÙ16Byte
 * Êä³ö£ºuint8_t ·µ»ØÖµ:0£ºNO_ERR 1£º´íÎó	
 * ¹¦ÄÜ£º»ñÈ¡SD¿¨µÄCIDĞÅÏ¢£¬°üÀ¨ÖÆÔìÉÌĞÅÏ¢
 */ 												   
uint8_t SD_GetCID(uint8_t *cid_data)
{
    uint8_t r1;	   
    //·¢CMD10ÃüÁî£¬¶ÁCID
    r1=SD_SendCmd(CMD10,0,0x01);
    if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//½ÓÊÕ16¸ö×Ö½ÚµÄÊı¾İ	 
    }
	SD_DisSelect();//È¡ÏûÆ¬Ñ¡
	if(r1)return 1;
	else return 0;
}		

/*
 * º¯ÊıÃû£ºSD_WaitReady()
 * ÊäÈë£ºvoid
 * Êä³ö£ºuint8_t:0,³É¹¦;1,Ê§°Ü;
 * ¹¦ÄÜ£ºµÈ´ı¿¨×¼±¸ºÃ£¬³¬Ê±±íÊ¾´íÎó
 */
uint8_t SD_WaitReady(void)
{
	uint32_t t=0;
	do
	{
		if(SD_SPI_ReadWriteByte(DUMMY_DATA)==0XFF)return 0;//OK,SD¿¨×¼±¸ºÃ»á·µ»Ø0XFF
		t++;		  	
	}while(t<0XFFFFFF);//µÈ´ı 
	return 1;
}

/*
 * º¯ÊıÃû£ºSD_SPI_ReadWriteByte()
 * ÊäÈë£ºuint8_t data£ºÒªĞ´ÈëµÄÊı¾İ
 * Êä³ö£ºuint8_t ¶Áµ½µÄÊı¾İ
 * ¹¦ÄÜ£º¶ÔSDĞ´Èë²¢¶Á³öÊı¾İ
 */
uint8_t SD_SPI_ReadWriteByte(uint8_t data)
{
	return SPI1_ReadWriteByte(data);
}	  

/*
 * º¯ÊıÃû£ºSD_SPI_SpeedLow()
 * ÊäÈë£ºvoid
 * Êä³ö£ºvoid
 * ¹¦ÄÜ£ºSD¿¨³õÊ¼»¯µÄÊ±ºò,ĞèÒªSPIÎªµÍËÙ
 */
void SD_SPI_SpeedLow(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_256);//ÉèÖÃµ½µÍËÙÄ£Ê½	
}

/*
 * º¯ÊıÃû£ºSD_SPI_SpeedLow()
 * ÊäÈë£ºvoid
 * Êä³ö£ºvoid
 * ¹¦ÄÜ£ºSD¿¨Õı³£¹¤×÷µÄÊ±ºò,ÉèÖÃÎª¸ßËÙ
 */
void SD_SPI_SpeedHigh(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);//ÉèÖÃµ½¸ßËÙÄ£Ê½	
}
////////////////////////////////////ÄÚ²¿µ÷ÓÃº¯ÊıÇø///////////////////////////////////
/*
 * º¯ÊıÃû£ºSD_SendBlock()
 * ÊäÈë£ºuint8_t*buf,uint16_t lens
 * 		buf:Êı¾İ»º´æÇø
 * 		cmd:Ö¸Áî
 * Êä³ö£ºuint8_t:0,³É¹¦;ÆäËû,Ê§°Ü;
 * ¹¦ÄÜ£ºÏòsd¿¨Ğ´ÈëÒ»¸öÊı¾İ°üµÄÄÚÈİ 512×Ö½Ú
 */
uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
{	
	uint16_t t;		  	  
	if(SD_WaitReady())return 1;//µÈ´ı×¼±¸Ê§Ğ§
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)//²»ÊÇ½áÊøÖ¸Áî
	{
		for(t=0;t<512;t++)SPI1_ReadWriteByte(buf[t]);//Ìá¸ßËÙ¶È,¼õÉÙº¯Êı´«²ÎÊ±¼ä
	    SD_SPI_ReadWriteByte(DUMMY_DATA);//ºöÂÔcrc
	    SD_SPI_ReadWriteByte(DUMMY_DATA);
		t=SD_SPI_ReadWriteByte(DUMMY_DATA);//½ÓÊÕÏìÓ¦
		if((t&0x1F)!=0x05)return 2;//ÏìÓ¦´íÎó									  					    
	}						 									  					    
    return 0;//Ğ´Èë³É¹¦
}


/*
 * º¯ÊıÃû£ºSD_RecvData()
 * ÊäÈë£ºuint8_t*buf,uint16_t lens
 * 		buf:Êı¾İ»º´æÇø
 * 		len:Òª¶ÁÈ¡µÄÊı¾İ³¤¶È.
 * Êä³ö£ºuint8_t:0,³É¹¦;ÆäËû,Ê§°Ü;	
 * ¹¦ÄÜ£º´Ósd¿¨¶ÁÈ¡Ò»¸öÊı¾İ°üµÄÄÚÈİ
 */
uint8_t SD_RecvData(uint8_t*buf,uint16_t len)
{			  	  
	if(SD_GetResponse(0xFE))return 1;//µÈ´ıSD¿¨·¢»ØÊı¾İÆğÊ¼ÁîÅÆ0xFE
    while(len--)//¿ªÊ¼½ÓÊÕÊı¾İ
    {
        *buf=SPI1_ReadWriteByte(DUMMY_DATA);
        buf++;
    }
    //ÏÂÃæÊÇ2¸öÎ±CRC£¨dummy CRC£©
    SD_SPI_ReadWriteByte(DUMMY_DATA);
    SD_SPI_ReadWriteByte(DUMMY_DATA);									  					    
    return 0;//¶ÁÈ¡³É¹¦
}
/*
 * º¯ÊıÃû£ºSD_TypeJudge()
 * ÊäÈë£ºvoid
 * Êä³ö£ºuint8_t:·µ»ØR1µÄÖµ
 * ¹¦ÄÜ£ºSD¿¨ÀàĞÍÅĞ¶Ïº¯Êı£¬ÓÃÀ´ÉèÖÃSD¿¨µÄÀàĞÍ
 * ÅĞ¶Ï·½·¨£ºyuleiSD¿¨Ñ§Ï°¸ĞÎòµÚËÄÌõ
 */
uint8_t SD_TypeJudge(void)
{
	uint16_t retry;	// ÓÃÀ´½øĞĞ³¬Ê±¼ÆÊı
	uint16_t i;		// ¸÷ÀàÑ­»·¼ÆÊı
	uint8_t r1;		// SDÃüÁîÏìÓ¦R1 
	uint8_t buf[4]; // ´æ´¢R7µÄ³ıR1ÒÔÍâµÄ32bit
	retry = 20;
	do
	{
		r1 = SD_SendCmd(CMD0,0,0x95);	//·µ»ØR1£¬½øÈëIDLE(¿ÕÏĞ)×´Ì¬
	}while(r1!=0x01 && retry--);		//ÏìÓ¦µÄ8bit£¬×îºóÒ»Î»ÊÇ¿ÕÏĞ±êÖ¾Î»£¬¿É²Î¼ûhttp://www.openedv.com/posts/list/21392.htmÖĞÄ³Â¥µÄÍ¼»òV2.0µÄ°×Æ¤Êé
	
	SD_Type = SD_TYPE_ERR;		//Ä¬ÈÏÃ»ÓĞSD¿¨»òSD¿¨ÓĞÎÊÌâ
	
	if(r1 == 0x01)			//·¢ËÍµÄCMD0µÃµ½ÏìÓ¦£¬±íÊ¾ÓĞ¿¨
	{
		if(SD_SendCmd(CMD8,0x1AA,0x87) == 1) //·µ»ØR7£ºR1+32bit£¬0x01±íÊ¾¿ÕÏĞ£¬Ã»ÓĞ´íÎó ÊÇSDV2.0
		{
			for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(DUMMY_DATA);	//½ÓÊÕR7Ê£ÏÂµÄ32bit
			if(buf[2] == 0x01 && buf[3] == 0xAA)	//SD¶ÔCMD8ÃüÁîÏìÓ¦ÕıÈ·
			{
				retry=0XFFFE;
				do
				{
					SD_SendCmd(CMD55,0,0X01);	//·¢ËÍCMD55,CMD55±íÊ¾ÏÂÒ»ÌõÊÇÓ¦ÓÃÖ¸Áî¼´ACMD
					r1=SD_SendCmd(CMD41,0x40000000,0X01);//·¢ËÍCMD41£¬·µ»ØR1
				}while(r1 && retry--);	//SD¿¨³õÊ¼»¯£¬r1=0Ö¤Ã÷³õÊ¼»¯³É¹¦
				
				//Èç¹ûretry=0±íÊ¾³õÊ¼»¯Ê§°Ü£¬retry!=0±íÊ¾³õÊ¼»¯³É¹¦£¬¼ø±ğSD2.0µÄ°æ±¾
				//³õÊ¼»¯Ö®Ç°£¬Ö»ÓĞÕâ¼¸¸öÃüÁîÓĞĞ§£¬³õÊ¼»¯³É¹¦ºóÆäËûÃüÁî²ÅÓĞĞ§
				if(retry && SD_SendCmd(CMD58,0,0X01) == 0x00)		//·µ»ØR3£ºR1+OCR(32bit) ÕıÈ·ÏìÓ¦0x00±íÊ¾´¦ÓÚÃ¦×´Ì¬
				{
					for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(DUMMY_DATA);//µÃµ½OCRÖµ OCRµÚ31Î»£¨30bit£©ÓÃÓÚÅĞ¶ÏV2.0µÄ¿¨ÊÇ·ñÎªSDHCÀàĞÍ
					if(buf[0] & 0x40) SD_Type = SD_TYPE_V2HC;	//Îª1£ºÊÇ¸æËßSD
					else SD_Type = SD_TYPE_V2;	//²»Îª1£ºÊÇÆÕÍ¨SD
				}
			}
		}
		else //²»ÊÇSDV2.0 ÅĞ¶ÏÊÇ·ñÎªSD V1.x»òÕßÊÇMMC V3
		{
			SD_SendCmd(CMD55,0,0X01);		//·¢ËÍCMD55
			r1=SD_SendCmd(CMD41,0,0X01);	//·¢ËÍCMD41
			if(r1<=1)
			{		
				SD_Type=SD_TYPE_V1;
				retry=0XFFFE;
				do //µÈ´ıÍË³öIDLEÄ£Ê½
				{
					SD_SendCmd(CMD55,0,0X01);	//·¢ËÍCMD55
					r1=SD_SendCmd(CMD41,0,0X01);//·¢ËÍCMD41
				}while(r1&&retry--);
			}else
			{
				SD_Type=SD_TYPE_MMC;//MMC V3
				retry=0XFFFE;
				do //µÈ´ıÍË³öIDLEÄ£Ê½
				{											    
					r1=SD_SendCmd(CMD1,0,0X01);//·¢ËÍCMD1
				}while(r1&&retry--);  
			}
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;//´íÎóµÄ¿¨£¬CMD16ÎªÑ¡ÔñÒ»¸öÉÈÇø
		}
	}
	return r1;
}
/*
 * º¯ÊıÃû£ºSD_SPI_Init()
 * ÊäÈë£ºvoid
 * Êä³ö£ºvoid
 * ¹¦ÄÜ£ºSPIÓ²¼ş²ã³õÊ¼»¯£¬ÄÚ²¿µ÷ÓÃ£¬¹©SD_Initialize()º¯Êıµ÷ÓÃ
 */
void SD_SPI_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	//¿ªÆôÍâÉèÊ±ÖÓ
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE); //PORTAÊ±ÖÓÊ¹ÄÜ¡
	
	//ÅäÖÃCSµÄGPIO
	GPIO_InitStructure.GPIO_Pin = SD_CS_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//Êä³ö
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//ÍÆÍì
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//ÉÏÀ­
	GPIO_Init(SD_GPIO_CS_PORT,&GPIO_InitStructure);
	
	
	SPI1_Config_Init();	//SPIÄ£¿é³õÊ¼»¯
	SD_CS_H;			//À­¸ßCSÊÍ·ÅSPI×ÜÏß
}

/*
 * º¯ÊıÃû£ºSD_DisSelect()
 * ÊäÈë£ºvoid
 * Êä³ö£ºvoid
 * ¹¦ÄÜ£ºÈ¡ÏûÑ¡Ôñ,ÊÍ·ÅSPI×ÜÏß
 */
void SD_DisSelect(void)
{
	SD_CS_H;
 	SD_SPI_ReadWriteByte(DUMMY_DATA);//Ìá¹©¶îÍâµÄ8¸öÊ±ÖÓ
}

/*
 * º¯ÊıÃû£ºSD_DisSelect()
 * ÊäÈë£ºvoid
 * Êä³ö£ºuint8_t:0,³É¹¦;1,Ê§°Ü;
 * ¹¦ÄÜ£ºÑ¡Ôñsd¿¨,²¢ÇÒµÈ´ı¿¨×¼±¸OK
 */
uint8_t SD_Select(void)
{
	SD_CS_L;
	if(SD_WaitReady()==0)return 0;//µÈ´ı³É¹¦
	SD_DisSelect();
	return 1;//µÈ´ıÊ§°Ü
}

/*
 * º¯ÊıÃû£ºSD_SendCmd()
 * ÊäÈë£º
 * 		uint8_t cmd   ÃüÁî 
 * 		uint8_t arg   ÃüÁî²ÎÊı
 * 		uint8_t crc   crcĞ£ÑéÖµ
 * Êä³ö£ºuint8_t:SD¿¨·µ»ØµÄÏìÓ¦
 * ¹¦ÄÜ£ºÏòSD¿¨·¢ËÍÒ»¸öÃüÁî
 */   
//·µ»ØÖµ:														  
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t r1;	
	uint8_t Retry=0; 
	SD_DisSelect();//È¡ÏûÉÏ´ÎÆ¬Ñ¡
	if(SD_Select())return 0XFF;//Æ¬Ñ¡Ê§Ğ§ 
	//·¢ËÍ
    SD_SPI_ReadWriteByte(cmd | 0x40);//·Ö±ğĞ´ÈëÃüÁî£¬Ê×Á½Î»¹Ì¶¨Îª01
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte(arg >> 16);
    SD_SPI_ReadWriteByte(arg >> 8);
    SD_SPI_ReadWriteByte(arg);	  
    SD_SPI_ReadWriteByte(crc); 
	if(cmd==CMD12)SD_SPI_ReadWriteByte(DUMMY_DATA);//Skip a stuff byte when stop reading
    //µÈ´ıÏìÓ¦£¬»ò³¬Ê±ÍË³ö
	Retry=0X1F;//31
	do
	{
		r1=SD_SPI_ReadWriteByte(DUMMY_DATA);
	}while((r1&0X80) && Retry--);	 //Ö»ÓĞÊÕµ½ÃüÁîÏìÓ¦£¬Ê×Î»²ÅÎª0£¬ÎªÊ²Ã´r1&0X80¿ÉÒÔ²Î¼ûhttp://www.openedv.com/posts/list/21392.htmÖĞÄ³Â¥µÄÍ¼»òV2.0µÄ°×Æ¤Êé
	//·µ»Ø×´Ì¬Öµ
    return r1;
}	

/*
 * º¯ÊıÃû£ºSD_GetResponse()
 * ÊäÈë£ºuint8_t Response:ÒªµÃµ½µÄ»ØÓ¦Öµ
 * Êä³ö£ºuint8_t:0,³É¹¦µÃµ½ÁË¸Ã»ØÓ¦Öµ£»ÆäËû,µÃµ½»ØÓ¦ÖµÊ§°Ü
 * ¹¦ÄÜ£ºµÈ´ıSD¿¨»ØÓ¦
 */
uint8_t SD_GetResponse(uint8_t Response)
{
	uint16_t Count=0xFFFF;//µÈ´ı´ÎÊı	   						  
	while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)Count--;//µÈ´ıµÃµ½×¼È·µÄ»ØÓ¦  	  
	if (Count==0)return MSD_RESPONSE_FAILURE;//µÃµ½»ØÓ¦Ê§°Ü   
	else return MSD_RESPONSE_NO_ERROR;//ÕıÈ·»ØÓ¦
}
////////////////////////////////////ÄÚ²¿µ÷ÓÃº¯ÊıÇø½áÊø///////////////////////////////////

