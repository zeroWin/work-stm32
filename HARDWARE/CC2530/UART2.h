#ifndef	__UART2_H
#define	__UART2_H

//////////////////////////////////////////////////////////////////////////////////

//UART2�ײ���������ZigBeeģ�����

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------ͷ�ļ�--------------------------
#include	"stdint.h"

//---------------------------UART1�˿ڶ���-------------------------- 
#define UART2_TX_PORT			GPIOA
#define UART2_RX_PORT			GPIOA

#define UART2_TX_PIN			GPIO_Pin_2		//UART2_TX PA2
#define UART2_RX_PIN			GPIO_Pin_3		//UART2_RX PA3

#define UART2_TX_AF_PINSOURCE	GPIO_PinSource2 //���ö˿ڵĶ˿ں�
#define UART2_RX_AF_PINSOURCE	GPIO_PinSource3	

//---------------------------��ر����궨��-------------------------- 
#define USART2_MAX_RECV_LEN		200					//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		200					//����ͻ����ֽ���



//---------------------------ȫ�ֱ�������--------------------------
extern uint8_t  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern uint8_t  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern uint16_t USART2_RX_STA;   							//��λ������ʾ��������״̬��ͬʱ��λ������ʾ�����˶�������


//---------------------------UART1���ƺ���-------------------------- 
void USART2_Init(uint32_t bound);				//����1��ʼ�� 
void u2_printf(char* fmt,...);					//���ݷ��ͺ���



















#endif
