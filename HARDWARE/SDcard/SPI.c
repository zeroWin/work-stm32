#include	"SPI.h"

//////////////////////////////////////////////////////////////////////////////////

//SPI底层驱动，供SD卡模块调用

//////////////////////////////////////////////////////////////////////////////////	

//=============== 函数实现 =====================
/*
 * 函数名：SPI_Config_Init()
 * 输入：void
 * 输出：void
 * 功能：对SPI进行初始化设置
 */
void SPI1_Config_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;
	
	//开启外设时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE); //PORTB时钟使能 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//SPI1时钟使能 
	
	//配置CLK端口的GPIO
	GPIO_InitStructure.GPIO_Pin = SPI_CLK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(SPI_CLK_GPIO_PORT,&GPIO_InitStructure);
	
	//配置MISO端口的GPIO
	GPIO_InitStructure.GPIO_Pin = SPI_MISO_GPIO_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出 因为复用的时候端口方向由内部控制，所以这里设置成输出相当于浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(SPI_MISO_GPIO_PORT,&GPIO_InitStructure);
	
	//配置MOSI端口的GPIO
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	//10MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(SPI_MOSI_GPIO_PORT,&GPIO_InitStructure);

	//设置端口复用
	GPIO_PinAFConfig(SPI_CLK_GPIO_PORT ,SPI_CLK_AF_PINSOURCE ,GPIO_AF_SPI1);	//PB3复用为SPI1
	GPIO_PinAFConfig(SPI_MISO_GPIO_PORT,SPI_MISO_AF_PINSOURCE,GPIO_AF_SPI1);	//PB4复用为SPI1
	GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT,SPI_MOSI_AF_PINSOURCE,GPIO_AF_SPI1);	//PB5复用为SPI1
	
	//这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);	//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;			//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			//NSS信号由软件（使用SSI位）管理
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; ////定义波特率预分频的值:波特率预分频值为256 为主时钟分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;			//CRC值计算的多项式,含义是CRC的简记式，使用CRC-8
	SPI_Init(SPIx_USE, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_Cmd(SPIx_USE,ENABLE);		//使能SPI外设
	
	SPI1_ReadWriteByte(0xff);		//启动传输
}

/*
 * 函数名：SPI1_SetSpeed()
 * 输入：uint8_t SpeedSet @ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256 
 * 输出：void
 * 功能：SPI1速度设置
 * SPI速度=fAPB2/分频系数
 * fAPB2时钟为32Mhz：
 */
void SPI1_SetSpeed(uint8_t SpeedSet)
{
	SPI_Cmd(SPI1,DISABLE); //关闭SPI1
	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CR1|=SpeedSet;
	SPI_Cmd(SPI1,ENABLE); //使能SPI1
}

/*
 * 函数名：SPI1_ReadWriteByte()
 * 输入：uint8_t TxData:要写入的字节
 * 输出：uint8_t 读取到的字节
 * 功能：读写一个字节
 */
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
	while(SPI_I2S_GetFlagStatus(SPIx_USE,SPI_I2S_FLAG_TXE) == RESET)//等待发送区空
	{
	} 
	SPI_I2S_SendData(SPIx_USE,TxData);
	
	while(SPI_I2S_GetFlagStatus(SPIx_USE,SPI_I2S_FLAG_RXNE) == RESET)//等待接收完一个byte 
	{	
	}
	return SPI_I2S_ReceiveData(SPIx_USE); //返回通过SPIx最近接收的数据	
	
		
}

