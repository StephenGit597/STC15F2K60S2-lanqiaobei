#include"ds1302.h"
unsigned char idata hour;
unsigned char idata minute;
unsigned char idata second;

void ds1302_WriteByte(unsigned char addr,unsigned char Byte)
{
	unsigned char i=0;
	RST=0;//先确保未使能，否则操作SCK SCA会导致通讯错误
	SCK=0;//初始电平为低（必须）否则会出现误动作
	SCA=0;//数据引脚初始化（建议）
	US_Delay(5);
	RST=1;//一定要最后使能
	US_Delay(1);
	for(i=0;i<8;i++)//上升沿有效
	{
		SCK=0;
		SCA=(addr>>i)&0x01;//准备数据，LSB
		US_Delay(2);
		SCK=1;
		US_Delay(5);
	}
	for(i=0;i<8;i++)
	{
		SCK=0;
		SCA=(Byte>>i)&0x01;//准备数据，LSB
		US_Delay(2);
		SCK=1;
		US_Delay(5);
	}
	RST=0;//使能失效，通讯结束
}

unsigned char ds1302ReadByte(unsigned char addr)
{
	unsigned char i=0;
	unsigned char temp=0;
	RST=0;//先确保未使能，否则操作SCK SCA会导致通讯错误
	SCK=0;//初始电平为低（必须）否则会出现误动作
	SCA=0;//数据引脚初始化（建议）
	US_Delay(5);
	RST=1;//一定要最后使能
	US_Delay(1);
	for(i=0;i<8;i++)//上升沿有效
	{
		SCK=0;
		SCA=(addr>>i)&0x01;//准备数据，LSB
		US_Delay(2);
		SCK=1;
		US_Delay(5);
	}
	SCA=1;//发送命令后一定要释放总线给从机应答
	for(i=0;i<8;i++)//下降延有效
	{
		SCK=1;
		temp>>=1;
		US_Delay(3);
		SCK=0;
		US_Delay(2);
		if(SCA)
		{
			temp|=0x80;
		}
	}
	RST=0;//使能失效，通讯结束
	return(temp);
}

void Set_Time(unsigned char hour,unsigned char minute,unsigned char second)//BCD码存储模式
{
	unsigned char temp=0;
	ds1302_WriteByte(0x8E,0x00);//关闭写保护
	temp=((hour/10)<<4)|((hour%10)&0x0F);
	ds1302_WriteByte(0x84,temp);
	temp=((minute/10)<<4)|((minute%10)&0x0F);
	ds1302_WriteByte(0x82,temp);
	temp=((second/10)<<4)|((second%10)&0x0F);
	ds1302_WriteByte(0x80,temp);
	ds1302_WriteByte(0x8E,0x80);//开启写保护
}

void Get_Time(void)//BCD码存储模式
{
	unsigned char temp=0;
	temp=ds1302ReadByte(0x85);
	hour=10*(temp>>4)+((temp)&0x0F);
	temp=ds1302ReadByte(0x83);
	minute=10*(temp>>4)+((temp)&0x0F);
	temp=ds1302ReadByte(0x81);
	second=10*(temp>>4)+((temp)&0x0F);
}