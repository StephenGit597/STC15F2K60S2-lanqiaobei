#include"onewire.h"

unsigned char onewire_Start(void)
{
	unsigned char ack=0;
	BUS=1;//初始BUS一定是高电平
	US_Delay(10);
	BUS=0;//发送起始信号
	US_Delay(520);//建议延时500us以上,800us以下
	BUS=1;//释放总线，等待从机应答
	US_Delay(50);//等待从机拉低电平
	ack=!BUS;//高电平表示未应答 低电平表示应答成功
	US_Delay(200);//等待从机应答结束，不宜过短
	return(ack);
}

void onewire_SendByte(unsigned char Byte)
{//通讯规则LSB
	unsigned char i=0;
	BUS=1;//初始BUS一定是高电平
	US_Delay(5);
	for(i=0;i<8;i++)
	{
		if((Byte>>i)&0x01)//写1
		{
			BUS=0;//拉低提供时钟，表示通讯开始
			US_Delay(6);//写一拉低6us
			BUS=1;//恢复总线
			US_Delay(55);//延时55us 等待从机读取电平并且完成时序
		}
		else//写0
		{
			BUS=0;//拉低提供时钟，表示通讯开始
			US_Delay(60);//写一拉低60us
			BUS=1;//恢复总线
			US_Delay(5);//延时5us 完成时序
		}
	}
	BUS=1;//建议释放总线
}

unsigned char onewire_ReadByte(void)
{//通讯规则LSB
	unsigned char i=0;
	unsigned char temp=0;
	BUS=1;//初始BUS一定是高电平
	US_Delay(5);
	for(i=0;i<8;i++)
	{
		temp>>=1;//一定要先空移动，否则最低位要被溢出
		BUS=0;//拉低提供时钟，表示通讯开始
		US_Delay(2);//等待从机反应
		BUS=1;//恢复总线
		US_Delay(8);//等待数据电平稳定，但是建议不宜过长，否则错过有效电平（低电平）
		if(BUS)//判断高电平
		{
			temp|=0x80;
		}
		US_Delay(55);//每一个比特的读取时序大于60us，延时完成时序
	}
	BUS=1;//建议释放总线
	return(temp);
}

float Get_tempature(void)
{
	unsigned char low=0;
	unsigned char high=0;
	unsigned int raw=0;
	float result=0;
	
	onewire_Start();
	onewire_SendByte(0xCC);//跳过地址模式
	onewire_SendByte(0x44);//开启温度转换
	
	MS_Delay(30);
	
	onewire_Start();
	onewire_SendByte(0xCC);//跳过地址模式
	onewire_SendByte(0xBE);//发送读取命令
	
	low=onewire_ReadByte();//低位优先
	high=onewire_ReadByte();
	raw=(high<<8)|low;//// S S S S x x x x x x x x | x x x x
	//S符号位，x前为整数位，x后为小数为 精度0.0625
	if(raw&0x8000)//判断温度绝对值
	{
		raw=~raw+1;
	}
	result=(raw>>4)+(raw&0x000F)*0.0625;//本程序仅支持显示正的二位小数点温度，
	return(result);
}