#include	"OLED.h"
#include	"oledfont.h"
#include 	"FreeRTOS.h"
#include 	"task.h"
//////////////////////////////////////////////////////////////////////////////////

//V1.0 	因为初始化函数OLED_Init()使用了vTaskDelay()所以初始化函数OLED_Init()
//		必须在任务中调用，如果不在任务中调用，会发生错误

//////////////////////////////////////////////////////////////////////////////////	


//OLED的显存
//存放格式如下，分辨率128*32
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	 

uint8_t OLED_GRAM[128][4];

//=============== 函数实现 =====================
//I2C相关函数,内部调用
/*
 * 函数名：I2C_Start()
 * 输入：void
 * 输出：void
 * 功能：I2C启动信号
 *			 无论SDA、SCL处于什么状态，本程序均能正确的产生开始状态
 */
void I2C_Start(void)
{
	SDA_H;
	SCL_H;			//SDA,SCL初始化为高
	 
	SDA_L;			//拉低SDA表示启动
	SCL_L;			//拉低SCL，嵌住I2C总线，准备发送或接受数据
}

/*
 * 函数名：I2C_stop()
 * 输入：void
 * 输出：void
 * 功能：I2C结束信号
 *			 无论SDA、SCL处于什么状态，本程序均能正确结束状态
 */
void I2C_Stop(void)
{
	SDA_L;		//拉低SDA的电平
	SCL_H;		//SCL为高电平
	
	SDA_H;		//SDA拉高表示结束
}

/*
 * 函数名：I2C_Ack()
 * 输入：void
 * 输出：void
 * 功能：发送应答信号
 */
void I2C_Ack(void)
{
	SDA_L;		//拉低SDA
	
	SCL_H;		//拉高SCL
	SCL_L;		//拉低SCL
	
}

/*
 * 函数名：I2C_WriteBit()
 * 输入：8bit数据
 * 输出：void
 * 功能：写入数据
 * 			 先发高位，再发低位
 */
void I2C_WriteBit(uint8_t WriteData)
{
	uint8_t	length = 8;
	
	SCL_L;		//保证SCL电平为低
	
	while(length--)
	{
		if(WriteData & 0x80)
			SDA_H;	//写入高电平
		else
			SDA_L;	//写入低电平
		
		SCL_H;		//拉高SCL写入数据到ROM
		
		SCL_L;		//拉低SCL修改下次SDA数据
		
		WriteData = WriteData << 1;	//数据左移位
	}
}

/*
 * 函数名：OLED_Init()
 * 输入：void
 * 输出：void
 * 功能：对OLED进行初始化设置
 */
void OLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//开启GPIOA的外设时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB,ENABLE);
	
	//配置SDA的GPIO
	GPIO_InitStructure.GPIO_Pin = OLED_SDA_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	//推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
	GPIO_Init(OLED_GPIO_SDA_PORT,&GPIO_InitStructure);
	
	//配置SCL的GPIO
	GPIO_InitStructure.GPIO_Pin = OLED_SCL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	//推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
	GPIO_Init(OLED_GPIO_SCL_PORT,&GPIO_InitStructure);
	
	//配置RST的GPIO
	GPIO_InitStructure.GPIO_Pin = OLED_RST_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	//推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
	GPIO_Init(OLED_GPIO_RST_PORT,&GPIO_InitStructure);
	
	//设置三个端口输出高电平
	RST_H;
	SDA_H; 
	SCL_H; 
	
	
	RST_L;					//复位
	vTaskDelay(OLED_RESET_WAIT_TIME);//延时10ms
	RST_H;

	//按照手册配置
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭显示
	
	OLED_WR_Byte(0xD5,OLED_CMD);//设置时钟分频因子,震荡频率
	OLED_WR_Byte(0x80,OLED_CMD);  //[3:0],分频因子;[7:4],震荡频率
	
	OLED_WR_Byte(0xA8,OLED_CMD);//设置驱动路数
	OLED_WR_Byte(0X1F,OLED_CMD);//默认0X1F(1/32) 
	
	OLED_WR_Byte(0xD3,OLED_CMD);//设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD);//默认为0

	OLED_WR_Byte(0x40,OLED_CMD);//设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD);//bit2，开启/关闭
	
	OLED_WR_Byte(0xA1,OLED_CMD);//段重定义设置,bit0:0,0->0;1,0->127;
	
	OLED_WR_Byte(0xC8,OLED_CMD);//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	
	OLED_WR_Byte(0xDA,OLED_CMD);//设置COM硬件引脚配置
	OLED_WR_Byte(0x02,OLED_CMD);//[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD);//对比度设置
	OLED_WR_Byte(0x8F,OLED_CMD);//1~255;默认0X7F (亮度设置,越大越亮)
	
	OLED_WR_Byte(0xD9,OLED_CMD);//设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD);//[3:0],PHASE 1;[7:4],PHASE 2;
	
	OLED_WR_Byte(0xDB,OLED_CMD);//设置VCOMH 电压倍率
	OLED_WR_Byte(0x40,OLED_CMD);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	
	OLED_WR_Byte(0xA4,OLED_CMD);//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD);//设置显示方式;bit0:1,反相显示;0,正常显示	    						   
 

	//设定写入模式，行模式，节约写入的时间
	OLED_WR_Byte(0x20,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
 
	OLED_WR_Byte(0x21,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
	OLED_WR_Byte(0x7f,OLED_CMD);
 
	OLED_WR_Byte(0x22,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
	OLED_WR_Byte(0x03,OLED_CMD);
	
	OLED_WR_Byte(0xAF,OLED_CMD);//开启显示	
	OLED_Clear();

}


/*
 * 函数名：OLED_WR_Byte()
 * 输入：data:要写入的数据/命令
 *		 cmd:数据/命令标志 0,表示命令;1,表示数据; 
 * 输出：void
 * 功能：向SSD1306写入一个字节。
 */
void OLED_WR_Byte(uint8_t data,uint8_t cmd)
{
	//I2C启动信号
	I2C_Start();
	
	//发送地址位 0111 1000 = 0x78;
	I2C_WriteBit(0x78);
	I2C_Ack();
	
	if(cmd == OLED_CMD)	//控制信号 0000 0000 = 0x00
		I2C_WriteBit(0x00);
	else				//普通数据 0100 0000 = 0x40
		I2C_WriteBit(0x40);
	I2C_Ack();
	
	//发送数据
	I2C_WriteBit(data);
	I2C_Ack();
	
	
	//I2C停止信号
	I2C_Stop();
	
}

/*
 * 函数名：OLED_Display_On()
 * 输入：void
 * 输出：void
 * 功能：开启OLED显示
 */

void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/*
 * 函数名：OLED_Display_Off()
 * 输入：void
 * 输出：void
 * 功能：关闭OLED显示
 */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		

/*
 * 函数名：OLED_Clear()
 * 输入：void
 * 输出：void
 * 功能：清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
 */
void OLED_Clear(void)  
{  
	uint8_t i,n;  
	for(i=0;i<4;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}

/*
 * 函数名：OLED_Refresh_Gram()
 * 输入：void
 * 输出：void
 * 功能：更新显存到LCD		  
 */	 
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;
	
	
	//I2C启动信号
	I2C_Start();
	//发送地址位 0111 1000 = 0x78;
	I2C_WriteBit(0x78);
	I2C_Ack();	
	
	I2C_WriteBit(0x40);
	I2C_Ack();
	
	
	for(i=0;i<4;i++)  
	{  
		for(n=0;n<128;n++)
		{
			I2C_WriteBit(OLED_GRAM[n][i]);
			I2C_Ack();
		}
	}   
	//I2C停止信号
	I2C_Stop();
} 


/*
 * 函数名：OLED_DrawPoint()
 * 输入：
		x:所画点横坐标
		y:所画点纵坐标
		t:填充清空选择控制
 * 输出：void
 * 功能：画点 
		 x:
		 y:0~63
		 t:1 填充 0,清空	  
 */	 
				   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>31)return;//超出范围了.
	pos=y/8;			//根据显示修改		
	bx=y%8;				
	temp=1<< (bx);		//根据显示结果修改
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

/*
 * 函数名：OLED_Fill()
 * 输入：见功能说明
 * 输出：void
 * 功能：  x1,y1,x2,y2 填充区域的对角坐标
		  确保x1<=x2;y1<=yl2 0<=x1<=127 0<=y1<=31	 	 
		  dot:0,清空;1,填充	  
 */	   
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)
{
	uint8_t x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//更新显示
}

/*
 * 函数名：OLED_ShowChar()
 * 输入：见功能说明
 * 输出：void
 * 功能：在指定位置显示一个字符,包括部分字符
		 x:0~127
		 y:0~31	
		 mode:0,反白显示;1,正常显示				 
		 size:选择字体 16/12  
 */	 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	uint8_t y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=oled_asc2_1206[chr][t];  //调用1206字体
		else temp=oled_asc2_1608[chr][t];		 //调用1608字体 	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}


/*
 * 函数名：oled_pows()
 * 输入：
		m:底
		n:幂
 * 输出：void
 * 功能：m^n
 */	 

uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}	

/*
 * 函数名：OLED_ShowNum()
 * 输入：见说明
 * 输出：void
 * 功能：显示2个数字
		 x,y :起点坐标	 
		 len :数字的位数
		 size:字体大小
		 mode:模式	0,填充模式;1,叠加模式
		 num:数值(0~4294967295)
 */	 
;	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);	//最后一个参数为mode
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 		//最后一个参数为mode
	}
} 

/*
 * 函数名：OLED_ShowString()
 * 输入：见说明
 * 输出：void
 * 功能：显示字符串
		 x,y:起点坐标  
		 *p:字符串起始地址
		 用16字体
 */	 
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 26	//58          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}				//换行
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,16,1);	 
        x+=8;
        p++;
    }  
}	

