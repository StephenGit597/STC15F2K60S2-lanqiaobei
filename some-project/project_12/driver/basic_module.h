#ifndef basic_module_h
#define basic_module_h

#include"STC15F2K60S2.H"
#include"intrins.h"

void Timer0Init(void);
void MS_delay(unsigned int ms);
void US_delay(unsigned int us);

void LED_display(unsigned char Byte);
void Seg_display(unsigned char sel,unsigned char Byte,unsigned char dot);
unsigned char scan_key(void);

extern unsigned long idata currenttim;
extern unsigned long idata now;
extern unsigned char code segcode[14];

#endif