#ifndef	__SDcard_H
#define	__SDcard_H

//////////////////////////////////////////////////////////////////////////////////

//����˵��

//////////////////////////////////////////////////////////////////////////////////	
//---------------------------ͷ�ļ�--------------------------
#include	"stdint.h"

//---------------------------SD�����Ͷ��� -------------------------- 
#define SD_TYPE_ERR     0X00		//�޿��򿨲���ʶ��
#define SD_TYPE_MMC     0X01		//MMC��
#define SD_TYPE_V1      0X02		//ʹ��V1.0��׼��SD��
#define SD_TYPE_V2      0X04		//ʹ��V2.0��׼��SD��
#define SD_TYPE_V2HC    0X06	 	//V2.0HC���ٿ�

//--------------------------- SD��ָ���-------------------------- 
#define CMD0    0       //����λ
#define CMD1    1		//����1 ����OCR�Ĵ���
#define CMD8    8       //����8 ��SEND_IF_COND ֻ��V2.0�����и�����������ж�SD�����ͣ�����0x01��V2.0������0x01����V2.0
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define CMD23   23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define CMD41   41      //����41��Ӧ����0x00	��ACOM41
#define CMD55   55      //����55��Ӧ����0x01 CMD55��ʾ��һ����Ӧ��ָ�ACMD CMD55+CMD41��������ж���V1.0����MMC�� 
#define CMD58   58      //����58����OCR��Ϣ	��31λ�������ж�V2.0���Ƿ�ΪSDHC����
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00

//--------------------------- ����д���Ӧ������ -------------------------- 
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

//--------------------------- SD����Ӧ����� -------------------------- 
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF

//--------------------------- SD��CS�˿ڼ������궨�� -------------------------- 
#define	SD_GPIO_CS_PORT							GPIOA
#define SD_CS_GPIO_PIN							GPIO_Pin_15		//PA15:CS

#define SD_CS_H GPIO_SetBits(SD_GPIO_CS_PORT, SD_CS_GPIO_PIN)	//CS = 1 
#define SD_CS_L GPIO_ResetBits(SD_GPIO_CS_PORT,SD_CS_GPIO_PIN) // CS = 0

//--------------------------- ��ض�ֵ�궨�� -------------------------- 
#define DUMMY_DATA 0xFF			//����SD��ʶ�������ֵ�����ڲ���ʱ���ź�
#define UNKNOW_ERROR 0xAA		//δ֪����

//---------------------------��ر�������-------------------------- 
extern uint8_t  SD_Type;//SD��������


//SDcard���ƺ���
uint8_t SD_Initialize(void);//SD��س�ʼ��
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);//��SD����������
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);//дSD����������

uint32_t SD_GetSectorCount(void);//�õ�SD��������������������
uint8_t SD_GetCSD(uint8_t *csd_data);//�õ�SD����CSD��Ϣ
uint8_t SD_GetCID(uint8_t *cid_data);//��ȡSD����CID��Ϣ
uint8_t SD_WaitReady(void);//�ȴ�SD��׼����

void SD_SPI_SpeedLow(void);//���õ���ģʽ	
void SD_SPI_SpeedHigh(void);//���ø���ģʽ
uint8_t SD_SPI_ReadWriteByte(uint8_t data);//��SD�����ͽ��յ��ֽ�����

#endif
