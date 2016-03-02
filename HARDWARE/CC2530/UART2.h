#ifndef	__UART2_H
#define	__UART2_H

//////////////////////////////////////////////////////////////////////////////////

//UART2底层驱动，供ZigBee模块调用

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------头文件--------------------------
#include	"stdint.h"

//---------------------------UART1端口定义-------------------------- 
#define UART2_TX_PORT			GPIOA
#define UART2_RX_PORT			GPIOA

#define UART2_TX_PIN			GPIO_Pin_2		//UART2_TX PA2
#define UART2_RX_PIN			GPIO_Pin_3		//UART2_RX PA3

#define UART2_TX_AF_PINSOURCE	GPIO_PinSource2 //复用端口的端口号
#define UART2_RX_AF_PINSOURCE	GPIO_PinSource3	

//---------------------------相关变量宏定义-------------------------- 
#define USART2_MAX_RECV_LEN		200					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		200					//最大发送缓存字节数



//---------------------------全局变量声明--------------------------
extern uint8_t  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern uint8_t  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern uint16_t USART2_RX_STA;   							//首位用来表示接受数据状态，同时低位用来表示接受了多少数据


//---------------------------UART1控制函数-------------------------- 
void USART2_Init(uint32_t bound);				//串口1初始化 
void u2_printf(char* fmt,...);					//数据发送函数



















#endif
