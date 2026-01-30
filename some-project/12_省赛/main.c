#include"basic_module.h"
#include"advance_module.h"

unsigned long idata currenttim=0;
unsigned long idata now=0;

float idata temperature=0.0f;//获取实时温度
unsigned char idata set_temperature=25;//设置温度
unsigned char idata confirm_temperature=25;//确认比较温度

unsigned char temperature_OK=0;//表示温度转换完成

unsigned char idata key_1=0;//获取第一次的按键值
unsigned char idata key_2=0;//获取第二次的按键值
unsigned char idata key_temp=0;//按键值中间存储变量
unsigned char idata key_flag=0;//按键按下稳定的标志
unsigned long idata key_now=0;//按键时间记录变量
unsigned char idata key_Update=0;//是否第一次标志位，用于判断是否需要重新记录时间

unsigned char idata mode1=0;//参数设置命令
unsigned char idata mode2=0;//最后得到的按键对应的模式
unsigned char idata mode3=0;//电压参数设置命令
unsigned char out_V_mode=0;//电压输出命令

float out_V=0.0f;

void main(void)
{
	Timer0Init();
	EA=1;
	LED_display(0x00);//显示初始化
	while(1)
	{
		LED_display(0x02|(0x01&out_V_mode));//显示初始化
		while(1)
		{//显示实时温度界面
			if(temperature_OK==0)//若没有发送温度转换命令
			{
				Get_temperature_Send();//发送温度转换命令
				now=currenttim;//记下温度转换时间
				temperature_OK=1;//表示已经发送温度转换命令
			}
			if((currenttim-now>=751)&&(temperature_OK==1))//延时足够而且已经发送温度转换指令
			{
				temperature=Get_temperature_Receive();//采集温度
				temperature_OK=0;//表示未发送温度转换命令（这次转换数据读取已完成）
			}
			Seg_display(1,11,0);
			Seg_display(2,13,0);
			Seg_display(3,13,0);
			Seg_display(4,13,0);
			Seg_display(5,((int)(temperature))/10,0);
			Seg_display(6,((int)(temperature))%10,1);
			Seg_display(7,((int)(temperature*10))%10,0);
			Seg_display(8,((int)(temperature*100))%10,0);
			if(key_Update==0)//如果是第一次
			{
				key_1=scan_key();//扫描一次按键
				key_now=currenttim;//记录第一次扫描按键的时间
				key_Update=1;//置标志位，表示第一次的时间已记录过
			}
			if(currenttim-key_now>10)//在循环中判断延时到没有 如果到达预计时间则进入
			{
				key_2=scan_key();//第二次按键的判断
				if((key_1==key_2)&&(key_1!=0))//判断按键是否稳定并且被按下
				{
					key_flag=1;//表示按键已经按下而且稳定
					key_temp=key_1;//把按键值放到缓冲区
				}
				key_Update=0;//清除第一次的时间已记录过标志位
			}
			if(key_flag)//按键已经按下而且稳定
			{
				if(!scan_key())//判断按键放下没有
				{
					mode1=key_temp;//如果按键放下 则把缓冲区的值给按键值
				}
			}//根据按键获得的值确定模式来觉得循环中的状态
			if(mode1==1)
			{
				mode1=0;//执行动作也要清除模式值
				key_flag=0;//清除按键已经按下而且稳定标志，否则循环会误触发多次
				LED_display(0x00|(0x01&out_V_mode));//显示模式清零 同时保留采集模式
				break;//退出循环
			}
		}
		LED_display(0x04|(0x01&out_V_mode));//显示初始化
		while(1)
		{//显示参数设置界面
			Seg_display(1,12,0);
			Seg_display(2,13,0);
			Seg_display(3,13,0);
			Seg_display(4,13,0);
			Seg_display(5,13,0);
			Seg_display(6,13,0);
			Seg_display(7,set_temperature/10,0);
			Seg_display(8,set_temperature%10,0);
			if(key_Update==0)//如果是第一次
			{
				key_1=scan_key();//扫描一次按键
			    key_now=currenttim;//记录第一次扫描按键的时间
				key_Update=1;//置标志位，表示第一次的时间已记录过
			}
			if(currenttim-key_now>10)//在循环中判断延时到没有 如果到达预计时间则进入
			{
				key_2=scan_key();//第二次按键的判断
				if((key_1==key_2)&&(key_1!=0))//判断按键是否稳定并且被按下
				{
					key_flag=1;//表示按键已经按下而且稳定
					key_temp=key_1;//把按键值放到缓冲区
				}
				key_Update=0;//清除第一次的时间已记录过标志位
			}
			if(key_flag)//按键已经按下而且稳定
			{
				if(!scan_key())//判断按键放下没有
				{
					mode2=key_temp;//如果按键放下 则把缓冲区的值给按键值
				}
			}//根据按键获得的值确定模式来觉得循环中的状态
			if(mode2==1)//退出循环
			{
				mode2=0;//执行动作也要清除模式值
				key_flag=0;//清除按键已经按下而且稳定标志，否则循环会误触发多次
				confirm_temperature=set_temperature;//退出该页面自动确认温度
				LED_display(0x00|(0x01&out_V_mode));//显示模式清零 同时保留采集模式
				break;
			}
			else if(mode2==3)//设定值减一
			{
				mode2=0;//执行动作也要清除模式值
				key_flag=0;//清除按键已经按下而且稳定标志，否则循环会误触发多次
				set_temperature--;
				if(set_temperature==255)//合法性判断 非负
				{
					set_temperature=0;//默认为0
				}
			}
			else if(mode2==4)//设定值加一
			{
				mode2=0;//执行动作也要清除模式值
				key_flag=0;//清除按键已经按下而且稳定标志，否则循环会误触发多次
				set_temperature++;
				if(set_temperature==100)//合法性判断 维数不能超
				{
					set_temperature=99;//默认99
				}
			}
			else
			{//执行动作也要清除模式值
				mode2=0;
			}
		}
		LED_display(0x08|(0x01&out_V_mode));//显示初始化
		while(1)
		{//显示电压输出界面
			Seg_display(1,10,0);
			Seg_display(2,13,0);
			Seg_display(3,13,0);
			Seg_display(4,13,0);
			Seg_display(5,13,0);
			Seg_display(6,((int)(out_V)%10),1);
			Seg_display(7,((int)(out_V*10))%10,0);
			Seg_display(8,((int)(out_V*100))%10,0);
			if(temperature_OK==0)//若没有发送温度转换命令
			{
				Get_temperature_Send();//发送温度转换命令
				now=currenttim;//记下温度转换时间
				temperature_OK=1;//表示已经发送温度转换命令
			}
			if((currenttim-now>=751)&&(temperature_OK==1))//延时足够而且已经发送温度转换指令
			{
				temperature=Get_temperature_Receive();//采集温度
				temperature_OK=0;//表示未发送温度转换命令（这次转换数据读取已完成）
			}
			if(out_V_mode==0)//电压输出模式1
			{
				if(confirm_temperature<temperature)
				{
					Out_V(255);//电压输出5
					out_V=5.0f;//获取电压为0.0v
				}
				else
				{
					Out_V(0);//电压输出0
					out_V=0.0f;//获取电压为5.0v
				}
			}
			else//电压输出模式2
			{
				if(temperature<20.0f|(temperature<20.0f))
				{
					Out_V(51);//电压输出1
					out_V=1.0f;//获取电压为1.0v
				}
				else if((temperature>20.0f)&&(temperature<40.0f))
				{
					Out_V(51+(char)((temperature-20.0f)*7.65f));//电压输出51-204
					out_V=1.0f+0.15f*(temperature-20.0f);//显示电压1.0-4.0
				}
				else
				{
					Out_V(204);//电压输出4
					out_V=4.0f;//获取电压为4.0v
				}
			}
			if(key_Update==0)//如果是第一次
			{
				key_1=scan_key();//扫描一次按键
				key_now=currenttim;//记录第一次扫描按键的时间
				key_Update=1;//置标志位，表示第一次的时间已记录过
			}
			if(currenttim-key_now>10)//在循环中判断延时到没有 如果到达预计时间则进入
			{
				key_2=scan_key();//第二次按键的判断
				if((key_1==key_2)&&(key_1!=0))//判断按键是否稳定并且被按下
				{
					key_flag=1;//表示按键已经按下而且稳定
					key_temp=key_1;//把按键值放到缓冲区
				}
				key_Update=0;//清除第一次的时间已记录过标志位
			}
			if(key_flag)//按键已经按下而且稳定
			{
				if(!scan_key())//判断按键放下没有
				{
					mode3=key_temp;//如果按键放下 则把缓冲区的值给按键值
				}
			}//根据按键获得的值确定模式来觉得循环中的状态
			if(mode3==2)
			{
				key_flag=0;//清除按键已经按下而且稳定标志，否则循环会误触发多次
				mode3=0;//执行动作也要清除模式值
				out_V_mode=!out_V_mode;//翻转按键
				LED_display(0x08|(0x01&out_V_mode));//同时应该变化对应的LED
			}
			if(mode3==1)
			{
				key_flag=0;//清除按键已经按下而且稳定标志，否则循环会误触发多次
				mode3=0;//执行动作也要清除模式值
				LED_display(0x00|(0x01&out_V_mode));//显示模式清零 同时保留采集模式
				break;
			}
		}
	}
}