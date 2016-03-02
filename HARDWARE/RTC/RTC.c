#include	"RTC.h"
#include	"OLED.h"
#include 	"FreeRTOS.h"
#include 	"task.h"
//////////////////////////////////////////////////////////////////////////////////

//V1.0 	��Ϊ��ʼ������RTC_Config_Init()ʹ����vTaskDelay()���Գ�ʼ������RTC_Config_Init()
//		�����������е��ã�������������е��ã��ᷢ������

//////////////////////////////////////////////////////////////////////////////////	

//ʱ�ӽṹ�� 
_calendar_obj calendar;

//=============== ����ʵ�� =====================
/*
 * ��������RTC_Config_Init()
 * ���룺void
 * �����uint8_t
 * ���ܣ���RTC���г�ʼ������
 *		����0���ɹ�������1��ʧ��
 */
uint8_t RTC_Config_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	uint8_t temp = 0;	//���������ȴ�LSE���úõ�ʱ��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	PWR_RTCAccessCmd(ENABLE);//ʹ��RTC�󱸼Ĵ�������
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x5050)	//�ж��Ƿ�Ϊ��һ������
	{
		RCC_LSEConfig(RCC_LSE_ON);	////LSE ����  
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && temp < 250)//���ָ����RCC��־λ�������,�ȴ����پ������
		{
			temp++;
			vTaskDelay(RTC_CLOCK_INIT_WAIT_TIME);//��ʱ10ms�����ȴ�2.5s
		}
		if(temp >= 250) return 1;//��ʼ��ʱ��ʧ��,����������	
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��   
		RCC_RTCCLKCmd(ENABLE);//ʹ��RTCʱ�� 
		RTC_WaitForSynchro();//�ȴ�ʱ��ͬ��
		
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC�첽��Ƶϵ��(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTCͬ����Ƶϵ��(0~7FFF) ��Ƶʱ��Ϊ1hz
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC����Ϊ,24Сʱ��ʽ
		RTC_Init(&RTC_InitStructure);
		
		RTC_Set_Time(23,59,0,RTC_H12_AM);	//����ʱ��
		RTC_Set_Date(15,4,12,7);		//��������
	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//����Ѿ���ʼ������
	}
	
	return 0;
}

/*
 * ��������RTC_Set_Time()
 * ���룺uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm
 * hour,min,sec:Сʱ,����,����
 * ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
 * �����uint8_t �ɹ���SUCEE(1) �����ʼ��ģʽʧ�� ��ERROR(0)
 * ���ܣ�RTCʱ������
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
 * ��������RTC_Set_Date()
 * ���룺uint8_t year,uint8_t month,uint8_t date,uint8_t week
 * year,month,date:��(0~99),��(1~12),��(0~31)
 * week:����(1~7,0,�Ƿ�!)
 * �����uint8_t �ɹ���SUCEE(1) �����ʼ��ģʽʧ�� ��ERROR(0)
 * ���ܣ�RTC��������
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
 * ��������RTC_Set_WakeUp()
 * ���룺uint32_t WakeUpClockSel,uint16_t cnt
 * WakeUpClockSel:  @ref RTC_Wakeup_Timer_Definitions
 * #define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
 * #define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
 * #define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
 * #define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
 * #define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)	//ѡ��1hz��Ϊʱ�����룬����ʱ��1s��18h
 * #define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)	//ѡ��1hz��Ϊʱ�����룬����ʱ��18h��36h
 *
 * cnt:�Զ���װ��ֵ.����0,�����ж�.
 * �����void
 * ���ܣ������Ի��Ѷ�ʱ������  
 */

void RTC_Set_WakeUp(uint32_t WakeUpClockSel,uint16_t cnt)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);//�ر�WAKE UP
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//����ʱ��ѡ��
	RTC_SetWakeUpCounter(cnt);//����WAKE UP�Զ���װ�ؼĴ���
	
	RTC_ClearITPendingBit(RTC_IT_WUT);//���RTC WAKE UP�ı�־
	EXTI_ClearITPendingBit(EXTI_Line20);//���LINE20�ϵ��жϱ�־λ 
	
	RTC_ITConfig(RTC_IT_WUT,ENABLE);//����WAKE UP��ʱ���ж�
	RTC_WakeUpCmd(ENABLE);//����WAKE UP ��ʱ����
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line20;//LINE20
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE20
	EXTI_Init(&EXTI_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�0,���ȼ���Ϊ���
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
 
}


