#ifndef	__UART1_H
#define	__UART1_H

//////////////////////////////////////////////////////////////////////////////////

//UART1底层驱动，供BLUETOOTH模块调用

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------头文件--------------------------
#include	"stdint.h"

//---------------------------UART1端口定义-------------------------- 
#define UART1_TX_PORT			GPIOB
#define UART1_RX_PORT			GPIOB

#define UART1_RX_PIN			GPIO_Pin_7		//UART1_RX PB7
#define UART1_TX_PIN			GPIO_Pin_6		//UART1_TX PB6

#define UART1_TX_AF_PINSOURCE	GPIO_PinSource6 //复用端口的端口号
#define UART1_RX_AF_PINSOURCE	GPIO_PinSource7	
//---------------------------相关变量宏定义-------------------------- 
#define USART1_MAX_RECV_LEN		200					//最大接收缓存字节数
#define USART1_MAX_SEND_LEN		200					//最大发送缓存字节数



//---------------------------全局变量声明--------------------------
extern uint8_t  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern uint8_t  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern uint16_t USART1_RX_STA;   							//首位用来表示接受数据状态，同时低位用来表示接受了多少数据


//---------------------------UART1控制函数-------------------------- 
void USART1_Init(uint32_t bound);				//串口1初始化 
void u1_printf(char* fmt,...);					//数据发送函数
void u1_send(uint8_t len);						//发送带0x00的数据


















#endif
