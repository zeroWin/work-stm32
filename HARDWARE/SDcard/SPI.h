#ifndef	__SPI_H
#define	__SPI_H

//////////////////////////////////////////////////////////////////////////////////

//SPI�ײ���������SD��ģ�����

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------ͷ�ļ�--------------------------
#include	"stdint.h"
#include	"stm32l1xx_conf.h"

//---------------------------KEY�˿ڶ���-------------------------- 
//��ֲ��ͬоƬ�˿�ʱ�������޸Ķ˿ں�
#define SPI_CLK_GPIO_PORT		GPIOB		
#define SPI_MISO_GPIO_PORT		GPIOB
#define SPI_MOSI_GPIO_PORT		GPIOB


#define SPI_CLK_GPIO_PIN     	GPIO_Pin_3		//CLK -- PB3	39	
#define SPI_MISO_GPIO_PIN    	GPIO_Pin_4		//MISO --PB4	40
#define SPI_MOSI_GPIO_PIN    	GPIO_Pin_5		//MOSI --PB5	41

#define SPI_CLK_AF_PINSOURCE	GPIO_PinSource3 //���ö˿ڵĶ˿ں�
#define SPI_MISO_AF_PINSOURCE	GPIO_PinSource4	
#define SPI_MOSI_AF_PINSOURCE	GPIO_PinSource5	

#define SPIx_USE					SPI1		//ʹ�õ�SPI���
//---------------------------SPI���ƺ���-------------------------- 
void SPI1_Config_Init(void);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);//SPI1���߶�дһ���ֽ�
void SPI1_SetSpeed(uint8_t SpeedSet); //����SPI1�ٶ� 



















#endif
