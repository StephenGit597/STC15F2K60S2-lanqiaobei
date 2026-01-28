#include"buffer_and_relay.h"

void Buffer_and_Relay_control(unsigned char a,unsigned char b)
{
	unsigned char temp=0;
	if((a>1)|(b>1))//检查输入是否合法
	{
		return;
	}
	if(a==1)//控制蜂鸣器
	{
		temp|=0x40;//控制蜂鸣器
	}
	if(b==1)//控制继电器
	{
		temp|=0x10;//控制继电器
	}
	P0=temp;//P0给出控制向量
	US_Delay(1);//等待电平稳定
	P2=(P1&0x1F)|0xA0;//选择功率器件控制寄存器
	US_Delay(1);//等待D触发器读取数据
	P2=(P1&0x1F)|0x00;//选择0通道
	P0=0x00;//恢复P0引脚电平
}