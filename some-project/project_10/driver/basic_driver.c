#include"basic_driver.h"
//定时器1初始化
void Timer1Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xCD;		//设置定时初值
	TH1 = 0xD4;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;//使能定时器一中断
}
//对应中断服务函数
void Timer1_(void) interrupt 3
{
	currenttim=currenttim+1;
}
//毫秒延时函数
void MS_Delay(unsigned char ms)
{
	unsigned long prev=currenttim+ms;
	while(prev>currenttim);
}
//微秒延时函数
void US_Delay(unsigned char us)
{
	unsigned char i=0;
	for(i=0;i<us;i++)
	{
		_nop_();
	}
}
//扫描矩阵按键函数
unsigned char Scan_Key(void)
{
	unsigned int temp=0;//16位按键接收区
	unsigned char value=0;
	//扫描第一列
	P44=0;
	P42=1;
	P35=1;
	P34=1;
	//读取第一列
	temp=P3&0x0F;
	//扫描第二列
	P44=1;
	P42=0;
	P35=1;
	P34=1;
	//读取第二列
	temp=(temp<<4)|(P3&0x0F);
	//扫描第三列
	P44=1;
	P42=1;
	P35=0;
	P34=1;
	//读取第三列
	temp=(temp<<4)|(P3&0x0F);
	//扫描第四列
	P44=1;
	P42=1;
	P35=1;
	P34=0;
	//读取第四列
	temp=(temp<<4)|(P3&0x0F);
	//扫描结果进行取反
	temp=~temp;
	switch(temp)
	{
		case 0x8000://S4被按下
		{
			value=1;
			break;
		}
		case 0x4000://S5被按下
		{
			value=2;
			break;
		}
		case 0x2000://S6被按下
		{
			value=3;
			break;
		}
		case 0x1000://S7被按下
		{
			value=4;
			break;
		}
		case 0x0800://S8被按下
		{
			value=5;
			break;
		}
		case 0x0400://S9被按下
		{
			value=6;
			break;
		}
		case 0x0200://S10被按下
		{
			value=7;
			break;
		}
		case 0x0100://S11被按下
		{
			value=8;
			break;
		}
		case 0x0080://S12被按下
		{
			value=9;
			break;
		}
		case 0x0040://S13被按下
		{
			value=10;
			break;
		}
		case 0x0020://S14被按下
		{
			value=11;
			break;
		}
		case 0x0010://S15被按下
		{
			value=12;
			break;
		}
		case 0x0008://S16被按下
		{
			value=13;
			break;
		}
		case 0x0004://S17被按下
		{
			value=14;
			break;
		}
		case 0x0002://S18被按下
		{
			value=15;
			break;
		}
		case 0x0001://S19被按下
		{
			value=16;
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
//LED显示函数
void LED_Display(unsigned char Byte)
{
	P2=(P2&0x1F)|0x00;
	US_Delay(1);
	P0=~Byte;
	US_Delay(1);
	P2=(P2&0x1F)|0x80;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}
//数码管显示函数
void Seg_Display(unsigned char sel,unsigned char Byte,unsigned char dot)
{
	if((sel==0)|(sel>8)|(Byte>15)|(dot>1))
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	US_Delay(1);
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
//继电器控制函数
void Relay_Control(unsigned char on)
{
	if(on>1)
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	US_Delay(1);
	if(on==0)
	{
		P0=0x00;//继电器关闭
	}
	else
	{
		P0=0x10;//继电器开启
	}
	US_Delay(1);
	P2=(P2&0x1F)|0xA0;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}
