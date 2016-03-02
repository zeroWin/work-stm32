#include "oled.h"
#include "key.h"
#include "RTC.h"
#include "SDcard.h" 
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
	const portTickType xPeriod = (200 / portTICK_RATE_MS); 
	uint32_t sd_size;
	uint8_t *buf1;
	uint8_t *buf2;
	xLastWakeTime = xTaskGetTickCount();
	//�豸��ʼ��
	OLED_Init();			//��ʼ��Һ��
	OLED_ShowString(0,0, "SD TEST"); 
	//RTC_Config_Init();
	//RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
	while(SD_Initialize())//��ⲻ��SD��
	{
		OLED_ShowString(0,16, "ERROR"); 
		OLED_Refresh_Gram();
	}
	OLED_ShowString(0,16, "OK"); 
	OLED_ShowNum(20,16,SD_Type,1,16);//��ʾASCII�ַ�����ֵ 
	
	sd_size=SD_GetSectorCount();//�õ�����KB
	OLED_ShowNum(35,16,sd_size>>11,4,16);//��ʾASCII�ַ�����ֵ 
	OLED_Refresh_Gram();
	
//	buf1 = pvPortMalloc( 512 );
//	buf2 = pvPortMalloc( 1024 );
//	for(sd_size=0;sd_size<512;sd_size++)
//		buf1[sd_size] = sd_size;
//	SD_WriteDisk(buf1,0,1);		//д��
//	for(sd_size=0;sd_size<512;sd_size++)
//		buf1[sd_size] = 512-sd_size;
//	SD_WriteDisk(buf1,1,1);		//д��
//	vPortFree(buf1);
//	if(SD_ReadDisk(buf2,0,2)==0)	//��ȡ0����������
//	{
//		OLED_ShowString(0,16, "ok"); 
//		OLED_Refresh_Gram();
//	}
//	sd_size = 0;
 	while(1)
   	{
//		OLED_ShowNum(80,16,buf2[sd_size],4,16);//��ʾASCII�ַ�����ֵ 
//		sd_size++;
//		if(sd_size == 1024)
//			sd_size = 0;
//		OLED_Refresh_Gram();
		vTaskDelayUntil(&xLastWakeTime,xPeriod);
   	 }
}




