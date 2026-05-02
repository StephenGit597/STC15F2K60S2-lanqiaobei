#include"advance_module.h"

void IIC_Start(void)
{
	SCL=1;
	SDA=1;
	US_delay(3);
	SDA=0;
	US_delay(2);
	SCL=0;
}

void IIC_Stop(void)
{
	SCL=0;
	SDA=0;
	US_delay(3);
	SCL=1;
	US_delay(2);
	SDA=1;
}

void IIC_SendByte(unsigned char Byte)
{
	unsigned char i=0;
	SCL=0;
	SDA=0;
	US_delay(1);
	for(i=0;i<8;i++)
	{
		SDA=(Byte>>(7-i))&0x01;
		US_delay(1);
		SCL=1;
		US_delay(5);
		SCL=0;
	}
	SDA=1;
}

unsigned char IIC_Waitack(void)
{
	unsigned char ack=0;
	SCL=0;
	SDA=1;
	US_delay(1);
	SCL=1;
	US_delay(1);
	ack=!SDA;
	SCL=0;
	return(ack);
}

unsigned char onewire_Start(void)
{
	unsigned char ack=0;
	BUS=1;
	US_delay(30);
	BUS=0;
	US_delay(520);
	BUS=1;
	US_delay(55);
	ack=!BUS;
	US_delay(200);
	return(ack);
}

void onewire_SendByte(Byte)
{
	unsigned char i=0;
	BUS=1;
	US_delay(10);
	for(i=0;i<8;i++)
	{
		if((Byte>>i)&0x01)
		{
			BUS=0;
			US_delay(6);
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

unsigned char onewire_Receive(void)
{
	unsigned char Byte=0;
	unsigned char i=0;
	BUS=1;
	US_delay(10);
	for(i=0;i<8;i++)
	{
		Byte>>=1;
		BUS=0;
		US_delay(2);
		BUS=1;
		US_delay(8);
		if(BUS)
		{
			Byte|=0x80;
		}
		US_delay(55);
	}
	BUS=1;
	return(Byte);
}

void Get_temperature_Send(void)
{
    onewire_Start();
	onewire_SendByte(0xCC);
	onewire_SendByte(0x44);
}

float Get_temperature_Receive(void)
{
	float temperature=0;
	unsigned char high=0;
	unsigned char low=0;
	unsigned int raw_data=0;
	onewire_Start();
	onewire_SendByte(0xCC);
	onewire_SendByte(0xBE);
	low=onewire_Receive();
	high=onewire_Receive();
	raw_data=(high<<8)|low;
	if(raw_data&0x8000)
	{
		raw_data=~raw_data+1;
	}
	temperature=(raw_data>>4)+(raw_data&0x000F)*0.0625;
	return(temperature);
}

void Out_V(unsigned char V)
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_Waitack();
	
	IIC_SendByte(0x41);
	IIC_Waitack();
	IIC_SendByte(V);
	IIC_Waitack();
	IIC_Stop();
}