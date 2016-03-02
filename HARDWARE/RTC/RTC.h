#ifndef	__RTC_H
#define	__RTC_H

//////////////////////////////////////////////////////////////////////////////////

//待填说明

//////////////////////////////////////////////////////////////////////////////////	

//---------------------------头文件--------------------------
#include	"stdint.h"
#include	"stm32l1xx_conf.h"

//---------------------------时间结构体--------------------------
typedef struct 
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;			
	//公历日月年周
	uint16_t w_year;		
	uint8_t  w_month;
	uint8_t  w_date;
	uint8_t  week;		 
}_calendar_obj;

//---------------------------全局变量声明--------------------------
extern _calendar_obj calendar;	//日历结构体

//---------------------------时间宏定义--------------------------
#define RTC_CLOCK_INIT_WAIT_TIME	 	(10 / portTICK_RATE_MS)		//10ms



//RTC控制函数
uint8_t RTC_Config_Init(void);
void RTC_Set_WakeUp(uint32_t WakeUpClockSel,uint16_t cnt);
void RTC_Get(void);

ErrorStatus RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm);
ErrorStatus RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week);


#endif
