#include"advance.h"
#include"basic.h"

unsigned char onewire_Start(void)
{
	unsigned char ack=0;
	BUS=1;
	US_delay(10);
	BUS=0;
	US_delay(520);
	BUS=1;
	US_delay(55);
	ack=!BUS;
	US_delay(200);
	return(ack);
}

void onewire_SendByte(unsigned char Byte)
{
	unsigned char i=0;
	BUS=1;
	US_delay(10);
	for(i=0;i<8;i++)
	{
		if((Byte>>i)&0x01)
		{
			BUS=0;
			US_delay(7);
			BUS=1;
			US_delay(55);
		}
		else
		{
			BUS=0;
			US_delay(60);
			BUS=1;
			US_delay(5);
		}
	}
	BUS=1;
}

unsigned char onewire_ReceiveByte(void)
{
	unsigned char i=0;
	unsigned char temp=0;
	BUS=1;
	US_delay(10);
	for(i=0;i<8;i++)
	{
		temp>>=1;
		BUS=0;
		US_delay(3);
		BUS=1;
		US_delay(7);
		if(BUS)
		{
			temp|=0x80;
		}
		US_delay(55);
	}
	BUS=1;
	return(temp);
}

void ds1302_Write(unsigned char addr,unsigned char Byte)
{
	unsigned char i=0;
	RST=0;
	CLK=0;
	SDA=0;
	US_delay(3);
	RST=1;
	US_delay(3);
	for(i=0;i<8;i++)
	{
		SDA=(addr>>i)&0x01;
		US_delay(1);
		CLK=1;
		US_delay(3);
		CLK=0;
		US_delay(1);
	}
	for(i=0;i<8;i++)
	{
		SDA=(Byte>>i)&0x01;
		US_delay(1);
		CLK=1;
		US_delay(3);
		CLK=0;
		US_delay(1);
	}
	RST=0;
	CLK=0;
	SDA=0;
}

unsigned char ds1302_Read(unsigned char addr)
{
	unsigned char i=0;
	unsigned char temp=0;
	RST=0;
	CLK=0;
	SDA=0;
	US_delay(3);
	RST=1;
	US_delay(3);
	for(i=0;i<8;i++)
	{
		SDA=(addr>>i)&0x01;
		US_delay(1);
		CLK=0;
		US_delay(3);
		CLK=1;
		US_delay(1);
	}
	SDA=1;
	CLK=1;
	US_delay(1);
	for(i=0;i<8;i++)
	{
		temp>>=1;
		CLK=0;
		US_delay(1);
		if(SDA)
		{
			temp|=0x80;
		}
		CLK=1;
		US_delay(1);
	}
	RST=0;
	CLK=0;
	SDA=0;
	return(temp);
}

void Get_tempature_ask(void)
{
	onewire_Start();
	onewire_SendByte(0xCC);
	onewire_SendByte(0x44);
}

float Get_tempature_receive(void)
{
	unsigned char low=0;
	unsigned char high=0;
	unsigned int raw=0;
	float temperature=0.0f;
	
	onewire_Start();
	onewire_SendByte(0xCC);
	onewire_SendByte(0xBE);
	
	low=onewire_ReceiveByte();
	high=onewire_ReceiveByte();
	raw=(high<<8)|low;
	if(raw&0x8000)
	{
		raw=~raw+1;
	}
	temperature=(raw>>4)+(raw&0x000F)*0.0625;
	return(temperature);
}

void Set_Time(unsigned char hour,unsigned char minute,unsigned char second)
{
	unsigned char temp=0;
	if((hour>23)|(minute>59)|(second>59))
	{
		return;
	}
	ds1302_Write(0x8E,0x00);
	temp=((hour/10)<<4)|((hour%10)&0x0F);
	ds1302_Write(0x84,temp);
	temp=((minute/10)<<4)|((minute%10)&0x0F);
	ds1302_Write(0x82,temp);
	temp=((second/10)<<4)|((second%10)&0x0F);
	ds1302_Write(0x80,temp);
	ds1302_Write(0x8E,0x80);
}

void Get_Time(void)
{
	unsigned char temp=0;
	temp=ds1302_Read(0x81);
	second=10*(temp>>4)+(temp&0x0F);
	temp=ds1302_Read(0x83);
	minute=10*(temp>>4)+(temp&0x0F);
	temp=ds1302_Read(0x85);
	hour=10*(temp>>4)+(temp&0x0F);
}