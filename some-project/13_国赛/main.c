#include"basic_driver.h"
#include"advance_driver.h"
//单片机时间戳
volatile unsigned long xdata currenttim=0;
//定时器1重复计数部分
volatile unsigned char xdata Re_num=0;
unsigned char xdata PWM_num=0;//PWM参数
//数码管段码 0 1 2 3 4 5 6 7 8 9 A F P H 熄灭
unsigned char code segcode[15]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x8e,0x8C,0x89,0xFF};
//按键获取
unsigned char xdata key_1=0;//按键第一次读取到的值
unsigned char xdata key_2=0;//按键第二次读取到的值
unsigned char xdata key_temp=0;//按键值缓存区
unsigned char xdata key_value=0;//按键最终获得的值
unsigned char xdata key_first_ok=0;//按键第一次扫描完成标志位
unsigned char xdata key_flag=0;//按键按下已稳定标准位
unsigned long xdata key_now=0;//按键当前时间戳
unsigned char xdata key_wait_flag=0;//按键长按开始等待标志位
unsigned long xdata key_long_wait=0;//按键长按等待时间戳
//页面模式
unsigned char xdata General_mode=0;//总界面 0：频率界面 1：湿度界面 2：距离界面 3：参数界面
unsigned char xdata pare_mode=0;//参数界面具体模式 0：频率参数 1：湿度参数 2：距离参数
unsigned char xdata Feq_seg_mode=0;//频率单位
unsigned char xdata distance_seg_mode=0;//距离单位
//参数值
float xdata para_Feq=9.0f;//频率参数值
unsigned char xdata para_humdity=40;//湿度参数值
float xdata para_distance=0.6f;//距离参数值
//频率
unsigned int xdata Feq=0;//测得频率初始化
unsigned char xdata Feq_Ask_Ok=0;//频率获取请求已发送标志位
unsigned long xdata Feq_now=0;//频率获取时间戳
unsigned char xdata Feq_flag=0;//频率超过频率参数标志位
//湿度
unsigned char xdata V_raw =0;//获取的原始电压数据
unsigned char xdata humdity=0;//湿度最终数据
unsigned char xdata humdity_Ask_OK=0;//湿度数据采集获取请求已发送
unsigned char xdata V_Trans=0;//湿度转换电压
unsigned char xdata humdity_flag=0;//湿度超限标志位
unsigned long xdata humdity_now=0;//湿度获取时间戳
//距离
unsigned long xdata distance_now=0;//距离采集时间戳
unsigned char xdata distance=0;//采集到的距离
unsigned char xdata distance_flag=0;//距离超限标志位
//继电器
unsigned char xdata Relay_num=0;//继电器吸合次数
unsigned char xdata Relay_On=0;//继电器状态
//LED
unsigned char xdata Led1=0;
unsigned char xdata Led2=0;
unsigned char xdata Led3=0;
unsigned char xdata Led4=0;
unsigned char xdata Led5=0;
unsigned char xdata Led6=0;
unsigned long xdata Led_now=0;
//数码管
unsigned char xdata Feq_seg_flag=0;//频率Hz高位灭零标志位
unsigned char xdata distance_seg_flag=0;//距离高位灭零标志位

