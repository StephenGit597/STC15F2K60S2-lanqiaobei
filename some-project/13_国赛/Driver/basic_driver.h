#ifndef basic_driver_h
#define basic_driver_h
//STC15F2K60S3标准头文件
#include"STC15F2K60S2.H"
//_nop_()函数头文件
#include"intrins.h"
//时间戳声明
extern volatile unsigned long xdata currenttim;
//重复计数说明
extern volatile unsigned char xdata Re_num;
//PWM参数声明
extern unsigned char xdata PWM_num;
//数码管段码声明 0 1 2 3 4 5 6 7 8 9 A F P H 熄灭
extern unsigned char code segcode[15];
//函数声明
void Timer1Init(void);//单片机时间戳定时器初始化
void Timer0_couter(void);//定时器0设置为计数器模式给频率采集
void Timer0_Timer(void);//定时器0设置为定时器模式给超声波
unsigned char scan_key(void);//按键扫描
void MS_Delay(unsigned char ms);//毫秒延时函数
void US_Delay(unsigned char us);//微秒延时函数
void Led_Display(unsigned char dat);//LED显示函数
void Seg_Display(unsigned char sel,unsigned char num,unsigned char dot);//数码管显示函数
void Get_Feq_Ask(void);//频率获取
unsigned int Get_Feq_Receive(void);//频率获取
unsigned char Get_distance(void);//距离获取
void Relay_control(unsigned char on);//继电器

#endif