#ifndef basic_driver_h
#define basic_driver_h

#include"STC15F2K60S2.H"
#include"intrins.h"
#include"basic_module.h"

//引脚位声明
//IIC
sbit SCK=P2^0;
sbit SDA=P2^1;
//onewire
sbit BUS=P1^4;
//DS1302
sbit RST=P1^3;
sbit SCL=P1^7;
sbit SCA=P2^3;

//IIC电压采集部分
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(unsigned char Byte);
unsigned char I2C_ReceiveByte(void);
void I2C_SendAck(unsigned char ack);
unsigned char I2C_ReceiveAck(void);
//DS18B20部分
unsigned char DS18B20_Init(void);
void DS18B20_SendByte(unsigned char Byte);
unsigned char DS18B20_ReadByte(void);
//DS1302时间获取部分
void DS1302_SendByte(unsigned char addr,unsigned char Byte);
unsigned char DS1302_ReadByte(unsigned char addr);

#endif