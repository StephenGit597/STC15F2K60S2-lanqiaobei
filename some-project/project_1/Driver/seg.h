#ifndef seg_h
#define seg_h

#include"STC15F2K60S2.H"
#include"tim.h"
#include"ds1302.h"
#include"PCF8591.h"
#include"onewire.h"
#include"distance.h"
#include"Key.h"

extern code unsigned char segcode[17];//数码管段码表
extern unsigned char xdata pre_hour;
extern unsigned char xdata pre_minute;
extern unsigned char xdata pre_second;
extern unsigned char xdata Clock_flag;//闹钟开关 0关闭，1打开
extern unsigned char xdata Clock_ON;//闹钟响标志位，防止按键声效误关闭闹钟

extern float xdata high_tempature;//高温预警温度
extern float xdata low_tempature;//低温预警温度
extern unsigned char xdata temp_flag;//温度报警开关
extern unsigned char xdata alarm_mode;//s设置温度报警模式 0为高温报警 1为低温报警 2为高低温报警
extern unsigned char xdata temp_ON;//温度报警标志位

extern unsigned char xdata distance_ON;//距离报警标志位
extern float xdata max_distance;//近距预警距离
extern float xdata min_distance;//远距预警距离
extern unsigned char xdata distance_flag;//距离报警开关
extern unsigned char xdata distance_alarm_mode;//设置距离报警模式 0为近距报警 1为远距报警 2为近远距报警

extern unsigned char xdata V_max;//高压预警电压
extern unsigned char xdata V_min;//低压预警电压
extern unsigned char xdata V_flag;//电压报警开关
extern unsigned xdata V_alarm_mode;//电压预警模式
extern unsigned char xdata V_ON;//电压报警触发标志位

void Segdisplay(unsigned char sel,unsigned char Byte,unsigned char dot);
void Time_Seg(unsigned char delay);
void Tempature_Seg(unsigned char delay);
void Volt_Seg(unsigned char mode,unsigned char delay);
void distance_Seg(unsigned char delay);
#endif