void main(void)
{
	//定时器1初始化
	Timer1Init();
	EA=1;//使能全局中断
	Timer0_couter();//定时器0先配置为计数器给NE555
	Relay_control(0);//初始状态继电器断开
	Led_Display(0x00);//初始LED全部熄灭
	//第一次上电给AT24C02：0
	if(AT24C02_Read(0x01)!=0x66)
	{
		AT24C02_Write(0x00,0x00);//初始化继电器吸合次数
		AT24C02_Write(0x01,0x66);//给予上电认证，表示已经上过电
	}
	//获取继电器吸合次数值
	Relay_num=AT24C02_Read(0x00);
	while(1)
	{
		//按键读取
		if((key_flag==0)&&(key_first_ok==0))//按键没有按下并稳定，执行第一次扫描
		{
			key_1=scan_key();//读取第一次按键值
			key_first_ok=1;//第一次扫描完毕
			key_now=currenttim;//记下按键第一次扫描完成时间戳
		}
		if((key_flag==0)&&(key_first_ok==1)&&(currenttim-key_now>20))//按键没有按下并稳定，20ms后 执行第二次扫描
		{
			key_2=scan_key();//读取第二次按键值
			key_first_ok=0;//清除第一次扫描完毕标志位
		}
		if((key_flag==0)&&(key_1==key_2)&&(key_2!=0))//按键按下并持续稳定
		{
			if(key_2==4)//S7被按下(判断长按或者短按)
			{
				key_wait_flag=1;//按键长按开始检测等待标准位置1
				key_flag=1;//按键按下已稳定标准位置1
				key_long_wait=currenttim;//按键长按开始等待
			}
			else
			{
				key_flag=1;//按键按下已稳定标准位置1
				key_temp=key_2;//暂存按键值
			}
		}
		if((key_flag==1)&&(!scan_key()))//在按键按下已稳定的情况下，检测到按键放下
		{
			if(key_wait_flag==0)//S4 S5 S6
			{
				key_flag=0;//清除按键按下已稳定标准位
				key_value=key_temp;//赋予最终按键值
			}
			else//S7 检测长按还是短按
			{
				if(currenttim-key_long_wait>1000)//是长按情况
				{
					key_value=5;//长按最终按键值为5
					key_wait_flag=0;//清除长按等待标志位
					key_flag=0;//清除按键按下已稳定标准位
				}
				else//是短按情况
				{
					key_value=4;//短按最终按键值为4
					key_wait_flag=0;//清除长按等待标志位
					key_flag=0;//清除按键按下已稳定标准位
				}
			}
		}
		//按键对应业务逻辑
		//在任何界面下S4切换总界面
		if(key_value==1)
		{
			if(General_mode==3)//到最后一个界面切换到第一个界面
			{
				General_mode=0;//切换到第一个界面
				Timer0_couter();//定时器0切回计数器模式给NE555测量频率
			}
			else//页面自增
			{
				if(General_mode==1)
				{
					Feq_flag=0;//每次由频率获取功能切出时清除频率超限标志位
				}
				if(General_mode==2)//每次由距离切到参数是默认显示参数界面
				{
					pare_mode=0;
					distance_flag=0;//清除距离超限标志位
					Timer0_Timer();//定时器0切换为定时器模式给测距使用
				}
				General_mode++;
			}
			key_value=0;//清除按键值防止多次触发
		}
		if(General_mode==0)//在频率界面下
		{
			if(key_value==4)//按下S7（短按）切换频率单位
			{
				if(Feq_seg_mode==0)//Hz KHz 单位乒乓切换
				{
					Feq_seg_mode=1;
				}
				else
				{
					Feq_seg_mode=0;
				}
				key_value=0;//清除按键值防止多次触发
			}
		}
		if(General_mode==1)//在湿度界面下
		{
			if(key_value==5)//S7长按清除继电器吸合次数
			{
				Relay_num=0;//清除内存中继电器吸合次数
				AT24C02_Write(0x00,0x00);//清除ROM的数据
				key_value=0;//清除按键值防止多次触发
			}
		}
		if(General_mode==2)//在距离界面下
		{
			if(key_value==3)//按下S6 切换距离显示单位
			{
				if(distance_seg_mode==0)//距离单位CM M乒乓切换
				{
					distance_seg_mode=1;
				}
				else
				{
					distance_seg_mode=0;
				}
				key_value=0;//清除按键值防止多次触发
			}
		}
		if(General_mode==3)//当前在参数切换界面
		{
			if(key_value==2)//S5切换参数界面
			{
				if(pare_mode==2)//到最后一个界面切换到第一个界面
				{
					pare_mode=0;//切换到第一个界面
				}
				else//页面自增
				{
					pare_mode++;
				}
				key_value=0;//清除按键值防止多次触发
			}
			if(pare_mode==0)//在频率参数设置子界面
			{
				if(key_value==3)//S6被按下增加频率参数值0.5kHz
				{
					if(para_Feq==12.0f)//频率参数达到最大允许值
					{
						para_Feq=1.0f;//频率参数变为最小值
					}
					else
					{
						para_Feq=para_Feq+0.5f;//频率值自增500Hz
					}
					key_value=0;//清除按键值防止多次触发
				}
				if(key_value==4)//S7被按下减少频率参数值0.5kHz
				{
					if(para_Feq==1.0f)//频率参数达到最小允许值
					{
						para_Feq=12.0f;//频率参数变为最大允许值
					}
					else
					{
						para_Feq=para_Feq-0.5f;//频率值自减500Hz
					}
					key_value=0;//清除按键值防止多次触发
				}
			}
			if(pare_mode==1)//在湿度参数设置子界面
			{
				if(key_value==3)//S6被按下增加湿度参数值10%
				{
					if(para_humdity==60)//湿度参数达到最大允许值
					{
						para_humdity=10;//湿度参数变为最小值
					}
					else
					{
						para_humdity=para_humdity+10;//湿度值自增10%
					}
					key_value=0;//清除按键值防止多次触发
				}
				if(key_value==4)//S7被按下减小湿度参数值10%
				{
					if(para_humdity==10)//湿度参数达到最小允许值10
					{
						para_humdity=60;//湿度参数变为最大值60
					}
					else
					{
						para_humdity=para_humdity-10;//湿度值自减10%
					}
					key_value=0;//清除按键值防止多次触发
				}
			}
			if(pare_mode==2)//在距离参数设置子界面
			{
				if(key_value==3)//S6被按下增加距离参数值0.1f
				{
					if(para_distance==1.2f)//距离参数达到最大允许值
					{
						para_distance=0.1f;//距离参数变为最小值
					}
					else
					{
						para_distance=para_distance+0.1f;//距离值自增0.1f
					}
					key_value=0;//清除按键值防止多次触发
				}
				if(key_value==4)//S6被按下减小距离参数值0.1f
				{
					if(para_distance==0.1f)//距离参数达到最小允许值
					{
						para_distance=1.2f;//距离参数变为最大值
					}
					else
					{
						para_distance=para_distance-0.1f;//距离值自减0.1f
					}
					key_value=0;//清除按键值防止多次触发
				}
			}
		}
		//执行测量和输出
		//获取频率(在频率界面才获取)
		if(General_mode==0)
		{
			if(Feq_Ask_Ok==0)//未发送频率获取请求的情况下
			{
				Get_Feq_Ask();//发送频率获取请求
				Feq_Ask_Ok=1;//频率获取请求发送完毕
				Feq_now=currenttim;//记录当前频率时间戳
			}
			if((Feq_Ask_Ok==1)&&(currenttim-Feq_now>1000))//在发送频率获取请求的情况下，1s后获取新的计数
			{
				Feq=Get_Feq_Receive();//获取频率
				Feq_Ask_Ok=0;//清除频率获取标志位
				if(Feq>para_Feq*1000)//获取频率大于频率参数(注意单位)
				{
					Feq_flag=1;
				}
				else//否则清除此标志位
				{
					Feq_flag=0;
				}
				//输出DAC电压
				if(Feq<=para_Feq)
				{
					V_Transfer(51);
				}
				else if((Feq>para_Feq)&&(Feq<80))
				{
					V_Transfer((unsigned char)(204*(Feq-para_Feq)/(80-para_Feq)));
				}
				else
				{
					V_Transfer(255);
				}
			}
		}
		//获取湿度（任何界面都可以获取）
		if(humdity_Ask_OK==0)//当前未发送湿度获取请求
		{
			Get_V_Ask();//获取当前电压
			humdity_Ask_OK=1;//电压获取请求发送标志位置1
			humdity_now=currenttim;//获取当前时间戳
		}
		if((humdity_Ask_OK==1)&&(currenttim-humdity_now>25))
		{
			V_raw=Get_V_Receive();//接收电压
			humdity_Ask_OK=0;//清除电压获取请求发送标志位
			humdity=(unsigned char)((100*V_raw)/255.0f);
			if(humdity>para_humdity)//检查湿度超限状态
			{
				humdity_flag=1;
			}
			else//清除湿度超限标志位
			{
				humdity_flag=0;
			}
		}
		//获取距离(在距离页面才获取)
		if(General_mode==2)
		{
			if(currenttim-distance_now>200)//200ms采集一次数据
			{
				distance=Get_distance();//采集数据
				distance_now=currenttim;//更新数据采集时间戳
				if(distance>100*para_distance)//判断距离是否超限（注意单位）
				{
					distance_flag=1;//距离超限标志位置1
				}
				else
				{
					distance_flag=0;//清除距离超限标志位
				}
			}
		}
		//继电器动作
		if(distance_flag==1)//距离超限：继电器吸合
		{
			Relay_On=1;
			Relay_num++;//更新内存中的吸合值
			AT24C02_Write(0x00,Relay_num);//修改ROM中的值
		}
		else
		{
			Relay_On=0;
		}
		Relay_control(Relay_On);//继电器执行操作
		//LED动作 湿度警告在所有界面有效
		if(General_mode==0)//在频率总界面下
		{
			Led1=1;//LED1点亮
			Led2=0;//LED2熄灭
			Led3=0;//LED3熄灭
			if(Feq_flag==1)//如果频率超限，LED4点亮
			{
				Led4=1;//LED4点亮
			}
			else//否则熄灭
			{
				Led4=0;//LED4熄灭
			}
			if(humdity_flag==1)//湿度超限 LED5点亮
			{
				Led5=1;//LED5点亮
			}
			else//否则熄灭
			{
				Led5=0;//LED5熄灭
			}
		}
		if(General_mode==1)//在湿度界面下
		{
			Led1=0;//LED1熄灭
			Led2=1;//LED2点亮
			Led3=0;//LED3熄灭
			if(humdity_flag==1)//湿度超限 LED5点亮
			{
				Led5=1;//LED5点亮
			}
			else//否则熄灭
			{
				Led5=0;//LED5熄灭
			}
		}
		if(General_mode==2)//在距离界面下
		{
			Led1=0;//LED1熄灭
			Led2=0;//LED2熄灭
			Led3=1;//LED3点亮
			if(distance_flag==1)//如果距离超限，LED6点亮
			{
				Led6=1;//LED6点亮
			}
			else//否则熄灭
			{
				Led6=0;//LED6熄灭
			}
			if(humdity_flag==1)//湿度超限 LED5点亮
			{
				Led5=1;//LED5点亮
			}
			else//否则熄灭
			{
				Led5=0;//LED5熄灭
			}
		}
		if(General_mode==3)//在参数设置界面
		{
			if(pare_mode==0)//在频率参数设置界面
			{
				if(currenttim-Led_now>100)//0.1s切换一次状态
				{
					if(Led1==0)
					{
						Led1=1;//LED1点亮
					}//LED1 0-1状态以0.1s为周期乒乓切换
					else
					{
						Led1=0;//LED1熄灭
					}
					Led_now=currenttim;
				}
				Led2=0;//LED2熄灭
				Led3=0;//LED3熄灭
			}
			else if(pare_mode==1)//在湿度参数设置界面
			{
				if(currenttim-Led_now>100)//0.1s切换一次状态
				{
					if(Led2==0)
					{
						Led2=1;//LED2点亮
					}//LED2 0-1状态以0.1s为周期乒乓切换
					else
					{
						Led2=0;//LED2熄灭
					}
					Led_now=currenttim;
				}
				Led1=0;//LED1熄灭
				Led3=0;//LED3熄灭
			}
			else//在距离参数设置界面
			{
				if(currenttim-Led_now>100)//0.1s切换一次状态
				{
					if(Led3==0)
					{
						Led3=1;//LED3点亮
					}//LED3 0-1状态以0.1s为周期乒乓切换
					else
					{
						Led3=0;//LED3熄灭
					}
					Led_now=currenttim;
				}
				Led1=0;//LED1熄灭
				Led2=0;//LED2熄灭
			}
		}
		//LED执行操作
		Led_Display((0x01&Led1)|((0x01&Led2)<<1)|((0x01&Led3)<<2)|((0x01&Led4)<<3)|((0x01&Led5)<<4)|((0x01&Led6)<<5));
		//数码管动作
		switch(General_mode)
		{
			case 0://频率显示界面
			{
				if(Feq_seg_mode==0)
				{
					Seg_Display(1,11,0);//F
					Seg_Display(2,14,0);//熄灭
					Seg_Display(3,14,0);//熄灭
					if((Feq/10000==0)&&(Feq_seg_flag==0))//万位是0而且高位没有其它数
					{
						Seg_Display(4,14,0);//熄灭
					}
					else
					{
						Seg_Display(4,(Feq/10000)%10,0);//显示这一位的数字
						Feq_seg_flag=1;//表示这一位有数字后面的0不能省
					}
					if((Feq/1000==0)&&(Feq_seg_flag==0))//千位是0而且高位没有其它数
					{
						Seg_Display(5,14,0);//熄灭
					}
					else
					{
						Seg_Display(5,(Feq/1000)%10,0);//显示这一位的数字
						Feq_seg_flag=1;//表示这一位有数字后面的0不能省
					}
					if((Feq/100==0)&&(Feq_seg_flag==0))//百位是0而且高位没有其它数
					{
						Seg_Display(6,14,0);//熄灭
					}
					else
					{
						Seg_Display(6,(Feq/100)%10,0);//显示这一位的数字
						Feq_seg_flag=1;//表示这一位有数字后面的0不能省
					}
					if((Feq/10==0)&&(Feq_seg_flag==0))//十位是0而且高位没有其它数
					{
						Seg_Display(7,14,0);//熄灭
					}
					else
					{
						Seg_Display(7,(Feq/10)%10,0);//显示这一位的数字
						Feq_seg_flag=1;//表示这一位有数字后面的0不能省
					}
					Seg_Display(8,Feq%10,0);//无论如何必须显示各位数字
					Feq_seg_flag=0;//清除高位灭零标志位以便于下一次显示
				}
				else
				{
					Seg_Display(1,11,0);//F
					Seg_Display(2,14,0);//熄灭
					Seg_Display(3,14,0);//熄灭
					Seg_Display(4,14,0);//熄灭
					Seg_Display(5,14,0);//熄灭
					if(Feq/10000==0)//高位没有数字且这位为0
					{
						Seg_Display(6,14,0);//熄灭
					}
					else
					{
						Seg_Display(6,(Feq/10000)%10,0);//显示这一位
					}
					Seg_Display(7,(Feq/1000)%10,1);//显示个位
					Seg_Display(8,(Feq/100)%10,0);//显示小数点后一位
				}
				break;
			}
			case 1://当前处在湿度测量界面
			{
				Seg_Display(1,13,0);//H
				Seg_Display(2,14,0);//熄灭
				Seg_Display(3,14,0);//熄灭
				Seg_Display(4,14,0);//熄灭
				Seg_Display(5,14,0);//熄灭
				Seg_Display(6,14,0);//熄灭
				Seg_Display(7,(humdity/10)%10,0);//显示十位
				Seg_Display(8,humdity%10,0);//显示个位
				break;
			}
			case 2://当前处在距离测量界面
			{
				if(distance_seg_mode==0)//当前单位为 cm
				{
					Seg_Display(1,10,0);//A
					Seg_Display(2,14,0);//熄灭
					Seg_Display(3,14,0);//熄灭
					Seg_Display(4,14,0);//熄灭
					Seg_Display(5,14,0);//熄灭
					if((distance/100==0)&&(distance_seg_flag==0))//百位为0 高位没有数字
					{
						Seg_Display(6,14,0);//熄灭
					}
					else
					{
						Seg_Display(6,(distance/100)%10,0);//显示该位数字
						distance_seg_flag=1;//表示此位有数，后面的0不能省略
					}
					if((distance/10==0)&&(distance_seg_flag==0))//十位为0 高位没有数字
					{
						Seg_Display(7,14,0);//熄灭
					}
					else
					{
						Seg_Display(7,(distance/10)%10,0);//显示该位数字
						distance_seg_flag=1;//表示此位有数，后面的0不能省略
					}
					Seg_Display(8,distance%10,0);//无论如何最后一位都要显示
					distance_seg_flag=0;//清除高位灭零标志位以便于下一次显示
				}
				else//当前单位为 m
				{
					Seg_Display(1,10,0);//A
					Seg_Display(2,14,0);//熄灭
					Seg_Display(3,14,0);//熄灭
					Seg_Display(4,14,0);//熄灭
					Seg_Display(5,14,0);//熄灭
					Seg_Display(6,(distance/100)%10,1);//显示百位
					Seg_Display(7,(distance/10)%10,0);//显示十位
					Seg_Display(8,distance%10,0);//显示个位
				}
				break;
			}
			case 3://当前位于参数设置界面
			{
				if(pare_mode==0)//在频率参数显示界面
				{
					Seg_Display(1,12,0);//P
					Seg_Display(2,1,0);//1
					Seg_Display(3,14,0);//熄灭
					Seg_Display(4,14,0);//熄灭
					Seg_Display(5,14,0);//熄灭
					if(((unsigned char)(para_Feq))/10==0)//十位为0
					{
						Seg_Display(6,14,0);//熄灭
					}
					else
					{
						Seg_Display(6,((unsigned char)(para_Feq))/10,0);//显示该位数字
					}
					Seg_Display(7,((unsigned char)(para_Feq))%10,1);//显示个位数字
					Seg_Display(8,((unsigned char)(para_Feq*10))%10,0);//显示小数点后一位数字
				}
				else if(pare_mode==1)//在湿度参数显示界面
				{
					Seg_Display(1,12,0);//P
					Seg_Display(2,2,0);//2
					Seg_Display(3,14,0);//熄灭
					Seg_Display(4,14,0);//熄灭
					Seg_Display(5,14,0);//熄灭
					Seg_Display(6,14,0);//熄灭
					Seg_Display(7,para_humdity/10,0);//显示十位
					Seg_Display(8,para_humdity%10,0);//显示个位
				}
				else//在距离参数显示界面
				{
					Seg_Display(1,12,0);//P
					Seg_Display(2,3,0);//3
					Seg_Display(3,14,0);//熄灭
					Seg_Display(4,14,0);//熄灭
					Seg_Display(5,14,0);//熄灭
					Seg_Display(6,14,0);//熄灭
					Seg_Display(7,((unsigned char)(para_distance))%10,1);//显示个位
					Seg_Display(8,((unsigned char)(para_distance*10))%10,0);//显示小数点后第一位
				}
				break;
			}
			default://非法情况全部熄灭
			{
				Seg_Display(1,14,0);//熄灭
				Seg_Display(2,14,0);//熄灭
				Seg_Display(3,14,0);//熄灭
				Seg_Display(4,14,0);//熄灭
				Seg_Display(5,14,0);//熄灭
				Seg_Display(6,14,0);//熄灭
				Seg_Display(7,14,0);//熄灭
				Seg_Display(8,14,0);//熄灭
				break;
			}
		}
		//PWM输出
		if(Feq_flag==1)//频率超限
		{
			PWM_num=8;//占空比80%
		}
		else//频率未超限
		{
			PWM_num=2;//占空比20%
		}
		if(Re_num<PWM_num)
	    {
			P05=1;
		}
		else
		{
			P05=0;
		}
	}
}