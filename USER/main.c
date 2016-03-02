#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"
 
//���ȼ�����
#define OLED_TASK_PRIO    ( tskIDLE_PRIORITY  + 2 )
 
void oled_try(void * pvParameters);
 
int main(void)
{		
	xTaskCreate(oled_try, "OLED_TRY", configMINIMAL_STACK_SIZE, NULL, OLED_TASK_PRIO, NULL)	;
	
	vTaskStartScheduler();
	
	
	//���Ƿ������󣬷�����򲻻�ﵽ�����ʱϵͳ�Ŀ���Ȩ�ǵ�����
	while(1){}
}


void oled_try(void * pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xPeriod = (1000 / portTICK_RATE_MS);
	uint8_t t=50;
	
	xLastWakeTime = xTaskGetTickCount();
	
	OLED_Init();			//��ʼ��Һ��
	OLED_ShowString(0,0, "0.96' OLED TEST");   
	OLED_ShowString(0,16,"ASCII:");  
	OLED_ShowString(63,16,"CODE:");  
	
	while(1)
	{
		//OLED_ShowChar(48,16,t,16,1);//��ʾASCII�ַ�	   
		OLED_Refresh_Gram();
		t++;
		if(t>59)t=0;
		OLED_ShowNum(103,16,t,3,16);//��ʾASCII�ַ�����ֵ 
		vTaskDelayUntil(&xLastWakeTime,xPeriod);
	}
}
