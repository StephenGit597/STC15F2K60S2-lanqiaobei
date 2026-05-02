#include"basic_module.h"
//数码管段码定义 0 1 2 3 4 5 6 7 8 9 - P F 无
unsigned char code segcode[14]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0x8C,0x8E,0xFF};
//定时器1初始化
void Timer1Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xCD;		//设置定时初值
	TH1 = 0xD4;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;          //使能定时器1中断
}
//定时器0初始化
void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式（16位不可重装模式）
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x00;		//设置定时初值
	TF0 = 0;		//清除TF0标志
}

void Timer1(void) interrupt 3
{
	currenttim++;
}
//数码管显示
void Seg_Display(unsigned char sel,unsigned char Byte,unsigned char dot)
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
//LED显示
void LED_display(unsigned char Byte)
{
	P2=(P2&0x1F)|0x00;
	P0=~Byte;
	US_Delay(1);
	P2=(P2&0x1F)|0x80;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}
//继电器控制
void Relay_control(unsigned char control_Byte)
{
	if(control_Byte>1)
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	if(control_Byte==1)
	{
		P0=0x10;
	}
	else
	{
		P0=0x00;
	}
	US_Delay(1);
	P2=(P2&0x1F)|0xA0;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}
//矩阵按键
unsigned char martix_scan(void)
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
		case 0x08://0000 1000
		{
			value=1;//S4
			break;
		}
		case 0x04://0000 0100
		{
			value=2;//S5
			break;
		}
		case 0x02://0000 0010
		{
			value=3;//S8
			break;
		}
		case 0x01://0000 0001
		{
			value=4;//S9
			break;
		}
		case 0x03://0000 0011
		{
			value=5;//S8 S9组合
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
//超声波测距
void Get_distance_ask(void)
{
	unsigned char i=0;//循环变量声明
	for(i=0;i<8;i++)
	{
		P10=1;
		US_Delay(13);
		P10=0;
		US_Delay(13);
	}
	TL0=0;//定时器0低位清零
	TH0=0;//定时器0高位清零
	TF0=0;//定时器0溢出标志位清零
	TR0=1;//定时器0开始计时
}
//传入声速
unsigned int Get_distance_receive(unsigned int Trans_V)//返回距离cm
{
	unsigned int raw=0;
	float distance=0.0f;
	TR0=0;//定时器0停止计时
	raw=(TH0<<8)|TL0;
	TL0=0;//定时器0低位清零
	TH0=0;//定时器0高位清零
	TF0=0;//定时器0溢出标志位清零
	distance=((Trans_V/10)*raw)/1000.0f;//S=VT/2
	return((unsigned int)(distance));
}