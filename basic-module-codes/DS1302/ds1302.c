//注意单片机上电电平不稳定建议初始要
P13=0;//（必须），上电DS1302先失能
P17=0;//（选择），时钟引脚初始化为低电平
P23=0;//（选择），数据引脚初始化为低电平
static unsigned char hour=0;
static unsigned char minute=0;
static unsigned char second=0;
//时间小时hour 分钟minute 秒second 采用全局变量
//常用地址
//写
//0x80-秒
//0x82-分钟
//0x84-小时
//0x8E-写保护处理 注0x00关闭写保护 0x80开启写保护

//读
//0x81-秒
//0x83-分钟
//0x85-小时
//存储格式
//八位BCD码
//0 0 0 0 0 0 0 0 
//--十位---个位---

void DS1302_SetTime(unsigned char hour,unsigned char minute,unsigned char second)
{
	unsigned char temp=0;
	if((hour>24)|(minute>60)|(second>60))
	{
		return;
	}
	DS1302_WriteCmd(0x8E,0x00);//关闭写保护
	temp=((hour/10)<<4)|((hour%10)&0x0F);
	DS1302_WriteCmd(0x84,temp);
	temp=((minute/10)<<4)|((minute%10)&0x0F);
	DS1302_WriteCmd(0x82,temp);
	temp=((second/10)<<4)|((second%10)&0x0F);
	DS1302_WriteCmd(0x80,temp);
	DS1302_WriteCmd(0x8E,0x80);//恢复写保护
}

void DS1302_GetTime()
{
	unsigned char value=0;
	value=DS1302_ReadCmd(0x85);
	hour=10*(value>>4)+(value&0x0F);
	value=DS1302_ReadCmd(0x83);
	minute=10*(value>>4)+(value&0x0F);
	value=DS1302_ReadCmd(0x81);
	second=10*(value>>4)+(value&0x0F);
}
