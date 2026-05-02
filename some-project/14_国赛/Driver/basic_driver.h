#ifndef basic_driver_h
#define basic_driver_h
//引用头文件
#include"STC15F2K60S2.H"
#include"intrins.h"
//位定义
//IIC
sbit SDA=P2^1;
sbit SCK=P2^0;
//one_wire
sbit BUS=P1^4;
//变量声明
extern unsigned long currenttim;
//函数声明
//IIC驱动函数
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(unsigned char Byte);
unsigned char I2C_ReceiveAck(void);
//one_wire驱动函数
unsigned char DS18B20_Init(void);
void DS18B20_SendByte(unsigned char Byte);
unsigned char DS18B20_ReadByte(void);
//电压转换
void V_Trans(unsigned char V);
//延时函数
void US_Delay(unsigned int us);
void MS_Delay(unsigned int ms);
//温度获取
void Get_Temperature_ask(void);
float Get_temperature_receive(void);
#endif