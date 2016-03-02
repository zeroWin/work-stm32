#include "UART1.h"
#include "stm32l1xx_conf.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  

//////////////////////////////////////////////////////////////////////////////////

//UART1底层驱动，供BLUETOOTH模块调用

//////////////////////////////////////////////////////////////////////////////////	


//串口发送缓存区 	
__align(8) uint8_t USART1_TX_BUF[USART1_MAX_SEND_LEN]; 	//发送缓冲,最大USART1_MAX_SEND_LEN字节								//如果使能了接收   	  
//串口接收缓存区 	
uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//接收缓冲,最大USART1_MAX_RECV_LEN个字节.
//接收状态标志，1表示接收到数据，0表示没有
uint16_t USART1_RX_STA=0; 

//---------------------------内部调用函数声明--------------------------
void UART1_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,uint32_t cpar,uint32_t cmar);
void TIM4_Init(uint16_t arr,uint16_t psc);
void TIM4_Set(uint8_t sta);
void UART1_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint8_t len);
//=============== 函数实现 =====================
/*
 * 函数名：USART1_Init()
 * 输入：uint32_t bound：要设置的波特率
 * 输出：void
 * 功能：对SPI进行初始化设置
 */
void USART1_Init(uint32_t bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//开启外设时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//UART1_RX端口配置
	GPIO_InitStructure.GPIO_Pin = UART1_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出 因为复用的时候端口方向由内部控制，所以这里设置成输出相当于浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(UART1_RX_PORT,&GPIO_InitStructure);
	
	//UART1_TX端口配置
	GPIO_InitStructure.GPIO_Pin = UART1_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(UART1_TX_PORT,&GPIO_InitStructure);
	
	//开启串口1对应引脚的复用映射
	GPIO_PinAFConfig(UART1_TX_PORT,UART1_TX_AF_PINSOURCE,GPIO_AF_USART1);	//GPIOB6复用为USART1--TX
	GPIO_PinAFConfig(UART1_RX_PORT,UART1_RX_AF_PINSOURCE,GPIO_AF_USART1);	//GPIOB7复用为USART1--RX
	
	//UART1初始化
	USART_InitStructure.USART_BaudRate = bound;		//波特率一般设置为9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件控制流
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//收发模式
	
	USART_Init(USART1, &USART_InitStructure);		//初始化串口1
	
	//DMA波特率设置
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);	//使能串口1的DMA发送
	//DMA配置 DMA1通道4,外设为串口1,存储器为USART1_TX_BUF 
	//DMA各通道表参考STM32参考手册的DMA中断表
	UART1_DMA_Config(DMA1_Channel4,(uint32_t)&USART1->DR,(uint32_t)USART1_TX_BUF);
	USART_Cmd(USART1,ENABLE);		//使能USART1串口
	
	
	//接收配置
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);		//开启接收中断
	
	//USART1中断配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢断优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	//TIM4初始化
	TIM4_Init(99,3199);		//10ms中断  T = 100 * 1/（3200/32000000）= 10ms
	USART1_RX_STA = 0;		//数据获取标志位清0
	TIM4_Set(0);			//关闭定时器4
	
}


/*
 * 函数名：u1_printf()
 * 输入：char* fmt,...要发送的数据，用printf的相同的格式输入
 * 输出：void
 * 功能：通过UART1发送数据到外设
 * PS：使用时要确保一次发送数据不超过USART1_MAX_SEND_LEN字节，且该种模式只能输出字符串，无法输出0x00
 *	   需要输出的数据里有0x00时，用u1_send()函数
 */
void u1_printf(char* fmt,...)  
{  
	va_list ap;
	while(DMA_GetCurrDataCounter(DMA1_Channel4)!=0);	//等待通道4传输完成
	va_start(ap,fmt);
	vsprintf((char*)USART1_TX_BUF,fmt,ap);
	va_end(ap);
	UART1_DMA_Enable(DMA1_Channel4,strlen((const char*)USART1_TX_BUF)); 	//通过dma发送出去
}

/*
 * 函数名：u1_send()
 * 输入：len 要输出USART1_TX_BUF的数据长度
 * 输出：void
 * 功能：通过UART1发送数据到外设
 * PS：使用时要确保一次发送数据不超过USART1_MAX_SEND_LEN字节,必须更改好USART1_TX_BUF内部
 * 		数据后才能调用此函数
 */
