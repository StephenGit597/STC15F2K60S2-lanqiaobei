#ifndef basic_driver_h
#define basic_driver_h
//STC15标准头文件
#include"STC15F2K60S2.H"
//时间戳变量说明
//_nop_()函数头文件引用
#include"intrins.h"
//时间戳
extern volatile unsigned long xdata currenttim;
//数码管显示数组
extern unsigned char code segcode[16];
//函数声明
void Timer1Init(void);//定时器1初始化
void MS_Delay(unsigned char ms);//毫秒延时函数
void US_Delay(unsigned char us);//微秒延时函数
unsigned char Scan_Key(void);//扫描矩阵按键函数
void LED_Display(unsigned char Byte);//LED显示函数
void Seg_Display(unsigned char sel,unsigned char Byte,unsigned char dot);//数码管显示函数
void Relay_Control(unsigned char on);//继电器控制函数
#endif