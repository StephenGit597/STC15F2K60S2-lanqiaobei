#include"Key.h"

unsigned char scan_matrix(void)//读取一次矩阵键盘
{
	unsigned char value=0;
	unsigned int temp=0;
	P44=0;
	P42=1;
	P35=1;
	P34=1;
	temp=P3&0x0F;
	P44=1;
	P42=0;
	P35=1;
	P34=1;
	temp=(temp<<4)|(P3&0x0F);
	P44=1;
	P42=1;
	P35=0;
	P34=1;
	temp=(temp<<4)|(P3&0x0F);
	P44=1;
	P42=1;
	P35=1;
	P34=0;
	temp=(temp<<4)|(P3&0x0F);
	temp=~temp;
	switch(temp)
	{
		case 0x8000://S1 被按下
		{
			value=1;
			break;
		}
		case 0x4000://S2 被按下
		{
			value=2;
			break;
		}
		case 0x2000://S3 被按下
		{
			value=3;
			break;
		}
		case 0x1000://S4 被按下
		{
			value=4;
			break;
		}
		case 0x0800://S5 被按下
		{
			value=5;
			break;
		}
		case 0x0400://S6 被按下
		{
			value=6;
			break;
		}
		case 0x0200://S7 被按下
		{
			value=7;
			break;
		}
		case 0x0100://S8 被按下
		{
			value=8;
			break;
		}
		case 0x0080://S9 被按下
		{
			value=9;
			break;
		}
		case 0x0040://S10 被按下
		{
			value=10;
			break;
		}
		case 0x0020://S11 被按下
		{
			value=11;
			break;
		}
		case 0x0010://S12 被按下
		{
			value=12;
			break;
		}
		case 0x0008://S13 被按下
		{
			value=13;
			break;
		}
		case 0x0004://S14 被按下
		{
			value=14;
			break;
		}
		case 0x0002://S15 被按下
		{
			value=15;
			break;
		}
		case 0x0001://S16 被按下
		{
			value=16;
			break;
		}
		default://按键未被按下
		{
			value=0;
			break;
		}		
	}
	return(value);
}

unsigned char Read_key(void)
{
	unsigned char V1=0;
	unsigned char V2=0;
	unsigned char temp=0;
	V1=scan_matrix();//读取第一次
	MS_Delay(5);//简易按键消抖
	V2=scan_matrix();//读取第二次
	if((V1==V2)&&(V1!=0))//按键值相同而且按键不为0（按键按下）
	{
		temp=V1;//保存按键值
		while(1)//等待按键放下，防止按键按下的时间差异导致多次触发
		{
			V1=scan_matrix();
			MS_Delay(5);
			V2=scan_matrix();
			if(V1==V2==0)
			{
				Buffer_and_Relay_control(1,0);//按键声效
				MS_Delay(5);
				if(Clock_ON==1)//判断闹钟是否已响起，不然按键声效会误关闭闹钟（指定按键S2除外）
				{
					Buffer_and_Relay_control(1,0);//继续响起闹钟
				}
				else if((temp_ON==1)|(distance_ON==1)|(V_ON==1))//判断报警是否已触发，防止按键声效会误关闭报警
				{
					Buffer_and_Relay_control(0,1);//继续保持报警
				}
				else
				{
				    Buffer_and_Relay_control(0,0);//按键声效	
				}					
				break;
			}
		}
	}
	return(temp);
}