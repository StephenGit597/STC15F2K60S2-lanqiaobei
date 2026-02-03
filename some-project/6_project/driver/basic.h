#ifndef basic_h
#define basic_h

#include"STC15F2K60S2.H"
#include"intrins.h"

extern unsigned long xdata currenttim;
extern unsigned char code segcode[12];

void Timer1Init(void);
void MS_Delay(unsigned int ms);
void US_Delay(unsigned int us);
void Led_display(unsigned char Byte);
void Seg_Display(unsigned char sel,unsigned char Byte,unsigned char dot);
unsigned char Get_key(void);

#endif