#include"basic_module.h"
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
//获取电压
void Get_V_ask(void)
{
	I2C_Start();
	I2C_SendByte(0x90);
	I2C_ReceiveAck();
	
	I2C_SendByte(0x41);
	I2C_ReceiveAck();
	I2C_Stop();
}

unsigned char Get_V_Receive(void)
{
	unsigned char V=0;
	
	I2C_Start();
	I2C_SendByte(0x91);
	I2C_ReceiveAck();
	
	V=I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();
	
	return(V);
}
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
//获取时间
void DS1302_Set_Time(unsigned char hour,unsigned char minute,unsigned char second)
{
	unsigned char temp=0;//BCD码转换缓冲区
	if((hour>23)|(minute>59)|(second>59))
	{
		return;
	}
	DS1302_SendByte(0x8E,0x00);//解除写保护
	temp=((hour/10)<<4)|((hour%10)&0x0F);//小时
	DS1302_SendByte(0x84,temp);
	temp=((minute/10)<<4)|((minute%10)&0x0F);//分钟
	DS1302_SendByte(0x82,temp);
	temp=((second/10)<<4)|((second%10)&0x0F);//秒
	DS1302_SendByte(0x80,temp);
	DS1302_SendByte(0x8E,0x80);//恢复写保护
}

void DS1302_Get_Time(void)
{
	unsigned char temp=0;//BCD码转换缓冲区
	temp=DS1302_ReadByte(0x85);//读取小时
	hour=10*(temp>>4)+(temp&0x0F);
	temp=DS1302_ReadByte(0x83);//读取分钟
	minute=10*(temp>>4)+(temp&0x0F);
	temp=DS1302_ReadByte(0x81);//读取秒
	second=10*(temp>>4)+(temp&0x0F);
}
//获取频率
void Get_Fequence_ask(void)
{
	TL0=0x00;//清除计数器的值，从零开始
	TH0=0x00;//清除计数器的值，从零开始
	TF0=0;//清除计数器0溢出标志
	TR0=1;//计数器0开始计数
}
	
unsigned int Get_Fequence_receive(void)
{
	unsigned int Feq=0;
	TR0=0;//计数器0停止计数
	Feq=(TH0<<8)|TL0;//在1s内触发，频率就是计数器的值
	TF0=0;//清除计数器0溢出标志
	TL0=0x00;//清除计数器的值，从零开始
	TH0=0x00;//清除计数器的值，从零开始
	return(Feq);
}
//LED显示
void LED_display(unsigned char Byte)
{
	P2=(P2&0x1F)|0x00;
	P0=~Byte;
	US_Delay(1);
	P2=(P2&0x1F)|0x80;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
}
//数码管显示
void Seg_display(unsigned char sel,unsigned char Byte,unsigned char dot)
{
	if((sel==0)|(sel>8)|(Byte>17)|(dot>1))
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	P0=0x01<<(sel-1);
	US_Delay(1);
	P2=(P2&0x1F)|0xC0;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
	P0=segcode[Byte];
	if(dot)
	{
		P07=0;
	}
	US_Delay(1);
	P2=(P2&0x1F)|0xE0;
	MS_Delay(1);
	P0=0xFF;
	P2=(P2&0x1F)|0x00;
}
//矩阵键盘扫描
unsigned char scan_martix_key(void)
{
	unsigned char temp=0;//数据缓冲区
	unsigned char value=0;//最终读取按键值
	P44=0;
	P42=1;
	temp=(P3&0x0C)>>2;
	P44=1;
	P42=0;
	temp=~(0xF0|(temp<<2)|((P3&0x0C)>>2));
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