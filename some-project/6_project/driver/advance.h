#ifndef advance_h
#define advance_h

#include"STC15F2K60S2.H"

extern unsigned char xdata hour;
extern unsigned char xdata minute;
extern unsigned char xdata second;

sbit SCA=P2^3;
sbit SCK=P1^7;
sbit RST=P1^3;
sbit BUS=P1^4;

void DS1302_SendByte(unsigned char addr,unsigned char Byte);
unsigned char DS1302_ReadByte(unsigned char addr);
unsigned char DS18B20_Init(void);
void DS18B20_SendByte(unsigned char Byte);
unsigned char DS18B20_ReadByte(void);
void ds1302_Set_time(unsigned char hour,unsigned char minute,unsigned char second);
void ds1302_Get_time(void);
void Get_temperature_ask(void);
unsigned char Get_temperature_receive(void);

#endif