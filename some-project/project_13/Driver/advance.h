#ifndef advance_h
#define advance_h

#include"STC15F2K60S2.h"
#include"intrins.h"

sbit BUS=P1^4;
sbit RST=P1^3;
sbit CLK=P1^7;
sbit SDA=P2^3;

extern unsigned char xdata hour;
extern unsigned char xdata minute;
extern unsigned char xdata second;

unsigned char onewire_Start(void);
void onewire_SendByte(unsigned char Byte);
unsigned char onewire_ReceiveByte(void);
void ds1302_Write(unsigned char addr,unsigned char Byte);
unsigned char ds1302_Read(unsigned char addr);
void Get_tempature_ask(void);
float Get_tempature_receive(void);
void Set_Time(unsigned char hour,unsigned char minute,unsigned char second);
void Get_Time(void);

#endif