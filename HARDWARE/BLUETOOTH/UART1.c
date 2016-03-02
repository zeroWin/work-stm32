#include "UART1.h"
#include "stm32l1xx_conf.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  

//////////////////////////////////////////////////////////////////////////////////

//UART1�ײ���������BLUETOOTHģ�����

//////////////////////////////////////////////////////////////////////////////////	


//���ڷ��ͻ����� 	
__align(8) uint8_t USART1_TX_BUF[USART1_MAX_SEND_LEN]; 	//���ͻ���,���USART1_MAX_SEND_LEN�ֽ�								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//���ջ���,���USART1_MAX_RECV_LEN���ֽ�.
//����״̬��־��1��ʾ���յ����ݣ�0��ʾû��
uint16_t USART1_RX_STA=0; 

//---------------------------�ڲ����ú�������--------------------------
void UART1_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,uint32_t cpar,uint32_t cmar);
void TIM4_Init(uint16_t arr,uint16_t psc);
void TIM4_Set(uint8_t sta);
void UART1_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint8_t len);
//=============== ����ʵ�� =====================
/*
 * ��������USART1_Init()
 * ���룺uint32_t bound��Ҫ���õĲ�����
 * �����void
 * ���ܣ���SPI���г�ʼ������
 */
void USART1_Init(uint32_t bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//��������ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//UART1_RX�˿�����
	GPIO_InitStructure.GPIO_Pin = UART1_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//������� ��Ϊ���õ�ʱ��˿ڷ������ڲ����ƣ������������ó�����൱�ڸ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(UART1_RX_PORT,&GPIO_InitStructure);
	
	//UART1_TX�˿�����
	GPIO_InitStructure.GPIO_Pin = UART1_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(UART1_TX_PORT,&GPIO_InitStructure);
	
	//��������1��Ӧ���ŵĸ���ӳ��
	GPIO_PinAFConfig(UART1_TX_PORT,UART1_TX_AF_PINSOURCE,GPIO_AF_USART1);	//GPIOB6����ΪUSART1--TX
	GPIO_PinAFConfig(UART1_RX_PORT,UART1_RX_AF_PINSOURCE,GPIO_AF_USART1);	//GPIOB7����ΪUSART1--RX
	
	//UART1��ʼ��
	USART_InitStructure.USART_BaudRate = bound;		//������һ������Ϊ9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//�շ�ģʽ
	
	USART_Init(USART1, &USART_InitStructure);		//��ʼ������1
	
	//DMA����������
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);	//ʹ�ܴ���1��DMA����
	//DMA���� DMA1ͨ��4,����Ϊ����1,�洢��ΪUSART1_TX_BUF 
	//DMA��ͨ����ο�STM32�ο��ֲ��DMA�жϱ�
	UART1_DMA_Config(DMA1_Channel4,(uint32_t)&USART1->DR,(uint32_t)USART1_TX_BUF);
	USART_Cmd(USART1,ENABLE);		//ʹ��USART1����
	
	
	//��������
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);		//���������ж�
	
	//USART1�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//�������ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	//TIM4��ʼ��
	TIM4_Init(99,3199);		//10ms�ж�  T = 100 * 1/��3200/32000000��= 10ms
	USART1_RX_STA = 0;		//���ݻ�ȡ��־λ��0
	TIM4_Set(0);			//�رն�ʱ��4
	
}


/*
 * ��������u1_printf()
 * ���룺char* fmt,...Ҫ���͵����ݣ���printf����ͬ�ĸ�ʽ����
 * �����void
 * ���ܣ�ͨ��UART1�������ݵ�����
 * PS��ʹ��ʱҪȷ��һ�η������ݲ�����USART1_MAX_SEND_LEN�ֽڣ��Ҹ���ģʽֻ������ַ������޷����0x00
 *	   ��Ҫ�������������0x00ʱ����u1_send()����
 */
void u1_printf(char* fmt,...)  
{  
	va_list ap;
	while(DMA_GetCurrDataCounter(DMA1_Channel4)!=0);	//�ȴ�ͨ��4�������
	va_start(ap,fmt);
	vsprintf((char*)USART1_TX_BUF,fmt,ap);
	va_end(ap);
	UART1_DMA_Enable(DMA1_Channel4,strlen((const char*)USART1_TX_BUF)); 	//ͨ��dma���ͳ�ȥ
}

