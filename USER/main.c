#include "oled.h"
#include "exfuns.h"
#include "SDcard.h" 
#include  "ff.h"
#include "FreeRTOS.h"
#include "UART2.h"
#include "UART1.h"
#include "task.h"
 
//优先级定义
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
	
	
	//除非发生错误，否则程序不会达到这里，此时系统的控制权是调度器
	//while(1){}
}


void vTaskButtonHandle(void * pvParameters)
{   
	portTickType xLastWakeTime;
	const portTickType xPeriod = (200 / portTICK_RATE_MS); 
//	uint32_t sd_size,sd_free;
	
	xLastWakeTime = xTaskGetTickCount();
//	//设备初始化
	OLED_Init();			//初始化液晶
//	OLED_ShowString(0,0, "0.96' OLED TEST"); 
//	//RTC_Config_Init();
//	//RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//配置WAKE UP中断,1秒钟中断一次
//	while(SD_Initialize())//检测不到SD卡
//	{
//		OLED_ShowString(0,16, "ERROR"); 
//		OLED_Refresh_Gram();
//	}
//	OLED_ShowString(0,16, "OK"); 
//	OLED_ShowNum(20,16,SD_Type,1,16);//显示ASCII字符的码值 
//	OLED_Refresh_Gram();
//	exfuns_init();						//为fatfs相关变量申请内存	
//	while(f_mount(0,fs))
//	{
//		OLED_Refresh_Gram();
//	} 					 	//挂载SD卡 
//	while(exf_getfree("0",&sd_size,&sd_free))	//得到SD卡的总容量和剩余容量
//	{
//		OLED_ShowString(30,16, "ERROR"); 
//		OLED_Refresh_Gram();
//	}
//	OLED_ShowString(30,16, "OK"); 	
//	OLED_ShowNum(50,16,sd_size>>10,4,16);//显示ASCII字符的码值 
//	OLED_ShowNum(90,16,sd_free>>10,4,16);//显示ASCII字符的码值 
//	OLED_Refresh_Gram();
 	while(1)
	{
		if(USART2_RX_STA & 0x8000)
		{
			OLED_ShowNum(0,0,0,1,16);//显示ASCII字符的码值 
			USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
		}
		else
			OLED_ShowNum(0,0,1,1,16);//显示ASCII字符的码值 
		OLED_Refresh_Gram();
		vTaskDelayUntil(&xLastWakeTime,xPeriod);
   	 }
}

