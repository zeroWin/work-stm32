//#include "FreeRTOS.h"
 #include "oled.h"
int main()
{
	uint8_t t;
 	//oledinit();     
	OLED_Init();
	OLED_ShowNum(16,0,25,2,16);
	OLED_ShowString(0,16,"CHAR:");
	OLED_Refresh_Gram();		
	
	
	t=' ';
	while(1) 
	{		
		OLED_ShowChar(48,16,t,16,1);//显示ASCII字符	   
		OLED_Refresh_Gram();
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,16,t,3,16);//显示ASCII字符的码值 
	}
}
