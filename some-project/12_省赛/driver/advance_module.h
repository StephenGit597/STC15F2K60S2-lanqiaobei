#ifndef advance_module_h
#define advance_module_h

#include"STC15F2K60S2.H"
#include"basic_module.h"

sbit SDA=P2^1;
sbit SCL=P2^0;
sbit BUS=P1^4;

void IIC_Start(void);
void IIC_Stop(void);
void IIC_SendByte(unsigned char Byte);
unsigned char IIC_Waitack(void);

unsigned char onewire_Start(void);
void onewire_SendByte(Byte);
unsigned char onewire_Receive(void);

void Get_temperature_Send(void);
float Get_temperature_Receive(void);
void Out_V(unsigned char V);

#endif