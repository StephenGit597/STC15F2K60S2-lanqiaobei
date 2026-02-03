#include"advance.h"
#include"basic.h"
//单片机时间戳
unsigned long xdata currenttim=0;
//计时时间
unsigned char xdata hour=0;
unsigned char xdata minute=0;
unsigned char xdata second=0;
//数码管段码 0 1 2 3 4 5 6 7 8 9 - 空
unsigned char code segcode[12]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};
//按键值状态读取
unsigned long xdata key_now=0;//按键判断时间戳
unsigned char xdata key_first_flag=0;//按键第一次按下标志位
unsigned char xdata key_1=0;//用于存储第一次按键读取到的值
unsigned char xdata key_2=0;//用于存储第二次按键读取到的值
unsigned char xdata key_flag=0;//按键已经按下而且稳定标志位
unsigned char xdata key_temp=0;//按键值缓冲区，在按键放下生效
unsigned char xdata key_value=0;//最终单片机读取到的按键值
//工作模式状态
unsigned char xdata seg_mode=0;//数码管显示模式
unsigned long xdata tim_now=0;//时间获取时间戳
unsigned char xdata tim_flag=10;//时间显示指示字符
unsigned long xdata tim_flag_now=0;//时间显示指示字符时间戳
//温度采集状态位
unsigned long xdata temperature_now=0;//温度采集时间戳
unsigned char xdata temperature_ask_ok=0;//温度转换指令发送完成标志位
unsigned char xdata temperature[10]={0,0,0,0,0,0,0,0,0,0};//采集到的温度的缓冲区
unsigned char xdata temperature_indicate=0;//温度采集指示位
unsigned char xdata temperature_complete_flag=0;//温度采集完成标志位
unsigned char xdata temperature_seg_indicate=0;//温度显示指示位
unsigned char xdata temperature_mode=0;//设置不同的温度采集时间间隔
unsigned int xdata temperature_latency=0;//温度采集时间间隔
//led状态
unsigned long xdata led1_now=0;//LED当前时间戳
unsigned char xdata led1_blanking=0;//LED1需要闪烁标志位
unsigned char xdata led1=0;//当前的LED状态
void main(void)
{
	Timer1Init();//定时器1初始化
	EA=1;//使能全局中断
	ds1302_Set_time(23,59,50);//设置初始时间
	Led_display(0x00);//LED显示初始化
	while(1)
	{
		//读取按键
		if((key_flag==0)&&(key_first_flag==0))//之前按键没有按下已稳定，而且是第一次获取按键时间戳
		{
			key_1=Get_key();//获取第一次的按键值
			key_now=currenttim;//记下按键第一次按下的时间戳
			key_first_flag=1;//表示按键第一次已经按下，后续无需扫描
		}
		if((key_flag==0)&&(key_first_flag==1)&&(currenttim-key_now>25))//之前按键没有按下已稳定，第一次按键按下，到达指定延时
		{
			key_2=Get_key();//获取第二次的按键值
			key_first_flag=0;//完成第二次按键的读取
		}
		if((key_flag==0)&&(key_1==key_2)&&(key_1!=0))//按键已经按下并且已经稳定
		{
			key_flag=1;//表示按键按下已经稳定
			key_temp=key_1;//把获取的值读入按键缓冲区
		}
		if((key_flag==1)&&(!Get_key()))//按键稳定并且按键放下
		{
			key_value=key_temp;//把缓冲区赋给按键值
			key_flag=0;//清除按键按下已稳定标志位，便于下一轮读取
		}
		//在不同界面下，不同按键的动作响应
		if(seg_mode==0)//位于温度采集参数设定界面上
		{
			if(key_value==1)//S4 按下切换温度采集时间间隔模式
			{
				temperature_mode++;//按一次自增一次
				if(temperature_mode==4)//如果到达上界 切换至初始状态
				{
					temperature_mode=0;//初始1s采集状态
				}
				key_value=0;//清除按键值
			}
			if(key_value==2)//S5 确认温度采集模式并开始温度采集
			{
				seg_mode=1;//进入温度采集界面显示模式
				key_value=0;//清除按键值
			}
		}
		if(seg_mode==2)//在温度数据显示界面上
		{
		    if(key_value==3)//S6 按下切换温度显示索引
			{
                temperature_seg_indicate++;//温度显示索引自增
				if(temperature_seg_indicate==10)//温度索引到达边界10
				{
					temperature_seg_indicate=0;//回到起始状态
				}
				led1_blanking=0;//LED停止闪烁
				led1=0;//LED1熄灭
				key_value=0;//清除按键值
			}
            if(key_value==4)//S7 按下退出温度数据显示界面
			{
				seg_mode=0;//返回温度采集参数设定界面
				led1_blanking=0;//LED停止闪烁
				led1=0;//LED1熄灭
				temperature_seg_indicate=0;//按键索引值清零
				key_value=0;//清除按键值
			}				
		}
		//执行对应动作
		//延时值设定
		switch(temperature_mode)
		{
			case 0://模式0 温度采集间隔1s
			{
				temperature_latency=1000;//1s
				break;
			}
			case 1://模式1 温度采集间隔5s
			{
				temperature_latency=5000;//5s
				break;
			}
			case 2://模式2 温度采集间隔30s
			{
				temperature_latency=30000;//30s
				break;
			}
			case 3://模式3 温度采集间隔60s
			{
				temperature_latency=60000;//60s
				break;
			}
			default://默认采集间隔1s
			{
				temperature_latency=1000;//1s
				break;
			}
		}
		//温度采集
		if((temperature_ask_ok==0)&&(seg_mode==1)&&(temperature_complete_flag==0))//在温度采集界面下，温度数据没有采集完并且没有发送温度转换指令发送温度转换指令
		{
			Get_temperature_ask();//发送温度转换指令
			temperature_ask_ok=1;//表示已经发送温度转换指令
			temperature_now=currenttim;//记下当前指令发送完成时间戳
		}
		if((temperature_ask_ok==1)&&(seg_mode==1)&&(currenttim-temperature_now>temperature_latency)&&(temperature_complete_flag==0))//在温度采集界面下，温度数据没有采集完成发送温度转换指令完成到达规定延时
		{
			temperature[temperature_indicate]=Get_temperature_receive();//采集本次获取到的温度到对应的缓冲区
			temperature_indicate++;//温度指示自增采集下一个数据
			if(temperature_indicate==10)//温度数据采集完成
			{
				temperature_indicate=0;//清零温度指示位
				temperature_complete_flag=1;//温度采集完成标志位置1
			}
			temperature_ask_ok=0;//清除已经发送温度转换指令标志位
		}
		if(temperature_complete_flag==1)//温度采集完成切换界面至温度数据显示界面
		{
			seg_mode=2;//进入温度数据显示界
			led1_blanking=1;//LED需要闪烁
			temperature_complete_flag=0;//清除温度采集完成标志位
		}
		//时间获取并且时间标志闪烁
		if((seg_mode==1)&&(currenttim-tim_now>500))//在时间显示界面上500ms采集一次时间
		{
			ds1302_Get_time();//获取当前实时时间
			tim_now=currenttim;//更新当前时间采集时间戳
		}
		if((seg_mode==1)&&(currenttim-tim_flag_now>1000))//在时间显示界面上1s变化一次时间标志状态
		{
			if(tim_flag==11)//时间标志状态0-1乒乓变化
			{
				tim_flag=10;
			}
			else//时间标志状态10-11乒乓变化
			{
				tim_flag=11;
			}
			tim_flag_now=currenttim;//更新时间标志位时间戳
		}
		//LED动作
		if((led1_blanking==1)&&(currenttim-led1_now>400))//400ms闪烁一次
		{
			if(led1==1)//LED状态0-1乒乓变换
			{
				led1=0;
			}
			else//LED状态0-1乒乓变换
			{
				led1=1;
			}
			led1_now=currenttim;//更新LED当前时间戳
		}
		//LED显示
		Led_display(0x01&led1);//单独控制LED1
		//数码管显示
		switch(seg_mode)
		{
			case 0://显示温度采集参数设定界面
			{
				Seg_Display(1,11,0);//熄灭
				Seg_Display(2,11,0);//熄灭
				Seg_Display(3,11,0);//熄灭
				Seg_Display(4,11,0);//熄灭
				Seg_Display(5,11,0);//熄灭
				Seg_Display(6,10,0);//提示符
				Seg_Display(7,(temperature_latency/1000)/10,0);//时间间隔十位
				Seg_Display(8,(temperature_latency/1000)%10,0);//时间间隔个位
				break;
			}
			case 1://时间显示界面
			{
				Seg_Display(1,hour/10,0);//小时十位
				Seg_Display(2,hour%10,0);//小时个位
				Seg_Display(3,tim_flag,0);//提示符
				Seg_Display(4,minute/10,0);//分钟十位
				Seg_Display(5,minute%10,0);//分钟个位
				Seg_Display(6,tim_flag,0);//提示符
				Seg_Display(7,second/10,0);//秒十位
				Seg_Display(8,second%10,0);//秒个位
				break;
			}
			case 2://数据显示界面
			{
				Seg_Display(1,10,0);//提示符
				Seg_Display(2,temperature_seg_indicate/10,0);//索引值十位
				Seg_Display(3,temperature_seg_indicate%10,0);//索引值个位
				Seg_Display(4,11,0);//熄灭
				Seg_Display(5,11,0);//熄灭
				Seg_Display(6,10,0);//提示符
				Seg_Display(7,temperature[temperature_seg_indicate]/10,0);//温度值十位
				Seg_Display(8,temperature[temperature_seg_indicate]%10,0);//温度值个位
				break;
			}
			default://非法情况
			{
				seg_mode=0;//进入温度参数设定界面
				break;
			}
		}
	}
}