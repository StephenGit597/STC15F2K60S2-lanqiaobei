#include"basic.h"

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;//使能定时器0开始中断
}

void Timer1Init(void)
{
	AUXR |=0x40;//定时器11T模式
	TMOD &= 0x0F;//保留定时器0的配置，同时设置定时器1的模式
	TMOD|=0x10;//设置16位不可重装载模式
	TF1=0;//清除TF1标志
	TL1=0x00;//第八位初始化清零
	TH1=0x00;//高八位初始化清零
}

void Timer1(void) interrupt 1
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

void LED_display(unsigned char Byte)
{
	P2=(P2&0x1F)|0x00;
	US_Delay(1);
	P0=~Byte;
	US_Delay(1);
	P2=(P2&0x1F)|0x80;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}

void Seg_display(unsigned char sel,unsigned char Byte,unsigned char dot)
{
	if((sel==0)|(sel>8)|(Byte>14)|(dot>1))
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

void Relay_control(unsigned char cmd)
{
	unsigned char temp;
	if(cmd==0)
	{
		temp=0x00;
	}
	else
	{
		temp=0x10;
	}
	P2=(P2&0x1F)|0x00;
	P0=temp;
	US_Delay(1);
	P2=(P2&0x1F)|0xA0;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}

unsigned char scan_key(void)
{
	unsigned char temp=0x00;
	unsigned char result=0;
	P44=0;
	P42=1;
	temp=(P3&0x0C)>>2;
	P44=1;
	P42=0;
	temp=(temp<<2)|((P3&0x0C)>>2);
	temp=~(temp|0xF0);
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
		case 0x03:
		{
			result=5;
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

unsigned char Get_distance(void)
{
	unsigned char i=0;
	float distance=0.0f;
	TH1=0;
	TL1=0;
	for(i=0;i<3;i++)
	{
		P10=1;
		US_Delay(13);
		P10=0;
		US_Delay(13);
	}
	TR1=1;
	while((P11==1)&&((TH1*256+TL1)<57000));
	TR1=0;//定时器停止计时
	TF1=0;//确保标志位清除
	distance=((TH1<<8)|TL1)*0.0014f;
	TH1=0;
	TL1=0;
	return((unsigned char)(distance));
}