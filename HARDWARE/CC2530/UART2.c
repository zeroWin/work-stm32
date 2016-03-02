#include "UART2.h"
#include "stm32l1xx_conf.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  

//////////////////////////////////////////////////////////////////////////////////

//UART2�ײ���������ZigBeeģ�����

//////////////////////////////////////////////////////////////////////////////////	


//���ڷ��ͻ����� 	
__align(8) uint8_t USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
uint8_t USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
//����״̬��־��1��ʾ���յ����ݣ�0��ʾû��
uint16_t USART2_RX_STA=0; 

//---------------------------�ڲ����ú�������--------------------------
void UART2_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,uint32_t cpar,uint32_t cmar);
void TIM3_Init(uint16_t arr,uint16_t psc);
void TIM3_Set(uint8_t sta);
void UART2_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint8_t len);
//=============== ����ʵ�� =====================
/*
 * ��������USART2_Init()
 * ���룺uint32_t bound��Ҫ���õĲ�����
 * �����void
 * ���ܣ���USART2���г�ʼ������
 */
void USART2_Init(uint32_t bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//��������ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//UART2_RX�˿�����
	GPIO_InitStructure.GPIO_Pin = UART2_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//������� ��Ϊ���õ�ʱ��˿ڷ������ڲ����ƣ������������ó�����൱�ڸ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;	//40MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//����
	GPIO_Init(UART2_RX_PORT,&GPIO_InitStructure);
	
	//UART2_TX�˿�����
	GPIO_InitStructure.GPIO_Pin = UART2_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;	//40MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//����
	GPIO_Init(UART2_TX_PORT,&GPIO_InitStructure);
	
	//��������2��Ӧ���ŵĸ���ӳ��
	GPIO_PinAFConfig(UART2_TX_PORT,UART2_TX_AF_PINSOURCE,GPIO_AF_USART2);	//GPIOA2����ΪUSART2--TX
	GPIO_PinAFConfig(UART2_RX_PORT,UART2_RX_AF_PINSOURCE,GPIO_AF_USART2);	//GPIOA3����ΪUSART2--RX
	
	//UART2��ʼ��
	USART_InitStructure.USART_BaudRate = bound;		//������һ������Ϊbound
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure);		//��ʼ������2
	
	//DMA����������
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);	//ʹ�ܴ���2��DMA����
	//DMA���� DMA1ͨ��7,����Ϊ����2,�洢��ΪUSART2_TX_BUF 
	//DMA��ͨ����ο�STM32�ο��ֲ��DMA�жϱ�
	UART2_DMA_Config(DMA1_Channel7,(uint32_t)&USART2->DR,(uint32_t)USART2_TX_BUF);
	USART_Cmd(USART2,ENABLE);		//ʹ��USART2����
	
	//��������
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);		//���������ж�
		
	//USART2�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//�������ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	
	//TIM3��ʼ��
	TIM3_Init(99,3199);		//10ms�ж�  T = 100 * 1/��3200/32000000��= 10ms
	USART2_RX_STA = 0;		//���ݻ�ȡ��־λ��0
	TIM3_Set(0);			//�رն�ʱ��3
	
}


/*
 * ��������u2_printf()
 * ���룺char* fmt,...Ҫ���͵����ݣ���printf����ͬ�ĸ�ʽ����
 * �����void
 * ���ܣ�ͨ��UART1�������ݵ�����
 * PS��ʹ��ʱҪȷ��һ�η������ݲ�����USART1_MAX_SEND_LEN�ֽ�
 */
void u2_printf(char* fmt,...)  
{  
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	while(DMA_GetCurrDataCounter(DMA1_Channel7)!=0);	//�ȴ�ͨ��4�������  
	UART2_DMA_Enable(DMA1_Channel7,strlen((const char*)USART2_TX_BUF)); 	//ͨ��dma���ͳ�ȥ
}
////////////////////////////////////�ڲ����ú�����///////////////////////////////////
/*
 * ��������TIM3_Set()
 * ���룺uint8_t sta��0���ر�;1,����;	
 * �����void
 * ���ܣ�����TIM3�Ŀ���
 */
