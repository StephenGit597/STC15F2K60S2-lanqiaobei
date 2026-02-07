#include"advance_driver.h"
#include"basic.h"
//单片机系统时间戳
unsigned long xdata currenttim=0;
unsigned char xdata hour=0;
unsigned char xdata minute=0;
unsigned char xdata second=0;
//数码管段码表 0 1 2 3 4 5 6 7 8 9 A F P H L -  无
unsigned char code segcode[17]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x8E,0x8C,0x89,0xC7,0xBF,0xFF};
//矩阵按键读取
unsigned long xdata key_now=0;//按键获取时间戳
unsigned char xdata key_1=0;//按键第一次读取到的值
unsigned char xdata key_2=0;//按键第二次读取到的值
unsigned char xdata key_first_ok=0;//按键第一次扫描完成标志位
unsigned char xdata key_flag=0;//按键按下并且已稳定标志位
unsigned char xdata key_temp=0;//按键缓冲区
unsigned char xdata key_value=0;//最终扫描得到的按键值
//显示模式
unsigned char xdata seg_mode1=0;//总的界面显示模式
unsigned char xdata seg_mode2=0;//参数子界面显示模式
unsigned char xdata seg_mode3=0;//回显子界面显示模式
//频率获取
unsigned long xdata Feq_now=0;//频率获取时间戳
unsigned char xdata Feq_ask_ok=0;//频率请求发送完成标志位
unsigned int xdata raw_Feq=0;//频率原始数据
signed int xdata correction_Feq=0;//频率修正值
signed int xdata Feq=0;//最终获取的频率值
unsigned int xdata Set_Feq=2000;//频率超限参数
signed int xdata MAX_Feq=0;//最大频率
//频率显示类标志位
unsigned char xdata MAX_hour=0;//最大频率发生小时
unsigned char xdata MAX_minute=0;//最大频率发生分钟
unsigned char xdata MAX_second=0;//最大频率发生秒
unsigned char xdata correction_Feq_flag=0;//频率修正值符号标志位
unsigned char xdata Feq_error_flag=0;//最终获取到的频率无效标志位
unsigned char xdata Feq_flag=0;//最终频率高位灭零标志位
unsigned char xdata MAX_Feq_flag=0;//最大频率高位灭零标志位
unsigned char xdata Feq_alarm=0;//超限报警标志位
//电压转换
unsigned long xdata V_now=0;//电压转换时间戳
unsigned char xdata V=0;//转换电压
//时间获取时间戳
unsigned long xdata tim_now=0;//时间获取时间戳
//LED
unsigned long xdata led_now=0;//LED时间戳
unsigned char xdata led_temp=0;//LED震荡切换缓冲区
unsigned char xdata Led1=0;//LED1
unsigned char xdata Led2=0;//LED2

