#include "oled.h"
#include "exfuns.h"
#include "SDcard.h" 
#include  "ff.h"
#include "FreeRTOS.h"
#include "UART2.h"
#include "UART1.h"
#include "task.h"
 
//���ȼ�����
#define BUTTON_TASK_PRIO  ( tskIDLE_PRIORITY  + 1 )
 
void vTaskButtonHandle(void * pvParameters);

int main(void)
{		
	uint8_t num;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	USART2_Init(115200);

	
	UINT test_data;
	UINT bw;
	uint8_t buffer[1];
 	while(1)
	{
		if(USART2_RX_STA & 0x8000)
		{
			num = USART2_RX_STA & 0x0FFF;
			while(1)
			{
				num  = num;
			}
		}
		else
		{
		}
   	 }
	
	
	//xTaskCreate(vTaskButtonHandle, "Button Handle", configMINIMAL_STACK_SIZE, NULL, BUTTON_TASK_PRIO, NULL)	;
	
	//vTaskStartScheduler();
	
	
	//���Ƿ������󣬷�����򲻻�ﵽ�����ʱϵͳ�Ŀ���Ȩ�ǵ�����
	//while(1){}
}


void vTaskButtonHandle(void * pvParameters)
{   
	portTickType xLastWakeTime;
	const portTickType xPeriod = (200 / portTICK_RATE_MS); 
//	uint32_t sd_size,sd_free;
	
	xLastWakeTime = xTaskGetTickCount();
//	//�豸��ʼ��
	OLED_Init();			//��ʼ��Һ��
//	OLED_ShowString(0,0, "0.96' OLED TEST"); 
//	//RTC_Config_Init();
//	//RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
//	while(SD_Initialize())//��ⲻ��SD��
//	{
//		OLED_ShowString(0,16, "ERROR"); 
//		OLED_Refresh_Gram();
//	}
//	OLED_ShowString(0,16, "OK"); 
//	OLED_ShowNum(20,16,SD_Type,1,16);//��ʾASCII�ַ�����ֵ 
//	OLED_Refresh_Gram();
//	exfuns_init();						//Ϊfatfs��ر��������ڴ�	
//	while(f_mount(0,fs))
//	{
//		OLED_Refresh_Gram();
//	} 					 	//����SD�� 
//	while(exf_getfree("0",&sd_size,&sd_free))	//�õ�SD������������ʣ������
//	{
//		OLED_ShowString(30,16, "ERROR"); 
//		OLED_Refresh_Gram();
//	}
//	OLED_ShowString(30,16, "OK"); 	
//	OLED_ShowNum(50,16,sd_size>>10,4,16);//��ʾASCII�ַ�����ֵ 
//	OLED_ShowNum(90,16,sd_free>>10,4,16);//��ʾASCII�ַ�����ֵ 
//	OLED_Refresh_Gram();
 	while(1)
	{
		if(USART2_RX_STA & 0x8000)
		{
			OLED_ShowNum(0,0,0,1,16);//��ʾASCII�ַ�����ֵ 
			USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
		}
		else
			OLED_ShowNum(0,0,1,1,16);//��ʾASCII�ַ�����ֵ 
		OLED_Refresh_Gram();
		vTaskDelayUntil(&xLastWakeTime,xPeriod);
   	 }
}

