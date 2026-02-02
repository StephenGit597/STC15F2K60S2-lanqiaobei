#include"advance.h"
#include"basic.h"
#include"math.h"
//单片机时间戳
unsigned long xdata currenttim=0;
//数码管段码表:0,1,2,3,4,5,6,7,8,9,C,P,L,N,无
unsigned char code segcode[15]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xC6,0x8E,0xC7,0xC8,0xFF};
//温度参数
unsigned long xdata temperature_now=0;//温度时间戳
unsigned char xdata temperature_ask_ok=0;//温度转换指令发送完成
float xdata temperature=0.0f;//温度数据
float xdata set_temperature=30.0f;//温度参数
unsigned char xdata temperature_flag=0;//高温指示位
//电压参数
unsigned long xdata V_now=0;//电压采集时间戳
unsigned char xdata V_ask_ok=0;//电压转换指令发送完成
unsigned char xdata Inu=0;//光照参数
//距离参数
unsigned long xdata distance_now=0;//距离转换时间戳
unsigned char xdata distance=0.0f;//距离缓冲区
unsigned char xdata prev_distance=0.0f;//存储上一次采集到的距离
unsigned char xdata set_distance=30.0f;//设置距离
unsigned char xdata distance_flag=0;//运动接近指示位
unsigned char xdata prev_move_mode_temp=0;//运动状态指示位缓冲区(前一次)
unsigned char xdata move_mode_temp=0;//运动状态指示位缓冲区(后一次)
unsigned char xdata lock_flag=0;//运动状态锁定标志位
unsigned char xdata move_mode=0;//运动状态指示位
unsigned long xdata lock_now=0;//运动状态锁定时间戳
//按键参数
unsigned long xdata key_now=0;//按键时间戳
unsigned char xdata key_1=0;//按键第一次扫描值
unsigned char xdata key_2=0;//按键第二次扫描值
unsigned char xdata key_first_ok=0;//按键第一次扫描完成标志位
unsigned char xdata key_flag=0;//按键按下而且稳定标志位
unsigned char xdata key_temp=0;//按键值缓冲区
unsigned char xdata key_value=0;//读取到的最终按键值
unsigned long xdata com_key_now=0;//组合按键检测时间戳
unsigned char xdata S8_S9_flag=0;//S8,S9组合长按开始标志位
//LED状态
unsigned char xdata led1=0;
unsigned char xdata led2=0;
unsigned char xdata led3=0;
unsigned char xdata led4=0;
unsigned char xdata led8=0;
unsigned char xdata led8_temp=0;//LED8闪烁振荡器
unsigned long xdata led_now=0;//LED闪烁时间戳
//继电器状态
unsigned char xdata relay=0;
unsigned char xdata relay_flag=0;//继电器已吸合标志位，防止循环中继电器一次吸合被多次计数
unsigned int xdata relay_num=0;
unsigned char xdata first_0_flag=0;//判断是否高位为0 用于灭零
//数码管显示模式
unsigned char xdata seg_mode=0;
//参数设置显示模式
unsigned char xdata set_mode=0;

