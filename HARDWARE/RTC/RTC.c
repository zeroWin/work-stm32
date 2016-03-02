#include	"RTC.h"
#include	"OLED.h"
#include 	"FreeRTOS.h"
#include 	"task.h"
//////////////////////////////////////////////////////////////////////////////////

//V1.0 	因为初始化函数RTC_Config_Init()使用了vTaskDelay()所以初始化函数RTC_Config_Init()
//		必须在任务中调用，如果不在任务中调用，会发生错误

//////////////////////////////////////////////////////////////////////////////////	

//时钟结构体 
_calendar_obj calendar;

//=============== 函数实现 =====================
/*
 * 函数名：RTC_Config_Init()
 * 输入：void
 * 输出：uint8_t
 * 功能：对RTC进行初始化设置
 *		返回0：成功；返回1：失败
 */
uint8_t RTC_Config_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	uint8_t temp = 0;	//用来计数等待LSE设置好的时间
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_RTCAccessCmd(ENABLE);//使能RTC后备寄存器访问
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x5050)	//判断是否为第一次设置
	{
		RCC_LSEConfig(RCC_LSE_ON);	////LSE 开启  
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && temp < 250)//检查指定的RCC标志位设置与否,等待低速晶振就绪
		{
			temp++;
			vTaskDelay(RTC_CLOCK_INIT_WAIT_TIME);//延时10ms，共等待2.5s
		}
		if(temp >= 250) return 1;//初始化时钟失败,晶振有问题	
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟   
		RCC_RTCCLKCmd(ENABLE);//使能RTC时钟 
		RTC_WaitForSynchro();//等待时钟同步
		
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF) 分频时钟为1hz
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
		RTC_Init(&RTC_InitStructure);
		
		RTC_Set_Time(23,59,0,RTC_H12_AM);	//设置时间
		RTC_Set_Date(15,4,12,7);		//设置日期
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	}
	
	return 0;
}

/*
 * 函数名：RTC_Set_Time()
 * 输入：uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm
 * hour,min,sec:小时,分钟,秒钟
 * ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
 * 输出：uint8_t 成功：SUCEE(1) 进入初始化模式失败 ：ERROR(0)
 * 功能：RTC时间设置
 */
ErrorStatus RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	
}

/*
 * 函数名：RTC_Set_Date()
 * 输入：uint8_t year,uint8_t month,uint8_t date,uint8_t week
 * year,month,date:年(0~99),月(1~12),日(0~31)
 * week:星期(1~7,0,非法!)
 * 输出：uint8_t 成功：SUCEE(1) 进入初始化模式失败 ：ERROR(0)
 * 功能：RTC日期设置
 */
ErrorStatus RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}


/*
 * 函数名：RTC_Set_WakeUp()
 * 输入：uint32_t WakeUpClockSel,uint16_t cnt
 * WakeUpClockSel:  @ref RTC_Wakeup_Timer_Definitions
 * #define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
 * #define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
 * #define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
 * #define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
 * #define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)	//选择1hz作为时钟输入，唤醒时间1s到18h
 * #define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)	//选择1hz作为时钟输入，唤醒时间18h到36h
 *
 * cnt:自动重装载值.减到0,产生中断.
 * 输出：void
 * 功能：周期性唤醒定时器设置  
 */

void RTC_Set_WakeUp(uint32_t WakeUpClockSel,uint16_t cnt)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);//关闭WAKE UP
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//唤醒时钟选择
	RTC_SetWakeUpCounter(cnt);//设置WAKE UP自动重装载寄存器
	
	RTC_ClearITPendingBit(RTC_IT_WUT);//清除RTC WAKE UP的标志
	EXTI_ClearITPendingBit(EXTI_Line20);//清除LINE20上的中断标志位 
	
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//开启WAKE UP定时器中断
	RTC_WakeUpCmd(ENABLE);//开启WAKE UP 定时器　
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line20;//LINE20
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE20
	EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级0,优先级设为最高
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
 
}


/*
 * 函数名： RTC_Get()
 * 输入：void
 * 输出：void
 * 功能：更新时间，将系统时间更新到设置的结构体中  
 */
void RTC_Get(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	
	calendar.hour = RTC_TimeStruct.RTC_Hours;
	calendar.min = RTC_TimeStruct.RTC_Minutes;
	calendar.sec = RTC_TimeStruct.RTC_Seconds;
	calendar.w_year = RTC_DateStruct.RTC_Year;
	calendar.w_month = RTC_DateStruct.RTC_Month;
	calendar.w_date = RTC_DateStruct.RTC_Date;
	calendar.week = RTC_DateStruct.RTC_WeekDay;

}


void shouye(void);
void zhujiemian(void);
void setjiemian(void);
void ZtoB(void);

