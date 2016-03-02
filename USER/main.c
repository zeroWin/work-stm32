#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"
 
//优先级定义
#define OLED_TASK_PRIO    ( tskIDLE_PRIORITY  + 2 )
 
void oled_try(void * pvParameters);
 
int main(void)
{		
	xTaskCreate(oled_try, "OLED_TRY", configMINIMAL_STACK_SIZE, NULL, OLED_TASK_PRIO, NULL)	;
	
	vTaskStartScheduler();
	
	
	//除非发生错误，否则程序不会达到这里，此时系统的控制权是调度器
	while(1){}
}


void oled_try(void * pvParameters)
{
	portTickType xLastWakeTime;
	const portTickType xPeriod = (1000 / portTICK_RATE_MS);
	uint8_t t=50;
	
	xLastWakeTime = xTaskGetTickCount();
	
	OLED_Init();			//初始化液晶
	OLED_ShowString(0,0, "0.96' OLED TEST");   
	OLED_ShowString(0,16,"ASCII:");  
	OLED_ShowString(63,16,"CODE:");  
	
	while(1)
	{
		//OLED_ShowChar(48,16,t,16,1);//显示ASCII字符	   
		OLED_Refresh_Gram();
		t++;
		if(t>59)t=0;
		OLED_ShowNum(103,16,t,3,16);//显示ASCII字符的码值 
		vTaskDelayUntil(&xLastWakeTime,xPeriod);
	}
}
