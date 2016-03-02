#ifndef	__UART1_H
#define	__UART1_H

//////////////////////////////////////////////////////////////////////////////////

//UART1�ײ���������BLUETOOTHģ�����

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------ͷ�ļ�--------------------------
#include	"stdint.h"

//---------------------------UART1�˿ڶ���-------------------------- 
#define UART1_TX_PORT			GPIOB
#define UART1_RX_PORT			GPIOB

#define UART1_RX_PIN			GPIO_Pin_7		//UART1_RX PB7
#define UART1_TX_PIN			GPIO_Pin_6		//UART1_TX PB6

#define UART1_TX_AF_PINSOURCE	GPIO_PinSource6 //���ö˿ڵĶ˿ں�
#define UART1_RX_AF_PINSOURCE	GPIO_PinSource7	
//---------------------------��ر����궨��-------------------------- 
#define USART1_MAX_RECV_LEN		200					//�����ջ����ֽ���
#define USART1_MAX_SEND_LEN		200					//����ͻ����ֽ���



//---------------------------ȫ�ֱ�������--------------------------
extern uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern uint8_t  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern uint16_t USART1_RX_STA;   							//��λ������ʾ��������״̬��ͬʱ��λ������ʾ�����˶�������


//---------------------------UART1���ƺ���-------------------------- 
void USART1_Init(uint32_t bound);				//����1��ʼ�� 
void u1_printf(char* fmt,...);					//���ݷ��ͺ���
void u1_send(uint8_t len);						//���ʹ�0x00������


















#endif
