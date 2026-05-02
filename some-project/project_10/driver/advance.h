#ifndef advance_h
#define advance_h
//STC15标准头文件
#include"STC15F2K60S2.H"
//I2C延迟时间常量
//_nop_()函数头文件引用
#include"intrins.h"
//延时函数头文件引用
#include"basic_driver.h"
#define DELAY_TIME	5
//蓝桥杯开发板AT24C02地址
#define AT24C02_Address 0xA0
//I2C引脚常量定义
sbit scl=P2^0;
sbit sda=P2^1;
//基本函数声明
//I2C部分
static void I2C_Delay(unsigned char n);
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);
//AT24C02读写函数
void AT24C20_Write(unsigned char addr,unsigned char *dat,unsigned char len);
void AT24C02_Read(unsigned char addr,unsigned char *dat,unsigned char len);
#endif