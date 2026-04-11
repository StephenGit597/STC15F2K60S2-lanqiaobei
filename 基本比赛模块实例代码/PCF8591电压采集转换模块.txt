PCF8591 I2C驱动代码（官方提供）
void I2C_Start(void)
{
	//空闲状态
	SDA=1;//SDA
	SCK=1;//SCK
	US_Delay(3);
	
	SDA=0;//拉低SDA，开始通讯
	US_Delay(3);//延时给从机反应时间
	SCK=0;//拉低时钟线防止iic误操作
}

void I2C_Stop(void)
{
	SCK=0;//先拉低时钟，防止SDA电平不确定误操作
	SDA=0;//先保持数据为低
	US_Delay(3);
	SCK=1;
	US_Delay(3);//给从机反应时间
	SDA=1;
}

void I2C_SendByte(unsigned char Byte)
{
	unsigned char i=0;//循环变量声明
	SCK=0;//先拉低SCK，防止误操作
	SDA=0;
	US_Delay(2);
	for(i=0;i<8;i++)
	{
		SDA=(Byte>>(7-i))&0x01;//准备数据，注意I2C是高位优先
		US_Delay(2);//等待SDA电平稳定
		SCK=1;
		US_Delay(3);//给从机反应时间
		SCK=0;
	}
	SCK=0;//确保I2C_主机不发送接收应答或数据时SCK为低，防止误操作
	SDA=1;//释放总线，供从机应答
}

unsigned char I2C_ReceiveByte(void)
{
	unsigned char i=0;//循环变量
	unsigned char temp=0;//函数的返回值
	SCK=0;//先拉低SCK，防止I2C误操作
	SDA=1;//释放总线，供从机发送数据
	US_Delay(3);//初始化
	for(i=0;i<8;i++)
	{
		temp<<=1;//先左移，接收数据temp只需要移7次，否则最高位会被溢出
		SCK=1;//拉高SCK
		US_Delay(3);//等待从机向SDA发送电平稳定
		if(SDA)
		{
			temp|=0x01;
		}
		SCK=0;//拉低SCK读取结束
		US_Delay(2);
	}
	SCK=0;//确保IIC使用中不误触发
	SDA=1;//释放总线，等待主机去发送应答
	return(temp);
}

void I2C_SendAck(unsigned char ack)
{
	SCK=0;
	SDA=0;
	US_Delay(1);//初始状态
	
	SDA=!ack;//在SCK为低电平期间准备应答信号，低电平有效
	US_Delay(1);
	SCK=1;//拉高SCK
	US_Delay(2);
	SCK=0;//拉低SCK
	
	SDA=1;//释放SDA,让从机继续发送数据
}

unsigned char I2C_ReceiveAck(void)
{
	unsigned char ack=0;
	SCK=0;
	SDA=0;
	US_Delay(1);//初始状态
	
	SCK=1;//拉高准备读取
	US_Delay(1);//等待SDA电平稳定后读取应答
	ack=SDA;//读取应答信号，低电平有效
	ack=!ack;
	SCK=0;//拉低表示接收完毕
	
	SDA=1;//释放总线，便于主机下一次发送数据
	return(ack);
}

自己写的代码
void Get_V_ask(void)
{
	I2C_Start();
	I2C_SendByte(0x90);写0x90 读0x91
	I2C_ReceiveAck();
	
	I2C_SendByte(0x41);0x41光敏电阻  0x43可调电位器
	I2C_ReceiveAck();
	I2C_Stop();
}

unsigned char Get_V_Receive(void)
{
	unsigned char V=0;
	
	I2C_Start();
	I2C_SendByte(0x91);写0x90 读0x91
	I2C_ReceiveAck();
	
	V=I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();
	
	return(V);
}

//电压转换
void V_Trans(unsigned char V)
{
	I2C_Start();
	I2C_SendByte(0x90);
	I2C_ReceiveAck();
	
	I2C_SendByte(0x41);
	I2C_ReceiveAck();
	
	I2C_SendByte(V);
	I2C_ReceiveAck();
	I2C_Stop();
}

主函数调用示例：
		if((trigger==0)&&(V_ask_ok==0))//在没有触发的情况下，发送电压转换指令
		{
			Get_V_ask();//发送电压转换指令
			V_ask_ok=1;//表示已经发送电压转换指令
			V_now=currenttim;//更新电压转换时间戳
		}
		if((trigger==0)&&(V_ask_ok==1)&&(currenttim-V_now>100))//在没有触发的情况下，发送电压转换指令后，读取电压（100ms）后
		{
			V=Get_V_Receive();//获取电压
			V_ask_ok=0;//清除电压指令发送完成标志位，便于下一次转换
			prev_Inu_flag=Inu_flag;//把上次光照状态调入缓冲区保存
			if(V<121)//状态标记为暗
			{
				Inu_flag=0;//0表示暗
			}
			else if(V<174)//状态标记为适中
			{
				Inu_flag=1;//1表示适中
			}
			else//表示亮
			{
				Inu_flag=2;//2表示亮
			}
			V_complete=1;//电压转换完成标志位
		}
