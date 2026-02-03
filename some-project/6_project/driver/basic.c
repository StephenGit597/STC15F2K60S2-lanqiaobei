#include"basic.h"

void Timer1Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xCD;		//设置定时初值
	TH1 = 0xD4;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;
}

void Timer1(void) interrupt 3
{
	currenttim++;
}

void MS_Delay(unsigned int ms)
{
	unsigned long prev=currenttim+ms;
	while(prev>currenttim);
}

void US_Delay(unsigned int us)
{
	unsigned int i=0;
	for(i=0;i<us;i++)
	{
	}
}

void Led_display(unsigned char Byte)
{
	P2=(P2&0x1F)|0x00;
	P0=~Byte;
	US_Delay(1);
	P2=(P2&0x1F)|0x80;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}

void Seg_Display(unsigned char sel,unsigned char Byte,unsigned char dot)
{
	if((sel==0)|(sel>8)|(Byte>11)|(dot>1))
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	P0=0x01<<(sel-1);
	US_Delay(1);
	P2=(P2&0x1F)|0xC0;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
	P0=segcode[Byte];
	if(dot)
	{
		P07=0;
	}
    US_Delay(1);
	P2=(P2&0x1F)|0xE0;
	MS_Delay(1);
	P0=0xFF;
	P2=(P2&0x1F)|0x00;
}

unsigned char Get_key(void)
{
	unsigned char temp=0;
	unsigned char value=0;
	P44=1;
	temp=P3&0x0F;
	switch(temp)
	{
		case 0x07://0000 0111
		{
			value=1;
			break;
		}
		case 0x0B://0000 1011
		{
			value=2;
			break;
		}
		case 0x0D://0000 1101
		{
			value=3;
			break;
		}
		case 0x0E://0000 1110
		{
			value=4;
			break;
		}
		default:
		{
			value=0;
			break;
		}
	}
	return(value);
}