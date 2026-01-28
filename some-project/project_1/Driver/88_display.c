#include"88_display.h"

code unsigned char display1[135]={0x00,0x74,0x54,0x54,0x5C,0x00,0x20,0x7E,0x24,0x00,0x1C,0x2A,0x2A,0x1A,0x00,0x3F,//长图动画显示数组
0x24,0x3C,0x00,0xFC,0x20,0x3C,0x00,0x1C,0x2A,0x2A,0x1A,0x00,0x1E,0x10,0x1E,0x00,
0x42,0x52,0x52,0x7E,0x00,0x7E,0x42,0x42,0x7E,0x00,0x01,0x06,0x00,0x00,0x38,0x04,
0x38,0x04,0x38,0x00,0x38,0x54,0x54,0x34,0x00,0xFC,0x04,0x00,0x3C,0x24,0x24,0x00,
0x3C,0x24,0x24,0x3C,0x00,0x3C,0x20,0x3C,0x20,0x3C,0x00,0x1C,0x2A,0x2A,0x1A,0x00,
0x00,0x10,0x3E,0x14,0x00,0x1E,0x12,0x12,0x1E,0x00,0x5E,0x52,0x72,0x00,0x7E,0x42,
0x7E,0x00,0x1D,0x15,0x17,0x00,0x2E,0x2A,0x3A,0x00,0x3E,0x22,0x3E,0x00,0x2E,0x2A,
0x3A,0x00,0x3E,0x2A,0x2E,0x00,0x00,0x00,0x3E,0x00,0x00,0x2E,0x2A,0x3A,0x00,0x20,0x20,0x3E,0x00,0x00,0x00,0x00,0x00};

code unsigned char Time[22]={0x40,0x40,0x7E,0x40,0x40,0x00,0x5E,0x00,0x00,0x1E,0x10,0x1E,0x10,0x1E,0x00,0x1E,//Time字符数组
0x2A,0x2A,0x1A,0x00,0x0A};

unsigned char xdata Time_display[29]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//时间数字显示缓冲区
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

code unsigned char Tempature[42]={0x00,0x40,0x40,0x7E,0x40,0x40,0x1E,0x2A,0x2A,0x1A,0x00,0x1E,0x10,0x1E,0x10,0x1E,0x00,//Tempature字符数组
0x3F,0x28,0x38,0x00,0x1C,0x24,0x1C,0x02,0x10,0x3E,0x12,0x00,0x1C,0x02,0x1E,0x02,
0x00,0x3E,0x10,0x20,0x1E,0x2A,0x2A,0x1A,0x00};

unsigned char xdata Tempature_display[17]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//温度数字显示缓冲区

unsigned char code Distance[33]={0x00,0x7E,0x42,0x42,0x3C,0x00,0x5E,0x00,0x3A,0x2A,0x2E,0x00,0x10,0x7E,0x14,0x00,//Distance字符数组
0x1E,0x12,0x1E,0x01,0x3E,0x20,0x3E,0x00,0x3E,0x22,0x22,0x00,0x1E,0x2A,0x2A,0x1A};

unsigned char xdata Distance_display[17]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//距离数字显示缓冲区

code unsigned char num[10][3]={{0x7C,0x44,0x7C},{0x00,0x7C,0x00},{0x5C,0x54,0x74},{0x54,0x54,0x7C},{0x70,0x10,0x7C},{0x74,0x54,0x5C},//数字字模存储数组
{0x7C,0x54,0x5C},{0x40,0x40,0x7C},{0x7C,0x54,0x7C},{0x74,0x54,0x7C}};

void Line_Display(unsigned char sel,unsigned char Byte)//单行显示
{
	if((sel==0)|(sel>8))
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	US_Delay(1);
	P0=(0x01)<<(sel-1);
	US_Delay(1);
	P2=(P2&0x1F)|0xC0;
	US_Delay(5);
	P2=(P2&0x1F)|0x00;
	P0=~(Byte);
	US_Delay(1);
	P2=(P2&0x1F)|0xE0;
	MS_Delay(2);
	P0=0xFF;
	P2=(P2&0x1F)|0x00;
}

