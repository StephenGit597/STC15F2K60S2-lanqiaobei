void LED_Display(unsigned char Byte)
{
	P2=(P2&0x0F)|0x00;//先失能
	P0=0xFF;//先关闭所有LED灯，防止P0的处置造成LED灯误点亮
	US_Delay(1);//等待稳定
	P0=~Byte;
	US_Delay(1);//等待稳定
	P2=(P2&0x0F)|0x80;//启动LED锁存器的数据使能
	US_Delay(3);//等待LED的D触发器稳定获取显示数据
	P2=(P2&0x0F)|0x00;//后失能
	P0=0x00;//最后P0 回归0x00
}