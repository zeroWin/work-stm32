#include "oled.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
 
//优先级定义
#define BUTTON_TASK_PRIO  ( tskIDLE_PRIORITY  + 1 )
 
void vTaskButtonHandle(void * pvParameters);
 
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
	const portTickType xPeriod = (10 / portTICK_RATE_MS); 
	uint8_t ButtonST = NO_BUTTON_PRESSED;
	uint8_t flag = 1;
	uint8_t num = 0;
	xLastWakeTime = xTaskGetTickCount();
	//设备初始化
	OLED_Init();			//初始化液晶
	KEY_Init();				//初始化键盘
	OLED_ShowString(0,16,"START-1");
	OLED_Refresh_Gram();
	
   	while(1)
   	{
		ButtonST =  KEY_Scan();
//		switch(ButtonST)
//		{
//			case BUTTON_A_SHORT:OLED_ShowString(0,0,"key1--S");OLED_Refresh_Gram(); break;
//			case BUTTON_A_LONG:OLED_ShowString(0,0,"key1--L");OLED_Refresh_Gram(); break;
//			case BUTTON_A_DOUBLE:OLED_ShowString(0,0,"key1--D");OLED_Refresh_Gram(); break;
//			case BUTTON_B_SHORT:OLED_ShowString(0,0,"key2--S");OLED_Refresh_Gram(); break;
//			case BUTTON_B_LONG:OLED_ShowString(0,0,"key2--L");OLED_Refresh_Gram(); break;
//			case BUTTON_B_DOUBLE:OLED_ShowString(0,0,"key2--D");OLED_Refresh_Gram(); break;
//			default: OLED_ShowString(56,16,"none");OLED_Refresh_Gram(); break;
//		}
		if(ButtonST != NO_BUTTON_PRESSED)
		{
			switch(flag)
			{
				case 1:OLED_WR_Byte(0xAE,OLED_CMD);flag = 0;break;//关闭显示
				case 0:OLED_WR_Byte(0xAF,OLED_CMD);flag = 1;break;//开启显示
			}
			num++;
		}
		OLED_ShowNum(0,0,num,3,16);
		OLED_Refresh_Gram();
		vTaskDelayUntil(&xLastWakeTime,xPeriod);
   	 }
}





