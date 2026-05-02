#include"basic_driver.h"
#include"advance.h"
//变量定义
volatile unsigned long xdata currenttim=0;//单片机上电时间戳
//数码管显示数组
//0 1 2 3 4 5 6 7 8 9 C E P N - 熄灭
unsigned char code segcode[16]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xc6,0x86,0x8C,0xC8,0x3F,0xFF};
//按键状态机
unsigned long xdata Key_Now=0;//按键检测时间戳
unsigned char xdata Key_First_Ok=0;//按键第一次扫描完成标志位
unsigned char xdata Key_Value1=0;//按键第一次扫描读取到的值
unsigned char xdata Key_Value2=0;//按键第二次扫描读取到的值
unsigned char xdata Key_temp=0;//按键值缓存区
unsigned char xdata Key_Flag=0;//按键按下已稳定标志位
unsigned char xdata Key_Value=0;//按键最终获得的值
//主要逻辑
unsigned char xdata mode=0;//0：初始化状态；1按键输入密码状态；2：开门状态 3：修改密码状态
unsigned char xdata num1=0;//输入密码位数计数
unsigned char xdata num2=0;//修改密码位数计数
unsigned char xdata check_num=0;//密码检查计数
unsigned char xdata fix_num=0;//密码修改计数
unsigned char xdata code_error_flag=0;//密码输入错误标准位
unsigned long xdata code_error_now=0;//密码输入错误时间戳
unsigned char xdata code_buffer1[6]={0,0,0,0,0,0};//输入密码缓冲区
unsigned char xdata code_buffer2[6]={0,0,0,0,0,0};//修改密码缓冲区
unsigned char xdata code_buffer3[6]={0,0,0,0,0,0};//硬盘密码读出存储区
unsigned char xdata inital_code[6]={8,8,8,8,8,8};//初始密码
unsigned char xdata First_Inital[1]={0};//第一次上电标志位
unsigned long xdata ON_Now=0;//门开启时间戳
//LED状态
unsigned char xdata Led1=0;
unsigned char xdata Led2=0;
unsigned char xdata Led3=0;
//继电器状态
unsigned char xdata Relay=0;
//数码管显示部分
unsigned char xdata Seg_num=0;//显示位计数