void long_display(unsigned char delay)//长图显示函数
{
	unsigned char i=0;
	unsigned char offset=0;//偏移量
	unsigned char break_flag=0;//退出标志位
	while(1)
	{
		now=currenttim;
		while(currenttim-now<delay)
		{
			for(i=0;i<8;i++)
			{
				Line_Display(i+1,display1[(offset+i)%135]);
				if(scan_matrix())//按下任何按键可以退出循环，以读取按键值
				{
					break_flag=1;
					break;
				}
			}
			if(break_flag)
			{
				break;
			}
		}
		if(offset==134)
		{
			offset=0;//偏移量到达长图最后一个清零
		}
		offset++;//偏移量加一
		if(break_flag)
		{
			break;
		}
	}
}
//时间显示字样
// ---- ----   ---- ----   ---- ----
// -  - -  - - -  - -  - - -  - -  -
// ---- ----   ---- ----   ---- ----
// -  - -  - - -  - -  - - -  - -  -
// ---- ----   ---- ----   ---- ----
void TimeDisplay(delay)//时间显示函数
{
	unsigned char i=0;
	unsigned char offset=0;//偏移量
	unsigned char break_flag=0;//退出标志位
	while(1)
	{
		Get_Time();//判断是否到达闹钟想起条件
		if((pre_hour==hour)&&(pre_minute==minute)&&(pre_second==second)&&(Clock_flag==1))
		{
			Clock_ON=1;//闹钟标志位置1
			Buffer_and_Relay_control(1,0);
		}
		Time_display[1]=num[hour/10][0];//给显示缓冲区赋予实际数据
		Time_display[2]=num[hour/10][1];//给显示缓冲区赋予实际数据
		Time_display[3]=num[hour/10][2];//给显示缓冲区赋予实际数据
		Time_display[4]=0x00;//空格保留
		Time_display[5]=num[hour%10][0];//给显示缓冲区赋予实际数据
		Time_display[6]=num[hour%10][1];//给显示缓冲区赋予实际数据
		Time_display[7]=num[hour%10][2];//给显示缓冲区赋予实际数据
		Time_display[8]=0x00;
		Time_display[9]=0x24;//时间分隔符
		Time_display[10]=0x00;
		Time_display[11]=num[minute/10][0];//给显示缓冲区赋予实际数据
		Time_display[12]=num[minute/10][1];//给显示缓冲区赋予实际数据
		Time_display[13]=num[minute/10][2];//给显示缓冲区赋予实际数据
		Time_display[14]=0x00;//空格保留
		Time_display[15]=num[minute%10][0];//给显示缓冲区赋予实际数据
		Time_display[16]=num[minute%10][1];//给显示缓冲区赋予实际数据
		Time_display[17]=num[minute%10][2];//给显示缓冲区赋予实际数据
		Time_display[18]=0x00;
		Time_display[19]=0x24;//时间分隔符
		Time_display[20]=0x00;
		Time_display[21]=num[second/10][0];//给显示缓冲区赋予实际数据
		Time_display[22]=num[second/10][1];//给显示缓冲区赋予实际数据
		Time_display[23]=num[second/10][2];//给显示缓冲区赋予实际数据
		Time_display[24]=0x00;//空格保留
		Time_display[25]=num[second%10][0];//给显示缓冲区赋予实际数据
		Time_display[26]=num[second%10][1];//给显示缓冲区赋予实际数据
		Time_display[27]=num[second%10][2];//给显示缓冲区赋予实际数据
		Time_display[28]=0x00;//空格保留
		now=currenttim;
		while(currenttim-now<delay)
		{
			for(i=0;i<8;i++)
			{
				if((offset+i>21)&(offset+i<51))//判断每一行显示的长图位置决定它该显示显示固定code数组还是显示时钟缓冲区
				{
					Line_Display(i+1,Time_display[offset+i-22]);//显示固定code数组
				}
				else
				{
					Line_Display(i+1,Time[(offset+i)%51]);//显示时钟缓冲区
				}
				if(scan_matrix())//按下任何按键可以退出循环，以读取按键值
				{
					break_flag=1;
					break;
				}
			}
			if(break_flag)
			{
				break;
			}
		}
		offset++;//偏移量加一
		if(offset==51)
		{
			offset=0;//偏移量到达长图最后一个清零
		}
		if(break_flag)
		{
			break;
		}
	}
}
//温度显示字样
// ---- ----   ---- ----
// -  - -  -   -  - -  -
// ---- ----   ---- ----
// -  - -  - - -  - -  -
// ---- ----   ---- ----
void Tempature_Display(delay)//温度显示函数
{
	unsigned char i=0;
	unsigned char offset=0;//偏移量
	unsigned char break_flag=0;//退出标志位
	float tempature=0.0f;
	while(1)
	{
		tempature=Get_tempature();
		if(temp_flag==1)//温度报警模式已经开启
		{
			switch(alarm_mode)
			{
				case 0://高温报警
				{
					if(tempature>=high_tempature)
					{
						Buffer_and_Relay_control(0,1);
						temp_ON=1;//温度报警已触发标志位
					}
					else
					{
						Buffer_and_Relay_control(0,0);
						temp_ON=0;//温度报警熄灭标志位
					}
					break;
				}
				case 1://低温报警
				{
					if(tempature<=low_tempature)
					{
						Buffer_and_Relay_control(0,1);
						temp_ON=1;//温度报警已触发标志位
					}
					else
					{
						Buffer_and_Relay_control(0,0);
						temp_ON=0;//温度报警熄灭标志位
					}
					break;
				}
				case 2://高低温报警
				{
					if((tempature>=high_tempature)|(tempature<=low_tempature))
					{
						Buffer_and_Relay_control(0,1);
						temp_ON=1;//温度报警已触发标志位
					}
					else
					{
						Buffer_and_Relay_control(0,0);
						temp_ON=0;//温度报警熄灭标志位
					}
					break;
				}
				default:
				{
					Buffer_and_Relay_control(0,0);
					break;
				}
			}
		}
		Tempature_display[0]=num[((int)(tempature))/10][0];//给显示缓冲区赋予实际数据
		Tempature_display[1]=num[((int)(tempature))/10][1];//给显示缓冲区赋予实际数据
		Tempature_display[2]=num[((int)(tempature))/10][2];//给显示缓冲区赋予实际数据
		Tempature_display[4]=num[((int)(tempature))%10][0];//给显示缓冲区赋予实际数据
		Tempature_display[5]=num[((int)(tempature))%10][1];//给显示缓冲区赋予实际数据
		Tempature_display[6]=num[((int)(tempature))%10][2];//给显示缓冲区赋予实际数据
		Tempature_display[10]=num[((int)(tempature*10))%10][0];//给显示缓冲区赋予实际数据
		Tempature_display[11]=num[((int)(tempature*10))%10][1];//给显示缓冲区赋予实际数据
		Tempature_display[12]=num[((int)(tempature*10))%10][2];//给显示缓冲区赋予实际数据
		Tempature_display[13]=num[((int)(tempature*100))%10][0];//给显示缓冲区赋予实际数据
		Tempature_display[14]=num[((int)(tempature*100))%10][1];//给显示缓冲区赋予实际数据
		Tempature_display[15]=num[((int)(tempature*100))%10][2];//给显示缓冲区赋予实际数据
		now=currenttim;
		while(currenttim-now<delay)
		{
			for(i=0;i<8;i++)
			{
				if((i+offset>41)&(i+offset<59))//判断每一行显示的长图位置决定它该显示显示固定code数组还是显示温度缓冲区
				{
					Line_Display(i+1,Tempature_display[(offset+i-42)%59]);//显示固定code数组
				}
				else
				{
					Line_Display(i+1,Tempature[(offset+i)%59]);//显示温度缓冲区
				}
				if(scan_matrix())//按下任何按键可以退出循环，以读取按键值
				{
					break_flag=1;
					break;
				}
			}
			if(break_flag)
			{
				break;
			}
		}
		offset++;//偏移量加一
		if(offset==59)
		{
			offset=0;//偏移量到达长图最后一个清零
		}
		if(break_flag)
		{
			break;
		}
	}
}
//距离显示字样
// ---- ----   ---- ----
// -  - -  -   -  - -  -
// ---- ----   ---- ----
// -  - -  - - -  - -  -
// ---- ----   ---- ----
void Distance_Display(unsigned char delay)//距离显示函数
{
	unsigned char i=0;
	unsigned char offset=0;//偏移量
	unsigned char break_flag=0;//退出标志位
	float distance=0.0f;
	while(1)
	{
		distance=Get_distance();
		if(distance_flag==1)//距离报警模式已经开启
		{
			switch(distance_alarm_mode)
			{
				case 0://远距离报警
				{
					if(distance>=max_distance)
					{
						Buffer_and_Relay_control(0,1);
						distance_ON=1;//距离报警触发标志位
					}
					else
					{
						Buffer_and_Relay_control(0,0);
						distance_ON=0;//距离报警熄灭标志位
					}
					break;
				}
				case 1://近距离报警
				{
					if(distance<=min_distance)
					{
						Buffer_and_Relay_control(0,1);
						distance_ON=1;//距离报警触发标志位
					}
					else
					{
						Buffer_and_Relay_control(0,0);
						distance_ON=0;//距离报警熄灭标志位
					}
					break;
				}
				case 2://近远距报警
				{
					if((distance>=max_distance)|(distance<=min_distance))
					{
						Buffer_and_Relay_control(0,1);
						distance_ON=1;//距离报警触发标志位
					}
					else
					{
						Buffer_and_Relay_control(0,0);
						distance_ON=0;//距离报警熄灭标志位
					}
					break;
				}
				default:
				{
					Buffer_and_Relay_control(0,0);
					distance_ON=0;
					break;
				}
			}
		}
		Distance_display[0]=num[((int)(distance))/10][0];//给显示缓冲区赋予实际数据
		Distance_display[1]=num[((int)(distance))/10][1];//给显示缓冲区赋予实际数据
		Distance_display[2]=num[((int)(distance))/10][2];//给显示缓冲区赋予实际数据
		Distance_display[4]=num[((int)(distance))%10][0];//给显示缓冲区赋予实际数据
		Distance_display[5]=num[((int)(distance))%10][1];//给显示缓冲区赋予实际数据
		Distance_display[6]=num[((int)(distance))%10][2];//给显示缓冲区赋予实际数据
		Distance_display[10]=num[((int)(distance*10))%10][0];//给显示缓冲区赋予实际数据
		Distance_display[11]=num[((int)(distance*10))%10][1];//给显示缓冲区赋予实际数据
		Distance_display[12]=num[((int)(distance*10))%10][2];//给显示缓冲区赋予实际数据
		Distance_display[14]=num[((int)(distance*100))%10][0];//给显示缓冲区赋予实际数据
		Distance_display[15]=num[((int)(distance*100))%10][1];//给显示缓冲区赋予实际数据
		Distance_display[16]=num[((int)(distance*100))%10][2];//给显示缓冲区赋予实际数据
		now=currenttim;
		while(currenttim-now<delay)
		{
			for(i=0;i<8;i++)
			{
				if((i+offset>32)&(i+offset<50))//判断每一行显示的长图位置决定它该显示显示固定code数组还是显示距离缓冲区
				{
					Line_Display(i+1,Distance_display[(offset+i-33)%50]);//显示固定code数组
				}
				else
				{
					Line_Display(i+1,Distance[(offset+i)%50]);//显示距离缓冲区
				}
				if(scan_matrix())//按下任何按键可以退出循环，以读取按键值
				{
					break_flag=1;
					break;
				}
			}
			if(break_flag)
			{
				break;
			}
		}
		offset++;//偏移量加一
		if(offset==50)
		{
			offset=0;//偏移量到达长图最后一个清零
		}
		if(break_flag)
		{
			break;
		}
	}
}