#include"basic_driver.h"
#include"basic_module.h"
//单片机时间变量声明
unsigned long xdata currenttim=0;//单片机系统时间戳
unsigned char xdata hour=0;//小时
unsigned char xdata minute=0;//分钟
unsigned char xdata second=0;//秒
//数码管段码表 0 1 2 3 4 5 6 7 8 9 A C E F H P - 无
unsigned char code segcode[18]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0xC6,0x86,0x8E,0x89,0x8C,0xBF,0xFF};
//数据采集
//获取电压
unsigned long xdata V_now=0;//测取电压时间戳
unsigned char xdata V=0;//从PCF8591获取的电压
unsigned char xdata V_ask_ok=0;//获取电压命令已发送标志位
unsigned char xdata V_complete=0;//电压获取完成标志位
unsigned char xdata Inu_flag=0;//光照明暗标志位 0 暗 1适中 2亮
unsigned char xdata prev_Inu_flag=0;//上一次光照明暗标志位 0 暗 1适中 2亮
unsigned char xdata trigger=0;//采集触发标志位 触发条件为亮到暗
unsigned long xdata trigger_now=0;//采集触发标志位时间戳
unsigned char xdata trigger_num=0;//触发采集的次数
unsigned char xdata trigger_hour=0;//触发小时
unsigned char xdata trigger_minute=0;//触发分钟
//获取频率
unsigned int xdata Feq=0;//当前读取到的频率
unsigned char xdata Feq_ask_ok=0;//频率采集命令已发送标志位
unsigned long xdata Feq_now=0;//频率采集时间戳
unsigned char xdata humidity=0;//当前湿度
unsigned char xdata prev_humidity=0;//上一次采集到的湿度
unsigned char xdata MAX_humidity=0;//最大湿度
unsigned char xdata humidity_num=0;//湿度采集次数
float xdata average_humidity=0;//平均湿度
unsigned char xdata humidity_none_flag=0;//湿度无效标志位
unsigned char xdata humidity_complete=0;//湿度采集完成标志位
unsigned char xdata humidity_up_flag=0;//湿度上升标志位
//获取温度
unsigned char xdata temperature=0;//当前读取到的温度
unsigned char xdata Set_temperature=30;//设置的报警温度
unsigned char xdata MAX_temperature=0;//当前获取的最大温度
unsigned char xdata prev_temperature=0;//上一次读取到的温度
float xdata average_temperature=0;//当前读取的平均温度
unsigned char xdata temperature_num=0;//湿度采集次数
unsigned char xdata temperature_ask_ok=0;//温度采集命令已发送标志位
unsigned char xdata temperature_complete=0;//温度采集完成标志位
unsigned long xdata temperature_now=0;//温度采集时间戳
unsigned char xdata temperature_alarm=0;//温度超过设定温度标志位
unsigned char xdata temperature_up_flag=0;//温度上升标志位
//获取时间
unsigned long xdata tim_now=0;//时间采集时间戳
//获取矩阵按键
unsigned long xdata key_now=0;//矩阵按键获取时间戳
unsigned char xdata key_1=0;//按键第一次读取的值
unsigned char xdata key_2=0;//按键第二次读取的值
unsigned char xdata key_scan_first=0;//按键第一次扫描标志位，防止第一次的时间戳被误更新
unsigned char xdata key_flag=0;//按键按下已稳定标志位
unsigned char xdata key_temp=0;//按键值缓冲区
unsigned char xdata key_value=0;//最终获取到的按键值
unsigned char xdata S9_long_flag=0;//等待判断S9长按标志位
unsigned long xdata S9_long_now=0;//判断S9长按时间戳
//数码管显示模式
unsigned char xdata seg_mode=0;//数码管显示模式
unsigned char xdata display_mode=0;//回显子页面模式
unsigned char xdata seg_temp=0;//数码管模式保持缓冲区，用于采集页面显示后恢复
//LED
unsigned long xdata led_now=0;//LED时间戳
unsigned char xdata led_temp=0;//LED闪烁震荡源
unsigned char xdata Led1=0;//LED1控制变量
unsigned char xdata Led2=0;//LED2控制变量
unsigned char xdata Led3=0;//LED3控制变量
unsigned char xdata Led4=0;//LED4控制变量
unsigned char xdata Led5=0;//LED5控制变量
unsigned char xdata Led6=0;//LED6控制变量
void main(void)
{
	Timer1Init();//定时器
	Timer0Init();//定时器
	EA=1;//使能全局中断
	DS1302_Set_Time(0,0,0);//ds1302时间初始化
	LED_display(0x00);//LED显示初始化
	while(1)
	{
		//采集矩阵键盘
		if((key_scan_first==0)&&(key_flag==0))//按键没有按下并稳定，开始第一次采集
		{
			key_1=scan_martix_key();//获取第一次的矩阵按键
			key_scan_first=1;//表示第一次矩阵按键已采集
			key_now=currenttim;//获取矩阵按键时间戳
		}
		if((key_scan_first==1)&&(key_flag==0)&&(currenttim-key_now>25))//按键没有按下并稳定，开始第二年次采集(25ms)
		{
			key_2=scan_martix_key();//获取第二次的矩阵按键
			key_scan_first=0;//清除第一次矩阵按键已采集标志位，便于下次采集
		}
		if((key_flag==0)&&(key_1==key_2)&&(key_1!=0))//矩阵键盘已按下并稳定
		{
			key_temp=key_1;//把按键值保持到缓冲区
			key_flag=1;//按下已稳定标志位置1
		}
		if(key_flag==1)//按键按下已稳定
		{
			if(key_temp==4)//如果按下的时按键S9
			{
				if(S9_long_flag==0)//开始记录S9长按时间
				{
					S9_long_flag=1;//S9长按等待时间标志位置1
					S9_long_now=currenttim;//记录当前按键长按时间戳
				}
				else//开始记录S9开始时间后 判断S9是长按还是短按
				{
					if((S9_long_flag==1)&&(currenttim-S9_long_now>2000))//S9按下并已经有2s
					{
						key_value=5;//按键S9长按取得按键值为5
						S9_long_flag=0;//清除长按等待标志位
						key_flag=0;//清除按键按下已稳定标志位
					}
					else//S9按下还没有2s
					{
						if((S9_long_flag==1)&&(!scan_martix_key()))//S9按下不足2s
						{
							key_value=4;//按键S9短按取得按键值为4
							S9_long_flag=0;//清除长按等待标志位
							key_flag=0;//清除按键按下已稳定标志位
						}
					}
				}
			}
			else//S4 S5 S8
			{
				if(!scan_martix_key())//按键放下
				{
					key_value=key_temp;//更新按键值
					key_flag=0;//清除按键按下已稳定标志位
				}
			}
		}
		//按键对应动作
		if(seg_mode!=3)//不在温湿度界面下
		{
			if(key_value==1)//S4按下切换显示页面
			{
				seg_mode++;//显示页面自增
				if(seg_mode==3)//0 1 2循环切换
				{
					seg_mode=0;//恢复初始值
				}
				key_value=0;//清除按键值，防止多次触发
			}
		}
		if(seg_mode==1)//在回显页面上
		{
			if(key_value==2)//S5按下切换回显页面
			{
				display_mode++;//回显界面自增
				if(display_mode==3)//0 1 2切换
				{
					display_mode=0;//恢复初始值
				}
				key_value=0;//清除按键值，防止多次触发
			}
			if(display_mode==2)//在时间回显字界面上
			{
				if(key_value==5)//S9长按
				{
					trigger_num=0;//触发次数清零
					trigger_hour=0;//触发小时清零
					trigger_minute=0;//触发分钟清零
					temperature=0;//当前读取到的温度清零
					MAX_temperature=0;//当前获取的最大温度清零
					average_temperature=0;//当前读取的平均温度清零
					temperature_num=0;//湿度采集次数清零
					humidity=0;//当前湿度清零
					MAX_humidity=0;//最大湿度清零
					average_humidity=0;//平均湿度清零
					humidity_num=0;//湿度采集次数清零
					key_value=0;//清除按键值，防止多次触发
				}
			}
		}
		if(seg_mode==2)//在温度参数设定界面上
		{
			if(key_value==3)//S8按下温度参数加一
			{
				Set_temperature++;//温度参数加一
				if(Set_temperature==100)//最大值边界检验
				{
					Set_temperature=99;//保持上界
				}
				key_value=0;//清除按键值，防止多次触发
			}
			if(key_value==4)//S9按下温度参数减一
			{
				Set_temperature--;//温度参数减一
				if(Set_temperature==9)//最小值边界检验
				{
					Set_temperature=10;//保持最小值
				}
				key_value=0;//清除按键值，防止多次触发
			}
		}
		//时间采集更新
		if(currenttim-tim_now>500)//500ms更新一次时间
		{
			DS1302_Get_Time();//更新时间
			tim_now=currenttim;//更新当前时间更新时间戳
		}
		//光照采集
		if((trigger==0)&&(V_ask_ok==0))//在没有触发的情况下，发送电压转换指令
		{
			Get_V_ask();//发送电压转换指令
			V_ask_ok=1;//表示已经发送电压转换指令
			V_now=currenttim;//更新电压转换时间戳
		}
		if((trigger==0)&&(V_ask_ok==1)&&(currenttim-V_now>100))//在没有触发的情况下，发送电压转换指令后，读取电压（100ms）后
		{
			V=Get_V_Receive();//获取电压
			V_ask_ok=0;//清除电压指令发送完成标志位，便于下一次转换
			prev_Inu_flag=Inu_flag;//把上次光照状态调入缓冲区保存
			if(V<121)//状态标记为暗
			{
				Inu_flag=0;//0表示暗
			}
			else if(V<174)//状态标记为适中
			{
				Inu_flag=1;//1表示适中
			}
			else//表示亮
			{
				Inu_flag=2;//2表示亮
			}
			V_complete=1;//电压转换完成标志位
		}
		//触发采集判断
		if((trigger==0)&&(prev_Inu_flag==2)&&(Inu_flag==0)&&(V_complete==1))//判断是否触发:环境状态由亮状态转变为暗状态(新的一次传感器数据采集完成)
		{
			trigger=1;//采集触发标志位置1（已触发）
			trigger_now=currenttim;//记录采集触发时间戳
			trigger_num++;//触发次数加一
			trigger_hour=hour;//记录触发小时
			trigger_minute=minute;//记录触发分钟
			seg_temp=seg_mode;//保存当前的显示界面
			seg_mode=3;//进入温湿度显示界面
			V_complete=0;//清除采集完成标志位，防止退出和传感器数据未更新而再次触发
		}
		//解除本次触发判断
		if((V_complete==0)&&(trigger==1)&&(currenttim-trigger_now>3000))//本次触发3S后解除
		{
			trigger=0;//清除触发标志位
			temperature_complete=0;//清除上一次触发的温度采集完成标志位，便于下一次采集
			humidity_complete=0;//清除上一次触发的湿度采集完成标志位，便于下一次采集
			seg_mode=seg_temp;//回到之前触发前的显示界面
		}
		//触发后温度采集
		if((trigger==1)&&(temperature_complete==0)&&(temperature_ask_ok==0))//触发后，在温度采集没有完成下，发送温度转换指令
		{
			Get_temperature_ask();//发起温度转换指令
			temperature_ask_ok=1;//温度转换指令发送标志位置1
			temperature_now=currenttim;//更新温度采集时间戳
		}
		if((trigger==1)&&(temperature_complete==0)&&(temperature_ask_ok==1)&&(currenttim-temperature_now>750))//读取温度
		{
			prev_temperature=temperature;//把上一次读取到的温度保存到缓冲区
			temperature=Get_temperature_receive();//获取当前温度
			if(temperature>Set_temperature)//测到的温度大于设定温度
			{
				temperature_alarm=1;//温度报警标志位置一
			}
			else
			{
				temperature_alarm=0;//清除温度报警标志位
			}
			if(temperature>prev_temperature)//检测到温度上升
			{
				temperature_up_flag=1;//温度上升标志位置1
			}
			else
			{
				temperature_up_flag=0;//清除温度上升标志位
			}
			if(temperature>MAX_temperature)//采用动态比较更新法获得当前温度的最大值
			{
				MAX_temperature=temperature;//更新温度最大值
			}
			temperature_num++;//温度采集次数加一
			average_temperature=(average_temperature*(temperature_num-1)+temperature)/temperature_num;//更新温度平均值（需要在温度采集数自增后计算）
			temperature_complete=1;//表示温度采集已经完成，一次触发仅采集一次温度（防止重复采集）
			temperature_ask_ok=0;//清除温度采集指令发送标志位，方便下一次采集
		}
		//触发湿度采集
		if((trigger==1)&&(humidity_complete==0)&&(Feq_ask_ok==0))////触发后，在湿度采集没有完成下，发送湿度转换指令
		{
			Get_Fequence_ask();//发送频率获取指令
			Feq_ask_ok=1;//频率获取指令已发送标志位置1
			Feq_now=currenttim;//更新频率获取时间戳
		}
		if((trigger==1)&&(humidity_complete==0)&&(Feq_ask_ok==1)&&(currenttim-Feq_now>1000))//触发采集后，在发送指令完成1s后获取计数置完成频率测量
		{
			prev_humidity=humidity;//将上次采集到的湿度保存到缓冲区(若本次采集频率非法，自动保存上一次采集到的频率值)
			Feq=Get_Fequence_receive();//获取当前频率
			if((Feq<200)|(Feq>2000))//检验频率值是否合法
			{
				humidity_none_flag=1;//频率值非法标志位置1
			}
			else
			{
				humidity_none_flag=0;//清除频率值非法标志位
				humidity=(unsigned char)(10+(Feq-200)*0.044f);//频率值与湿度值转换
				if(humidity>prev_humidity)//检测到湿度上升
				{
					humidity_up_flag=1;//湿度上升标志位置1
				}
				else
				{
					humidity_up_flag=0;//清除湿度上升标志位
				}
				if(humidity>MAX_humidity)//采用动态比较更新法获得当前湿度的最大值
				{
					MAX_humidity=humidity;//更新湿度最大值
				}
				humidity_num++;//湿度采集次数自增
				average_humidity=(average_humidity*(humidity_num-1)+humidity)/humidity_num;//更新湿度平均值（需要在湿度采集数自增后计算）
			}
			humidity_complete=1;//表示湿度采集已完成，一次触发仅采集一次湿度（防止重复采集）
			Feq_ask_ok=0;//清除频率采集指令发送完成标志位，方便下一次采集
		}
		//LED动作
		//LED4震荡源
		if(currenttim-led_now>100)//100ms切换一次状态
		{
			if(led_temp==0)//LED4震荡源0-1乒乓切换
			{
				led_temp=1;//置1
			}
			else//LED4震荡源0-1乒乓切换
			{
				led_temp=0;//清零
			}
			led_now=currenttim;//更新LED新的时间戳
		}
		if(seg_mode==0)//位于时间显示页面
		{
			Led1=1;//LED1点亮
		}
		else//不在时间显示界面
		{
			Led1=0;//LED1熄灭
		}
		if(seg_mode==1)//位于回显页面
		{
			Led2=1;//LED2点亮
		}
		else//不在回显界面
		{
			Led2=0;//LED2熄灭
		}
		if(seg_mode==2)//位于参数显示页面
		{
			Led3=1;//LED3点亮
		}
		else//不在参数显示界面
		{
			Led3=0;//LED3熄灭
		}
		if(temperature_alarm==1)//温度报警标志位
		{
			Led4=led_temp;//以0.1s间隔闪烁
		}
		else//解除温度报警
		{
			Led4=0;//Led4熄灭
		}
		if(humidity_none_flag==1)//采集到无效湿度数据
		{
			Led5=1;//LED5点亮
		}
		else//清除无效显示
		{
			Led5=0;//LED5熄灭
		}
		if((humidity_up_flag)&&(temperature_up_flag))//温度，湿度同时上升触发LED6点亮
		{
			Led6=1;//LED6点亮
		}
		else//其它情况保持熄灭
		{
			Led6=0;//LED6熄灭
		}//LED显示动作
		LED_display((0x01&Led1)|((0x01&Led2)<<1)|((0x01&Led3)<<2)|((0x01&Led4)<<3)|((0x01&Led5)<<4)|((0x01&Led6)<<5));//LED1 LED2 LED3 LED4 LED5 LED6
		//数码管显示
		switch(seg_mode)//选择模式
		{
			case 0://时间显示模式
			{
				Seg_display(1,hour/10,0);//小时十位
				Seg_display(2,hour%10,0);//小时个位
				Seg_display(3,16,0);//-
				Seg_display(4,minute/10,0);//分钟十位
				Seg_display(5,minute%10,0);//分钟个位
				Seg_display(6,16,0);//-
				Seg_display(7,second/10,0);//秒十位
				Seg_display(8,second%10,0);//秒个位
				break;
			}
			case 1://回显界面
			{
				switch(display_mode)//选择回显模式
				{
					case 0://温度回显模式
					{
						Seg_display(1,11,0);//界面标识符：C
						Seg_display(2,17,0);//熄灭
						if(trigger_num==0)//还没有开始采集
						{
							Seg_display(3,17,0);//熄灭
							Seg_display(4,17,0);//熄灭
							Seg_display(5,17,0);//熄灭
							Seg_display(6,17,0);//熄灭
							Seg_display(7,17,0);//熄灭
							Seg_display(8,17,0);//熄灭
						}
						else//开始采集了
						{
							Seg_display(3,MAX_temperature/10,0);//最大温度十位
							Seg_display(4,MAX_temperature%10,0);//最大温度个位
							Seg_display(5,16,0);//-
							Seg_display(6,((unsigned char)(average_temperature))/10,0);//平均温度十位
							Seg_display(7,((unsigned char)(average_temperature))%10,1);//平均温度个位
							Seg_display(8,((unsigned char)(average_temperature*10))%10,0);//平均温度小数点第一位
						}
						break;
					}
					case 1://湿度回显模式
					{
						Seg_display(1,14,0);//界面标识符：H
						Seg_display(2,17,0);//熄灭
						if(trigger_num==0)//还没有开始采集
						{
							Seg_display(3,17,0);//熄灭
							Seg_display(4,17,0);//熄灭
							Seg_display(5,17,0);//熄灭
							Seg_display(6,17,0);//熄灭
							Seg_display(7,17,0);//熄灭
							Seg_display(8,17,0);//熄灭
						}
						else//开始采集了
						{
							Seg_display(3,MAX_humidity/10,0);//最大湿度十位
							Seg_display(4,MAX_humidity%10,0);//最大湿度个位
							Seg_display(5,16,0);//-
							Seg_display(6,((unsigned char)(average_humidity))/10,0);//平均湿度十位
							Seg_display(7,((unsigned char)(average_humidity))%10,1);//平均湿度个位
							Seg_display(8,((unsigned char)(average_humidity*10))%10,0);//平均湿度第一个小数位
						}
						break;
					}
					case 2://时间回显模式
					{
						Seg_display(1,13,0);//界面标识符：F
						Seg_display(2,trigger_num/10,0);//触发次数十位
						Seg_display(3,trigger_num%10,0);//触发次数个位
						if(trigger_num==0)//还没有开始采集
						{
							Seg_display(4,17,0);//熄灭
							Seg_display(5,17,0);//熄灭
							Seg_display(6,17,0);//熄灭
							Seg_display(7,17,0);//熄灭
							Seg_display(8,17,0);//熄灭
						}
						else//开始采集了
						{
							Seg_display(4,trigger_hour/10,0);//触发小时十位
							Seg_display(5,trigger_hour%10,0);//触发小时个位
							Seg_display(6,16,0);//-
							Seg_display(7,trigger_minute/10,0);//触发分钟十位
							Seg_display(8,trigger_minute%10,0);//触发分钟个位
						}
						break;
					}
					default://非法情况默认为温度回显模式
					{
						display_mode=0;//切换温度回显模式
						break;
					}
				}
				break;
			}
			case 2://参数显示模式
			{
				Seg_display(1,15,0);//界面标识符：P
				Seg_display(2,17,0);//熄灭
				Seg_display(3,17,0);//熄灭
				Seg_display(4,17,0);//熄灭
				Seg_display(5,17,0);//熄灭
				Seg_display(6,17,0);//熄灭
				Seg_display(7,Set_temperature/10,0);//设置温度十位
				Seg_display(8,Set_temperature%10,0);//设置温度个位
				break;
			}
			case 3://温度湿度数据显示模式
			{
				Seg_display(1,12,0);//界面标识符：E
				Seg_display(2,17,0);//熄灭
				Seg_display(3,17,0);//熄灭
				Seg_display(4,temperature/10,0);//此次获取温度的十位
				Seg_display(5,temperature%10,0);//此次获取温度的个位
				Seg_display(6,16,0);//-
				if(humidity_none_flag==1)//此次湿度采集数据无效
				{
					Seg_display(7,10,0);//A 表示数据无效
					Seg_display(8,10,0);//A 表示数据无效
				}
				else
				{
					Seg_display(7,humidity/10,0);//获取湿度的十位
					Seg_display(8,humidity%10,0);//获取湿度的个位
				}
				break;
			}
			default://非法情况默认为时间显示模式
			{
				seg_mode=0;//时间显示模式
				break;
			}
		}
	}
}