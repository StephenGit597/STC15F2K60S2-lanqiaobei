#ifndef buffer_and_relay_h
#define buffer_and_relay_h

#include"STC15F2K60S2.H"
#include"tim.h"

extern unsigned char xdata Clock_ON;//闹钟响标志位，防止按键声效误关闭闹钟
extern unsigned char xdata temp_ON;//温度报警已触发标志位，防止按键声效误关闭温度报警
extern unsigned char xdata distance_ON;//距离报警标志位
extern unsigned char xdata V_ON;//电压报警触发标志位
//a,控制蜂鸣器;b,控制继电器
void Buffer_and_Relay_control(unsigned char a,unsigned char b);
#endif