#ifndef	__KEY_H
#define	__KEY_H

//////////////////////////////////////////////////////////////////////////////////


//V1.0目前仅支持一个时间按一个键，两个键不能同时按下，不然可能出错误

//////////////////////////////////////////////////////////////////////////////////	


//---------------------------头文件--------------------------

#include	"stdint.h"
#include	"stm32l1xx_conf.h"


//---------------------------KEY端口定义-------------------------- 
//移植不同芯片端口时在这里修改端口号
//两个键A、B
#define BUTTON_A_GPIO_PORT		GPIOA		
#define BUTTON_B_GPIO_PORT		GPIOA

#define BUTTON_A_GPIO_PIN     	GPIO_Pin_5		//BUTTONA--PA5	
#define BUTTON_B_GPIO_PIN    	GPIO_Pin_6		//BUTTONB--PA6	


//---------------------------端口操作宏定义--------------------------
#define READ_BUTTON_A			GPIO_ReadInputDataBit(BUTTON_A_GPIO_PORT,BUTTON_A_GPIO_PIN)	//读BUTTONA的电平值
#define READ_BUTTON_B			GPIO_ReadInputDataBit(BUTTON_B_GPIO_PORT,BUTTON_B_GPIO_PIN)	//读BUTTONB的电平值

//---------------------------相关变量宏定义--------------------------
#define BUTTON_A_PRESSED		0x01			//A按下
#define BUTTON_B_PRESSED		0x02			//B按下
#define NO_BUTTON_PRESSED		0x03			//没有按键按下

#define BUTTON_X_SHORT			0x04			//短按
#define BUTTON_X_LONG			0x05			//长按
#define BUTTON_X_DOUBLE			0x06			//双击

#define BUTTON_A_TYPE_ADRRESS	0x00
#define BUTTON_B_TYPE_ADRRESS	0x03

#define BUTTON_A_SHORT			BUTTON_X_SHORT+BUTTON_A_TYPE_ADRRESS			//短按 0x04
#define BUTTON_A_LONG			BUTTON_X_LONG+BUTTON_A_TYPE_ADRRESS				//长按 0x05
#define BUTTON_A_DOUBLE			BUTTON_X_DOUBLE+BUTTON_A_TYPE_ADRRESS			//双击 0x06

#define	BUTTON_B_SHORT			BUTTON_X_SHORT+BUTTON_B_TYPE_ADRRESS			//短按 0x07
#define BUTTON_B_LONG			BUTTON_X_LONG+BUTTON_B_TYPE_ADRRESS				//长按 0x08
#define BUTTON_B_DOUBLE			BUTTON_X_DOUBLE+BUTTON_B_TYPE_ADRRESS			//双击 0x09
	


//---------------------------时间宏定义--------------------------
#define SHORT_PRESS_NUM			5			//短按的最小时间 25ms
#define LONG_PRESS_NUM			150			//长按的最小时间	1s
#define PRESS_WAIT				200			//等待第二次按键按下等待时间 1s 1s内必须有键按下，没有默认为单击
#define Button_DebounceTime	 	(10 / portTICK_RATE_MS)
#define Button_SampleTime    	(5 / portTICK_RATE_MS)



//---------------------------KEY控制函数-------------------------- 
void KEY_Init(void);
uint8_t KEY_Scan(void);

#endif
