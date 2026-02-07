#ifndef advance_driver_h
#define advance_driver_h

#include"basic.h"
#include"STC15F2K60S2.H"
#include"intrins.h"

#define DELAY_TIME	5

sbit RST=P1^3;
sbit SCK=P1^7;
sbit SDA=P2^3;
sbit sda=P2^1;
sbit scl=P2^0;

extern unsigned char xdata hour;
extern unsigned char xdata minute;
extern unsigned char xdata second;

void Write_Ds1302(unsigned  char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte ( unsigned char address );
static void I2C_Delay(unsigned char n);
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CWaitAck(void);
void DS1302_Set_Time(unsigned char hour,unsigned char minute,unsigned char second);
void DS1302_Get_Time(void);
void PCF8591_Trans_V(unsigned char V);

#endif