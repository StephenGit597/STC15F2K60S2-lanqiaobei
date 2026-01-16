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