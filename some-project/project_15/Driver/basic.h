#ifndef basic_h
#define basic_h

#include"STC15F2K60S2.H"
#include"intrins.h"

extern unsigned char code segcode[17];
extern unsigned long xdata currenttim;

void Timer1Init(void);
void Timer0Init(void);
void MS_Delay(unsigned int ms);
void US_Delay(unsigned int us);
void Seg_display(unsigned char sel,unsigned char Byte,unsigned char dot);
void LED_display(unsigned char Byte);
unsigned char Scan_Martix_Key(void);
void Get_Feq_ask(void);
unsigned int Get_Feq_receive(void);

#endif