#include"basic_driver.h"
//IIC电压采集部分
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
//DS18B20部分
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
//DS1302时间获取部分
void DS1302_SendByte(unsigned char addr,unsigned char Byte)
{
	unsigned char i=0;//循环变量声明
	RST=0;//首先失能
	SCA=0;//数据IO口初始化为低电平
	SCL=0;//时钟初始化为低电平
	RST=1;//必须在数据IO口，时钟引脚初始化完成后使能，否则会出现误动作
	US_Delay(10);//延时等待稳定
	
	for(i=0;i<8;i++)//首先发送地址,注意DS1302是LSB
	{
		SCA=(addr>>i)&0x01;//数据引脚准备数据
		US_Delay(5);//等待P23引脚电平稳定
		SCL=1;//拉高时钟引脚，DS1302写为上升沿有效
		US_Delay(5);//电平保持时间，给从机反应时间
		SCL=0;//恢复时钟引脚低电平
		US_Delay(1);//等待引脚电平稳定
	}
	//注意不能让P13引脚失能，否则写入失败
	for(i=0;i<8;i++)
	{
		SCA=(Byte>>i)&0x01;//准备数据
		US_Delay(5);//等待数据引脚电平稳定
		SCL=1;//P17拉高。上升沿读取P23数据
		US_Delay(5);//电平保持，给从机反应时间
		SCL=0;//时钟引脚恢复低电平
		US_Delay(1);//等待时钟引脚电平稳定
	}
	RST=0;//数据发送完成，DS1302不使能，防止误动作
	SCA=0;//数据IO口回归低电平，注意操作一定要在P13=0后做，否则误动作
	SCL=0;//时钟引脚回归低电平
}

unsigned char DS1302_ReadByte(unsigned char addr)
{
	unsigned char i=0;//循环变量声明
	unsigned char temp=0;//接受变量
	RST=0;//首先失能
	SCA=0;//数据IO口初始化为低电平
	SCL=0;//时钟初始化为低电平
	RST=1;//必须在数据IO口，时钟引脚初始化完成后使能，否则会出现误动作
	US_Delay(10);//延时等待稳定
	
	for(i=0;i<8;i++)//首先发送地址（虚写），但是下降沿触发（易错），注意和正常写的区别
	{
		SCA=(addr>>i)&0x01;//数据引脚准备数据
		US_Delay(5);//等待P23引脚电平稳定
		SCL=0;//拉低时钟引脚，DS1302读为下降沿有效
		US_Delay(5);//电平保持时间，给从机反应时间
		SCL=1;//恢复时钟引脚高电平
		US_Delay(1);//等待引脚电平稳定
	}
	//注意不能让P13引脚失能，否则读取失败
	SCA=1;//注意读取要置P23引脚为1，此为单片机的若上拉模式，释放数据总线给DS1302
	US_Delay(1);//延时稳定
	for(i=0;i<8;i++)//读取，下降沿读取，规则LSB
	{
		temp>>=1;//先右移，读取只用移7次，否则最低位会被溢出，数据读取规则LSB
		SCL=1;//先把时钟引脚置为高电平
		US_Delay(3);//延时等待电平稳定
		SCL=0;//拉低时钟引脚，主机在下降沿读取数据
		US_Delay(1);//短暂延时，等待从机发送数据并且P23电平稳定
		if(SCA)
		{
			temp|=0x80;
		}
	}
	RST=0;//数据读取完成，DS1302不使能，防止误动作
	SCA=0;//数据口回归低电平，注意这些操作一定在P13=0后做，否则有误动作
	SCL=0;//时钟引脚回归低电平
	return(temp);
}