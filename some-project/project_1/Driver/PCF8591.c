#include"PCF8591.h"
//I2C驱动函数为自己编写
void I2C_Start(void)
{
	SCL=1;//注意初始保持高电平
	SDA=1;//注意初始保持高电平
	US_Delay(3);
	SDA=0;//拉低时钟线
	US_Delay(1);
	SCL=0;//注意I2C通讯开始后，主机不发送或接收数据建议保持时钟电平为低，防止主从机电平误读取
}

void I2C_Stop(void)
{
	SCL=0;//注意初始保持低电平
	SDA=0;//注意初始保持低电平
	US_Delay(3);
	SCL=1;//先释放时钟线
	US_Delay(1);
	SDA=1;//再释放数据线
}

void I2C_SendByte(unsigned char Byte)
{
	unsigned char i=0;
	SCL=0;//时钟初始为低电平
	SDA=0;//数据初始电平随意
	US_Delay(1);
	for(i=0;i<8;i++)
	{
		SDA=(Byte>>(7-i))&0x01;//在拉高时钟线之前一个要让数据线数据准备完成而且数据稳定
		US_Delay(2);
		SCL=1;//拉高时钟线
		US_Delay(3);//给从机反应时间读取主机给出的电平
		SCL=0;//拉低时钟线表示一个时钟周期结束
	}
	SCL=0;//注意：I2C过程中若有短暂空闲，保持时钟为低电平
	SDA=1;//一定要释放总线，供从机应答
}

unsigned char I2C_ReceiveByte(void)
{
	unsigned char i=0;
	unsigned char temp=0;
	SCL=0;//时钟初始为低电平
	SDA=1;//一定要释放总线，否则从机无法提供数据
	US_Delay(1);
	for(i=0;i<8;i++)
	{
		temp<<=1;//一定要先空左移一次，否则最高位会被溢出
		US_Delay(3);
		SCL=1;//释放时钟线，供读取数据
		US_Delay(2);
		if(SDA)
		{
			temp|=0x01;
		}
		SCL=0;//拉低时钟线表示一个周期结束
		US_Delay(1);
	}
	SCL=0;//注意：I2C过程中若有短暂空闲，保持时钟为低电平
	SDA=1;//建议要释放总线，主机需要应答
	return(temp);
}

void I2C_SendAck(unsigned char ack)
{
	SCL=0;//时钟初始为低电平
	SDA=!ack;//I2C,当SCL位高SDA为高是表示从机未应答，为低表示从机应答
	US_Delay(1);
	SCL=1;
	US_Delay(3);
	SCL=0;//注意：I2C过程中若有短暂空闲，保持时钟为低电平
	SDA=1;//建议要释放总线，让从机继续发送数据
}

unsigned char I2C_Waitack(void)
{
	unsigned char ack=0;
	SCL=0;//时钟初始为低电平
	SDA=1;//一定要释放总线，供从机应答
	US_Delay(1);
	SCL=1;
	US_Delay(3);
	ack=!SDA;//I2C,当SCL位高SDA为高是表示从机未应答，为低表示从机应答
	return(ack);
}

unsigned char PCF8591(unsigned char mode)//采集电压 0为采集光敏电阻电压 1为采集电位器电压
{
	unsigned char temp=0;
	unsigned char cmd=0;
	switch(mode)
	{
		case 0:
		{
			cmd=0x40;//采集AIN0，开发板输入引脚的电压
			break;
		}
		case 1:
		{
			cmd=0x41;//采集光敏电阻电压
			break;
		}
		case 2:
		{
			cmd=0x43;//采集电位器电压
			break;
		}
		default:
		{
			return(0);
		}
	}
	I2C_Start();
	I2C_SendByte(0x90);//发送写入指令，假设PCF8591的地址线A2 A1 A0都接地
	I2C_Waitack();
	
	I2C_SendByte(cmd);
	I2C_Waitack();
	I2C_Stop();
	
	I2C_Start();
	I2C_SendByte(0x91);//发送读取指令，假设PCF8591的地址线A2 A1 A0都接地
	I2C_Waitack();
	
	temp=I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();
	return(temp);
}

void PCF8591_Analog(unsigned char V)//模拟电压输出
{
	I2C_Start();
	I2C_SendByte(0x90);//发送写入指令，假设PCF8591的地址线A2 A1 A0都接地
	I2C_Waitack();
	
	I2C_SendByte(0x41);
	I2C_Waitack();
	
	I2C_SendByte(V);
	I2C_Waitack();
	I2C_Stop();
}