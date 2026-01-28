#include"led.h"

void LED_Display(unsigned char Byte)//注意 由与D锁存器 引脚电平不必一直保持
{
	P2=(P2&0x1F)|0x00;
	US_Delay(1);
	P0=~Byte;
	US_Delay(1);
	P2=(P2&0x1F)|0x80;
	US_Delay(5);
	P2=(P2&0x1F)|0x00;
	P0=0x00;
}