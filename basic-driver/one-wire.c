void US_Delay(unsigned int us)//为阻塞延时 1T模式 11.0592MHz
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

unsigned char DS18B20_Init(void)
{
	unsigned char ack=0;
	P14=1;
	US_Delay(10);//延时10us,先保证P14高电平稳定
	P14=0;
	US_Delay(520);//延时520us，让温度传感器稳定读取电平
	P14=1;
	US_Delay(57);//等待温度传感器的拉低电平应答
	ack=P14;
	ack=!ack;  //温度传感器低电平应答有效
	US_Delay(200);//等待温度传感器的低电平应答时间结束，防止与以后的指令冲突
	return(ack);
}

void DS18B20_SendByte(unsigned char Byte)
{
	unsigned char i=0;//循环变量的声明
	P14=1;
	US_Delay(10);//延时10us,先保证P14高电平稳定
	for(i=0;i<8;i++)//低位先行
	{
		if((Byte>>i)&0x01)//1
		{
			P14=0;//拉低提供下降沿时钟
			US_Delay(6);//给传感器检测下降边沿时间
			P14=1;//拉高写1
			US_Delay(55);//传感读取高电平延时时间
		}
		else//0
		{
			P14=0;//拉低提供下降沿时钟
			US_Delay(60);//给传感器检测时间，同时检测低电平
			P14=1;//拉高便于下一次下降延时钟
			US_Delay(5);//高电平保持时间		
		}
	}
	P14=1;//单片机释放总线
}

unsigned char DS18B20_ReadByte(void)
{
	unsigned char i=0;//循环变量的声明
	unsigned char temp=0;//接收变量
	P14=1;
	US_Delay(10);//延时10us,保证P14高电平稳定
	for(i=0;i<8;i++)//低位先行
	{
		temp>>=1;//先左移，接收temp只用左移7次,否则数据为溢出
		P14=0;//拉低P14，提供下降延时钟
		US_Delay(2);//给从机提供反应时间
		P14=1;//主机释放总线，供从机应答
		US_Delay(8);//等待P14电平稳定，但是延时不宜过长，因为温度传感器的电平（低电平）不能一直保持
		if(P14)
		{
			temp|=0x80;
		}
		US_Delay(55);//延时，每一个比特的读时序必须大于60us
	}
	P14=1;//确实P14总线释放
	return(temp);
}