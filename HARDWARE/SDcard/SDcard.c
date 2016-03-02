#include	"SDcard.h"
#include	"SPI.h"
#include	"stm32l1xx_conf.h"
//////////////////////////////////////////////////////////////////////////////////

//����˵��

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------��ر�������-------------------------- 
uint8_t  SD_Type; 	//SD��������

//---------------------------�ڲ����ú�������--------------------------
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t SD_Select(void);
uint8_t SD_TypeJudge(void);
uint8_t SD_RecvData(uint8_t*buf,uint16_t len);
uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd);
uint8_t SD_GetResponse(uint8_t Response);


void SD_DisSelect(void);
void SD_SPI_Init(void);

//=============== ����ʵ�� =====================
/*
 * ��������SD_Initialize()
 * ���룺void
 * �����uint8_t ����0����ȷ������R1������R1�жϴ��󣻷���UNKNOW_ERROR:δ֪����
 * ���ܣ�SPIӲ�����ʼ�����ڲ����ã���SD_Initialize()��������
 */
uint8_t SD_Initialize(void)							
{
	uint8_t r1;      // ���SD���ķ���ֵ
	uint16_t i;		 // ����ѭ������
	
	SD_SPI_Init();
	SD_SPI_SpeedLow();	//���õ�����ģʽ 
	
	for(i=0;i<10;i++)SD_SPI_ReadWriteByte(DUMMY_DATA);		//SD����ǰ���ٷ�Ŷ���Ǹ�74�������źţ���80��
	
	r1 = SD_TypeJudge();	//SD�������ж�
	
	SD_DisSelect();//ȡ��Ƭѡ
	SD_SPI_SpeedHigh();//����

	if(SD_Type)return 0;	//SD_TYPE��Ϊ0����ʾ�жϳ�SD�����ͣ�����0��ȷ
	else if(r1)return r1; 	//R1��Ϊ0����ʾ��SD���������������⣬����R1����R1�����ж�
	return UNKNOW_ERROR;	//0xAAΪδ֪����	
}

/*
 * ��������SD_ReadDisk()
 * ���룺uint8_t*buf,uint32_t sector,uint8_t cnt
 * 		buf�����ݻ�����
 *		sector��Ҫ����������ʼ���
 * 		cnt:Ҫ������ȡ��������
 * �����uint8_t ����ֵ:0,ok;����,ʧ��.
 * ���ܣ���SD��
 */
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t r1;
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;//ת��Ϊ�ֽڵ�ַ
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD17,sector,0X01);//������
		if(r1==0)//ָ��ͳɹ�
		{
			r1=SD_RecvData(buf,512);//����512���ֽ�	   
		}
	}else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//����������
		do
		{
			r1=SD_RecvData(buf,512);//����512���ֽ�	 
			buf+=512;  
		}while(--cnt && r1==0); 	
		SD_SendCmd(CMD12,0,0X01);	//����ֹͣ����
	}   
	SD_DisSelect();//ȡ��Ƭѡ
	return r1;
}

/*
 * ��������SD_WriteDisk()
 * ���룺uint8_t*buf,uint32_t sector,uint8_t cnt
 * 		buf�����ݻ�����
 *		sector��Ҫд��������ʼ���
 * 		cnt:Ҫ����д���������
 * �����uint8_t ����ֵ:0,ok;����,ʧ��.
 * ���ܣ�дSD��
 */
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t r1;
	if(SD_Type!=SD_TYPE_V2HC)sector *= 512;//ת��Ϊ�ֽڵ�ַ
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01);//д����
		if(r1==0)//ָ��ͳɹ�
		{
			r1=SD_SendBlock(buf,0xFE);//д512���ֽ�	   
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//����ָ��	
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);//����д����
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);//д��512���ֽ�	 
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);//����ֹͣ����
		}
	}   
	SD_DisSelect();//ȡ��Ƭѡ
	return r1;//
}	   

/*
 * ��������SD_GetSectorCount()
 * ���룺void
 * �����uint32_t
 * 		0�� ȡ��������
 *		����:SD��������(������*512�ֽ�)
 * ���ܣ���ȡSD����������������������
 * PS��//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.	
 */  										  
