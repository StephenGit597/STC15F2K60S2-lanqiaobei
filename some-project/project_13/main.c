#include"advance.h"
#include"basic.h"

//数码管段码表
unsigned char code segcode[13]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xc1,0xFF};
//时间保存变量
volatile unsigned long xdata currenttim=0;
unsigned char xdata hour=0;
unsigned char xdata minute=1;
unsigned char xdata second=1;
//温度转换
unsigned char xdata temperature_fisrt_ok=0;//温度转换指令发送完成标志位
unsigned long xdata temperature_now=0;//温度时间戳
float temperature=0.0f;//温度保存变量
float set_temperature=23.0f;//设置温度保存变量
//时间转换
unsigned long xdata tim_now=0;//时间获取时间戳
//按键值读取
unsigned char xdata key_1=0;//按键第一次扫描值
unsigned char xdata key_2=0;//按键第二次扫描值
unsigned long xdata key_now=0;//按键计时时间戳
unsigned char xdata key_first_ok=0;//按键第一次扫描完毕标志，否则循环会重复覆盖计时
unsigned char xdata key_flag=0;//按键按下并且已稳定标志位
unsigned char xdata key_temp=0;//按键值缓冲区 等待按键值放下赋值给模式
unsigned char xdata key_value=0;//最终读取到的按键值
//LED灯控制(状态判断)
unsigned char xdata led=0;//led变化
unsigned long xdata led_now=0;//LED时间戳
unsigned char xdata led_first_ok=0;//LED第一次变化完成标志位
//LED灯对应的控制模式
unsigned char xdata relay_on=0;//继电器已吸合标志位
unsigned char xdata tim_on=0;//时间到达整点标志位
unsigned char xdata mode=0;//继电器控制模式
unsigned long xdata relay_now=0;//用于判断继电器吸合的时间
//显示模式
unsigned char xdata display_mode=0;//显示模式
void main(void)
{
	Timer0Init();//定时器0初始化
	EA=1;//打开总中断使能
	Relay_control(0);//继电器初始化
	LED_Display(0x00);//LED位显示初始化
	Set_Time(11,59,54);//设置初始时间
	while(1)//进入显示函数
	{
		LED_Display((0x01&tim_on)|((0x01&!mode)<<1)|((0x01&led)<<2));//LED各个标志位的状态机显示
		//温度获取
		if(temperature_fisrt_ok==0)//未发送温度转换指令
		{
			Get_tempature_ask();//发送温度转换指令
			temperature_fisrt_ok=1;//温度转换指令发送完成标志位置1
			temperature_now=currenttim;//记下温度当前时间戳
		}
		if((currenttim-temperature>750)&&(temperature_fisrt_ok==1))//温度转换命令已发送，并且延时到达
		{
			temperature_fisrt_ok=0;//温度转换指令发送标志位清零
			temperature=Get_tempature_receive();//读取温度传感器的值
		}
		//时间获取
		if(currenttim-tim_now>500)//500ms获取一次时间
		{
			tim_now=currenttim;//更新获取时间标志位
			Get_Time();//获取ds1302时间
		}
		//按键读取
		if((key_first_ok==0)&&(key_flag==0))//若按键第一次没有扫描，并且按键未读取到稳定的值
		{
			key_1=scan_key();//读取第一次读到的按键值
			key_first_ok=1;//按键第一次扫描完毕标志位置1 表示已经完成第一次扫描
			key_now=currenttim;//记下按键当前时间戳
		}
		if((currenttim-key_now>10)&&(key_first_ok==1)&&(key_flag==0))//按键第一次已经扫描，到达指定延时，并且按键未读取到稳定的值
		{
			key_2=scan_key();//读取第二次读到的按键值
			if((key_1!=0)&&(key_1==key_2))//检测到按键按下并且已稳定
			{
				key_flag=1;//按键按下并且已稳定标志位置1
				key_temp=key_1;//把按键值保存到按键缓冲区等待按键放下
			}
			key_first_ok=0;//按键第一次扫描完毕标志位置0（清除）
		}
		if(key_flag==1)//按键按下并且已稳定
		{//判断按键是否放下
			if(!scan_key())
			{
				key_value=key_temp;//把缓冲区的按键值赋值给最终的按键值
				key_flag=0;//清除按键按下并且已稳定标志位
			}
		}
		//分析按键值
		if(key_value==1)//S12 切换显示页面
		{
			key_value=0;//清除最终的按键值
			display_mode++;
			if(display_mode==3)
			{
				display_mode=0;
			}
		}
		if(key_value==2)//S13 切换继电器控制模式
		{
			key_value=0;//清除最终的按键值
			relay_on=0;//清除继电器已吸合标志位
			tim_on=0;//清除时间到达整点标志位
			led=0;//L3的显示状态清零
			mode=!mode;//模式切换
			Relay_control(0);//继电器放下
		}
		if(display_mode==2)//如果是在参数设置模式
		{
			//S16按下
			if(key_value==3)//设置温度减一
			{
				key_value=0;//清除最终的按键值
				set_temperature=set_temperature-1.0f;
				if(set_temperature==9.0f)//温度下限
				{
					set_temperature=10.0f;//最低10.0f
				}
			}
			//S17按下
			if(key_value==4)//设置温度加一
			{
				key_value=0;//清除最终的按键值
				set_temperature=set_temperature+1.0f;
				if(set_temperature==100.0f)//温度下限
				{
					set_temperature=99.0f;//最高99.0f
				}
			}
		}
		//LED（L2）闪烁
		//继电器吸合指示灯
		if((relay_on)&&(led_first_ok==0))//继电器吸合并且第一次LED转换未完成
		{
			led=!led;//转换LED的标志位，使其闪烁
			led_first_ok=1;//LED第一次转换完成
			led_now=currenttim;//记录当前LED时间戳
		}
		if((relay_on)&&(currenttim-led_now>100)&&(led_first_ok==1))
		{
			led_first_ok=0;//清除LED第一次转换完成标志位
		}
		//到达时钟整点
		//L1整点长亮
		if((minute==0)&&(second==0)&&(mode==0))//普通模式整点LED长亮5s
		{
			tim_on=1;//整点标志位置一
			led_now=currenttim;//记录整点时间戳
		}
		if((mode==0)&&(currenttim-led_now>5000))//长亮5s
		{
			tim_on=0;
		}
		//温度模式判断
		if(mode==0)//温度判断模式
		{
			if(temperature>set_temperature)//温度大于设定值
			{
				Relay_control(1);//继电器吸合
				relay_on=1;//继电器吸合标志位置1
			}
			else
			{
				Relay_control(0);//继电器断开
				relay_on=0;//清除继电器吸合标志位
				led=0;//L3的显示清零
			}
		}
		else//时间判断模式
		{
			if((minute==0)&&(second==0))
			{
				Relay_control(1);//继电器吸合
				relay_on=1;//继电器吸合标志位置1
				tim_on=1;//时间到达整点标志位置1
				relay_now=currenttim;//记下当前继电器吸合的时间
			}
			else
			{
				if(relay_on==1)
				{
					if(currenttim-relay_now>5000)
					{
						relay_on=0;//清除继电器吸合标志位
						tim_on=0;//时间到达整点标志位清零
						Relay_control(0);//继电器断开
						led=0;//L3的状态清零
					}
				}
			}
		}
		//数码管显示
		switch(display_mode)
		{
			case 0://显示温度显示页面
			{
				Seg_Display(1,11,0);//U
				Seg_Display(2,1,0);//1
				Seg_Display(3,12,0);//熄灭
				Seg_Display(4,12,0);//熄灭
				Seg_Display(5,12,0);//熄灭
				Seg_Display(6,((int)(temperature))/10,0);//温度整数位
				Seg_Display(7,((int)(temperature))%10,1);//温度整数位
				Seg_Display(8,((int)(temperature*10))%10,0);//温度小数位
				break;
			}
			case 1://时间显示页面
			{
				if(scan_key()==4)//在时间显示模式下，若检测到S17持续按下
				{
					Seg_Display(1,11,0);//U
					Seg_Display(2,2,0);//2
					Seg_Display(3,12,0);//熄灭
					Seg_Display(4,minute/10,0);//分钟
					Seg_Display(5,minute%10,0);//分钟
					Seg_Display(6,10,0);//-
					Seg_Display(7,second/10,0);//秒
					Seg_Display(8,second%10,0);//秒
				}
				else//检测到S17没有按下
				{
					Seg_Display(1,11,0);//U
					Seg_Display(2,2,0);//2
					Seg_Display(3,12,0);//熄灭
					Seg_Display(4,hour/10,0);//小时
					Seg_Display(5,hour%10,0);//小时
					Seg_Display(6,10,0);//-
					Seg_Display(7,minute/10,0);//分钟
					Seg_Display(8,minute%10,0);//分钟
				}
				break;
			}
			case 2://温度参数设置界面
			{
				Seg_Display(1,11,0);//U
				Seg_Display(2,3,0);//3
				Seg_Display(3,12,0);//熄灭
				Seg_Display(4,12,0);//熄灭
				Seg_Display(5,12,0);//熄灭
				Seg_Display(6,12,0);//熄灭
				Seg_Display(7,((int)(set_temperature))/10,0);//整数位
				Seg_Display(8,((int)(set_temperature))%10,0);//整数位
				break;
			}
			default://防止意外参数越界
			{
				break;//什么都不要做开启下一次循环
			}
		}
	}
}