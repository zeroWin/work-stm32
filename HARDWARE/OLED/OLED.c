#include	"OLED.h"
#include	"oledfont.h"
#include 	"FreeRTOS.h"
#include 	"task.h"
//////////////////////////////////////////////////////////////////////////////////

//V1.0 	��Ϊ��ʼ������OLED_Init()ʹ����vTaskDelay()���Գ�ʼ������OLED_Init()
//		�����������е��ã�������������е��ã��ᷢ������

//////////////////////////////////////////////////////////////////////////////////	


//OLED���Դ�
//��Ÿ�ʽ���£��ֱ���128*32
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	 

uint8_t OLED_GRAM[128][4];

//=============== ����ʵ�� =====================
//I2C��غ���,�ڲ�����
/*
 * ��������I2C_Start()
 * ���룺void
 * �����void
 * ���ܣ�I2C�����ź�
 *			 ����SDA��SCL����ʲô״̬�������������ȷ�Ĳ�����ʼ״̬
 */
void I2C_Start(void)
{
	SDA_H;
	SCL_H;			//SDA,SCL��ʼ��Ϊ��
	 
	SDA_L;			//����SDA��ʾ����
	SCL_L;			//����SCL��ǶסI2C���ߣ�׼�����ͻ��������
}

/*
 * ��������I2C_stop()
 * ���룺void
 * �����void
 * ���ܣ�I2C�����ź�
 *			 ����SDA��SCL����ʲô״̬�������������ȷ����״̬
 */
void I2C_Stop(void)
{
	SDA_L;		//����SDA�ĵ�ƽ
	SCL_H;		//SCLΪ�ߵ�ƽ
	
	SDA_H;		//SDA���߱�ʾ����
}

/*
 * ��������I2C_Ack()
 * ���룺void
 * �����void
 * ���ܣ�����Ӧ���ź�
 */
void I2C_Ack(void)
{
	SDA_L;		//����SDA
	
	SCL_H;		//����SCL
	SCL_L;		//����SCL
	
}

/*
 * ��������I2C_WriteBit()
 * ���룺8bit����
 * �����void
 * ���ܣ�д������
 * 			 �ȷ���λ���ٷ���λ
 */
void I2C_WriteBit(uint8_t WriteData)
{
	uint8_t	length = 8;
	
	SCL_L;		//��֤SCL��ƽΪ��
	
	while(length--)
	{
		if(WriteData & 0x80)
			SDA_H;	//д��ߵ�ƽ
		else
			SDA_L;	//д��͵�ƽ
		
		SCL_H;		//����SCLд�����ݵ�ROM
		
		SCL_L;		//����SCL�޸��´�SDA����
		
		WriteData = WriteData << 1;	//��������λ
	}
}

/*
 * ��������OLED_Init()
 * ���룺void
 * �����void
 * ���ܣ���OLED���г�ʼ������
 */
void OLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//����GPIOA������ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB,ENABLE);
	
	//����SDA��GPIO
	GPIO_InitStructure.GPIO_Pin = OLED_SDA_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
	GPIO_Init(OLED_GPIO_SDA_PORT,&GPIO_InitStructure);
	
	//����SCL��GPIO
	GPIO_InitStructure.GPIO_Pin = OLED_SCL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
	GPIO_Init(OLED_GPIO_SCL_PORT,&GPIO_InitStructure);
	
	//����RST��GPIO
	GPIO_InitStructure.GPIO_Pin = OLED_RST_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
	GPIO_Init(OLED_GPIO_RST_PORT,&GPIO_InitStructure);
	
	//���������˿�����ߵ�ƽ
	RST_H;
	SDA_H; 
	SCL_H; 
	
	
	RST_L;					//��λ
	vTaskDelay(OLED_RESET_WAIT_TIME);//��ʱ10ms
	RST_H;

	//�����ֲ�����
	OLED_WR_Byte(0xAE,OLED_CMD);//�ر���ʾ
	
	OLED_WR_Byte(0xD5,OLED_CMD);//����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(0x80,OLED_CMD);  //[3:0],��Ƶ����;[7:4],��Ƶ��
	
	OLED_WR_Byte(0xA8,OLED_CMD);//��������·��
	OLED_WR_Byte(0X1F,OLED_CMD);//Ĭ��0X1F(1/32) 
	
	OLED_WR_Byte(0xD3,OLED_CMD);//������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD);//Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD);//������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD);//bit2������/�ر�
	
	OLED_WR_Byte(0xA1,OLED_CMD);//���ض�������,bit0:0,0->0;1,0->127;
	
	OLED_WR_Byte(0xC8,OLED_CMD);//����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	
	OLED_WR_Byte(0xDA,OLED_CMD);//����COMӲ����������
	OLED_WR_Byte(0x02,OLED_CMD);//[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD);//�Աȶ�����
	OLED_WR_Byte(0x8F,OLED_CMD);//1~255;Ĭ��0X7F (��������,Խ��Խ��)
	
	OLED_WR_Byte(0xD9,OLED_CMD);//����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD);//[3:0],PHASE 1;[7:4],PHASE 2;
	
	OLED_WR_Byte(0xDB,OLED_CMD);//����VCOMH ��ѹ����
	OLED_WR_Byte(0x40,OLED_CMD);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	
	OLED_WR_Byte(0xA4,OLED_CMD);//ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD);//������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
 

	//�趨д��ģʽ����ģʽ����Լд���ʱ��
	OLED_WR_Byte(0x20,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
 
	OLED_WR_Byte(0x21,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
	OLED_WR_Byte(0x7f,OLED_CMD);
 
	OLED_WR_Byte(0x22,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
	OLED_WR_Byte(0x03,OLED_CMD);
	
	OLED_WR_Byte(0xAF,OLED_CMD);//������ʾ	
	OLED_Clear();

}


/*
 * ��������OLED_WR_Byte()
 * ���룺data:Ҫд�������/����
 *		 cmd:����/�����־ 0,��ʾ����;1,��ʾ����; 
 * �����void
 * ���ܣ���SSD1306д��һ���ֽڡ�
 */
void OLED_WR_Byte(uint8_t data,uint8_t cmd)
{
	//I2C�����ź�
	I2C_Start();
	
	//���͵�ַλ 0111 1000 = 0x78;
	I2C_WriteBit(0x78);
	I2C_Ack();
	
	if(cmd == OLED_CMD)	//�����ź� 0000 0000 = 0x00
		I2C_WriteBit(0x00);
	else				//��ͨ���� 0100 0000 = 0x40
		I2C_WriteBit(0x40);
	I2C_Ack();
	
	//��������
	I2C_WriteBit(data);
	I2C_Ack();
	
	
	//I2Cֹͣ�ź�
	I2C_Stop();
	
}

/*
 * ��������OLED_Display_On()
 * ���룺void
 * �����void
 * ���ܣ�����OLED��ʾ
 */

void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/*
 * ��������OLED_Display_Off()
 * ���룺void
 * �����void
 * ���ܣ��ر�OLED��ʾ
 */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		

/*
 * ��������OLED_Clear()
 * ���룺void
 * �����void
 * ���ܣ���������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
 */
void OLED_Clear(void)  
{  
	uint8_t i,n;  
	for(i=0;i<4;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}

/*
 * ��������OLED_Refresh_Gram()
 * ���룺void
 * �����void
 * ���ܣ������Դ浽LCD		  
 */	 
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;
	
	
	//I2C�����ź�
	I2C_Start();
	//���͵�ַλ 0111 1000 = 0x78;
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
	//I2Cֹͣ�ź�
	I2C_Stop();
} 


/*
 * ��������OLED_DrawPoint()
 * ���룺
		x:�����������
		y:������������
		t:������ѡ�����
 * �����void
 * ���ܣ����� 
		 x:
		 y:0~63
		 t:1 ��� 0,���	  
 */	 
				   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>31)return;//������Χ��.
	pos=y/8;			//������ʾ�޸�		
	bx=y%8;				
	temp=1<< (bx);		//������ʾ����޸�
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

/*
 * ��������OLED_Fill()
 * ���룺������˵��
 * �����void
 * ���ܣ�  x1,y1,x2,y2 �������ĶԽ�����
		  ȷ��x1<=x2;y1<=yl2 0<=x1<=127 0<=y1<=31	 	 
		  dot:0,���;1,���	  
 */	   
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)
{
	uint8_t x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}

/*
 * ��������OLED_ShowChar()
 * ���룺������˵��
 * �����void
 * ���ܣ���ָ��λ����ʾһ���ַ�,���������ַ�
		 x:0~127
		 y:0~31	
		 mode:0,������ʾ;1,������ʾ				 
		 size:ѡ������ 16/12  
 */	 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	uint8_t y0=y;
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=oled_asc2_1206[chr][t];  //����1206����
		else temp=oled_asc2_1608[chr][t];		 //����1608���� 	                          
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
 * ��������oled_pows()
 * ���룺
		m:��
		n:��
 * �����void
 * ���ܣ�m^n
 */	 

uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}	

/*
 * ��������OLED_ShowNum()
 * ���룺��˵��
 * �����void
 * ���ܣ���ʾ2������
		 x,y :�������	 
		 len :���ֵ�λ��
		 size:�����С
		 mode:ģʽ	0,���ģʽ;1,����ģʽ
		 num:��ֵ(0~4294967295)
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
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);	//���һ������Ϊmode
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 		//���һ������Ϊmode
	}
} 

/*
 * ��������OLED_ShowString()
 * ���룺��˵��
 * �����void
 * ���ܣ���ʾ�ַ���
		 x,y:�������  
		 *p:�ַ�����ʼ��ַ
		 ��16����
 */	 
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 26	//58          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}				//����
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,16,1);	 
        x+=8;
        p++;
    }  
}	

