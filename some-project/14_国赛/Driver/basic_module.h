#ifndef basic_module_h
#define basic_module_h
//必要头文件引用
#include"STC15F2K60S2.H"
#include"basic_driver.h"
//变量声明
//单片机时间戳
extern unsigned long currenttim;
//函数声明
//定时器初始化
void Timer1Init(void);
void Timer0Init(void);
//显示输出函数
void Seg_Display(unsigned char sel,unsigned char Byte,unsigned char dot);
void LED_display(unsigned char Byte);
void Relay_control(unsigned char control_Byte);
//按键输入函数
unsigned char martix_scan(void);
//距离获取函数
void Get_distance_ask(void);
unsigned int Get_distance_receive(unsigned int Trans_V);
#endif