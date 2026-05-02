代码实现
继电器控制
继电器0x10 蜂鸣器0x40 
void Relay_control(unsigned char on)
{
	if(on>1)
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	US_delay(1);
	if(on)
	{
		P0=0x10;
	}
	else
	{
		P0=0x00;
	}
	US_delay(1);
	P2=(P2&0x1F)|0xA0;
	US_delay(3);
	P2=(P2&0x1F)|0x00;
}
