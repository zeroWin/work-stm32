#include	"KEY.h"
#include	"FreeRTOS.h"
#include	"task.h"
//////////////////////////////////////////////////////////////////////////////////

//V1.0Ŀǰ��֧��һ��ʱ�䰴һ����������������ͬʱ���£���Ȼ���ܳ�����

//////////////////////////////////////////////////////////////////////////////////	





//=============== ����ʵ�� =====================
/*
 * ��������KEY_Init()
 * ���룺void
 * �����void
 * ���ܣ���KEY���г�ʼ������
 */
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//�����˿�ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	//����BUTTONA��GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_InitStructure.GPIO_Pin = BUTTON_A_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(BUTTON_A_GPIO_PORT,&GPIO_InitStructure);
	
	//����BUTTONB��GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_InitStructure.GPIO_Pin = BUTTON_B_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(BUTTON_B_GPIO_PORT,&GPIO_InitStructure);
}

/*
 * ��������IsButtonPressed()
 * ���룺void
 * �����ButtonX_Pressed ���¼��ļ�ֵ
 * ���ܣ��ж��Ƿ��м�����
 */
uint8_t IsButtonPressed(void)
{
	uint8_t ButtonX_Pressed = NO_BUTTON_PRESSED;
	if(!READ_BUTTON_A)		//�͵�ƽ��ʾ����
	{
		 vTaskDelay(Button_DebounceTime);//����,�ȴ�10ms���ٶ�ȡ  delay(10ms)
		if(!READ_BUTTON_A)
			ButtonX_Pressed = BUTTON_A_PRESSED;
	}
	else if(!READ_BUTTON_B)
	{
		vTaskDelay(Button_DebounceTime);//����,�ȴ�10ms���ٶ�ȡ  delay(10ms)
		if(!READ_BUTTON_B)
			ButtonX_Pressed = BUTTON_B_PRESSED;
	}
	
	return ButtonX_Pressed;
}
/*
 * ��������GetPressType()
 * ���룺GPIO_TypeDef* ButtonX_PORT	���µ����ĸ��˿�
 *		 uint32_t ButtonX_PIN    	���¶˿ڵ�����
 * �����uint8_t ButtonType:		 	���°���������
 * ���ܣ��жϰ�������,������������ͱ�ʾ�϶��м�����
 */
uint8_t GetPressType(GPIO_TypeDef* ButtonX_PORT,uint32_t ButtonX_PIN)
{
	uint16_t PresTimeCount = 0;			//�������µ�ʱ�������
	uint16_t WaitDoubleCount = 0;		//�ȴ��ڶ��ΰ������¼�����
	uint8_t  ButtonType;				
	
	while(!GPIO_ReadInputDataBit(ButtonX_PORT,ButtonX_PIN))
	{
		PresTimeCount++;
		vTaskDelay(Button_SampleTime);			//delay(5ms)
	}//��һ�ΰ���
	
	
	if(PresTimeCount > SHORT_PRESS_NUM)
	{
		if(PresTimeCount > LONG_PRESS_NUM)		//����
			ButtonType =  BUTTON_X_LONG;
		else
		{
			//�жϵ�˫��
			PresTimeCount = 0;
			for(WaitDoubleCount = 0;WaitDoubleCount < PRESS_WAIT ; WaitDoubleCount++)
			{
				if(!GPIO_ReadInputDataBit(ButtonX_PORT,ButtonX_PIN)) 
					PresTimeCount++;
				vTaskDelay(Button_SampleTime);	//delay(5ms);
			}
			
			if(PresTimeCount > SHORT_PRESS_NUM)
				ButtonType = BUTTON_X_DOUBLE;
			else
				ButtonType = BUTTON_X_SHORT;
		}
	}
	
	return ButtonType;
	

}



/*
 * ��������KEY_Scan()
 * ���룺void
 * �����uint ButtonType:	���°���������
 * ���ܣ����ذ�������
 */
uint8_t KEY_Scan(void)
{
	uint8_t ButtonType;
	ButtonType = IsButtonPressed();		//��ȡ�Ƿ��°���
	
	switch(ButtonType)
	{
		case BUTTON_A_PRESSED:ButtonType = (GetPressType(BUTTON_A_GPIO_PORT,BUTTON_A_GPIO_PIN)+BUTTON_A_TYPE_ADRRESS);break;	//��ȡA�����µ�����
		case BUTTON_B_PRESSED:ButtonType = (GetPressType(BUTTON_B_GPIO_PORT,BUTTON_B_GPIO_PIN)+BUTTON_B_TYPE_ADRRESS);break;	//��ȡB�����µ�����
		default:ButtonType = NO_BUTTON_PRESSED;break;								//�޼�����
		
	}
	return ButtonType;					//���ذ�������
}






