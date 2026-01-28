#ifndef display_h
#define display_h

#include"STC15F2K60S2.H"
#include"tim.h"
#include"ds1302.h"
#include"onewire.h"
#include"Key.h"
#include"distance.h"

extern code unsigned char display1[135];//图案显示数据
extern code unsigned char num[10][3];//0,1,2,3,4,5,6,7,8,9字模
extern code unsigned char Time[22];//TIME 显示字符
extern code unsigned char Tempature[42];//Tempature 显示字符
extern unsigned char code Distance[33];//Distance显示字符

extern unsigned char xdata pre_hour;//闹钟预定时间
extern unsigned char xdata pre_minute;//闹钟预定时间
extern unsigned char xdata pre_second;//闹钟预定时间
extern unsigned char xdata Clock_flag;//闹钟开关 0关闭，1打开
extern unsigned char xdata Clock_ON;//闹钟响标志位，防止按键声效误关闭闹钟

extern float xdata high_tempature;//高温预警温度
extern float xdata low_tempature;//低温预警温度
extern unsigned char xdata temp_flag;//温度报警开关
extern unsigned char xdata alarm_mode;//s设置温度报警模式 0为高温报警 1为低温报警 2为高低温报警

extern unsigned char xdata distance_ON;//距离报警标志位
extern float xdata max_distance;//近距预警距离
extern float xdata min_distance;//远距预警距离
extern unsigned char xdata distance_flag;//距离报警开关
extern unsigned char xdata distance_alarm_mode;//设置距离报警模式 0为近距报警 1为远距报警 2为近远距报警

extern unsigned char xdata Time_display[29];//时间显示缓冲区
extern unsigned char xdata Tempature_display[17];//温度显示缓冲区
extern unsigned char xdata Distance_display[17];//距离显示缓冲区

void Line_Display(unsigned char sel,unsigned char Byte);//单行驱动函数
void long_display(unsigned char delay);//长图显示函数
void TimeDisplay(delay);//时间显示函数
void Tempature_Display(delay);//温度显示函数
void Distance_Display(unsigned char delay);//距离显示函数
#endif