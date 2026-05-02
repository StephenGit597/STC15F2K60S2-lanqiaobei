驱动代码（官方版本）
unsigned char DS18B20_Init(void)
{
	unsigned char ack=0;
	BUS=1;
	US_Delay(10);//延时10us,先保证P14高电平稳定
	BUS=0;
	US_Delay(520);//延时520us，让温度传感器稳定读取电平
	BUS=1;
	US_Delay(57);//等待温度传感器的拉低电平应答
	ack=BUS;
	ack=!ack;  //温度传感器低电平应答有效
	US_Delay(200);//等待温度传感器的低电平应答时间结束，防止与以后的指令冲突
	return(ack);
}

void DS18B20_SendByte(unsigned char Byte)
{
	unsigned char i=0;//循环变量的声明
	BUS=1;
	US_Delay(10);//延时10us,先保证P14高电平稳定
	for(i=0;i<8;i++)//低位先行
	{
		if((Byte>>i)&0x01)//1
		{
			BUS=0;//拉低提供下降沿时钟
			US_Delay(6);//给传感器检测下降边沿时间
			BUS=1;//拉高写1
			US_Delay(55);//传感读取高电平延时时间
		}
		else//0
		{
			BUS=0;//拉低提供下降沿时钟
			US_Delay(60);//给传感器检测时间，同时检测低电平
			BUS=1;//拉高便于下一次下降延时钟
			US_Delay(5);//高电平保持时间		
		}
	}
	BUS=1;//单片机释放总线
}

unsigned char DS18B20_ReadByte(void)
{
	unsigned char i=0;//循环变量的声明
	unsigned char temp=0;//接收变量
	BUS=1;
	US_Delay(10);//延时10us,保证P14高电平稳定
	for(i=0;i<8;i++)//低位先行
	{
		temp>>=1;//先左移，接收temp只用左移7次,否则数据为溢出
		BUS=0;//拉低P14，提供下降延时钟
		US_Delay(2);//给从机提供反应时间
		BUS=1;//主机释放总线，供从机应答
		US_Delay(8);//等待P14电平稳定，但是延时不宜过长，因为温度传感器的电平（低电平）不能一直保持
		if(BUS)
		{
			temp|=0x80;
		}
		US_Delay(55);//延时，每一个比特的读时序必须大于60us
	}
	BUS=1;//确实P14总线释放
	return(temp);
}

自己写的部分：
//获取温度
void Get_temperature_ask(void)
{
	DS18B20_Init();//初始化
	DS18B20_SendByte(0xCC);//跳过地址模式
	DS18B20_SendByte(0x44);//发起温度转换命令
}

unsigned char Get_temperature_receive(void)
{
	unsigned char low=0;
	unsigned char high=0;
	unsigned int raw_data=0;
	float temperature=0.0f;
	
	DS18B20_Init();//初始化
	DS18B20_SendByte(0xCC);//跳过地址模式
	DS18B20_SendByte(0xBE);//发起读取命令
	
	low=DS18B20_ReadByte();//获取低八位
	high=DS18B20_ReadByte();//获取高八位
	raw_data=(high<<8)|low;
	
	if(raw_data&0x8000)//负值取绝对值
	{
		raw_data=~raw_data+1;//取反加一
	}
	temperature=((raw_data>>4)&0x00FF)+(raw_data&0x000F)*0.0625f;
	return((unsigned char)(temperature));
}

主函数调用例程：
//触发后温度采集
		if((trigger==1)&&(temperature_complete==0)&&(temperature_ask_ok==0))//触发后，在温度采集没有完成下，发送温度转换指令
		{
			Get_temperature_ask();//发起温度转换指令
			temperature_ask_ok=1;//温度转换指令发送标志位置1
			temperature_now=currenttim;//更新温度采集时间戳
		}
		if((trigger==1)&&(temperature_complete==0)&&(temperature_ask_ok==1)&&(currenttim-temperature_now>750))//读取温度
		{
			prev_temperature=temperature;//把上一次读取到的温度保存到缓冲区
			temperature=Get_temperature_receive();//获取当前温度
			if(temperature>Set_temperature)//测到的温度大于设定温度
			{
				temperature_alarm=1;//温度报警标志位置一
			}
			else
			{
				temperature_alarm=0;//清除温度报警标志位
			}
			if(temperature>prev_temperature)//检测到温度上升
			{
				temperature_up_flag=1;//温度上升标志位置1
			}
			else
			{
				temperature_up_flag=0;//清除温度上升标志位
			}
			if(temperature>MAX_temperature)//采用动态比较更新法获得当前温度的最大值
			{
				MAX_temperature=temperature;//更新温度最大值
			}
			temperature_num++;//温度采集次数加一
			average_temperature=(average_temperature*(temperature_num-1)+temperature)/temperature_num;//更新温度平均值（需要在温度采集数自增后计算）
			temperature_complete=1;//表示温度采集已经完成，一次触发仅采集一次温度（防止重复采集）
			temperature_ask_ok=0;//清除温度采集指令发送标志位，方便下一次采集
		}