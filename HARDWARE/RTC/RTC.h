#ifndef	__RTC_H
#define	__RTC_H

//////////////////////////////////////////////////////////////////////////////////

//����˵��

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------ͷ�ļ�--------------------------
#include	"stdint.h"
#include	"stm32l1xx_conf.h"

//---------------------------ʱ��ṹ��--------------------------
typedef struct 
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;			
	//������������
	uint16_t w_year;		
	uint8_t  w_month;
	uint8_t  w_date;
	uint8_t  week;		 
}_calendar_obj;

//---------------------------ȫ�ֱ�������--------------------------
extern _calendar_obj calendar;	//�����ṹ��

//---------------------------ʱ��궨��--------------------------
#define RTC_CLOCK_INIT_WAIT_TIME	 	(10 / portTICK_RATE_MS)		//10ms



//RTC���ƺ���
uint8_t RTC_Config_Init(void);
void RTC_Set_WakeUp(uint32_t WakeUpClockSel,uint16_t cnt);
void RTC_Get(void);

ErrorStatus RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm);
ErrorStatus RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week);


#endif
