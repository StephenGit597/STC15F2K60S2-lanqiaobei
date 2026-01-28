#ifndef tim_h
#define tim_h

#include"STC15F2K60S2.H"
#include"intrins.h"
void Timer0Init(void);
void Timer1Init(void);
void MS_Delay(unsigned int ms);
void US_Delay(unsigned int us);
void Pin_Init(void);
extern unsigned long idata currenttim;//全局变量，毫秒中断加一，表示机器时间
extern unsigned long idata now;//获取当前时间刻，便于定时延时循环判断

#endif