#ifndef Key_h
#define Key_h

#include"STC15F2K60S2.H"
#include"tim.h"
#include"buffer_and_relay.h"

unsigned char scan_matrix(void);//一次扫描
unsigned char Read_key(void);//阻塞等待读取按键值

#endif