/*
 * ��������u1_send()
 * ���룺len Ҫ���USART1_TX_BUF�����ݳ���
 * �����void
 * ���ܣ�ͨ��UART1�������ݵ�����
 * PS��ʹ��ʱҪȷ��һ�η������ݲ�����USART1_MAX_SEND_LEN�ֽ�,������ĺ�USART1_TX_BUF�ڲ�
 * 		���ݺ���ܵ��ô˺���
 */
void u1_send(uint8_t len)
{
	while(DMA_GetCurrDataCounter(DMA1_Channel4)!=0);	//�ȴ�ͨ��4�������
	UART1_DMA_Enable(DMA1_Channel4,len); 	//ͨ��dma���ͳ�ȥ
}

////////////////////////////////////�ڲ����ú�����///////////////////////////////////
/*
 * ��������TIM4_Set()
 * ���룺uint8_t sta��0���ر�;1,����;	
 * �����void
 * ���ܣ�����TIM4�Ŀ���
 */
void TIM4_Set(uint8_t sta)
{
	if(sta)
	{
		TIM_SetCounter(TIM4,0);//���������
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	
	}else TIM_Cmd(TIM4, DISABLE);//�رն�ʱ��4	   	
}

/*
 * ��������TIM4_Init()
 * ���룺uint16_t arr,uint16_t psc
 * 		arr���Զ���װֵ��
 *		psc��ʱ��Ԥ��Ƶ��	
 * �����void
 * ���ܣ�ͨ�ö�ʱ���жϳ�ʼ������Ҫ��������Ƿ��������ʱ�����
 *		 10ms��û�н��������¼���������Ϊ����
 *		PS:��ʱ��ʱ��Ϊ32Mhz
 */
void TIM4_Init(uint16_t arr,uint16_t psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//����TIM4��ʱ������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	//TIM4��ʱ��	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim���ô�����
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//TIM4�ж�ʹ��
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//�������ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}


///////////////////////////////////////USART1 DMA�������ò���//////////////////////////////////	   		    
/*
 * ��������USART1_Init()
 * ���룺DMA_Channel_TypeDef*DMA_CHx,uint32_t cpar,uint32_t cmar
 * 		DMA_CHx:DMAͨ��CHx
 *		cpar:�����ַ
 * 		cmar:�洢����ַ
 * �����void
 * ���ܣ�DMA1�ĸ�ͨ������
 */
void UART1_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,uint32_t cpar,uint32_t cmar)
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
void UART1_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,uint8_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر� ָʾ��ͨ��        
	DMA_SetCurrDataCounter(DMA_CHx,len);//DMAͨ����DMA����Ĵ�С	
	DMA_Cmd(DMA_CHx, ENABLE);           //����DMA����
}	   
////////////////////////////////////�ڲ����ú�����///////////////////////////////////

////////////////////////////////////�жϷ�������///////////////////////////////////
//��ʱ��4�жϷ������
void TIM4_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		USART1_RX_STA|=1<<15;	//��ǽ�����ɣ����λ��1��ʾ�������
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //���TIMx�����жϱ�־    
		TIM4_Set(0);			//�ر�TIM4  
	}	    
}


//USART1�жϷ������
//PS��ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//	  ���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//	  �κ�����,���ʾ�˴ν������.
void USART1_IRQHandler(void)
{
	uint8_t res;	    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res =USART_ReceiveData(USART1);		
		if(USART1_RX_STA<USART1_MAX_RECV_LEN)		//���յ��ֽڲ��������������������Խ�������
		{
			TIM_SetCounter(TIM4,0);//��������գ���ʾ10ms�����´��������������������ж�        				 
			if(USART1_RX_STA==0)TIM4_Set(1);	 	//��һ�ν��յ�����,ʹ�ܶ�ʱ��4���ж� 
			USART1_RX_BUF[USART1_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART1_RX_STA|=1<<15;					//���յ��ֽڲ���������������ǿ�Ʊ�ǽ������
		} 
	}  											 
}   


