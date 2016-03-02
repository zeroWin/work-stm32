#include	"SPI.h"

//////////////////////////////////////////////////////////////////////////////////

//SPI�ײ���������SD��ģ�����

//////////////////////////////////////////////////////////////////////////////////	

//=============== ����ʵ�� =====================
/*
 * ��������SPI_Config_Init()
 * ���룺void
 * �����void
 * ���ܣ���SPI���г�ʼ������
 */
void SPI1_Config_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;
	
	//��������ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE); //PORTBʱ��ʹ�� 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//SPI1ʱ��ʹ�� 
	
	//����CLK�˿ڵ�GPIO
	GPIO_InitStructure.GPIO_Pin = SPI_CLK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(SPI_CLK_GPIO_PORT,&GPIO_InitStructure);
	
	//����MISO�˿ڵ�GPIO
	GPIO_InitStructure.GPIO_Pin = SPI_MISO_GPIO_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//������� ��Ϊ���õ�ʱ��˿ڷ������ڲ����ƣ������������ó�����൱�ڸ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(SPI_MISO_GPIO_PORT,&GPIO_InitStructure);
	
	//����MOSI�˿ڵ�GPIO
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(SPI_MOSI_GPIO_PORT,&GPIO_InitStructure);

	//���ö˿ڸ���
	GPIO_PinAFConfig(SPI_CLK_GPIO_PORT ,SPI_CLK_AF_PINSOURCE ,GPIO_AF_SPI1);	//PB3����ΪSPI1
	GPIO_PinAFConfig(SPI_MISO_GPIO_PORT,SPI_MISO_AF_PINSOURCE,GPIO_AF_SPI1);	//PB4����ΪSPI1
	GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT,SPI_MOSI_AF_PINSOURCE,GPIO_AF_SPI1);	//PB5����ΪSPI1
	
	//����ֻ���SPI�ڳ�ʼ��
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);	//��λSPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//ֹͣ��λSPI1
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;			//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			//NSS�ź��������ʹ��SSIλ������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; ////���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256 Ϊ��ʱ�ӷ�Ƶ
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;			//CRCֵ����Ķ���ʽ,������CRC�ļ��ʽ��ʹ��CRC-8
	SPI_Init(SPIx_USE, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	SPI_Cmd(SPIx_USE,ENABLE);		//ʹ��SPI����
	
	SPI1_ReadWriteByte(0xff);		//��������
}

/*
 * ��������SPI1_SetSpeed()
 * ���룺uint8_t SpeedSet @ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256 
 * �����void
 * ���ܣ�SPI1�ٶ�����
 * SPI�ٶ�=fAPB2/��Ƶϵ��
 * fAPB2ʱ��Ϊ32Mhz��
 */
void SPI1_SetSpeed(uint8_t SpeedSet)
{
	SPI_Cmd(SPI1,DISABLE); //�ر�SPI1
	SPI1->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
	SPI1->CR1|=SpeedSet;
	SPI_Cmd(SPI1,ENABLE); //ʹ��SPI1
}

/*
 * ��������SPI1_ReadWriteByte()
 * ���룺uint8_t TxData:Ҫд����ֽ�
 * �����uint8_t ��ȡ�����ֽ�
 * ���ܣ���дһ���ֽ�
 */
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
	while(SPI_I2S_GetFlagStatus(SPIx_USE,SPI_I2S_FLAG_TXE) == RESET)//�ȴ���������
	{
	} 
	SPI_I2S_SendData(SPIx_USE,TxData);
	
	while(SPI_I2S_GetFlagStatus(SPIx_USE,SPI_I2S_FLAG_RXNE) == RESET)//�ȴ�������һ��byte 
	{	
	}
	return SPI_I2S_ReceiveData(SPIx_USE); //����ͨ��SPIx������յ�����	
	
		
}

