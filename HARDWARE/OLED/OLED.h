#ifndef	__OLED_H
#define	__OLED_H

//////////////////////////////////////////////////////////////////////////////////

//����˵��

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------ͷ�ļ�--------------------------
#include	"stdint.h"
#include	"stm32l1xx_conf.h"



//---------------------------OLED�˿ڶ���-------------------------- 
//��ֲ��ͬоƬ�˿�ʱ�������޸Ķ˿ں�
//I2Cģʽ
#define	OLED_GPIO_RST_PORT							GPIOA
#define OLED_GPIO_SDA_PORT							GPIOB
#define OLED_GPIO_SCL_PORT							GPIOB
#define OLED_SDA_GPIO_PIN							GPIO_Pin_0		//PB0:SDA
#define OLED_SCL_GPIO_PIN							GPIO_Pin_1		//PB1:SCL
#define OLED_RST_GPIO_PIN							GPIO_Pin_7		//PA7:RST

//---------------------------�˿ڲ����궨��-------------------------- 
#define SDA_H GPIO_SetBits(OLED_GPIO_SDA_PORT,OLED_SDA_GPIO_PIN)
#define SDA_L GPIO_ResetBits(OLED_GPIO_SDA_PORT,OLED_SDA_GPIO_PIN)

#define SCL_H GPIO_SetBits(OLED_GPIO_SCL_PORT,OLED_SCL_GPIO_PIN)
#define SCL_L GPIO_ResetBits(OLED_GPIO_SCL_PORT,OLED_SCL_GPIO_PIN)

#define RST_H GPIO_SetBits(OLED_GPIO_RST_PORT,OLED_RST_GPIO_PIN)
#define RST_L GPIO_ResetBits(OLED_GPIO_RST_PORT,OLED_RST_GPIO_PIN)


//---------------------------��ر����궨��-------------------------- 
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


//---------------------------ʱ��궨��--------------------------
#define OLED_RESET_WAIT_TIME	 	(10 / portTICK_RATE_MS)		//10ms


//OLED���ƺ���
void OLED_WR_Byte(uint8_t data,uint8_t cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);

void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p);
#endif