void main(void)
{
	Timer1Init();//定时器1初始化
	Timer0Init();//定时器0初始化
	EA=1;//使能全局中断
	LED_display(0x00);//输出外设初始化
	Relay_control(0);//输出外设初始化
	while(1)
	{
		//温度采集
		if(temperature_ask_ok==0)//还没有发送温度转换指令
		{
			Temperature_ask();//发送温度转换指令
			temperature_ask_ok=1;//已发送温度转换指令
			temperature_now=currenttim;//记下当前温度时间戳
		}
		if((temperature_ask_ok==1)&&(currenttim-temperature_now>800))//已经发送温度转换指令并且已经延时足够
		{
			temperature=Temperature_receive();//接收DS18B20传回来的温度数据
			temperature_ask_ok=0;//清零温度已发送指令标志位，开始下次发送温度转换命令
		}
		//光强采集
		if(V_ask_ok==0)//还没有发送电压开始接收指令
		{
			Get_Inu_ask();//发送电压开始读取命令
			V_ask_ok=1;//表示已经发送电压读取命令
			V_now=currenttim;//记下当前电压时间戳
		}
		if((V_ask_ok==1)&&(currenttim-V_now>100))//已经发送电压转换指令并且已经延时足够
		{
			Inu=Get_Inu_receive();//接收PCF8591传回来的光照数据
			V_ask_ok=0;//清零电压已发送指令标志位，开始发送下一次电压转换指令
		}
		//距离采集
		if(currenttim-distance_now>1000)//距离数据1s采集一次
		{
			distance_now=currenttim;//跟新距离数据采集时间戳
			prev_distance=distance;//把上一次的distance值赋给上一次距离保存prev_distance
			distance=Get_distance();//读取新的距离值
		}
		//读取按键
		//读取单独按键
		if((key_flag==0)&&(key_first_ok==0))//在没有检测到按键按下需要等待放下的情况下，按键第一次没有扫描
		{
			key_1=scan_key();//读取第一次的按键值
			key_first_ok=1;//表示已经读取第一次按键值，等待下一次读取
			key_now=currenttim;//记下当前按键时间戳
		}
		if((key_flag==0)&&(key_first_ok==1)&&(currenttim-key_now>20))//在没有检测到按键按下需要等待放下的情况下，按键第一次完成读取，延时读取第二次按键值
		{
			key_2=scan_key();//读取第二次按键值
			if((key_1==key_2)&&(key_1!=0))//检测到按键值稳定，并且按键已经按下
			{
				key_temp=key_1;//按键值存储到缓冲区，等待按键放下触发
				key_flag=1;//按键按下而且稳定标志位
			}
			key_first_ok=0;//表示按键已经读取，可以进行下一次扫描
		}
		if(key_flag==1)//按键已经按下，检测按键是否放下（并且检测S8,S9组合长按）
		{
			if((key_temp!=5)&&(!scan_key()))//检测到按键放下（而且不是S8，S9组合长按）S4 S5 S8 S9单独触发
			{
				key_value=key_temp;//把缓冲区的按键值赋给按键值
				key_flag=0;//按键已按下且稳定标志位清零
			}
			if(key_temp==5)//如果读到组合按键
			{
				key_temp=0;//按键缓冲区清零
				key_flag=0;//按键已按下且稳定标志位清零
			}
		}
		//读取组合按键
		if((S8_S9_flag==0)&&(scan_key()==5))//按键检测到S8,S9同时按下，并且先前没有计时的状态
		{
			com_key_now=currenttim;//组合按键时间戳
			S8_S9_flag=1;//开始计时等待2s后读取是否还是保持同时按下
		}
		if((S8_S9_flag==1)&&(currenttim-com_key_now>2000))//触发后2s后查看状态
		{
			if(scan_key()==5)//仍然保持S8,S9同时按下的状态
			{
				key_value=5;//赋予按键值
			}
			S8_S9_flag=0;//清除按键值开始计时标志位
		}
		//按键动作响应
		if(key_value==1)//任何页面下，S4切换显示页面
		{
			seg_mode++;//依次显示界面轮换
			if(seg_mode==4)//返回循环
			{
				seg_mode=0;//返回到第一个界面
			}
			if(seg_mode==2)//当切换至参数显示页面时，参数设置模式初始化
			{
				set_mode=0;//初始为温度参数显示界面
			}
			key_value=0;//按键值清零
		}
		if(seg_mode==2)//在参数显示页面
		{
			if(key_value==2)//S5 切换参数设置子页面
			{
				if(set_mode==0)//参数设置子页面乒乓切换
				{
					set_mode=1;
				}
				else
				{
					set_mode=0;
				}
				key_value=0;//按键值清零
			}
			if(set_mode==0)//正在温度参数显示页面
			{
				if(key_value==3)//S8设置温度参数
				{
					set_temperature=set_temperature+1.0f;//温度参数自增
					if(set_temperature==81.0f)//最大值检验
					{
						set_temperature=80.0f;//最大
					}
					key_value=0;//按键值清零
				}
				if(key_value==4)//S9设置温度参数
				{
					set_temperature=set_temperature-1.0f;//温度参数自减
					if(set_temperature==19.0f)//最小值检验
					{
						set_temperature=20.0f;//最小
					}
					key_value=0;//按键值清零
				}
			}
			if(set_mode==1)//正在距离参数显示页面
			{
				if(key_value==3)//S8 设置距离参数
				{
					set_distance=set_distance+5;//距离参数自增
					if(set_distance==85)//最大值检验
					{
						set_distance=80;//最大
					}
					key_value=0;//按键值清零
				}
				if(key_value==4)//S9 设置距离参数
				{
					set_distance=set_distance-5;//距离参数自减
					if(set_distance==15)//最小值检验
					{
						set_distance=20;//最小
					}
					key_value=0;//按键值清零
				}
			}
		}
		if(seg_mode==3)//在统计显示界面
		{
			if(key_value==5)//统计值清零
			{
				relay_num=0;//继电器吸合次数清零
				key_value=0;//按键值清零
			}
		}
		//报警状态响应
		//运动接近
		if(distance<set_distance)//触发接近判断
		{
			distance_flag=1;//触发接近标志位置1
		}
		else
		{
			distance_flag=0;//清除接近标志位置1
		}
		//高温报警
		if(temperature>set_temperature)//触发高温判断
		{
			temperature_flag=1;//高温指示位置一
		}
		else
		{
			temperature_flag=0;//清除高温指示位
		}
		//运动状态在未锁定的情况下
		if(lock_flag==0)//状态还未锁定，
		{
			if(abs(distance-prev_distance)<5.0)//静止状态
			{//运动状态指示位缓冲区
				prev_move_mode_temp=move_mode_temp;//把上一次的运动状态存入上一次缓冲区
				move_mode_temp=0;//表示静止
			}
			else if(abs(distance-prev_distance)<10)//徘徊状态
			{//运动状态指示位缓冲区
				prev_move_mode_temp=move_mode_temp;//把上一次的运动状态存入上一次缓冲区
				move_mode_temp=1;//表示徘徊
			}
			else
			{//运动状态指示位缓冲区
				prev_move_mode_temp=move_mode_temp;//把上一次的运动状态存入上一次缓冲区
				move_mode_temp=2;//表示奔跑
			}
	    }
		if((lock_flag==0)&&(prev_move_mode_temp!=move_mode_temp))//物体的运动状态发生了变化并且上一次锁定时间已过3s
		{
			lock_flag=1;//运动状态锁定
			lock_now=currenttim;//记录运动状态锁定时间戳
		}
		if((lock_flag==1)&&(currenttim-lock_now>3000))//运动状态已锁定判断延时是否足够(3s)
		{
			move_mode=move_mode_temp;//更新运动状态位
			lock_flag=0;//运动状态解锁
		}
		//综合输出指示功能
		if(seg_mode!=2)//不在参数设置界面
		{
			//LED显示控制
			if(distance_flag==1)//触发接近判定
			{
				switch(Inu)//Inu对应4个光强等级
				{
					case 1://光强等级1
					{
						led1=1;//L1点亮
						led2=0;//L2熄灭
						led3=0;//L3熄灭
						led4=0;//L4熄灭
						break;
					}
					case 2://光强等级2
					{
						led1=1;//L1点亮
						led2=1;//L2点亮
						led3=0;//L3熄灭
						led4=0;//L4熄灭
						break;
					}
					case 3://光强等级3
					{
						led1=1;//L1点亮
						led2=1;//L2点亮
						led3=1;//L3点亮
						led4=0;//L4熄灭
						break;
					}
					case 4://光强等级4
					{
						led1=1;//L1点亮
						led2=1;//L2点亮
						led3=1;//L3点亮
						led4=1;//L4点亮
						break;
					}
					default:
					{
						break;//非法情况直接退出
					}
				}
			}
			switch(move_mode)
			{
				case 0://运动状态为静止 熄灭
				{
					led8=0;
					break;
				}
				case 1://运动状态为徘徊 点亮
				{
					led8=1;
					break;
				}
				case 2://运动状态为跑动 闪烁
				{
					led8=led8_temp;
					break;
				}
				default://非法状态默认退出
				{
					break;
				}
			}
			if(currenttim-led_now>100)//LED闪烁功能状态切换
			{
				if(led8_temp==0)//在触发接近报警模式下 led8_temp周期性震荡
				{
					led8_temp=1;
				}
				else
				{
					led8_temp=0;
				}
				led_now=currenttim;//更新切换状态的时间
			}
			//继电器控制
			if((distance_flag==1)&&(temperature_flag==1))//同时触发接近和高温报警
			{
				relay=1;//继电器吸合
				if(relay_flag==0)
				{
					relay_num++;//吸合次数加一
					if(relay_num==10000)
					{
						relay_num=9999;//显示最大值
					}
					relay_flag=1;//继电器已经吸合
				}
			}
			else
			{
			    relay=0;//继电器断开
				relay_flag=0;//此次继电器吸合断开，可以开始下一轮计数
			}
		}
		//最终LED显示
		LED_display((0x01&led1)|((0x01&led2)<<1)|((0x01&led3)<<2)|((0x01&led4)<<3)|((0x01&led8)<<7));
		//最终继电器动作
		Relay_control(relay);
		//数码管显示
		switch(seg_mode)
		{
			case 0://环境状态显示界面
			{
				Seg_display(1,10,0);//C
				Seg_display(2,((int)(temperature))/10,0);//温度十位
				Seg_display(3,((int)(temperature))%10,0);//温度各位
				Seg_display(4,14,0);//熄灭
				Seg_display(5,14,0);//熄灭
				Seg_display(6,14,0);//熄灭
				Seg_display(7,13,0);//N
				Seg_display(8,Inu%10,0);//显示光照强度
				break;
			}
			case 1://运动检测页面
			{
				Seg_display(1,12,0);//L
				Seg_display(2,move_mode%10,0);//显示运动状态
				Seg_display(3,14,0);//熄灭
				Seg_display(4,14,0);//熄灭
				Seg_display(5,14,0);//熄灭
				Seg_display(6,(distance/100)%10,0);//距离百位
				Seg_display(7,(distance/10)%10,0);//距离十位
				Seg_display(8,distance%10,0);//距离个位
				break;
			}
			case 2://设置参数显示界面
			{
				if(set_mode==0)//温度参数显示页面
				{
					Seg_display(1,11,0);//P
					Seg_display(2,10,0);//C
					Seg_display(3,14,0);//熄灭
					Seg_display(4,14,0);//熄灭
					Seg_display(5,14,0);//熄灭
					Seg_display(6,14,0);//熄灭
					Seg_display(7,((int)(set_temperature))/10,0);//设置温度十位
					Seg_display(8,((int)(set_temperature))%10,0);//设置温度各位
				}
				else//距离参数显示页面
				{
					Seg_display(1,11,0);//P
					Seg_display(2,13,0);//L
					Seg_display(3,14,0);//熄灭
					Seg_display(4,14,0);//熄灭
					Seg_display(5,14,0);//熄灭
					Seg_display(6,14,0);//熄灭
					Seg_display(7,(set_distance)/10,0);//设置距离各位
					Seg_display(8,(set_distance)%10,0);//设置距离各位
				}
				break;
			}
			case 3://统计显示界面
			{
				Seg_display(1,13,0);//N
				Seg_display(2,10,0);//C
				Seg_display(3,14,0);//熄灭
				Seg_display(4,14,0);//熄灭
				if((first_0_flag==0)&&((relay_num/1000)%10==0))//判断高位是不是空0
				{
					Seg_display(5,14,0);//高位灭零显示
				}
				else
				{
					Seg_display(5,(relay_num/1000)%10,0);//继电器吸合次数 千位
					first_0_flag=1;//高位有数标志位置1，表示后面的0不能省略
				}
				if((first_0_flag==0)&&((relay_num/100)%10==0))//判断高位是不是空0
				{
					Seg_display(5,14,0);//高位灭零显示
				}
				else
				{
					Seg_display(6,(relay_num/100)%10,0);//继电器吸合次数 百位
					first_0_flag=1;//高位有数标志位置1，表示后面的0不能省略
				}
				if((first_0_flag==0)&&((relay_num/10)%10==0))
				{
					Seg_display(5,14,0);//高位灭零显示
				}
				else
				{
					Seg_display(7,(relay_num/10)%10,0);//继电器吸合次数 十位
				}//最低位任何情况都必须显示
				Seg_display(8,relay_num%10,0);//继电器吸合次数 个位
				break;
			}
			default:
			{
				break;//非法状态 直接跳过
			}
		}
	}
}