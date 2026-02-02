#include"basic.h"
#include"advance.h"

void I2C_Start(void)
{
	//空闲状态
	SDA=1;//SDA
	SCK=1;//SCK
	US_Delay(3);
	
	SDA=0;//拉低SDA，开始通讯
	US_Delay(3);//延时给从机反应时间
	SCK=0;//拉低时钟线防止iic误操作
}

void I2C_Stop(void)
{
	SCK=0;//先拉低时钟，防止SDA电平不确定误操作
	SDA=0;//先保持数据为低
	US_Delay(3);
	SCK=1;
	US_Delay(3);//给从机反应时间
	SDA=1;
}

void I2C_SendByte(unsigned char Byte)
{
	unsigned char i=0;//循环变量声明
	SCK=0;//先拉低SCK，防止误操作
	SDA=0;
	US_Delay(2);
	for(i=0;i<8;i++)
	{
		SDA=(Byte>>(7-i))&0x01;//准备数据，注意I2C是高位优先
		US_Delay(2);//等待SDA电平稳定
		SCK=1;
		US_Delay(3);//给从机反应时间
		SCK=0;
	}
	SCK=0;//确保I2C_主机不发送接收应答或数据时SCK为低，防止误操作
	SDA=1;//释放总线，供从机应答
}

unsigned char I2C_ReceiveByte(void)
{
	unsigned char i=0;//循环变量
	unsigned char temp=0;//函数的返回值
	SCK=0;//先拉低SCK，防止I2C误操作
	SDA=1;//释放总线，供从机发送数据
	US_Delay(3);//初始化
	for(i=0;i<8;i++)
	{
		temp<<=1;//先左移，接收数据temp只需要移7次，否则最高位会被溢出
		SCK=1;//拉高SCK
		US_Delay(3);//等待从机向SDA发送电平稳定
		if(SDA)
		{
			temp|=0x01;
		}
		SCK=0;//拉低SCK读取结束
		US_Delay(2);
	}
	SCK=0;//确保IIC使用中不误触发
	SDA=1;//释放总线，等待主机去发送应答
	return(temp);
}

void I2C_SendAck(unsigned char ack)
{
	SCK=0;
	SDA=0;
	US_Delay(1);//初始状态
	
	SDA=!ack;//在SCK为低电平期间准备应答信号，低电平有效
	US_Delay(1);
	SCK=1;//拉高SCK
	US_Delay(2);
	SCK=0;//拉低SCK
	
	SDA=1;//释放SDA,让从机继续发送数据
}

unsigned char I2C_ReceiveAck(void)
{
	unsigned char ack=0;
	SCK=0;
	SDA=0;
	US_Delay(1);//初始状态
	
	SCK=1;//拉高准备读取
	US_Delay(1);//等待SDA电平稳定后读取应答
	ack=SDA;//读取应答信号，低电平有效
	ack=!ack;
	SCK=0;//拉低表示接收完毕
	
	SDA=1;//释放总线，便于主机下一次发送数据
	return(ack);
}

unsigned char DS18B20_Init(void)
{
	unsigned char ack=0;
	BUS=1;
	US_Delay(10);//延时10us,先保证P14高电平稳定
	BUS=0;
	US_Delay(520);//延时520us，让温度传感器稳定读取电平
	BUS=1;
	US_Delay(57);//等待温度传感器的拉低电平应答
	ack=BUS;
	ack=!ack;  //温度传感器低电平应答有效
	US_Delay(200);//等待温度传感器的低电平应答时间结束，防止与以后的指令冲突
	return(ack);
}

void DS18B20_SendByte(unsigned char Byte)
{
	unsigned char i=0;//循环变量的声明
	BUS=1;
	US_Delay(10);//延时10us,先保证P14高电平稳定
	for(i=0;i<8;i++)//低位先行
	{
		if((Byte>>i)&0x01)//1
		{
			BUS=0;//拉低提供下降沿时钟
			US_Delay(6);//给传感器检测下降边沿时间
			BUS=1;//拉高写1
			US_Delay(55);//传感读取高电平延时时间
		}
		else//0
		{
			BUS=0;//拉低提供下降沿时钟
			US_Delay(60);//给传感器检测时间，同时检测低电平
			BUS=1;//拉高便于下一次下降延时钟
			US_Delay(5);//高电平保持时间		
		}
	}
	BUS=1;//单片机释放总线
}

unsigned char DS18B20_ReadByte(void)
{
	unsigned char i=0;//循环变量的声明
	unsigned char temp=0;//接收变量
	BUS=1;
	US_Delay(10);//延时10us,保证P14高电平稳定
	for(i=0;i<8;i++)//低位先行
	{
		temp>>=1;//先左移，接收temp只用左移7次,否则数据为溢出
		BUS=0;//拉低P14，提供下降延时钟
		US_Delay(2);//给从机提供反应时间
		BUS=1;//主机释放总线，供从机应答
		US_Delay(8);//等待P14电平稳定，但是延时不宜过长，因为温度传感器的电平（低电平）不能一直保持
		if(BUS)
		{
			temp|=0x80;
		}
		US_Delay(55);//延时，每一个比特的读时序必须大于60us
	}
	BUS=1;//确实P14总线释放
	return(temp);
}

void Temperature_ask(void)
{
	DS18B20_Init();
	DS18B20_SendByte(0xCC);
	DS18B20_SendByte(0x44);
}

float Temperature_receive(void)
{
	unsigned char low=0;
	unsigned char high=0;
	unsigned int raw=0;
	float temperature=0.0f;
	DS18B20_Init();
	DS18B20_SendByte(0xCC);
	DS18B20_SendByte(0xBE);
	
	low=DS18B20_ReadByte();
	high=DS18B20_ReadByte();
	raw=(high<<8)|low;
	
	if(raw&0x8000)
	{
		raw=~raw+1;
	}
	temperature=((raw&0x0FF0)>>4)+(raw&0x000F)*0.0625f;
	return(temperature);
}

void Get_Inu_ask(void)
{
	I2C_Start();
	I2C_SendByte(0x90);
	I2C_ReceiveAck();
	
	I2C_SendByte(0x41);
	I2C_ReceiveAck();
	I2C_Stop();
}

unsigned char Get_Inu_receive(void)
{
	unsigned char Inu=0;
	unsigned char V=0;
	I2C_Start();
	I2C_SendByte(0x91);
	I2C_ReceiveAck();
	
	V=I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();
	if((V>153)|(V==153))
	{
		Inu=1;
	}
	else if((V<153)&&(V>101))
	{
		Inu=2;
	}
	else if((V>25)&&(V<102))
	{
		Inu=3;
	}
	else
	{
		Inu=4;
	}
	return(Inu);
}