uint32_t SD_GetSectorCount(void)
{
    uint8_t csd[16];
    uint32_t Capacity;  
    uint8_t n;
	uint16_t csize;  					    
	//ȡCSD��Ϣ������ڼ��������0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)	 //V2.00HC��
    {	
		csize = csd[9] + ((uint16_t)csd[8] << 8) + 1; //�õ�������
		Capacity = (uint32_t)csize << 10;//������*512KB���õ��ж��ٸ�KB�ֽ�,֮�����ƶ�10λ����Ϊ�˺������ͨ��
    }else//V2.0��V1.XX��MMC��
    {	
		//READ_BL_LENΪCSD[83:80]   csd[5]��4λ
		//C_SIZE_MULTΪCSD[59:47]	csd[9]�ĵ���λ��csd[10]���λ���
		//C_SIZEΪ	   CSD[73:62]	csd[6]�ĵ���λ��csd[7]����λ��csd[8]�������λ���
		//���� = 2^(READ_BL_LEN + C_SIZE_MULT + 2) *  ��C_SIZE + 1��
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
		Capacity= (uint32_t)csize << (n - 9);//�õ�������   
    }
    return Capacity;
}

/*
 * ��������SD_GetCSD()
 * ���룺uint8_t *:���CID���ڴ棬����16Byte
 * �����uint8_t ����ֵ:0��NO_ERR 1������	
 * ���ܣ���ȡSD����CSD��Ϣ�������������ٶ���Ϣ
 */  			   
uint8_t SD_GetCSD(uint8_t *csd_data)
{
    uint8_t r1;	 
    r1=SD_SendCmd(CMD9,0,0x01);//��CMD9�����CSD
    if(r1==0)
	{
    	r1=SD_RecvData(csd_data, 16);//����16���ֽڵ����� 
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}  

/*
 * ��������SD_GetCID()
 * ���룺uint8_t *:���CID���ڴ棬����16Byte
 * �����uint8_t ����ֵ:0��NO_ERR 1������	
 * ���ܣ���ȡSD����CID��Ϣ��������������Ϣ
 */ 												   
uint8_t SD_GetCID(uint8_t *cid_data)
{
    uint8_t r1;	   
    //��CMD10�����CID
    r1=SD_SendCmd(CMD10,0,0x01);
    if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//����16���ֽڵ�����	 
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}		

/*
 * ��������SD_WaitReady()
 * ���룺void
 * �����uint8_t:0,�ɹ�;1,ʧ��;
 * ���ܣ��ȴ���׼���ã���ʱ��ʾ����
 */
uint8_t SD_WaitReady(void)
{
	uint32_t t=0;
	do
	{
		if(SD_SPI_ReadWriteByte(DUMMY_DATA)==0XFF)return 0;//OK,SD��׼���û᷵��0XFF
		t++;		  	
	}while(t<0XFFFFFF);//�ȴ� 
	return 1;
}

/*
 * ��������SD_SPI_ReadWriteByte()
 * ���룺uint8_t data��Ҫд�������
 * �����uint8_t ����������
 * ���ܣ���SDд�벢��������
 */
uint8_t SD_SPI_ReadWriteByte(uint8_t data)
{
	return SPI1_ReadWriteByte(data);
}	  

/*
 * ��������SD_SPI_SpeedLow()
 * ���룺void
 * �����void
 * ���ܣ�SD����ʼ����ʱ��,��ҪSPIΪ����
 */
void SD_SPI_SpeedLow(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_256);//���õ�����ģʽ	
}

/*
 * ��������SD_SPI_SpeedLow()
 * ���룺void
 * �����void
 * ���ܣ�SD������������ʱ��,����Ϊ����
 */
void SD_SPI_SpeedHigh(void)
{
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);//���õ�����ģʽ	
}
////////////////////////////////////�ڲ����ú�����///////////////////////////////////
/*
 * ��������SD_SendBlock()
 * ���룺uint8_t*buf,uint16_t lens
 * 		buf:���ݻ�����
 * 		cmd:ָ��
 * �����uint8_t:0,�ɹ�;����,ʧ��;
 * ���ܣ���sd��д��һ�����ݰ������� 512�ֽ�
 */
uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
{	
	uint16_t t;		  	  
	if(SD_WaitReady())return 1;//�ȴ�׼��ʧЧ
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)//���ǽ���ָ��
	{
		for(t=0;t<512;t++)SPI1_ReadWriteByte(buf[t]);//����ٶ�,���ٺ�������ʱ��
	    SD_SPI_ReadWriteByte(DUMMY_DATA);//����crc
	    SD_SPI_ReadWriteByte(DUMMY_DATA);
		t=SD_SPI_ReadWriteByte(DUMMY_DATA);//������Ӧ
		if((t&0x1F)!=0x05)return 2;//��Ӧ����									  					    
	}						 									  					    
    return 0;//д��ɹ�
}


/*
 * ��������SD_RecvData()
 * ���룺uint8_t*buf,uint16_t lens
 * 		buf:���ݻ�����
 * 		len:Ҫ��ȡ�����ݳ���.
 * �����uint8_t:0,�ɹ�;����,ʧ��;	
 * ���ܣ���sd����ȡһ�����ݰ�������
 */
uint8_t SD_RecvData(uint8_t*buf,uint16_t len)
{			  	  
	if(SD_GetResponse(0xFE))return 1;//�ȴ�SD������������ʼ����0xFE
    while(len--)//��ʼ��������
    {
        *buf=SPI1_ReadWriteByte(DUMMY_DATA);
        buf++;
    }
    //������2��αCRC��dummy CRC��
    SD_SPI_ReadWriteByte(DUMMY_DATA);
    SD_SPI_ReadWriteByte(DUMMY_DATA);									  					    
    return 0;//��ȡ�ɹ�
}
/*
 * ��������SD_TypeJudge()
 * ���룺void
 * �����uint8_t:����R1��ֵ
 * ���ܣ�SD�������жϺ�������������SD��������
 * �жϷ�����yuleiSD��ѧϰ���������
 */
uint8_t SD_TypeJudge(void)
{
	uint16_t retry;	// �������г�ʱ����
	uint16_t i;		// ����ѭ������
	uint8_t r1;		// SD������ӦR1 
	uint8_t buf[4]; // �洢R7�ĳ�R1�����32bit
	retry = 20;
	do
	{
		r1 = SD_SendCmd(CMD0,0,0x95);	//����R1������IDLE(����)״̬
	}while(r1!=0x01 && retry--);		//��Ӧ��8bit�����һλ�ǿ��б�־λ���ɲμ�http://www.openedv.com/posts/list/21392.htm��ĳ¥��ͼ��V2.0�İ�Ƥ��
	
	SD_Type = SD_TYPE_ERR;		//Ĭ��û��SD����SD��������
	
	if(r1 == 0x01)			//���͵�CMD0�õ���Ӧ����ʾ�п�
	{
		if(SD_SendCmd(CMD8,0x1AA,0x87) == 1) //����R7��R1+32bit��0x01��ʾ���У�û�д��� ��SDV2.0
		{
			for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(DUMMY_DATA);	//����R7ʣ�µ�32bit
			if(buf[2] == 0x01 && buf[3] == 0xAA)	//SD��CMD8������Ӧ��ȷ
			{
				retry=0XFFFE;
				do
				{
					SD_SendCmd(CMD55,0,0X01);	//����CMD55,CMD55��ʾ��һ����Ӧ��ָ�ACMD
					r1=SD_SendCmd(CMD41,0x40000000,0X01);//����CMD41������R1
				}while(r1 && retry--);	//SD����ʼ����r1=0֤����ʼ���ɹ�
				
				//���retry=0��ʾ��ʼ��ʧ�ܣ�retry!=0��ʾ��ʼ���ɹ�������SD2.0�İ汾
				//��ʼ��֮ǰ��ֻ���⼸��������Ч����ʼ���ɹ��������������Ч
				if(retry && SD_SendCmd(CMD58,0,0X01) == 0x00)		//����R3��R1+OCR(32bit) ��ȷ��Ӧ0x00��ʾ����æ״̬
				{
					for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(DUMMY_DATA);//�õ�OCRֵ OCR��31λ��30bit�������ж�V2.0�Ŀ��Ƿ�ΪSDHC����
					if(buf[0] & 0x40) SD_Type = SD_TYPE_V2HC;	//Ϊ1���Ǹ���SD
					else SD_Type = SD_TYPE_V2;	//��Ϊ1������ͨSD
				}
			}
		}
		else //����SDV2.0 �ж��Ƿ�ΪSD V1.x������MMC V3
		{
			SD_SendCmd(CMD55,0,0X01);		//����CMD55
			r1=SD_SendCmd(CMD41,0,0X01);	//����CMD41
			if(r1<=1)
			{		
				SD_Type=SD_TYPE_V1;
				retry=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{
					SD_SendCmd(CMD55,0,0X01);	//����CMD55
					r1=SD_SendCmd(CMD41,0,0X01);//����CMD41
				}while(r1&&retry--);
			}else
			{
				SD_Type=SD_TYPE_MMC;//MMC V3
				retry=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{											    
					r1=SD_SendCmd(CMD1,0,0X01);//����CMD1
				}while(r1&&retry--);  
			}
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;//����Ŀ���CMD16Ϊѡ��һ������
		}
	}
	return r1;
}
/*
 * ��������SD_SPI_Init()
 * ���룺void
 * �����void
 * ���ܣ�SPIӲ�����ʼ�����ڲ����ã���SD_Initialize()��������
 */
