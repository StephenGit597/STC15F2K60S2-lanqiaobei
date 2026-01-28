#include"tim.h"
#include"led.h"
#include"PCF8591.h"
#include"onewire.h"
#include"ds1302.h"
#include"seg.h"
#include"88_display.h"
#include"Key.h"
#include"buffer_and_relay.h"

unsigned char idata Key_Value=0;//按键值存储区
unsigned char idata mode1=0;//开发板工作模式

unsigned char idata time_mode=0;//时间显示模式
unsigned char xdata time_buffer[6]={0,0,0,0,0,0};//显示时间设置缓冲区，xdata
unsigned char xdata clcok_buffer[6]={0,0,0,0,0,0};//闹钟显示时钟缓存区
//闹钟设置时间以及开关，注意闹钟只有在时间显示（数码管，点阵屏）有效，在设置时间和设置闹钟时间模式下无效
unsigned char xdata pre_hour=0;
unsigned char xdata pre_minute=0;
unsigned char xdata pre_second=0;
unsigned char xdata Clock_flag=0;//闹钟开关 0关闭，1打开
unsigned char xdata Clock_ON=0;//闹钟响标志位，防止按键声效误关闭闹钟

unsigned char xdata tim_sel=0;//时间选择位选指示位
unsigned char xdata delay=0;//logo显示暂停时间变量

unsigned char xdata temp_mode=0;//温度显示模式
unsigned char xdata temp_flag=0;//温度报警功能开关
unsigned char xdata temp_ON=0;//温度报警已触发标志位，防止按键声效误关闭温度报警
float xdata high_tempature=25.0f;//初始高温警报设定温度为25.0C
float xdata low_tempature=5.0f;//初始低温警报设定温度为5.0C
float xdata tempature_temp=0.0f;//温度交换缓冲区
unsigned char xdata alarm_mode=0;//设置温度报警模式 0为高温报警 1为低温报警 2为高低温报警
unsigned char xdata temp_buffer[8]={0,0,0,0,0,0,0,0};//温度设置缓冲区
unsigned char xdata temp_sel=0;//温度数位选择

unsigned char xdata distance_mode=0;//测距模式选择
unsigned char xdata distance_flag=0;//距离报警功能开关
unsigned char xdata distance_alarm_mode=0;//设置距离报警模式 0为近距 1为远距报警 2为近远距报警
float xdata min_distance=10.0f;//设定最小距离初始化为10cm
float xdata max_distance=50.0f;//设定最大距离初始化为90cm
float xdata distance_temp=0.0f;//距离交换缓冲区
unsigned char xdata distance_buffer[8]={0,0,0,0,0,0,0,0};//距离设置缓冲区
unsigned char xdata distance_sel=0;//距离数位选择
unsigned char xdata distance_ON=0;//距离报警已触发标志位，防止按键声效误关闭距离报警

unsigned char xdata V_mode=0;//电压显示模式
unsigned char xdata V_max=255;//设定电压最大
unsigned char xdata V_min=0;//设定电压最小
unsigned char xdata V_flag=0;//电压报警开关
unsigned xdata V_alarm_mode=0;//电压报警模式及 0为高压报警 1为低压报警 2为高低压报警
unsigned char xdata V_ON=0;//电压报警已触发标志位，防止按键声效误关闭报警
unsigned char xdata V_buffer[6]={0,0,0,0,0,0};////距离电压缓冲区
unsigned char xdata V_sel=0;////电压数位选择
unsigned char xdata V_temp=0;//电压交换缓冲区
unsigned char Set_v=0;//设置模拟电压
unsigned char current_V=0;//初始化当前电压

