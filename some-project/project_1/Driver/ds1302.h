#ifndef ds1302_h
#define ds1302_h
#include"STC15F2K60S2.H"
#include"tim.h"
sbit SCK=P1^7;
sbit RST=P1^3;
sbit SCA=P2^3;
extern unsigned char idata hour;
extern unsigned char idata minute;
extern unsigned char idata second;
void ds1302_WriteByte(unsigned char addr,unsigned char Byte);
unsigned char ds1302ReadByte(unsigned char addr);
void Set_Time(unsigned char hour,unsigned char minute,unsigned char second);
void Get_Time(void);
#endif