void u1_send(uint8_t len)
{
	while(DMA_GetCurrDataCounter(DMA1_Channel4)!=0);	//等待通道4传输完成
	UART1_DMA_Enable(DMA1_Channel4,len); 	//通过dma发送出去
}

////////////////////////////////////内部调用函数区///////////////////////////////////
/*
 * 函数名：TIM4_Set()
 * 输入：uint8_t sta：0，关闭;1,开启;	
 * 输出：void
 * 功能：设置TIM4的开关
 */
void TIM4_Set(uint8_t sta)
{
	if(sta)
	{
		TIM_SetCounter(TIM4,0);//计数器清空
		TIM_Cmd(TIM4, ENABLE);  //使能TIMx	
	}else TIM_Cmd(TIM4, DISABLE);//关闭定时器4	   	
}

/*
 * 函数名：TIM4_Init()
 * 输入：uint16_t arr,uint16_t psc
 * 		arr：自动重装值。
 *		psc：时钟预分频数	
 * 输出：void
 * 功能：通用定时器中断初始化，主要用来检测是否接受数据时间结束
 *		 10ms内没有接受数据事件发生，认为结束
 *		PS:定时器时钟为32Mhz
 */
void TIM4_Init(uint16_t arr,uint16_t psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//开启TIM4的时钟是能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	//TIM4定时器	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim，用处不明
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	
	//TIM4中断使能
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢断优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}


///////////////////////////////////////USART1 DMA发送配置部分//////////////////////////////////	   		    
/*
 * 函数名：USART1_Init()
 * 输入：DMA_Channel_TypeDef*DMA_CHx,uint32_t cpar,uint32_t cmar
 * 		DMA_CHx:DMA通道CHx
 *		cpar:外设地址
 * 		cmar:存储器地址
 * 输出：void
 * 功能：DMA1的各通道配置
 */
void UART1_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,uint32_t cpar,uint32_t cmar)
{
	DMA_InitTypeDef DMA_InitStructure;
	//开启DMA1的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	//初始化DMA 通道X
	DMA_DeInit(DMA_CHx);
	
	//配置DMA模块
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar; //DMA外设地址，设置为UART->CR地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar; //DMA内存基地址 设置为USART1_TX_BUF,用于存放数据
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //数据传输方向，双向传输
	DMA_InitStructure.DMA_BufferSize = 0; //DMA通道的DMA缓存的大小，先预设成0
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc =  DMA_MemoryInc_Enable; //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据传输宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据存储宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx,&DMA_InitStructure);	//初始化DMA通道x
}


//开启一次DMA传输
void UART1_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint8_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭 指示的通道        
	DMA_SetCurrDataCounter(DMA_CHx,len);//DMA通道的DMA缓存的大小	
	DMA_Cmd(DMA_CHx, ENABLE);           //开启DMA传输
}	   
////////////////////////////////////内部调用函数区///////////////////////////////////

////////////////////////////////////中断服务函数区///////////////////////////////////
//定时器4中断服务程序
void TIM4_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		USART1_RX_STA|=1<<15;	//标记接收完成，最高位置1表示接受完成
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //清除TIMx更新中断标志    
		TIM4_Set(0);			//关闭TIM4  
	}	    
}


//USART1中断服务程序
//PS：通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//	  如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//	  任何数据,则表示此次接收完毕.
void USART1_IRQHandler(void)
{
	uint8_t res;	    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res =USART_ReceiveData(USART1);		
		if(USART1_RX_STA<USART1_MAX_RECV_LEN)		//接收的字节不大于最大接收数，还可以接收数据
		{
			TIM_SetCounter(TIM4,0);//计数器清空，表示10ms内有下次数据来到，不会引发中断        				 
			if(USART1_RX_STA==0)TIM4_Set(1);	 	//第一次接收到数据,使能定时器4的中断 
			USART1_RX_BUF[USART1_RX_STA++]=res;		//记录接收到的值	 
		}else 
		{
			USART1_RX_STA|=1<<15;					//接收的字节不大于最大接收数，强制标记接收完成
		} 
	}  											 
}   