void main()
{
	Timer1Init();//定时器1初始化
	EA=1;//使能全局中断
	LED_Display(0x00);//LED初始化
	Relay_Control(0x00);//继电器初始化关闭
	//第一次上电写入初始密码888888
	AT24C02_Read(0x06,First_Inital,1);
	if(First_Inital[0]!=0x01)
	{
		AT24C20_Write(0x00,inital_code,6);//设置初始密码
		First_Inital[0]=0x01;//表示第一次上电已设置初始密码
		AT24C20_Write(0x06,First_Inital,1);//存储第一次上电信息
	}
	AT24C02_Read(0x00,code_buffer3,6);//加载密码到缓冲区
	while(1)
	{
		//按键检测
		if((Key_Flag==0)&&(Key_First_Ok==0))//按键没有被按下已稳定并且是第一次扫描
		{
			Key_Value1=Scan_Key();//读取第一次按键值
			Key_First_Ok=1;//按键第一次按下完成标准位置1
			Key_Now=currenttim;//记下第一次读取完成的时间戳
		}
		if((Key_Flag==0)&&(Key_First_Ok==1)&&(currenttim-Key_Now>25))//按键没有被按下已稳定并且是第二次扫描
		{
			Key_Value2=Scan_Key();//读取第二次按键值
			Key_First_Ok=0;//清除按键第一次扫描完成标志位，便于下一次扫描
		}
		if((Key_Value1==Key_Value2)&&(Key_Value2!=0)&&(Key_Flag==0))//按键被按下已稳定
		{
			Key_Flag=1;//按键按下已稳定标志位置1
			Key_temp=Key_Value1;//按键置存储到缓冲区
		}
		if((Key_Flag==1)&&(!Scan_Key()))//按键被按下已稳定检测按键放下
		{
			Key_Value=Key_temp;//按键获取最终值
			Key_Flag=0;//清除按键按下已稳定标志位便于下一次读取
		}
		//按键动作 业务逻辑
		if(mode==0)//在初始化状态
		{
			if(Key_Value==13)//S16按下 进入输入密码模式
			{
				mode=1;//进入输入密码模式
				Key_Value=0;//清除按键值防止多次触发
			}
		}
		if(mode==1)//在输入密码状态
		{
			//8 4 0 9 5 1 6 2 7 3 用户按键输入的是数字 注意密码位数没有输入到6位
			if(((Key_Value==2)|(Key_Value==3)|(Key_Value==4)|(Key_Value==6)|(Key_Value==7)|(Key_Value==8)|(Key_Value==11)|(Key_Value==12)|(Key_Value==15)|(Key_Value==16))&&(num1<5))
			{
				code_buffer1[num1]=Key_Value;//存储输入的这一位数字
				num1++;//输入的密码位数加一
				Key_Value=0;//清除按键值防止多次触发
			}
			//S8按下清除输入的位数
			if(Key_Value==5)
			{
				if(num1==0)//如果位数已经全部清除完毕，保持完全清空状态
				{
					num1=0;//位数下界，确保不进入非法状态
				}
				else
				{
					num1--;//位数减一
				}
				Key_Value=0;//清除按键值防止多次触发
			}
			//密码到6位自动判断密码是否正确
			if(num1==5)
			{
				for(check_num=0;check_num<6;check_num++)
				{
					//密码输入错误
					if(code_buffer3[check_num]!=code_buffer1[check_num])
					{
						code_error_flag=1;//密码输入错误标志位值1
					}
					if(code_error_flag==1)//检测到密码输入错误
					{
						code_error_now=currenttim;//记下当前密码输入错误时间戳
						for(check_num=0;check_num<6;check_num++)//清空密码输入缓冲区
						{
							code_buffer1[check_num]=0;//清空密码输入缓冲区
						}
						num1=0;//清空密码的输入计数
					}
					else//密码输入正确
					{
						for(check_num=0;check_num<6;check_num++)//清空密码输入缓冲区
						{
							code_buffer1[check_num]=0;//清空密码输入缓冲区
						}
						num1=0;//清空密码的输入计数
						mode=2;//进入开门状态
						ON_Now=currenttim;//记下这次密码输入正确的时间戳
					}
				}
			}
			if((code_error_flag==1)&&(currenttim-code_error_now>5000))//密码输入错误后等待5s后进入初始化状态
			{
				code_error_flag=0;//清除密码输入标志位
				mode=0;//进入初始化状态
			}
		}
		if(mode==2)//在开门状态下
		{
			if(Key_Value==9)//按下修改密码键
			{
				mode=3;//进入修改密码状态
				ON_Now=currenttim;//修正开门时这次按键动作的时间戳
				Key_Value=0;//清除按键值防止多次触发
			}
			if(currenttim-ON_Now>5000)
			{
				mode=0;//回到初始化状态
			}
		}
		if(mode==3)//在修改密码状态下
		{
			//8 4 0 9 5 1 6 2 7 3 用户按键输入的是数字 注意密码位数没有输入到6位
			if(((Key_Value==2)|(Key_Value==3)|(Key_Value==4)|(Key_Value==6)|(Key_Value==7)|(Key_Value==8)|(Key_Value==11)|(Key_Value==12)|(Key_Value==15)|(Key_Value==16))&&(num2<5))
			{
				code_buffer2[num2]=Key_Value;//存储输入的这一位数字
				num2++;//输入的密码位数加一
				Key_Value=0;//清除按键值防止多次触发
			}
			//S8按下清除输入的位数
			if(Key_Value==5)
			{
				if(num2==0)//如果位数已经全部清除完毕，保持完全清空状态
				{
					num2=0;//位数下界，确保不进入非法状态
				}
				else
				{
					num2--;//位数减一
				}
				Key_Value=0;//清除按键值防止多次触发
			}
			//6位密码输入完成再S12按下确认新密码
			if((Key_Value==9)&&(num2==5))//按下确认修改密码键
			{
				//先修改内存中的密码
				for(fix_num=0;fix_num<6;fix_num++)
				{
					code_buffer3[fix_num]=code_buffer2[fix_num];//修改存储新密码
				}
				AT24C20_Write(0x00,code_buffer3,6);//更新ROM中的密码
				for(fix_num=0;fix_num<6;fix_num++)
				{
					code_buffer2[fix_num]=0;//清除修改密码输入缓冲区
				}
				num2=0;//清除输入密码计数位数
				mode=2;//回到开门状态
				Key_Value=0;//清除按键值防止多次触发
			}
		}
		//LED状态
		if(mode==0)//初始状态LED全熄灭
		{
			Led1=0;
			Led2=0;
			Led3=0;
		}
	    else if(mode==1)//在输入密码状态 LED7点亮
		{
			Led1=0;
			Led2=1;
			Led3=0;
		}
		else if(mode==3)//在修改密码状态 LED8点亮
		{
			Led1=0;
			Led2=0;
			Led3=1;
		}
		else if(code_error_flag==1)//密码输入错误 LED1点亮
		{
			Led1=1;
			Led2=0;
			Led3=0;
		}
		else//其它状态全熄灭
		{
			Led1=0;
			Led2=0;
			Led3=0;
		}
		LED_Display((0x01&Led1)|((0x01&Led2)<<6)|((0x01&Led3)<<7));//LED动作
		//继电器状态
		if((mode==2)|(mode==3))//在开门或者设置密码模式下；继电器打开
		{
			Relay=1;
		}
		else//其余继电器关闭
		{
			Relay=0;
		}
		//继电器动作
		Relay_Control(Relay);
		//数码管显示部分
		if(mode==0)//在初始状态下;数码管全部熄灭
		{
			Seg_Display(1,15,0);//熄灭
			Seg_Display(2,15,0);//熄灭
			Seg_Display(3,15,0);//熄灭
			Seg_Display(4,15,0);//熄灭
			Seg_Display(5,15,0);//熄灭
			Seg_Display(6,15,0);//熄灭
			Seg_Display(7,15,0);//熄灭
			Seg_Display(8,15,0);//熄灭
		}
		if(mode==1)//在输入密码状态
		{
			Seg_Display(1,14,0);//-
			Seg_Display(2,15,0);//熄灭
			//控制熄灭的位数
			for(Seg_num=0;Seg_num<6-num1;Seg_num++)
			{
				Seg_Display(3+Seg_num,15,0);//熄灭
			}
			//控制点亮的位数
			for(Seg_num=0;Seg_num<num1;Seg_num++)
			{
				Seg_Display(8-Seg_num,code_buffer1[num1-Seg_num-1],0);//显示密码
			}
		}
		if(mode==2)//在开门状态
		{
			Seg_Display(1,0,0);//0
			Seg_Display(2,15,0);//熄灭
			Seg_Display(3,15,0);//熄灭
			Seg_Display(4,15,0);//熄灭
			Seg_Display(5,0,0);//0
			Seg_Display(6,12,0);//P
			Seg_Display(7,11,0);//E
			Seg_Display(8,13,0);//N
		}
		if(mode==3)//在修改密码状态
		{
			Seg_Display(1,10,0);//C
			Seg_Display(2,15,0);//熄灭
			//控制熄灭的位数
			for(Seg_num=0;Seg_num<6-num2;Seg_num++)
			{
				Seg_Display(3+Seg_num,15,0);//熄灭
			}
			//控制点亮的位数
			for(Seg_num=0;Seg_num<num2;Seg_num++)
			{
				Seg_Display(8-Seg_num,code_buffer2[num2-Seg_num-1],0);//显示密码
			}
		}
	}
}