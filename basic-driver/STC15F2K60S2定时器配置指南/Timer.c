#定时器0-定时器配置
void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;//打开定时器0开关
}

void Timer0(void) interrupt 1
{
	currenttim++;
}
延时函数
void MS_Delay(unsigned int ms)//为阻塞延时
{
	unsigned long prev=currenttim+ms;
	while(prev>=currenttim);
}

void US_Delay(unsigned int us)//为阻塞延时
{
	volatile unsigned int i=0;
	for(i=0;i<us;i++)
	{
		_nop_();//8个_nop_()
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}
}
#定时器1-定时器配置（自己开发板定时器1配置1T模式延时会不正确）
void Timer1Init(void)		//1毫秒@11.0592MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x66;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;
}

void Timer1(void) interrupt 3
{
	currenttim++;
}
#定时器0配置为计数器，注计数脉冲来自P34，同时如果是定时器1引脚配置为计数器，计数脉冲来自P35
void Timer0_Init(void)
{
	AUXR=0X80;//1分频
	TMOD=0x05;//0 0 0 0 1 0 0 1，模式计数器模式，01需要手动的16位重装载模式
	TH0=0;//TH0初值为0
	TL0=0;//TL0初值为0
	TR0=1;//使能计数器0
	ET0=1//(按需)，如果需要计数器溢出中断则保留
}