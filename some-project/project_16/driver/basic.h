#ifndef basic_h
#define basic_h

#include"STC15F2K60S2.H"
#include"intrins.h"

extern unsigned long xdata currenttim;
extern unsigned char code segcode[15];

void Timer0Init(void);
void Timer1Init(void);
void MS_Delay(unsigned int ms);
void US_Delay(unsigned int us);
void LED_display(unsigned char Byte);
void Seg_display(unsigned char sel,unsigned char Byte,unsigned char dot);
void Relay_control(unsigned char cmd);
unsigned char scan_key(void);
unsigned char Get_distance(void);

#endif