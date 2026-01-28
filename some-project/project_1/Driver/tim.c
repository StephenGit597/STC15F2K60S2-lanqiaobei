#include"tim.h"
unsigned long idata currenttim=0;
unsigned long idata now=0;

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;//开启定时器0中断标志
}

void Timer1Init(void)//定时器1初始化,但不开始计时，用作测距计时使用
{
	AUXR|=0x40;//配置为12T模式
	TMOD&=0x0F;//保留定时器0的配置
	TMOD|=0x10;//配置为16T手动重装载模式
	TF1=0;//溢出标志位清零
	TL1=0;//低位清零
	TH1=0;//高位清零
}

void Time0(void) interrupt 1
{
	currenttim++;//时间全局变量
}

void MS_Delay(unsigned int ms)//阻塞毫秒级延时
{
	unsigned long prev=currenttim+ms;
	while(prev>currenttim);
}

void US_Delay(unsigned int us)//阻塞微秒延时,延时约等于1微秒 1T模式下 12MHz下
{
	unsigned int i=0;
	for(i=0;i<us;i++)//八个_nop_();
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

void Pin_Init(void)//引脚初始化,P0数据选择选择为无 （未挂载设备）
{
	P2=(P2&0x0F)|0x00;
	P0=0xFF;
	P1=0x00;//P1清零
}