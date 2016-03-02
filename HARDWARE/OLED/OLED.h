#ifndef	__OLED_H
#define	__OLED_H

//////////////////////////////////////////////////////////////////////////////////

//待填说明

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------头文件--------------------------
#include	"stdint.h"
#include	"stm32l1xx_conf.h"



//---------------------------OLED端口定义-------------------------- 
//移植不同芯片端口时在这里修改端口号
//I2C模式
#define	OLED_GPIO_RST_PORT							GPIOA
#define OLED_GPIO_SDA_PORT							GPIOB
#define OLED_GPIO_SCL_PORT							GPIOB
#define OLED_SDA_GPIO_PIN							GPIO_Pin_0		//PB0:SDA
#define OLED_SCL_GPIO_PIN							GPIO_Pin_1		//PB1:SCL
#define OLED_RST_GPIO_PIN							GPIO_Pin_7		//PA7:RST

//---------------------------端口操作宏定义-------------------------- 
#define SDA_H GPIO_SetBits(OLED_GPIO_SDA_PORT,OLED_SDA_GPIO_PIN)
#define SDA_L GPIO_ResetBits(OLED_GPIO_SDA_PORT,OLED_SDA_GPIO_PIN)

#define SCL_H GPIO_SetBits(OLED_GPIO_SCL_PORT,OLED_SCL_GPIO_PIN)
#define SCL_L GPIO_ResetBits(OLED_GPIO_SCL_PORT,OLED_SCL_GPIO_PIN)

#define RST_H GPIO_SetBits(OLED_GPIO_RST_PORT,OLED_RST_GPIO_PIN)
#define RST_L GPIO_ResetBits(OLED_GPIO_RST_PORT,OLED_RST_GPIO_PIN)


//---------------------------相关变量宏定义-------------------------- 
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//---------------------------时间宏定义--------------------------
#define OLED_RESET_WAIT_TIME	 	(10 / portTICK_RATE_MS)		//10ms


//OLED控制函数
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
