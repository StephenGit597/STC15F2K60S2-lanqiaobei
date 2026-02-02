#ifndef advance_h
#define advance_h

#include"STC15F2K60S2.H"
#include"intrins.h"

sbit SCK=P2^0;
sbit SDA=P2^1;
sbit BUS=P1^4;

void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(unsigned char Byte);
unsigned char I2C_ReceiveByte(void);
void I2C_SendAck(unsigned char ack);
unsigned char I2C_ReceiveAck(void);
unsigned char DS18B20_Init(void);
void DS18B20_SendByte(unsigned char Byte);
unsigned char DS18B20_ReadByte(void);
void Temperature_ask(void);
float Temperature_receive(void);
void Get_Inu_ask(void);
unsigned char Get_Inu_receive(void);

#endif