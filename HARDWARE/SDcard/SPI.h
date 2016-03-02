#ifndef	__SPI_H
#define	__SPI_H

//////////////////////////////////////////////////////////////////////////////////

//SPI底层驱动，供SD卡模块调用

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------头文件--------------------------
#include	"stdint.h"
#include	"stm32l1xx_conf.h"

//---------------------------KEY端口定义-------------------------- 
//移植不同芯片端口时在这里修改端口号
#define SPI_CLK_GPIO_PORT		GPIOB		
#define SPI_MISO_GPIO_PORT		GPIOB
#define SPI_MOSI_GPIO_PORT		GPIOB


#define SPI_CLK_GPIO_PIN     	GPIO_Pin_3		//CLK -- PB3	39	
#define SPI_MISO_GPIO_PIN    	GPIO_Pin_4		//MISO --PB4	40
#define SPI_MOSI_GPIO_PIN    	GPIO_Pin_5		//MOSI --PB5	41

#define SPI_CLK_AF_PINSOURCE	GPIO_PinSource3 //复用端口的端口号
#define SPI_MISO_AF_PINSOURCE	GPIO_PinSource4	
#define SPI_MOSI_AF_PINSOURCE	GPIO_PinSource5	

#define SPIx_USE					SPI1		//使用的SPI标号
//---------------------------SPI控制函数-------------------------- 
void SPI1_Config_Init(void);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);//SPI1总线读写一个字节
void SPI1_SetSpeed(uint8_t SpeedSet); //设置SPI1速度 



















#endif
