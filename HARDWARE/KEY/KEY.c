#include	"KEY.h"
#include	"FreeRTOS.h"
#include	"task.h"
//////////////////////////////////////////////////////////////////////////////////

//V1.0目前仅支持一个时间按一个键，两个键不能同时按下，不然可能出错误

//////////////////////////////////////////////////////////////////////////////////	





//=============== 函数实现 =====================
/*
 * 函数名：KEY_Init()
 * 输入：void
 * 输出：void
 * 功能：对KEY进行初始化设置
 */
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//开启端口时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	//配置BUTTONA的GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_InitStructure.GPIO_Pin = BUTTON_A_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(BUTTON_A_GPIO_PORT,&GPIO_InitStructure);
	
	//配置BUTTONB的GPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_InitStructure.GPIO_Pin = BUTTON_B_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(BUTTON_B_GPIO_PORT,&GPIO_InitStructure);
}

/*
 * 函数名：IsButtonPressed()
 * 输入：void
 * 输出：ButtonX_Pressed 按下键的键值
 * 功能：判断是否有键按下
 */
uint8_t IsButtonPressed(void)
{
	uint8_t ButtonX_Pressed = NO_BUTTON_PRESSED;
	if(!READ_BUTTON_A)		//低电平表示按下
	{
		 vTaskDelay(Button_DebounceTime);//防抖,等待10ms，再读取  delay(10ms)
		if(!READ_BUTTON_A)
			ButtonX_Pressed = BUTTON_A_PRESSED;
	}
	else if(!READ_BUTTON_B)
	{
		vTaskDelay(Button_DebounceTime);//防抖,等待10ms，再读取  delay(10ms)
		if(!READ_BUTTON_B)
			ButtonX_Pressed = BUTTON_B_PRESSED;
	}
	
	return ButtonX_Pressed;
}
/*
 * 函数名：GetPressType()
 * 输入：GPIO_TypeDef* ButtonX_PORT	按下的是哪个端口
 *		 uint32_t ButtonX_PIN    	按下端口的引脚
 * 输出：uint8_t ButtonType:		 	按下按键的类型
 * 功能：判断按键类型,进入这个函数就表示肯定有键按下
 */
uint8_t GetPressType(GPIO_TypeDef* ButtonX_PORT,uint32_t ButtonX_PIN)
{
	uint16_t PresTimeCount = 0;			//按键按下的时间计数器
	uint16_t WaitDoubleCount = 0;		//等待第二次按键按下计数器
	uint8_t  ButtonType;				
	
	while(!GPIO_ReadInputDataBit(ButtonX_PORT,ButtonX_PIN))
	{
		PresTimeCount++;
		vTaskDelay(Button_SampleTime);			//delay(5ms)
	}//第一次按下
	
	
	if(PresTimeCount > SHORT_PRESS_NUM)
	{
		if(PresTimeCount > LONG_PRESS_NUM)		//长按
			ButtonType =  BUTTON_X_LONG;
		else
		{
			//判断单双击
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
 * 函数名：KEY_Scan()
 * 输入：void
 * 输出：uint ButtonType:	按下按键的类型
 * 功能：返回按键类型
 */
uint8_t KEY_Scan(void)
{
	uint8_t ButtonType;
	ButtonType = IsButtonPressed();		//获取是否按下按键
	
	switch(ButtonType)
	{
		case BUTTON_A_PRESSED:ButtonType = (GetPressType(BUTTON_A_GPIO_PORT,BUTTON_A_GPIO_PIN)+BUTTON_A_TYPE_ADRRESS);break;	//获取A键按下的类型
		case BUTTON_B_PRESSED:ButtonType = (GetPressType(BUTTON_B_GPIO_PORT,BUTTON_B_GPIO_PIN)+BUTTON_B_TYPE_ADRRESS);break;	//获取B键按下的类型
		default:ButtonType = NO_BUTTON_PRESSED;break;								//无键按下
		
	}
	return ButtonType;					//返回按键类型
}






