liujingtong是我的榜样 我要好好向他学习
基本官驱：
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}
自己要编写的时间读取写入函数：
void DS1302_Set_Time(unsigned char hour,unsigned char minute,unsigned char second)
{
	unsigned char temp=0;
	Write_Ds1302_Byte(0x8E,0x00);
	temp=((hour/10)<<4)|((hour%10)&0x0F);
	Write_Ds1302_Byte(0x84,temp);
	temp=((minute/10)<<4)|((minute%10)&0x0F);
	Write_Ds1302_Byte(0x82,temp);
	temp=((second/10)<<4)|((second%10)&0x0F);
	Write_Ds1302_Byte(0x80,temp);
	Write_Ds1302_Byte(0x8E,0x80);
}

void DS1302_Get_Time(void)
{
	unsigned char temp=0;
	temp=Read_Ds1302_Byte(0x85);
	hour=10*(temp>>4)+(temp&0x0F);
	temp=Read_Ds1302_Byte(0x83);
	minute=10*(temp>>4)+(temp&0x0F);
	temp=Read_Ds1302_Byte(0x81);
	second=10*(temp>>4)+(temp&0x0F);
}
主函数获取时间：
while(1)
{
  //时间获取
		if(currenttim-tim_now>500)//500ms更新一次时间
		{
			DS1302_Get_Time();//获取时间
			tim_now=currenttim;//更新时间标志位
		}
}