//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP中断?
	{
		RTC_Get();
		ZtoB();
		OLED_Refresh_Gram();
		RTC_ClearFlag(RTC_FLAG_WUTF);	//清除中断标志
	}
	EXTI_ClearITPendingBit(EXTI_Line20);//清除中断线20的中断标志 								
}
//以下删除
void shouye(void)
{
	uint8_t TIME_BASR_WAKEUP = 35;
	uint8_t TIME_BASR_WAKEUP_BASE = 6;
	uint8_t TIME_BASR_WAKEUP_SIZE = 12;
	
	uint8_t TIME_BASR_WAKEUP_1 = 15;
	uint8_t TIME_BASR_WAKEUP_BASE_1 = 8;
	//uint8_t TIME_BASR_WAKEUP_SIZE_1 = 16;
	OLED_ShowNum(TIME_BASR_WAKEUP,0,20,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*2,0,calendar.w_year,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*4,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*5,0,calendar.w_month,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*7,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*8,0,calendar.w_date,2,TIME_BASR_WAKEUP_SIZE);


	OLED_ShowNum(TIME_BASR_WAKEUP_1,16,calendar.hour,2,16);//显示ASCII字符的码值
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*2,16,' ',16,1);
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*3,16,':',16,1);
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*4,16,' ',16,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*5,16,calendar.min,2,16);
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*7,16,' ',16,1);
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*8,16,':',16,1);
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*9,16,' ',16,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*10,16,calendar.sec,2,16);
}

void zhujiemian(void)
{
	uint8_t TIME_BASR_WAKEUP = 0;
	uint8_t TIME_BASR_WAKEUP_BASE = 6;
	uint8_t TIME_BASR_WAKEUP_SIZE = 12;
	
	//日期
	uint8_t TIME_BASR_WAKEUP_1 = TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*12;
	uint8_t TIME_BASR_WAKEUP_BASE_1 = 6;
	uint8_t TIME_BASR_WAKEUP_SIZE_1 = 12;
	//年月日
	OLED_ShowNum(TIME_BASR_WAKEUP,0,20,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*2,0,calendar.w_year,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*4,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*5,0,calendar.w_month,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*7,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*8,0,calendar.w_date,2,TIME_BASR_WAKEUP_SIZE);


	OLED_ShowNum(TIME_BASR_WAKEUP_1,0,calendar.hour,2,TIME_BASR_WAKEUP_SIZE_1);//显示ASCII字符的码值
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*2,0,':',TIME_BASR_WAKEUP_SIZE_1,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*3,0,calendar.min,2,TIME_BASR_WAKEUP_SIZE_1);
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*5,0,':',TIME_BASR_WAKEUP_SIZE_1,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*6,0,calendar.sec,2,TIME_BASR_WAKEUP_SIZE_1);
	
	
	//文本界面
	OLED_ShowString(0,16,"1.SDcard");
	OLED_ShowString(88,16,"2.Set");
}

void setjiemian(void)
{
	uint8_t TIME_BASR_WAKEUP = 0;
	uint8_t TIME_BASR_WAKEUP_BASE = 6;
	uint8_t TIME_BASR_WAKEUP_SIZE = 12;
	
	//日期
	uint8_t TIME_BASR_WAKEUP_1 = TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*12;
	uint8_t TIME_BASR_WAKEUP_BASE_1 = 6;
	uint8_t TIME_BASR_WAKEUP_SIZE_1 = 12;
	//年月日
	OLED_ShowNum(TIME_BASR_WAKEUP,0,20,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*2,0,calendar.w_year,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*4,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*5,0,calendar.w_month,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*7,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*8,0,calendar.w_date,2,TIME_BASR_WAKEUP_SIZE);


	OLED_ShowNum(TIME_BASR_WAKEUP_1,0,calendar.hour,2,TIME_BASR_WAKEUP_SIZE_1);//显示ASCII字符的码值
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*2,0,':',TIME_BASR_WAKEUP_SIZE_1,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*3,0,calendar.min,2,TIME_BASR_WAKEUP_SIZE_1);
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*5,0,':',TIME_BASR_WAKEUP_SIZE_1,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*6,0,calendar.sec,2,TIME_BASR_WAKEUP_SIZE_1);
	
	
	//文本界面
	OLED_ShowString(0,16,"1.Time");
	OLED_ShowString(56,16,"2.B");
	OLED_ShowString(88,16,"3.Z");
}

void ZtoB(void)
{
	uint8_t TIME_BASR_WAKEUP = 32;
	uint8_t TIME_BASR_WAKEUP_BASE = 8;
	OLED_ShowString(TIME_BASR_WAKEUP,8,"Z");
	OLED_ShowString(TIME_BASR_WAKEUP + TIME_BASR_WAKEUP_BASE*3,8,"--->");
	OLED_ShowString(TIME_BASR_WAKEUP + TIME_BASR_WAKEUP_BASE*9,8,"B");
}
