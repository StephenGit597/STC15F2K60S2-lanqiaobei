#ifndef basic_module_h
#define basic_module_h

#include"STC15F2K60S2.H"
#include"basic_driver.h"
//时间变量声明
extern unsigned long xdata currenttim;//时间戳
extern unsigned char xdata hour;//小时
extern unsigned char xdata minute;//分钟
extern unsigned char xdata second;//秒
//数码管段码声明
extern unsigned char code segcode[18];

//定时器初始化
void Timer1Init(void);
void Timer0Init(void);
//延时函数定义：毫秒，微秒
void MS_Delay(unsigned int ms);
void US_Delay(unsigned int us);
//获取电压
void Get_V_ask(void);
unsigned char Get_V_Receive(void);
//获取温度
void Get_temperature_ask(void);
unsigned char Get_temperature_receive(void);
//获取时间
void DS1302_Set_Time(unsigned char hour,unsigned char minute,unsigned char second);
void DS1302_Get_Time(void);
//获取频率
void Get_Fequence_ask(void);
unsigned int Get_Fequence_receive(void);
//LED
void LED_display(unsigned char Byte);
//数码管
void Seg_display(unsigned char sel,unsigned char Byte,unsigned char dot);
//矩阵按键
unsigned char scan_martix_key(void);

#endif