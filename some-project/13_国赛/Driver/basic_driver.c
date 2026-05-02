#include"basic_driver.h"
//定时器1初始化
void Timer1Init(void)		//100微秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xAE;		//设置定时初值
	TH1 = 0xFB;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;          //使能定时器1中断
}

//对应中断服务函数
void Timer1(void) interrupt 3
{
	Re_num++;
	if(Re_num==10)
	{
		Re_num=0;
		currenttim++;
	}
}
//定时器0设置为计数器模式给频率采集
void Timer0_couter(void)
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//保留定时器1模式
	TMOD |= 0x05;       //定时器0设置为计数器模式
	TF0 = 0;		//清除TF0标志
	TL0 = 0x00;     //设置定时器初值
	TH0 = 0x00;		//设置定时器初值
	
}
//定时器0设置为定时器模式给超声波
void Timer0_Timer(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式:16位不可重装载
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x00;		//设置定时初值
	TF0 = 0;		//清除TF0标志
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
//按键扫描
unsigned char scan_key(void)
{
	unsigned char value=0;
	unsigned char temp=0;
	temp=~(0xF0|(P3&0x0F));
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
//LED显示
void Led_Display(unsigned char dat)
{
	P2=(P2&0x1F)|0x00;
	P0=~dat;
	US_Delay(1);
	P2=(P2&0x1F)|0x80;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}
//数码管显示
void Seg_Display(unsigned char sel,unsigned char num,unsigned char dot)
{
	if((sel==0)|(sel>8)|(num>14)|(dot>1))
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	P0=0x01<<(sel-1);
	US_Delay(1);
	P2=(P2&0x1F)|0xC0;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
	P0=segcode[num];
	if(dot)
	{
		P07=0;
	}
	US_Delay(3);
	P2=(P2&0x1F)|0xE0;
	MS_Delay(1);
	P0=0xFF;
	P2=(P2&0x1F)|0x00;
}
//频率获取
void Get_Feq_Ask(void)
{
	TF0 = 0;		//清除TF0标志
	TL0 = 0x00;     //设置定时器初值
	TH0 = 0x00;		//设置定时器初值
	TR0=1;          //计数器开始计数
}

unsigned int Get_Feq_Receive(void)
{
	unsigned int temp=0;
	TR0=0;          //计数器停止计数
	temp=(TH0<<8)|TL0;
	TF0 = 0;		//清除TF0标志
	TL0 = 0x00;     //设置定时器初值
	TH0 = 0x00;		//设置定时器初值
	return(temp);
}
//继电器
void Relay_control(unsigned char on)
{
	unsigned char temp=0;
	if(on>1)
	{
		return;
	}
	if(on==1)
	{
		temp=0x10;
	}
	else
	{
		temp=0x00;
	}
	P2=(P2&0x1F)|0x00;
	P0=temp;
	US_Delay(1);
	P2=(P2&0x1F)|0xA0;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}
//距离获取
unsigned char Get_distance(void)
{
	unsigned char i=0;
	unsigned int temp=0;
	unsigned char result=0;
	TF0 = 0;		//清除TF0标志
	TL0 = 0x00;     //设置定时器初值
	TH0 = 0x00;		//设置定时器初值
	
	TR0=1;          //定时器开始计时
	for(i=0;i<5;i++)
	{
		P10=1;
		US_Delay(12);
		P10=0;
		US_Delay(12);
	}
	//while((P11==1)&&(TF0==0));//在没有收到超声波同时计数器还没有溢出
	TR0=0;          //定时器停止计时
	temp=(TH0<<8)|(TL0);
	if(TF0==1)
	{
		result=93;
	}
	else
	{
		result=(unsigned char)(temp*0.0014f);
	}
	TF0 = 0;		//清除TF0标志
	TL0 = 0x00;     //设置定时器初值
	TH0 = 0x00;		//设置定时器初值
	return(result);
}