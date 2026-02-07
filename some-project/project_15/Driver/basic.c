#include"basic.h"

void Timer1Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xCD;		//设置定时初值
	TH1 = 0xD4;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;//使能定时器1中断
}

void Timer0Init(void)		//@11.0592MHz
{
	AUXR |= 0x80;		//计数器时钟1T模式
	TMOD &= 0xF0;		//保留定时器1的配置
	TMOD|=0x05;     //设置计数器模式
	TL0 = 0x00;		//设置计数初值
	TH0 = 0x00;		//设置计数初值
	TF0 = 0;		//清除TF0标志
}

void Timer1(void) interrupt 3
{
	currenttim++;
}

void MS_Delay(unsigned int ms)
{
	unsigned long prev=currenttim+ms;
	while(prev>=currenttim);
}

void US_Delay(unsigned int us)
{
	unsigned int i=0;
	for(i=0;i<us;i++)
	{
		_nop_();
	}
}

void Seg_display(unsigned char sel,unsigned char Byte,unsigned char dot)
{
	if((sel==0)|(sel>8)|(Byte>16)|(dot>1))
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
	if(dot==1)
	{
		P07=0;
	}
	US_Delay(1);
	P2=(P2&0x1F)|0xE0;
	MS_Delay(1);
	P0=0xFF;
	P2=(P2&0x1F)|0x00;
}

void LED_display(unsigned char Byte)
{
	P2=(P2&0x1F)|0x00;
	P0=~Byte;
	US_Delay(1);
	P2=(P2&0x1F)|0x80;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}

unsigned char Scan_Martix_Key(void)
{
	unsigned char temp=0;
	unsigned char value=0;
	P44=0;
	P42=1;
	temp=(P3&0x0C)>>2;
	P44=1;
	P42=0;
	temp=~(0xF0|(temp<<2)|((P3&0x0C)>>2));
	switch(temp)
	{
		case 0x08:
		{
			value=1;
			break;
		}
		case 0x04:
		{
			value=2;
			break;
		}
		case 0x02:
		{
			value=3;
			break;
		}
		case 0x01:
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

void Get_Feq_ask(void)
{
	TL0 = 0x00;		//设置计数初值
	TH0 = 0x00;		//设置计数初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;        //计数器0开始计数
}

unsigned int Get_Feq_receive(void)
{
	unsigned int Feq=0;
	TR0 = 0;        //计数器0停止计数
	Feq=(TH0<<8)|TL0;//获取计数值
	TL0 = 0x00;		//设置计数初值
	TH0 = 0x00;		//设置计数初值
	TF0 = 0;		//清除TF0标志
	return(Feq);
}