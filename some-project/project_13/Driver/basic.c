#include"basic.h"

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;//使能定时器0中断
}

void Timer0(void) interrupt 1
{
	currenttim++;
}

void MS_delay(unsigned int ms)
{
	unsigned long prev=currenttim+ms;
	while(prev>currenttim);
}

void US_delay(unsigned int us)
{
	unsigned int i=0;
	for(i=0;i<us;i++)
	{
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}
}

void LED_Display(unsigned char Byte)
{
	P2=(P2&0x1F)|0x00;
	US_delay(1);
	P0=~Byte;
	US_delay(1);
	P2=(P2&0x1F)|0x80;
	US_delay(3);
	P2=(P2&0x1F)|0x00;
}

void Seg_Display(unsigned char sel,unsigned char Byte,unsigned char dot)
{
	if((sel==0)|(sel>8)|(Byte>12)|(dot>1))
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	US_delay(1);
	P0=0x01<<(sel-1);
	US_delay(1);
	P2=(P2&0x1F)|0xC0;
	US_delay(3);
	P2=(P2&0x1F)|0x00;
	P0=segcode[Byte];
	if(dot)
	{
		P07=0;
	}
	US_delay(1);
	P2=(P2&0x1F)|0xE0;
	MS_delay(1);
	P0=0xFF;
	P2=(P2&0x1F)|0x00;
}

void Relay_control(unsigned char on)
{
	if(on>1)
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	US_delay(1);
	if(on)
	{
		P0=0x10;
	}
	else
	{
		P0=0x00;
	}
	US_delay(1);
	P2=(P2&0x1F)|0xA0;
	US_delay(3);
	P2=(P2&0x1F)|0x00;
}

unsigned char scan_key(void)
{
	unsigned char temp=0;
	unsigned char result=0;
	P35=0;
	P34=1;
	temp=((P3&0x0C)>>2);
	P35=1;
	P34=0;
	temp=0xF0|((temp<<2)|((P3&0x0C)>>2));
	temp=~temp;
	switch(temp)
	{
		case 0x08:
		{
			result=1;
			break;
		}
		case 0x04:
		{
			result=2;
			break;
		}
		case 0x02:
		{
			result=3;
			break;
		}
		case 0x01:
		{
			result=4;
			break;
		}
		default:
		{
			result=0;
			break;
		}
	}
	return(result);
}