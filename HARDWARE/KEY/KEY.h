#ifndef	__KEY_H
#define	__KEY_H

//////////////////////////////////////////////////////////////////////////////////


//V1.0Ŀǰ��֧��һ��ʱ�䰴һ����������������ͬʱ���£���Ȼ���ܳ�����

//////////////////////////////////////////////////////////////////////////////////	


//---------------------------ͷ�ļ�--------------------------

#include	"stdint.h"
#include	"stm32l1xx_conf.h"


//---------------------------KEY�˿ڶ���-------------------------- 
//��ֲ��ͬоƬ�˿�ʱ�������޸Ķ˿ں�
//������A��B
#define BUTTON_A_GPIO_PORT		GPIOA		
#define BUTTON_B_GPIO_PORT		GPIOA

#define BUTTON_A_GPIO_PIN     	GPIO_Pin_5		//BUTTONA--PA5	
#define BUTTON_B_GPIO_PIN    	GPIO_Pin_6		//BUTTONB--PA6	


//---------------------------�˿ڲ����궨��--------------------------
#define READ_BUTTON_A			GPIO_ReadInputDataBit(BUTTON_A_GPIO_PORT,BUTTON_A_GPIO_PIN)	//��BUTTONA�ĵ�ƽֵ
#define READ_BUTTON_B			GPIO_ReadInputDataBit(BUTTON_B_GPIO_PORT,BUTTON_B_GPIO_PIN)	//��BUTTONB�ĵ�ƽֵ

//---------------------------��ر����궨��--------------------------
#define BUTTON_A_PRESSED		0x01			//A����
#define BUTTON_B_PRESSED		0x02			//B����
#define NO_BUTTON_PRESSED		0x03			//û�а�������

#define BUTTON_X_SHORT			0x04			//�̰�
#define BUTTON_X_LONG			0x05			//����
#define BUTTON_X_DOUBLE			0x06			//˫��

#define BUTTON_A_TYPE_ADRRESS	0x00
#define BUTTON_B_TYPE_ADRRESS	0x03

#define BUTTON_A_SHORT			BUTTON_X_SHORT+BUTTON_A_TYPE_ADRRESS			//�̰� 0x04
#define BUTTON_A_LONG			BUTTON_X_LONG+BUTTON_A_TYPE_ADRRESS				//���� 0x05
#define BUTTON_A_DOUBLE			BUTTON_X_DOUBLE+BUTTON_A_TYPE_ADRRESS			//˫�� 0x06

#define	BUTTON_B_SHORT			BUTTON_X_SHORT+BUTTON_B_TYPE_ADRRESS			//�̰� 0x07
#define BUTTON_B_LONG			BUTTON_X_LONG+BUTTON_B_TYPE_ADRRESS				//���� 0x08
#define BUTTON_B_DOUBLE			BUTTON_X_DOUBLE+BUTTON_B_TYPE_ADRRESS			//˫�� 0x09
	


//---------------------------ʱ��궨��--------------------------
#define SHORT_PRESS_NUM			5			//�̰�����Сʱ�� 25ms
#define LONG_PRESS_NUM			150			//��������Сʱ��	1s
#define PRESS_WAIT				200			//�ȴ��ڶ��ΰ������µȴ�ʱ�� 1s 1s�ڱ����м����£�û��Ĭ��Ϊ����
#define Button_DebounceTime	 	(10 / portTICK_RATE_MS)
#define Button_SampleTime    	(5 / portTICK_RATE_MS)



//---------------------------KEY���ƺ���-------------------------- 
void KEY_Init(void);
uint8_t KEY_Scan(void);

#endif