void TIM3_Set(uint8_t sta)
{
	if(sta)
	{
		TIM_SetCounter(TIM3,0);//���������
		TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx	
	}else TIM_Cmd(TIM3, DISABLE);//�رն�ʱ��5	   
}

/*
 * ��������TIM3_Init()
 * ���룺uint16_t arr,uint16_t psc
 * 		arr���Զ���װֵ��
 *		psc��ʱ��Ԥ��Ƶ��	
 * �����void
 * ���ܣ�ͨ�ö�ʱ���жϳ�ʼ������Ҫ��������Ƿ��������ʱ�����
 *		 10ms��û�н��������¼���������Ϊ����
 *		PS:��ʱ��ʱ��Ϊ32Mhz
 */
void TIM3_Init(uint16_t arr,uint16_t psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//����TIM3��ʱ������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//TIM3��ʱ��	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim���ô�����
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//TIM3�ж�ʹ��
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//�������ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}


///////////////////////////////////////USART2 DMA�������ò���//////////////////////////////////	   		    
/*
 * ��������UART2_DMA_Config()
 * ���룺DMA_Channel_TypeDef*DMA_CHx,uint32_t cpar,uint32_t cmar
 * 		DMA_CHx:DMAͨ��CHx
 *		cpar:�����ַ
 * 		cmar:�洢����ַ
 * �����void
 * ���ܣ�DMA1�ĸ�ͨ������
 */
void UART2_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,uint32_t cpar,uint32_t cmar)
{
	DMA_InitTypeDef DMA_InitStructure;
	//����DMA1��ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	//��ʼ��DMA ͨ��X
	DMA_DeInit(DMA_CHx);
	
	//����DMAģ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar; //DMA�����ַ������ΪUART->CR��ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar; //DMA�ڴ����ַ ����ΪUSART1_TX_BUF,���ڴ������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //���ݴ��䷽��˫����
	DMA_InitStructure.DMA_BufferSize = 0; //DMAͨ����DMA����Ĵ�С����Ԥ���0
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc =  DMA_MemoryInc_Enable; //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݴ�����Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݴ洢���Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx,&DMA_InitStructure);	//��ʼ��DMAͨ��x
}


//����һ��DMA����
void UART2_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint8_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر� ָʾ��ͨ��        
	DMA_SetCurrDataCounter(DMA_CHx,len);//DMAͨ����DMA����Ĵ�С	
	DMA_Cmd(DMA_CHx, ENABLE);           //����DMA����
}	   
////////////////////////////////////�ڲ����ú�����///////////////////////////////////

////////////////////////////////////�жϷ�������///////////////////////////////////
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		USART2_RX_STA|=1<<15;	//��ǽ�����ɣ����λ��1��ʾ�������
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־    
		TIM3_Set(0);			//�ر�TIM3  
	}	    
}


//USART2�жϷ������
//PS��ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//	  ���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//	  �κ�����,���ʾ�˴ν������.
void USART2_IRQHandler(void)
{
	uint8_t res;	    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res =USART_ReceiveData(USART2);		
		if(USART2_RX_STA<USART2_MAX_RECV_LEN)		//���յ��ֽڲ��������������������Խ�������
		{
			TIM_SetCounter(TIM3,0);//��������գ���ʾ10ms�����´��������������������ж�        				 
			if(USART2_RX_STA ==0 )TIM3_Set(1);	 	//��һ�ν��յ�����,ʹ�ܶ�ʱ��3���ж� 
			USART2_RX_BUF[USART2_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART2_RX_STA |= 1<<15;					//���յ��ֽڲ���������������ǿ�Ʊ�ǽ������
		} 
	}
	
 	
}   


