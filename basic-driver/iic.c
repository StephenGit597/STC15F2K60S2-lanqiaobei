void US_Delay(unsigned int us)//为阻塞延时 1T模式 外部晶振11.0592MHz
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

void I2C_Start(void)
{
	//空闲状态
	P21=1;//SDA
	P20=1;//SCK
	US_Delay(3);
	
	P21=0;//拉低SDA，开始通讯
	US_Delay(3);//延时给从机反应时间
	P20=0;//拉低时钟线防止iic误操作
}

void I2C_Stop(void)
{
	P20=0;//先拉低时钟，防止SDA电平不确定误操作
	P21=0;//先保持数据为低
	US_Delay(3);
	P20=1;
	US_Delay(3);//给从机反应时间
	P21=1;
}

void I2C_SendByte(unsigned char Byte)
{
	unsigned char i=0;//循环变量声明
	P20=0;//先拉低SCK，防止误操作
	P21=0;
	US_Delay(2);
	for(i=0;i<8;i++)
	{
		P21=(Byte>>(7-i))&0x01;//准备数据，注意I2C是高位优先
		US_Delay(2);//等待SDA电平稳定
		P20=1;
		US_Delay(3);//给从机反应时间
		P20=0;
	}
	P20=0;//确保I2C_主机不发送接收应答或数据时SCK为低，防止误操作
	P21=1;//释放总线，供从机应答
}

unsigned char I2C_ReceiveByte(void)
{
	unsigned char i=0;//循环变量
	unsigned char temp=0;//函数的返回值
	P20=0;//先拉低SCK，防止I2C误操作
	P21=1;//释放总线，供从机发送数据
	US_Delay(3);//初始化
	for(i=0;i<8;i++)
	{
		temp<<=1;//先左移，接收数据temp只需要移7次，否则最高位会被溢出
		P20=1;//拉高SCK
		US_Delay(3);//等待从机向SDA发送电平稳定
		if(P21)
		{
			temp|=0x01;
		}
		P20=0;//拉低SCK读取结束
		US_Delay(2);
	}
	P20=0;//确保IIC使用中不误触发
	P21=1;//释放总线，等待主机去发送应答
	return(temp);
}

void I2C_SendAck(unsigned char ack)
{
	P20=0;
	P21=0;
	US_Delay(1);//初始状态
	
	P21=!ack;//在SCK为低电平期间准备应答信号，低电平有效
	US_Delay(1);
	P20=1;//拉高SCK
	US_Delay(2);
	P20=0;//拉低SCK
	
	P21=1;//释放SDA,让从机继续发送数据
}

unsigned char I2C_ReceiveAck(void)
{
	unsigned char ack=0;
	P20=0;
	P21=0;
	US_Delay(1);//初始状态
	
	P20=1;//拉高准备读取
	US_Delay(1);//等待SDA电平稳定后读取应答
	ack=P21;//读取应答信号，低电平有效
	ack=!ack;
	P20=0;//拉低表示接收完毕
	
	P21=1;//释放总线，便于主机下一次发送数据
	return(ack);
}