void SD_SPI_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//��������ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE); //PORTAʱ��ʹ��
	//����CS��GPIO
	GPIO_InitStructure.GPIO_Pin = SD_CS_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(SD_GPIO_CS_PORT,&GPIO_InitStructure);
	
	
	SPI1_Config_Init();	//SPIģ���ʼ��
	SD_CS_H;			//����CS�ͷ�SPI����
}

/*
 * ��������SD_DisSelect()
 * ���룺void
 * �����void
 * ���ܣ�ȡ��ѡ��,�ͷ�SPI����
 */
void SD_DisSelect(void)
{
	SD_CS_H;
 	SD_SPI_ReadWriteByte(DUMMY_DATA);//�ṩ�����8��ʱ��
}

/*
 * ��������SD_DisSelect()
 * ���룺void
 * �����uint8_t:0,�ɹ�;1,ʧ��;
 * ���ܣ�ѡ��sd��,���ҵȴ���׼��OK
 */
uint8_t SD_Select(void)
{
	SD_CS_L;
	if(SD_WaitReady()==0)return 0;//�ȴ��ɹ�
	SD_DisSelect();
	return 1;//�ȴ�ʧ��
}

/*
 * ��������SD_SendCmd()
 * ���룺
 * 		uint8_t cmd   ���� 
 * 		uint8_t arg   �������
 * 		uint8_t crc   crcУ��ֵ
 * �����uint8_t:SD�����ص���Ӧ
 * ���ܣ���SD������һ������
 */   
//����ֵ:														  
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t r1;	
	uint8_t Retry=0; 
	SD_DisSelect();//ȡ���ϴ�Ƭѡ
	if(SD_Select())return 0XFF;//ƬѡʧЧ 
	//����
    SD_SPI_ReadWriteByte(cmd | 0x40);//�ֱ�д���������λ�̶�Ϊ01
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte(arg >> 16);
    SD_SPI_ReadWriteByte(arg >> 8);
    SD_SPI_ReadWriteByte(arg);	  
    SD_SPI_ReadWriteByte(crc); 
	if(cmd==CMD12)SD_SPI_ReadWriteByte(DUMMY_DATA);//Skip a stuff byte when stop reading
    //�ȴ���Ӧ����ʱ�˳�
	Retry=0X1F;//31
	do
	{
		r1=SD_SPI_ReadWriteByte(DUMMY_DATA);
	}while((r1&0X80) && Retry--);	 //ֻ���յ�������Ӧ����λ��Ϊ0��Ϊʲôr1&0X80���Բμ�http://www.openedv.com/posts/list/21392.htm��ĳ¥��ͼ��V2.0�İ�Ƥ��
	//����״ֵ̬
    return r1;
}	

/*
 * ��������SD_GetResponse()
 * ���룺uint8_t Response:Ҫ�õ��Ļ�Ӧֵ
 * �����uint8_t:0,�ɹ��õ��˸û�Ӧֵ������,�õ���Ӧֵʧ��
 * ���ܣ��ȴ�SD����Ӧ
 */
uint8_t SD_GetResponse(uint8_t Response)
{
	uint16_t Count=0xFFFF;//�ȴ�����	   						  
	while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)Count--;//�ȴ��õ�׼ȷ�Ļ�Ӧ  	  
	if (Count==0)return MSD_RESPONSE_FAILURE;//�õ���Ӧʧ��   
	else return MSD_RESPONSE_NO_ERROR;//��ȷ��Ӧ
}
////////////////////////////////////�ڲ����ú���������///////////////////////////////////

