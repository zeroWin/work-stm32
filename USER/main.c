#include "oled.h"
#include "key.h"
#include "RTC.h"
#include "FreeRTOS.h"
#include "task.h"
 
//���ȼ�����
#define BUTTON_TASK_PRIO  ( tskIDLE_PRIORITY  + 1 )
 
void vTaskButtonHandle(void * pvParameters);
uint8_t t=50;
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
	const portTickType xPeriod = (1000 / portTICK_RATE_MS); 
	
	xLastWakeTime = xTaskGetTickCount();
	//�豸��ʼ��
	OLED_Init();			//��ʼ��Һ��
	//OLED_ShowString(0,0, "0.96' OLED TEST"); 
	RTC_Config_Init();
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
   	while(1)
   	{

		vTaskDelayUntil(&xLastWakeTime,xPeriod);
   	 }
}





