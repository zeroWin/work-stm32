#include "oled.h"
#include "key.h"
#include "RTC.h"
#include "FreeRTOS.h"
#include "task.h"
 
//优先级定义
#define BUTTON_TASK_PRIO  ( tskIDLE_PRIORITY  + 1 )
 
void vTaskButtonHandle(void * pvParameters);
uint8_t t=50;
int main(void)
{		

	
	xTaskCreate(vTaskButtonHandle, "Button Handle", configMINIMAL_STACK_SIZE, NULL, BUTTON_TASK_PRIO, NULL)	;
	
	vTaskStartScheduler();
	
	
	//除非发生错误，否则程序不会达到这里，此时系统的控制权是调度器
	while(1){}
}


void vTaskButtonHandle(void * pvParameters)
{   
	portTickType xLastWakeTime;
	const portTickType xPeriod = (1000 / portTICK_RATE_MS); 
	
	xLastWakeTime = xTaskGetTickCount();
	//设备初始化
	OLED_Init();			//初始化液晶
	//OLED_ShowString(0,0, "0.96' OLED TEST"); 
	RTC_Config_Init();
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//配置WAKE UP中断,1秒钟中断一次
   	while(1)
   	{

		vTaskDelayUntil(&xLastWakeTime,xPeriod);
   	 }
}





