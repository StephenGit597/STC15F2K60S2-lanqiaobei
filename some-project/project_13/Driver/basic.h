#ifndef basic_h
#define basic_h

#include"STC15F2K60S2.h"
#include"intrins.h"

extern volatile unsigned long xdata currenttim;
extern unsigned char code segcode[13];

void Timer0Init(void);
void MS_delay(unsigned int ms);
void US_delay(unsigned int us);
void LED_Display(unsigned char Byte);
void Seg_Display(unsigned char sel,unsigned char Byte,unsigned char dot);
void Relay_control(unsigned char on);
unsigned char scan_key(void);

#endif