void main(void)
{
	Timer1Init();//定时器1初始化 
	Timer0Init();//计数器0初始化
	EA=1;//开启全局中断
	LED_display(0x00);//LED显示初始化
	DS1302_Set_Time(0,0,0);//设置上电初始时间 0时 0分 0秒
	while(1)//业务逻辑
	{
		//获取按键
		if((key_flag==0)&&(key_first_ok==0))//在未读取到稳定的有效按键值时开始第一次扫描
		{
			key_1=Scan_Martix_Key();//获取第一次的矩阵按键值
			key_first_ok=1;//第一次扫描完成标志位置1（防止时间戳被重复更新）
			key_now=currenttim;//记录当前第一次扫描时间戳
		}
		if((key_flag==0)&&(key_first_ok==1)&&(currenttim-key_now>30))//在未读取到稳定的有效按键值时开始第二次扫描
		{
			key_2=Scan_Martix_Key();//获取第二次的矩阵按键值
			key_first_ok=0;//清除第一次扫描完成标志位，进行下一次扫描
		}
		if((key_flag==0)&&(key_1==key_2)&&(key_1!=0))//在上一次按键没有被按下且稳定的情况下：检查这次的按键是否被按下并稳定
		{
			key_temp=key_1;//把本次获取到的按键值存储到缓冲区
			key_flag=1;//按键按下已稳定标志位置1
		}
		if((key_flag==1)&&(!Scan_Martix_Key()))//在按键按下已稳定的同时检测按键是否放下
		{
			key_value=key_temp;//把缓冲区的值给按键最终值
			key_flag=0;//清除按键按下已稳定标志位，便于下一次读取
		}
		//按键值执行动作
		if(key_value==1)//在任何情况：S4被按下切换总的界面显示模式
		{
			if(seg_mode1==1)//当前在参数显示界面
			{
				seg_mode2=0;//参数显示子界面回归超限参数显示子界面
			}
			if(seg_mode1==3)//当前在回显页面
			{
				seg_mode3=0;//回显子界面回归最大频率显示子界面
			}
			seg_mode1++;//显示模式自增
			if(seg_mode1==4)//0 1 2 3循环切换
			{
				seg_mode1=0;//超过有效值回到0
			}
			key_value=0;//按键值清零。防止多次触发
		}
		if(seg_mode1==1)//当前位于参数显示页面
		{
			if(key_value==2)//S5按下 切换参数子界面
			{
				if(seg_mode2==0)//超限参数 校正值来回乒乓切换
				{
					seg_mode2=1;
				}
				else//确保不切换到非法状态
				{
					seg_mode2=0;
				}
				key_value=0;//按键值清零。防止多次触发
			}
			if(seg_mode2==0)//当前位于超限参数子界面
			{
				if(key_value==3)//S8 被按下 超限参数增加1000Hz
				{
					Set_Feq=Set_Feq+1000;//超限参数加1000
					if(Set_Feq==10000)//上界检验
					{
						Set_Feq=9000;//最大值9000
					}
					key_value=0;//按键值清零。防止多次触发
				}
				if(key_value==4)//S9 被按下 超限参数减小1000Hz
				{
					Set_Feq=Set_Feq-1000;//超限参数减1000
					if(Set_Feq==0)//下界检验
					{
						Set_Feq=1000;//最小值1000
					}
					key_value=0;//按键值清零。防止多次触发
				}
			}
			if(seg_mode2==1)//当前位于校准值子界面
			{
				if(key_value==3)//S8 被按下 校正值增加100Hz
				{
					correction_Feq=correction_Feq+100;//校正值增加100
					if(correction_Feq==1000)//上界检验
					{
						correction_Feq=900;//最大值900Hz
					}
					if(correction_Feq<0)//如果频率校正值小于0
					{
						correction_Feq_flag=1;//负号标志位置1
					}
					else
					{
						correction_Feq_flag=0;//清除负号标志位
					}
					key_value=0;//按键值清零。防止多次触发
				}
				if(key_value==4)//S9 被按下 校正值减小100Hz
				{
					correction_Feq=correction_Feq-100;//校正值减少100
					if(correction_Feq==-1000)//下界检验
					{
						correction_Feq=-900;//最小值-900Hz
					}
					if(correction_Feq<0)//如果频率校正值小于0
					{
						correction_Feq_flag=1;//负号标志位置1
					}
					else
					{
						correction_Feq_flag=0;//清除负号标志位
					}
					key_value=0;//按键值清零。防止多次触发
				}
			}
		}
		if(seg_mode1==3)//当前位于回显界面
		{
			if(key_value==2)//S5被按下 切换回显子页面
			{
				if(seg_mode3==0)//频率最大值，频率最大值发生的时间来回乒乓切换
				{
					seg_mode3=1;
				}
				else//确保不切换到非法状态
				{
					seg_mode3=0;
				}
				key_value=0;//按键值清零。防止多次触发
			}
		}
		//频率获取
		if(Feq_ask_ok==0)//发生频率获取请求，（防止时间戳被重复更新）
		{
			Get_Feq_ask();
			Feq_ask_ok=1;//频率获取请求发生完成标志位置1、
			Feq_now=currenttim;//获取频率获取请求发生时间戳
		}
		if((Feq_ask_ok==1)&&(currenttim-Feq_now>1000))//1000ms后发生频率获取指令
		{
			raw_Feq=Get_Feq_receive();//获取频率原始数据
			Feq=raw_Feq+correction_Feq;//最终频率等于原始频率加校正值
			if(Feq<0)//获取的最终频率无效
			{
				Feq_error_flag=1;//获取频率无效标志位值1
			}
			else
			{
				Feq_error_flag=0;//清除频率无效标志位
				if(Feq>Set_Feq)//当前测取的频率超过设置频率
				{
					Feq_alarm=1;//频率超限标志位置1
				}
				else
				{
					Feq_alarm=0;//清除频率超限标志位
				}
				if(Feq>MAX_Feq)//采用动态更新法获取最大频率
				{
					MAX_Feq=Feq;//把当前最大值给最大值存储变量
					MAX_hour=hour;//获取当前最大频率值的时间
					MAX_minute=minute;//获取当前最大频率值的时间
					MAX_second=second;//获取当前最大频率值的时间
				}
			}
			Feq_ask_ok=0;//清除频率获取请求发生完成标志位，便于下次采集
		}
		//时间获取
		if(currenttim-tim_now>500)//500ms更新一次时间
		{
			DS1302_Get_Time();//获取时间
			tim_now=currenttim;//更新时间标志位
		}
		//电压输出
		if(currenttim-V_now>100)//100ms更新一次电压
		{
			if(Feq_error_flag==1)//频率无效
			{
				V=0;//在错误情况下电压默认输出0
			}
			else//频率合法
			{
				if(Feq<500)//0-500Hz
				{
					V=51;//电压输出1V
				}
				else if(Feq<=Set_Feq)//500-Set_FeqHz
				{//注意做除法一定要转换为浮点型，否则小数位直接被省略，导致增加量恒为0！！！
					V=(unsigned char)(51+(Feq-500)*(204.0f/((float)(Set_Feq)-500.0f)));//斜率=(y2-y1)/(x2-x1)=(255-51)/(Set_Feq-500).
				}
				else
				{
					V=255;//电压输出5v
				}
			}
			PCF8591_Trans_V(V);//输出电压
			V_now=currenttim;//更新电压时间戳
		}
		//LED动作
		if(currenttim-led_now>200)//200ms周期震荡
		{
			if(led_temp==0)//LED_temp 0-1周期乒乓切换
			{
				led_temp=1;
			}
			else//确保不切换到非法状态
			{
				led_temp=0;
			}
			led_now=currenttim;//更新LED周期震荡时间戳
		}
		if(seg_mode1==0)//当前位于频率显示界面
		{
			Led1=led_temp;//LED1以200ms周期切换亮灭状态
		}
		else
		{
			Led1=0;//LED1熄灭
		}
		if(Feq_alarm==1)//频率超限
		{
			Led2=led_temp;//LED2以200ms周期切换亮灭状态
		}
		else//没有超限，判断频率状态的正确性
		{
			if(Feq_error_flag==1)//频率数据错误
			{
				Led2=1;//LED2点亮
			}
			else
			{
				Led2=0;//LED2熄灭
			}
		}
		LED_display((0x01&Led1)|((0x01&Led2)<<1));//最终LED动作
		//数码管最终显示
		switch(seg_mode1)//总界面
		{
			case 0://频率界面
			{
				if(Feq_error_flag==0)//获取到的最终频率有效
				{
					Seg_display(1,11,0);//标志位显示 F
					Seg_display(2,16,0);//熄灭
					Seg_display(3,16,0);//熄灭
					if((Feq_flag==0)&&((Feq/10000)%10==0))//当此位是0，而且还是最高位时，熄灭
					{
						Seg_display(4,16,0);//熄灭
					}
					else//显示第一位
					{
						Feq_flag=1;//表示高位已经有数，低位的0不能熄灭
						Seg_display(4,(Feq/10000)%10,0);//显示此位的数字
					}
					if((Feq_flag==0)&&((Feq/1000)%10==0))//当此位是0，而且还是最高位时，熄灭
					{
						Seg_display(5,16,0);//熄灭
					}
					else//显示第二位
					{
						Feq_flag=1;//表示高位已经有数，低位的0不能熄灭
						Seg_display(5,(Feq/1000)%10,0);//显示此位的数字
					}
					if((Feq_flag==0)&&((Feq/100)%10==0))//当此位是0，而且还是最高位时，熄灭
					{
						Seg_display(6,16,0);//熄灭
					}
					else//显示第三位
					{
						Feq_flag=1;//表示高位已经有数，低位的0不能熄灭
						Seg_display(6,(Feq/100)%10,0);//显示此位的数字
					}
					if((Feq_flag==0)&&((Feq/10)%10==0))//当此位是0，而且还是最高位时，熄灭
					{
						Seg_display(7,16,0);//熄灭
					}
					else//显示第四位
					{
						Feq_flag=1;//表示高位已经有数，低位的0不能熄灭
						Seg_display(7,(Feq/10)%10,0);//显示此位的数字
					}
					//无论如何最低位都要显示
					Seg_display(8,Feq%10,0);
					Feq_flag=0;//显示完一次数据后，标志位清零，便于下一次数据的显示
				}
				else//获取到的频率无效
				{
					Seg_display(4,16,0);//熄灭
					Seg_display(5,16,0);//熄灭
					Seg_display(6,16,0);//熄灭
					Seg_display(7,14,0);//显示错误符 L
					Seg_display(8,14,0);//显示错误符 L
				}
				break;
			}
			case 1://参数界面
			{
				if(seg_mode2==0)//超限参数显示界面
				{
					Seg_display(1,12,0);//标志位：P
					Seg_display(2,1,0);//1
					Seg_display(3,16,0);//熄灭
					Seg_display(4,16,0);//熄灭
					Seg_display(5,(Set_Feq/1000)%10,0);//显示超限参数的千位
					Seg_display(6,0,0);//0 只能千位的变换 百位一直是0
					Seg_display(7,0,0);//0 只能千位的变换 十位一直是0
					Seg_display(8,0,0);//0 只能千位的变换 个位一直是0
				}
				else//校正置显示界面
				{
					Seg_display(1,12,0);//标志位：P
					Seg_display(2,2,0);//2
					Seg_display(3,16,0);//熄灭
					Seg_display(4,16,0);//熄灭
					//注频率校正值只能以100为单位变化，不必按位检查
					if(correction_Feq_flag==1)//频率校正值小于0
					{
						Seg_display(5,15,0);//-
						Seg_display(6,((-correction_Feq)/100)%10,0);//显示百位（注意要求绝对值，否则不显示）
						Seg_display(7,0,0);//0 只能百位的变换 十位一直是0
						Seg_display(8,0,0);//0 只能百位的变换 个位一直是0
					}
					else if(correction_Feq==0)//如果是0的情况
					{
						Seg_display(5,16,0);//熄灭
						Seg_display(6,16,0);//熄灭
						Seg_display(7,16,0);//熄灭
						Seg_display(8,0,0);//0
					}
					else//为正数的情况
					{
						Seg_display(5,16,0);//熄灭
						Seg_display(6,(correction_Feq/100)%10,0);//显示百位
						Seg_display(7,0,0);//0 只能百位的变换 十位一直是0
						Seg_display(8,0,0);//0 只能百位的变换 个位一直是0
					}
				}
				break;
			}
			case 2://时间显示界面
			{
				Seg_display(1,hour/10,0);//显示小时十位
				Seg_display(2,hour%10,0);//显示小时个位
				Seg_display(3,15,0);//分隔符
				Seg_display(4,minute/10,0);//显示分钟十位
				Seg_display(5,minute%10,0);//显示分钟个位
				Seg_display(6,15,0);//分隔符
				Seg_display(7,second/10,0);//显示秒十位
				Seg_display(8,second%10,0);//显示秒个位
				break;
			}
			case 3://回显界面
			{
				if(seg_mode3==0)//位于最大频率回显界面
				{
					Seg_display(1,13,0);//标志 H
					Seg_display(2,11,0);//标志 F
					Seg_display(3,16,0);//熄灭
					if((MAX_Feq_flag==0)&&((MAX_Feq/10000)%10==0))//当此位是0，而且还是最高位时，熄灭
					{
						Seg_display(4,16,0);//熄灭
					}
					else//显示第一位
					{
						MAX_Feq_flag=1;//表示高位已经有数，低位的0不能熄灭
						Seg_display(4,(MAX_Feq/10000)%10,0);//显示此位的数字
					}
					if((MAX_Feq_flag==0)&&((MAX_Feq/1000)%10==0))//当此位是0，而且还是最高位时，熄灭
					{
						Seg_display(5,16,0);//熄灭
					}
					else//显示第二位
					{
						MAX_Feq_flag=1;//表示高位已经有数，低位的0不能熄灭
						Seg_display(5,(MAX_Feq/1000)%10,0);//显示此位的数字
					}
					if((MAX_Feq_flag==0)&&((MAX_Feq/100)%10==0))//当此位是0，而且还是最高位时，熄灭
					{
						Seg_display(6,16,0);//熄灭
					}
					else//显示第三位
					{
						MAX_Feq_flag=1;//表示高位已经有数，低位的0不能熄灭
						Seg_display(6,(MAX_Feq/100)%10,0);//显示此位的数字
					}
					if((MAX_Feq_flag==0)&&((MAX_Feq/10)%10==0))//当此位是0，而且还是最高位时，熄灭
					{
						Seg_display(7,16,0);//熄灭
					}
					else//显示第四位
					{
						MAX_Feq_flag=1;//表示高位已经有数，低位的0不能熄灭
						Seg_display(7,(MAX_Feq/10)%10,0);//显示此位的数字
					}
					//无论如何最低位都要显示
					Seg_display(8,MAX_Feq%10,0);//第五位
					MAX_Feq_flag=0;//显示完一次数据后，标志位清零，便于下一次数据的显示
				}
				else//位于最低频率的发生时间回显界面
				{
					Seg_display(1,13,0);//标志 H
					Seg_display(2,10,0);//标志 A
					Seg_display(3,MAX_hour/10,0);//最大频率发生时间 小时 十位
					Seg_display(4,MAX_hour%10,0);//最大频率发生时间 小时 个位
					Seg_display(5,MAX_minute/10,0);//最大频率发生时间 分钟 十位
					Seg_display(6,MAX_minute%10,0);//最大频率发生时间 分钟 个位
					Seg_display(7,MAX_second/10,0);//最大频率发生时间 秒 十位
					Seg_display(8,MAX_second%10,0);//最大频率发生时间 秒 个位
				}
				break;
			}
			default://非法状态，回到第一个子界面
			{
				seg_mode1=0;
				break;
			}
		}
	}
}