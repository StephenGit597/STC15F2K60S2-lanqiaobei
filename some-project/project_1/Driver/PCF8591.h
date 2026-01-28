#ifndef PCF8591_h
#define PCF8591_h

#include"STC15F2K60S2.H"
#include"tim.h"
sbit SCL=P2^0;
sbit SDA=P2^1;
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(unsigned char Byte);
unsigned char I2C_ReceiveByte(void);
void I2C_SendAck(unsigned char ack);
unsigned char I2C_Waitack();
unsigned char PCF8591(unsigned char mode);
void PCF8591_Analog(unsigned char V);
#endif