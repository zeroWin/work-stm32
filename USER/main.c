#include "oled.h"
#include "exfuns.h"
#include "SDcard.h" 
#include  "ff.h"
#include "FreeRTOS.h"
#include "task.h"
#include "RTC.h"
 
//���ȼ�����
#define BUTTON_TASK_PRIO  ( tskIDLE_PRIORITY  + 1 )
 
void vTaskButtonHandle(void * pvParameters);
int main(void)
{		
	xTaskCreate(vTaskButtonHandle, "Button Handle", configMINIMAL_STACK_SIZE, NULL, BUTTON_TASK_PRIO, NULL)	;
	
	
	
	vTaskStartScheduler();
	
	
	//���Ƿ������󣬷�����򲻻�ﵽ�����ʱϵͳ�Ŀ���Ȩ�ǵ�����
	while(1){}
}


void vTaskButtonHandle(void * pvParameters)
{   
	portTickType xLastWakeTime;
	const portTickType xPeriod = (200 / portTICK_RATE_MS); 
	uint32_t sd_size,sd_free;
	
	xLastWakeTime = xTaskGetTickCount();
	//�豸��ʼ��
	OLED_Init();			//��ʼ��Һ��
	OLED_ShowString(0,0, "0.96' OLED TEST"); 
	//RTC_Config_Init();
	//RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
	while(SD_Initialize())//��ⲻ��SD��
	{
		OLED_ShowString(0,16, "ERROR"); 
		OLED_Refresh_Gram();
	}
	OLED_ShowString(0,16, "OK"); 
	OLED_ShowNum(20,16,SD_Type,1,16);//��ʾASCII�ַ�����ֵ 
	OLED_Refresh_Gram();
	exfuns_init();						//Ϊfatfs��ر��������ڴ�	
	while(f_mount(0,fs))
	{
		OLED_ShowString(30,16, "ERROR-1"); 
		OLED_Refresh_Gram();
	} 					 	//����SD�� 
	//while(f_mkfs(0,0,4096))//4096��λΪ�ֽڣ���ʾÿ�صĴ�С  _MAX_SSһ��Ϊ512�ֽڱ�ʾ������С
	//{
	//	OLED_ShowString(30,16, "ERROR-2"); 
	//	OLED_Refresh_Gram();
	//}
	while(exf_getfree("0",&sd_size,&sd_free))	//�õ�SD������������ʣ������
	{
		OLED_ShowString(30,16, "ERROR"); 
		OLED_Refresh_Gram();
	}
	OLED_ShowString(30,16, "OK"); 	
	OLED_ShowNum(50,16,sd_size>>10,4,16);//��ʾASCII�ַ�����ֵ 
	OLED_ShowNum(90,16,sd_free>>10,4,16);//��ʾASCII�ַ�����ֵ 
	OLED_Refresh_Gram();
 	while(1)
	{
		vTaskDelayUntil(&xLastWakeTime,xPeriod);
	}
}

