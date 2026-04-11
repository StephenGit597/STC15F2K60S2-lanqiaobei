//系统主时钟初始化(使用定时器1)
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

void Timer0Init(void)//配置为P34计数器模式
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//保持定时器1的配置不变
	TMOD |= 0x05;		//设置定时器0为16位不可重装载计数器模式
	TF0 = 0;		//清除TF0标志
	TL0=0x00;       //清除计数器不确定的值
	TH0=0x00;       //清除计数器不确定的值
}

void Timer1(void) interrupt 3
{
    currenttim++;
}

void MS_Delay(unsigned int ms)//毫秒级延时
{
	unsigned long prev=currenttim+ms;
	while(prev>=currenttim);
}

void US_Delay(unsigned int us)//微秒级延时
{
	unsigned int i=0;
	for(i=0;i<us;i++)
	{
		_nop_();
	}
}