/*
 * �������� RTC_Get()
 * ���룺void
 * �����void
 * ���ܣ�����ʱ�䣬��ϵͳʱ����µ����õĽṹ����  
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

//RTC WAKE UP�жϷ�����
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP�ж�?
	{
		RTC_Get();
		ZtoB();
		OLED_Refresh_Gram();
		RTC_ClearFlag(RTC_FLAG_WUTF);	//����жϱ�־
	}
	EXTI_ClearITPendingBit(EXTI_Line20);//����ж���20���жϱ�־ 								
}
//����ɾ��
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


	OLED_ShowNum(TIME_BASR_WAKEUP_1,16,calendar.hour,2,16);//��ʾASCII�ַ�����ֵ
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
	
	//����
	uint8_t TIME_BASR_WAKEUP_1 = TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*12;
	uint8_t TIME_BASR_WAKEUP_BASE_1 = 6;
	uint8_t TIME_BASR_WAKEUP_SIZE_1 = 12;
	//������
	OLED_ShowNum(TIME_BASR_WAKEUP,0,20,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*2,0,calendar.w_year,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*4,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*5,0,calendar.w_month,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*7,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*8,0,calendar.w_date,2,TIME_BASR_WAKEUP_SIZE);


	OLED_ShowNum(TIME_BASR_WAKEUP_1,0,calendar.hour,2,TIME_BASR_WAKEUP_SIZE_1);//��ʾASCII�ַ�����ֵ
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*2,0,':',TIME_BASR_WAKEUP_SIZE_1,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*3,0,calendar.min,2,TIME_BASR_WAKEUP_SIZE_1);
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*5,0,':',TIME_BASR_WAKEUP_SIZE_1,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*6,0,calendar.sec,2,TIME_BASR_WAKEUP_SIZE_1);
	
	
	//�ı�����
	OLED_ShowString(0,16,"1.SDcard");
	OLED_ShowString(88,16,"2.Set");
}

void setjiemian(void)
{
	uint8_t TIME_BASR_WAKEUP = 0;
	uint8_t TIME_BASR_WAKEUP_BASE = 6;
	uint8_t TIME_BASR_WAKEUP_SIZE = 12;
	
	//����
	uint8_t TIME_BASR_WAKEUP_1 = TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*12;
	uint8_t TIME_BASR_WAKEUP_BASE_1 = 6;
	uint8_t TIME_BASR_WAKEUP_SIZE_1 = 12;
	//������
	OLED_ShowNum(TIME_BASR_WAKEUP,0,20,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*2,0,calendar.w_year,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*4,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*5,0,calendar.w_month,2,TIME_BASR_WAKEUP_SIZE);
	OLED_ShowChar(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*7,0,'/',TIME_BASR_WAKEUP_SIZE,1);
	OLED_ShowNum(TIME_BASR_WAKEUP+TIME_BASR_WAKEUP_BASE*8,0,calendar.w_date,2,TIME_BASR_WAKEUP_SIZE);


	OLED_ShowNum(TIME_BASR_WAKEUP_1,0,calendar.hour,2,TIME_BASR_WAKEUP_SIZE_1);//��ʾASCII�ַ�����ֵ
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*2,0,':',TIME_BASR_WAKEUP_SIZE_1,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*3,0,calendar.min,2,TIME_BASR_WAKEUP_SIZE_1);
	OLED_ShowChar(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*5,0,':',TIME_BASR_WAKEUP_SIZE_1,1);
	OLED_ShowNum(TIME_BASR_WAKEUP_1+TIME_BASR_WAKEUP_BASE_1*6,0,calendar.sec,2,TIME_BASR_WAKEUP_SIZE_1);
	
	
	//�ı�����
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
