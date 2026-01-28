#ifndef onewire_h
#define onewire_h

#include"STC15F2K60S2.H"
#include"tim.h"
sbit BUS=P1^4;
unsigned char onewire_Start(void);
void onewire_SendByte(unsigned char Byte);
unsigned char onewire_ReadByte(void);
float Get_tempature(void);

#endif