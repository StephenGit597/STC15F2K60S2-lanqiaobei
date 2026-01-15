void Buffer_and_Relay_Control(unsigned char a,unsigned char b)//继电器 蜂鸣器控制函数
{
	unsigned char temp=0;//控制变量声明
	if((a>1)|(b>1))//输入非法直接返回
	{
		return;
	}
	P2=(P2&0x0F)|0x00;//先失能
	P0=0x00;//先不使能任何外设，防止误操作
	US_Delay(1);//等待稳定
	if(a==1)//继电器开关
	{
		temp|=0x10;
	}
	//注：必须使用或，如果单独赋值，那么同时对蜂鸣器和继电器的操作会冲突
	if(b==1)//蜂鸣器开关
	{
		temp|=0x40;
	}
	P0=temp;//准备好数据
	US_Delay(1);//等待稳定
	P2=(P2&0x0F)|0xA0;//选择功率外设控制D触发器，使能读取
	US_Delay(3);//让D触发器读到的电平稳定
	P2=(P2&0x0F)|0x00;//后失能
	P0=0x00;//重新设置P0为0x00;
}