void main(void)
{
	Timer0Init();
	Timer1Init();
	EA=1;
	Pin_Init();//引脚初始化
	Buffer_and_Relay_control(0,0);//防止蜂鸣器，继电器误动作
	LED_Display(0x00);//初始化LED,防止上次数据残留导致LED误点亮
	Set_Time(0,0,0);//初始时间位0时0分0秒
	while(1)
	{
		Key_Value=Read_key();//读取按键值
		if((Key_Value==1)|(Key_Value==2)|(Key_Value==3)|(Key_Value==4)|(Key_Value==5))
		{
			mode1=Key_Value;
			Key_Value=0;
		}
		else//如果没有任何按键按下或者其它按键按下 则初始数码管默认显示C1 表示菜单1
		{
			mode1=0;//按键不按下,默认不选择任何模式
			Segdisplay(1,12,0);
			Segdisplay(2,1,0);
		}
		switch(mode1)
		{			
			case 1://S1按下为时间显示模式
			{
				LED_Display((0x01)|((0x01&&Clock_flag)<<7));//L1点亮表示模式1，图案显示,同时指示闹钟是否开启
				while(1)
				{
					Key_Value=Read_key();//读取按键值
					if((Key_Value==13)|(Key_Value==14)|(Key_Value==15)|(Key_Value==16))
					{//时间模式只有数码管模式，点阵屏模式，设置时间模式，和设置闹钟模式
						time_mode=Key_Value;
						Key_Value=0;//按键值清零
					}
					else if((Key_Value)==1)//S1 按下退出时间显示模式
					{
						Key_Value=0;//按键值清零
						Clock_ON=0;//闹钟响标志位清零，防止退出后闹钟误触发
						Buffer_and_Relay_control(0,0);//关闭报警装置
						LED_Display(0x00);//该模式的LED指示也应该清除
						break;
					}
					else//没有然后按键按下,显示C2 表示时间选择页面
					{
						time_mode=0;//按键不按下,默认不选择任何模式
						Segdisplay(1,12,0);
			            Segdisplay(2,2,0);
					}
					switch(time_mode)
					{
						case 13://时间数码管显示
						{
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if(Key_Value==2)//S2 在闹钟响起时，按下关闭闹钟
								{
									Clock_ON=0;//闹钟标志位清0
									Buffer_and_Relay_control(0,0);
									Key_Value=0;//按键值清零
								}
								if(Key_Value==1)//S1 按下退出该模式
								{
									Key_Value=0;//按键值清零
									Clock_ON=0;//闹钟标志位清0
									Buffer_and_Relay_control(0,0);//关闭闹钟（如果闹钟想起）
									break;
								}
								Time_Seg(500);//死循环显示，按键按下可以跳出
							}
							break;
						}
						case 14://时间点阵屏显示
						{
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if(Key_Value==2)//S2 在闹钟响起时，按下关闭闹钟
								{
									Clock_ON=0;//闹钟标志位清0
									Buffer_and_Relay_control(0,0);
									Key_Value=0;//按键值清零
								}
								if(Key_Value==1)//S1 按下退出该模式
								{
									Key_Value=0;//按键值清零
									Clock_ON=0;//闹钟标志位清0
									Buffer_and_Relay_control(0,0);//关闭闹钟（如果闹钟想起）
									break;
								}
							    TimeDisplay(150);//死循环显示，按键按下可以跳出
							}
							break;
						}
						case 15://设置时间
						{
							Get_Time();//获取当前时间
							tim_sel=0;//时间选择位初始化为0
							//给缓冲区赋予初值
							time_buffer[0]=hour/10;
							time_buffer[1]=hour%10;
							time_buffer[2]=minute/10;
							time_buffer[3]=minute%10;
							time_buffer[4]=second/10;
							time_buffer[5]=second%10;
							while(1)
							{
								Key_Value=Read_key();//读取当前按键值
								if(Key_Value==11)//位加一 S11选择按键位
								{
									tim_sel++;
									if(tim_sel==6)
									{
										tim_sel=0;
									}
									Key_Value=0;//按键值归位
								}
								if((Key_Value>0)&&(Key_Value<11))//赋予值1-10 对应0-9 #10-9选择赋值位
								{
									time_buffer[tim_sel]=Key_Value-1;
									Key_Value=0;//按键值归位
								}
								if(Key_Value==12)//确认设置时间并退出 S12选择确认退出位
								{
									Key_Value=0;//按键值归位
									break;//跳出循环
								}
								if(Clock_flag)//闹钟开启标志显示
								{
									LED_Display(((0x01)<<tim_sel)|0x80);
								}
								LED_Display((0x01)<<tim_sel);//LED控制位指示
								Segdisplay(1,time_buffer[0],0);//显示当前参数
								Segdisplay(2,time_buffer[1],0);//显示当前参数
								Segdisplay(3,16,0);
								Segdisplay(4,time_buffer[2],0);//显示当前参数
								Segdisplay(5,time_buffer[3],0);//显示当前参数
								Segdisplay(6,16,0);
								Segdisplay(7,time_buffer[4],0);//显示当前参数
								Segdisplay(8,time_buffer[5],0);//显示当前参数
							}//显示复原到L1点亮 模式1(时间模式)
							LED_Display((0x01)|((0x01&Clock_flag)<<7));
							hour=time_buffer[0]*10+time_buffer[1];//整合新的时间
							minute=time_buffer[2]*10+time_buffer[3];
							second=time_buffer[4]*10+time_buffer[5];
							//小时，分钟，秒合法性判断
							if((time_buffer[0]*10+time_buffer[1])>23)
							{
								hour=23;
							}
							if((time_buffer[2]*10+time_buffer[3])>59)
							{
								minute=59;
							}
							if((time_buffer[4]*10+time_buffer[5])>59)
							{
								second=59;
							}
							Set_Time(hour,minute,second);//给DS1302设置新的时间
							break;
						}
						case 16://设置闹钟时间
						{
							tim_sel=0;//时间选择位初始化为0
							clcok_buffer[0]=pre_hour/10;//初始化闹钟缓冲区时间，便于使用
							clcok_buffer[1]=pre_hour%10;//初始化闹钟缓冲区时间，便于使用
							clcok_buffer[2]=pre_minute/10;//初始化闹钟缓冲区时间，便于使用
							clcok_buffer[3]=pre_minute%10;//初始化闹钟缓冲区时间，便于使用
							clcok_buffer[4]=pre_second/10;//初始化闹钟缓冲区时间，便于使用
							clcok_buffer[5]=pre_second%10;//初始化闹钟缓冲区时间，便于使用
							while(1)
							{
								Key_Value=Read_key();//读取当前按键值
								if(Key_Value==11)//位加一 S11选择时间选择位
								{
									tim_sel++;
									if(tim_sel==6)
									{
										tim_sel=0;
									}
									Key_Value=0;//按键值归位
								}
								if((Key_Value>0)&&(Key_Value<11))//赋予值1-10 对应0-9 S0-9选择时间赋值
								{
									clcok_buffer[tim_sel]=Key_Value-1;
									Key_Value=0;//按键值归位
								}
								if(Key_Value==12)//确认设置时间并退出 S12选择确认退出
								{
									Key_Value=0;//按键值归位
									break;//跳出循环
								}
								if(Key_Value==13)//设置闹钟的开关
								{
									Key_Value=0;//按键值归位
									Clock_flag=!Clock_flag;//开关切换型
								}
								if(Clock_flag)//闹钟开启指示灯
								{
									LED_Display((((0x01)<<tim_sel)|0x40)|0x80);
								}
								LED_Display(((0x01)<<tim_sel)|0x40);//LED控制位指示
								Segdisplay(1,clcok_buffer[0],0);//显示当前参数
								Segdisplay(2,clcok_buffer[1],0);//显示当前参数
								Segdisplay(3,16,0);
								Segdisplay(4,clcok_buffer[2],0);//显示当前参数
								Segdisplay(5,clcok_buffer[3],0);//显示当前参数
								Segdisplay(6,16,0);
								Segdisplay(7,clcok_buffer[4],0);//显示当前参数
								Segdisplay(8,clcok_buffer[5],0);//显示当前参数
							}//恢复显示
							LED_Display((0x01)|((0x01&Clock_flag)<<7));
							pre_hour=clcok_buffer[0]*10+clcok_buffer[1];//整合新的闹钟时间
							pre_minute=clcok_buffer[2]*10+clcok_buffer[3];
							pre_second=clcok_buffer[4]*10+clcok_buffer[5];
							//小时，分钟，秒合法性判断
							if((clcok_buffer[0]*10+clcok_buffer[1])>23)
							{
								pre_hour=23;
							}
							if((clcok_buffer[2]*10+clcok_buffer[3])>59)
							{
								pre_minute=59;
							}
							if((clcok_buffer[4]*10+clcok_buffer[5])>59)
							{
								pre_second=59;
							}
							break;
						}
						default://若没有按键按下(以及除了是退出键的按键被按下)，继续等待按键按下
						{
							Key_Value=0;//按键值清零
							break;
						}
					}
				}
				break;
			}
			case 2://图案logo显示
			{
				LED_Display(0x02);//L2点亮表示模式2，图案显示
				delay=60;//延时赋予初值，否则按键未按下，long_delay(0)会出现故障,出现卡死（最内部循环判断按键状态）
				while(1)
				{
					Key_Value=Read_key();//读取当前按键值
					if((Key_Value>0)&&(Key_Value<11))//赋予值1-10 对应0-9
					{
						delay=50+Key_Value*10;//60-150
						Key_Value=0;//恢复按键变量值
					}
					else if(Key_Value==0)//没有按下任何按键，保持不变
					{
						long_display(delay);
					}
					else//按下其它任何按键退出
					{
						Key_Value=0;
						LED_Display(0x00);//该模式的LED指示也应该清除
						break;
					}
				}
				break;
			}
			case 3://温度显示
			{
				LED_Display((0x04)|(0x01<<(4+alarm_mode))|((0x01&temp_flag)<<7));//0 0 0 1 x x x y
				while(1)
				{
					Key_Value=Read_key();//读取按键值
					if((Key_Value==13)|(Key_Value==14)|(Key_Value==15))
					{//温度显示模式只有温度点阵屏显示模式S13,温度数码管显示模式S14，温度目标值设置模式以及温度报警装置S15
						temp_mode=Key_Value;
						Key_Value=0;//按键值清零
					}
					else if((Key_Value)==1)//S1 按下退出温度显示模式
					{
						Key_Value=0;//按键值清零
						temp_ON=0;//温度报警已触发标志位清零,防止退出后继电器误触发
						Buffer_and_Relay_control(0,0);//关闭报警装置
						LED_Display(0x00);//该模式的LED指示也应该清除
						break;
					}
					else//没有然后按键按下,显示C3 表示温度选择页面
					{
						temp_mode=0;//按键不按下,默认不选择任何模式
						Segdisplay(1,12,0);
			            Segdisplay(2,3,0);
					}
					switch(temp_mode)
					{
						case 13://温度点阵屏显示 S13
						{
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if(Key_Value==1)//S1 按下退出该模式
								{
									temp_ON=0;//温度报警触发标志位清零
									Buffer_and_Relay_control(0,0);//恢复报警装置的关闭状态
									Key_Value=0;//按键值清零
									break;
								}
								Tempature_Display(90);//死循环显示，按键按下可以跳出
							}
							break;
						}
						case 14://温度数码管显示 S14
						{
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if(Key_Value==1)//S1 按下退出该模式
								{
									temp_ON=0;//温度报警触发标志位清零
									Buffer_and_Relay_control(0,0);//恢复报警装置的关闭状态
									Key_Value=0;//按键值清零
									break;
								}
								Tempature_Seg(1000);//死循环显示，按键按下可以跳出
							}
							break;
						}
						case 15://S15 温度预警温度设置 温度预警模式设置 注意温度预警只有在温度显示模式下有效
						{//0-3表示最低正温度 4-7表示最高正温度
							temp_sel=0;//温度选择位初始化
							temp_buffer[0]=((int)(low_tempature))/10;//温度缓冲区初始化，便于温度的设置
							temp_buffer[1]=((int)(low_tempature))%10;//温度缓冲区初始化，便于温度的设置
							temp_buffer[2]=((int)(low_tempature*10))%10;//温度缓冲区初始化，便于温度的设置
							temp_buffer[3]=((int)(low_tempature*100))%10;//温度缓冲区初始化，便于温度的设置
							temp_buffer[4]=((int)(high_tempature))/10;//温度缓冲区初始化，便于温度的设置
							temp_buffer[5]=((int)(high_tempature))%10;//温度缓冲区初始化，便于温度的设置
							temp_buffer[6]=((int)(high_tempature*10))%10;//温度缓冲区初始化，便于温度的设置
							temp_buffer[7]=((int)(high_tempature*100))%10;//温度缓冲区初始化，便于温度的设置
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if((Key_Value>0)&&(Key_Value<11))//赋予值1-10 对应0-9 S0-9选择温度赋值
								{
									temp_buffer[temp_sel]=Key_Value-1;
									Key_Value=0;//按键值归位
								}
								if(Key_Value==11)//位加一 S11选择温度选择位
								{
									temp_sel++;
									if(temp_sel==8)
									{
										temp_sel=0;
									}
									Key_Value=0;//按键值归位
								}
								if(Key_Value==12)//确认设置预计温度并退出 S12选择确认退出
								{
									
									Key_Value=0;//按键值归位
									break;//跳出循环
								}
								if(Key_Value==13)//设置温度报警的开关
								{
									Key_Value=0;//按键值归位
									temp_flag=!temp_flag;//开关切换型
								}
								if(Key_Value==14)//温度控制模式加一
								{
									alarm_mode++;
									if(alarm_mode==3)
									{
										alarm_mode=0;
									}
									Key_Value=0;//按键值归位
								}
								LED_Display((0x01)<<temp_sel);
								Segdisplay(1,temp_buffer[0],0);
								Segdisplay(2,temp_buffer[1],1);
								Segdisplay(3,temp_buffer[2],0);
								Segdisplay(4,temp_buffer[3],0);
								Segdisplay(5,temp_buffer[4],0);
								Segdisplay(6,temp_buffer[5],1);
								Segdisplay(7,temp_buffer[6],0);
								Segdisplay(8,temp_buffer[7],0);
							}//恢复显示
							LED_Display((0x04)|(0x01<<(4+alarm_mode))|((0x01&temp_flag)<<7));//0 0 0 1 x x x y
							low_tempature=temp_buffer[0]*10+temp_buffer[1]+temp_buffer[2]*0.1f+temp_buffer[3]*0.01f;
							high_tempature=temp_buffer[4]*10+temp_buffer[5]+temp_buffer[6]*0.1f+temp_buffer[7]*0.01f;
							if(low_tempature>high_tempature)//校验是否正确 最低温度不能超过最高温度
							{//交换二者温度
								tempature_temp=high_tempature;
								high_tempature=low_tempature;
								low_tempature=tempature_temp;
							}
							break;
						}
						default://若没有按键按下(以及除了是退出键的按键被按下)，继续等待按键按下
						{
							Key_Value=0;//按键值清零
							break;
						}
					}
				}
				break;
			}
			case 4://距离测量
			{
				LED_Display((0x08)|(0x01<<(4+distance_alarm_mode))|((0x01&distance_flag)<<7));//0 0 0 1 x x x y
				while(1)
				{
					Key_Value=Read_key();//读取按键值
					if((Key_Value==13)|(Key_Value==14)|(Key_Value==15))//S13 距离数码管显示模式
					{
						distance_mode=Key_Value;
						Key_Value=0;//按键值清零
					}
					else if((Key_Value)==1)//S1 按下退出距离显示模式
					{
						Key_Value=0;//按键值清零
						distance_ON=0;//距离报警已触发标志位,防止退出后继电器误触发
						Buffer_and_Relay_control(0,0);//关闭报警装置
						LED_Display(0x00);//该模式的LED指示也应该清除
						break;
					}
					else//没有然后按键按下,显示C4 表示温度选择页面
					{
						distance_mode=0;//按键不按下,默认不选择任何模式
						Segdisplay(1,12,0);
			            Segdisplay(2,4,0);
					}
					switch(distance_mode)
					{
						case 13://距离点阵屏显示
						{
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if(Key_Value==1)//S1 按下退出该模式
								{
									temp_ON=0;//温度报警触发标志位清零
									Buffer_and_Relay_control(0,0);//恢复报警装置的关闭状态
									Key_Value=0;//按键值清零
									break;
								}
								Distance_Display(100);
							}
							break;
						}
						case 14://距离数码管显示
						{
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if(Key_Value==1)//S1 按下退出该模式
								{
									distance_ON=0;//距离报警触发标志位清零
									Buffer_and_Relay_control(0,0);//恢复报警装置的关闭状态
									Key_Value=0;//按键值清零
									break;
								}
								distance_Seg(90);
							}
							break;
						}
						case 15://S15 距离报警设置 距离预警模式设置 注意距离预警只有在距离显示模式下有效
						{//0-3表示最小距离 4-7表示最大距离
							distance_sel=0;//距离选择位初始化
							distance_buffer[0]=((int)(min_distance))/10;//距离缓冲区初始化，便于距离的设置
							distance_buffer[1]=((int)(min_distance))%10;//距离缓冲区初始化，便于距离的设置
							distance_buffer[2]=((int)(min_distance*10))%10;//距离缓冲区初始化，便于距离的设置
							distance_buffer[3]=((int)(min_distance*100))%10;//距离缓冲区初始化，便于距离的设置
							distance_buffer[4]=((int)(max_distance))/10;//距离缓冲区初始化，便于距离的设置
							distance_buffer[5]=((int)(max_distance))%10;//距离缓冲区初始化，便于距离的设置
							distance_buffer[6]=((int)(max_distance*10))%10;//距离缓冲区初始化，便于距离的设置
							distance_buffer[7]=((int)(max_distance*100))%10;//距离缓冲区初始化，便于距离的设置
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if((Key_Value>0)&&(Key_Value<11))//赋予值1-10 对应0-9 S0-9选择温度赋值
								{
									distance_buffer[distance_sel]=Key_Value-1;
									Key_Value=0;//按键值归位
								}
								if(Key_Value==11)//位加一 S11选择距离选择位
								{
									distance_sel++;
									if(distance_sel==8)
									{
										distance_sel=0;
									}
									Key_Value=0;//按键值归位
								}
								if(Key_Value==12)//确认设置预计距离并退出 S12选择确认退出
								{
									Key_Value=0;//按键值归位
									break;//跳出循环
								}
								if(Key_Value==13)//设置距离报警的开关
								{
									Key_Value=0;//按键值归位
									distance_flag=!distance_flag;//开关切换型
								}
								if(Key_Value==14)//距离控制模式加一
								{
									distance_alarm_mode++;
									if(distance_alarm_mode==3)
									{
										distance_alarm_mode=0;
									}
									Key_Value=0;//按键值归位
								}
								LED_Display((0x01)<<distance_sel);
								Segdisplay(1,distance_buffer[0],0);
								Segdisplay(2,distance_buffer[1],1);
								Segdisplay(3,distance_buffer[2],0);
								Segdisplay(4,distance_buffer[3],0);
								Segdisplay(5,distance_buffer[4],0);
								Segdisplay(6,distance_buffer[5],1);
								Segdisplay(7,distance_buffer[6],0);
								Segdisplay(8,distance_buffer[7],0);
							}//恢复显示
							LED_Display((0x08)|(0x01<<(4+distance_alarm_mode))|((0x01&distance_flag)<<7));//0 0 0 1 x x x y
							min_distance=distance_buffer[0]*10+distance_buffer[1]+distance_buffer[2]*0.1f+distance_buffer[3]*0.01f;
							max_distance=distance_buffer[4]*10+distance_buffer[5]+distance_buffer[6]*0.1f+distance_buffer[7]*0.01f;
							if(min_distance>max_distance)//校验是否正确 最小距离不能超过最大距离
							{//交换二者距离
								distance_temp=max_distance;
								max_distance=min_distance;
								min_distance=distance_temp;
							}
							break;
						}
						default://若没有按键按下(以及除了是退出键的按键被按下)，继续等待按键按下
						{
							Key_Value=0;//按键值清零
							break;
						}
					}
				}
				break;
			}
			case 5://电压测量
			{//电压测量 模式5 L1 L2 L3 L4都不发光
				LED_Display((0x00)|(0x01<<(4+V_alarm_mode))|((0x01&V_flag)<<7));//0 0 0 1 x x x y
				while(1)
				{
					Key_Value=Read_key();//读取按键值
					if((Key_Value==13)|(Key_Value==14)|(Key_Value==15)|(Key_Value==16))//S13光敏电阻采集模式 S14电位器电压采集模式 S15报警电压以及报警模式选择 S16模拟电压输出
					{
						V_mode=Key_Value;
						Key_Value=0;//按键值清零
					}
					else if((Key_Value)==1)//S1 按下退出电压模式
					{
						Key_Value=0;//按键值清零
						V_ON=0;//电压报警已触发标志位,防止退出后继电器误触发
						Buffer_and_Relay_control(0,0);//关闭报警装置
						LED_Display(0x00);//该模式的LED指示也应该清除
						break;
					}
					else//没有然后按键按下,显示C5 表示电压选择页面
					{
						V_mode=0;//按键不按下,默认不选择任何模式
						Segdisplay(1,12,0);
			            Segdisplay(2,5,0);
					}
					switch(V_mode)
					{
						case 13:
						{
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if(Key_Value==1)//S1 按下退出该模式
								{
									V_ON=0;//电压报警触发标志位清零
									Buffer_and_Relay_control(0,0);//恢复报警装置的关闭状态
									Key_Value=0;//按键值清零
									break;
								}
								Volt_Seg(1,5);
							}
							break;
						}
						case 14:
						{
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if(Key_Value==1)//S1 按下退出该模式
								{
									V_ON=0;//电压报警触发标志位清零
									Buffer_and_Relay_control(0,0);//恢复报警装置的关闭状态
									Key_Value=0;//按键值清零
									break;
								}
								Volt_Seg(2,5);
							}
							break;
						}
						case 15://S15报警电压以及报警模式选择
						{//0-3表示最小电压 4-7表示最大电压
							V_sel=0;//电压选择位初始化
							V_buffer[0]=(V_min/100)%10;//电压缓冲区初始化，便于电压的设置
							V_buffer[1]=(V_min/10)%10;//电压缓冲区初始化，便于电压的设置
							V_buffer[2]=V_min%10;//电压缓冲区初始化，便于电压的设置
							V_buffer[3]=(V_max/100)%10;//电压缓冲区初始化，便于电压的设置
							V_buffer[4]=(V_max/10)%10;//电压缓冲区初始化，便于电压的设置
							V_buffer[5]=V_max%10;//电压缓冲区初始化，便于电压的设置
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if((Key_Value>0)&&(Key_Value<11))//赋予值1-10 对应0-9 S0-9选择电压赋值
								{
									V_buffer[V_sel]=Key_Value-1;
									Key_Value=0;//按键值归位
								}
								if(Key_Value==11)//位加一 S11选择电压选择位
								{
									V_sel++;
									if(V_sel==6)
									{
										V_sel=0;
									}
									Key_Value=0;//按键值归位
								}
								if(Key_Value==12)//确认设置预计电压并退出 S12选择确认退出
								{
									Key_Value=0;//按键值归位
									break;//跳出循环
								}
								if(Key_Value==13)//设置电压报警的开关
								{
									Key_Value=0;//按键值归位
									V_flag=!V_flag;//开关切换型
								}
								if(Key_Value==14)//电压控制模式加一
								{
									V_alarm_mode++;
									if(V_alarm_mode==3)
									{
										V_alarm_mode=0;
									}
									Key_Value=0;//按键值归位
								}
								LED_Display((0x01)<<V_sel);
								Segdisplay(1,16,0);
								Segdisplay(2,V_buffer[0],0);
								Segdisplay(3,V_buffer[1],0);
								Segdisplay(4,V_buffer[2],0);
								Segdisplay(5,16,0);
								Segdisplay(6,V_buffer[3],0);
								Segdisplay(7,V_buffer[4],0);
								Segdisplay(8,V_buffer[5],0);
							}//恢复显示
							LED_Display((0x00)|(0x01<<(4+V_alarm_mode))|((0x01&V_flag)<<7));//0 0 0 1 x x x y
							if((100*V_buffer[3]+10*V_buffer[4]+V_buffer[5])>255)//电压边界校验
							{
							    V_max=255;
							}
							else
							{
								V_max=100*V_buffer[3]+10*V_buffer[4]+V_buffer[5];
							}
							if((100*V_buffer[0]+10*V_buffer[1]+V_buffer[2])>255)//电压边界校验
							{
								V_min=255;
							}
							else
							{
								V_min=100*V_buffer[0]+10*V_buffer[1]+V_buffer[2];
							}
							if(V_min>V_max)//校验是否正确 最小电压不能超过最大电压
							{//交换二者电压
								V_temp=V_min;
								V_min=V_max;
								V_max=V_temp;
							}
							break;
						}
						case 16://模拟电压输出
						{
							V_sel=0;//电压选择位初始化
							V_buffer[0]=(Set_v/100)%10;//模拟电压缓冲区初始化
							V_buffer[1]=(Set_v/10)%10;//模拟电压缓冲区初始化
							V_buffer[2]=Set_v%10;//模拟电压缓冲区初始化
							while(1)
							{
								Key_Value=Read_key();//读取按键值
								if((Key_Value>0)&&(Key_Value<11))//赋予值1-10 对应0-9 S0-9选择电压赋值
								{
									V_buffer[V_sel]=Key_Value-1;
									Key_Value=0;//按键值归位
								}
								if(Key_Value==11)//位加一 S11选择电压选择位
								{
									V_sel++;
									if(V_sel==3)
									{
										V_sel=0;
									}
									Key_Value=0;//按键值归位
								}
								if(Key_Value==12)//确认设置预计电压并退出 S12选择确认退出
								{
									Key_Value=0;//按键值归位
									break;//跳出循环
								}
								if(Key_Value==13)//显示当前模拟输出的电压
								{
									Key_Value=0;//按键值归位
									while(1)
									{
										Key_Value=Read_key();//读取按键值
										current_V=PCF8591(0);//获取当前输出的电压
										Segdisplay(1,14,0);
										Segdisplay(2,15,0);
										Segdisplay(3,16,0);
										Segdisplay(4,16,0);
										Segdisplay(5,16,0);
										Segdisplay(6,(current_V/100)%10,0);
										Segdisplay(7,(current_V/10)%10,0);
										Segdisplay(8,current_V%10,0);
										if(Key_Value==1)//S1按下退出
										{
											Key_Value=0;//按键值归位
											break;
										}
									}
								}
								//显示EAnnnxxx字样
								LED_Display((0x01)<<V_sel);
								Segdisplay(1,14,0);
								Segdisplay(2,10,0);
								Segdisplay(3,16,0);
								Segdisplay(4,16,0);
								Segdisplay(5,16,0);
								Segdisplay(6,V_buffer[0],0);
								Segdisplay(7,V_buffer[1],0);
								Segdisplay(8,V_buffer[2],0);
							}//恢复显示
							LED_Display((0x00)|(0x01<<(4+V_alarm_mode))|((0x01&V_flag)<<7));//0 0 0 1 x x x y
							if((100*V_buffer[0]+10*V_buffer[1]+V_buffer[2])>255)//模拟电压设置合法性校验
							{//超过最大电压输出直接按照最大电压输出
								Set_v=255;
							}
							else
							{
								Set_v=100*V_buffer[0]+10*V_buffer[1]+V_buffer[2];
							}
							PCF8591_Analog(Set_v);//交给电压模数转换器转换
							break;
						}
						default://若没有按键按下(以及除了是退出键的按键被按下)，继续等待按键按下
						{
							Key_Value=0;//按键值清零
							break;
						}
					}
				}
				break;
			}
			default:
			{
				Key_Value=0;//按键值清零
				break;
			}
		}
	}
}
