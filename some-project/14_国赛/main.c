#include"basic_driver.h"
#include"basic_module.h"
//单片机时间戳
unsigned long currenttim=0;
//按键处理
unsigned char xdata key_ena=1;//按键启用标志位，0为测距检测启动
unsigned long xdata key_ena_now=0;//按键失能时间戳
unsigned long xdata key_now=0;//按键时间戳
unsigned char xdata key_1=0;//第一次读取到的按键值
unsigned char xdata key_2=0;//第二次读取到的按键值
unsigned char xdata key_first_ok=0;//按键第一次扫描完成标志位
unsigned char xdata key_flag=0;//按键按下已稳定标志位
unsigned char xdata long_wait_flag=0;//S8,S9长按等待标志位
unsigned long xdata long_wait_flag_now=0;//S8,S9长按时间戳
unsigned char xdata key_temp=0;//按键值存储缓冲区
unsigned char xdata key_value=0;//最终获取到的按键值
//单片机系统工作模式
unsigned char xdata seg_mode1=0;//主界面切换
unsigned char xdata seg_mode2=0;//测距显示单位切换
unsigned char xdata seg_mode3=0;//参数界面显示切换 温度-距离
unsigned char xdata seg_mode4=0;//显示模式控制
//温度测量
float xdata temperature=0.0f;//当前采集到的温度
float xdata Set_temperature=30.0f;//温度参数
unsigned char xdata temperature_ask_ok=0;//温度转换指令已发送标志位
unsigned long xdata temperature_now=0;//温度转换时间戳
//距离测量
unsigned int xdata distance=0;//当前采集到的距离
unsigned char xdata Set_distance=40;//距离参数
signed char xdata distance_correct=0;//距离修正值
unsigned char xdata distance_correct_flag=0;//距离校正值符号标志位
signed int xdata distance_processed=0;//最终处理得到的距离
unsigned int xdata Trans_V=340;//默认介质声音传播速度
unsigned char xdata distance_ask_ok=0;//距离获取指令已发送标志位
//距离测量结果电压输出
float xdata V_min=1.0f;//默认最小电压输出
unsigned char xdata V_ena=0;//电压输出使能标志位
unsigned char xdata V_temp=0;//电压转换缓冲区
//继电器动作
unsigned char xdata relay_on=0;//当前继电器状态指示位
//LED动作
unsigned char xdata Led_temp=0;//LED闪烁震荡源
unsigned long xdata Led_now=0;//LED时间戳
unsigned char xdata Led1=0;//LED1控制变量
unsigned char xdata Led2=0;//LED2控制变量
unsigned char xdata Led3=0;//LED3控制变量
unsigned char xdata Led4=0;//LED4控制变量
unsigned char xdata Led5=0;//LED5控制变量
unsigned char xdata Led6=0;//LED6控制变量
unsigned char xdata Led7=0;//LED7控制变量
unsigned char xdata Led8=0;//LED8控制变量
//数码管显示
unsigned char xdata distance_processed_zero=0;//最终处理cm数据高位灭零标志位
unsigned char xdata distance_processed_zero_m=0;//最终处理m数据高位灭零标志位
unsigned char xdata Trans_V_zero=0;//介质声波传播速度高位灭零标志位
unsigned char xdata distance_correct_zero=0;//距离校正值高位灭零标志位

void main(void)
{
	Timer1Init();//定时器1初始化
	Timer0Init();//定时器0初始化
	EA=1;//使能全局中断
	LED_display(0x00);//LED灯显示初始化
	Relay_control(0);//继电器状态初始化
	while(1)
	{
		//扫描矩阵按键
		if((key_ena==1)&&(key_flag==0)&&(key_first_ok==0))//在按键启用的情况下，按键还没有稳定，开始第一次按键值扫描
		{
			key_1=martix_scan();//获取第一次的按键值
			key_first_ok=1;//按键第一次扫描完成标志位置1
			key_now=currenttim;//记录当前第一次按键扫描时间戳
		}
		if((key_ena==1)&&(key_flag==0)&&(key_first_ok==1)&&(currenttim-key_now>30))//在按键启用的情况下，按键还没有稳定，在第一次按键扫描完成延时30ms后开始第一次按键值扫描
		{
			key_2=martix_scan();//获取第二次的按键值
			key_first_ok=0;//清除按键第一次扫描完成标志位，便于下一次扫描
		}
		if((key_ena==1)&&(key_flag==0)&&(key_1==key_2)&&(key_1!=0))//在按键启用的情况下，处理完上一次按键按下稳定结果后，按键再次按下稳定，处理新值
		{
			key_flag=1;//按键按下并已稳定标志位置1
			key_temp=key_1;//按键值先存入缓冲区
		}
		if((key_ena==1)&&(key_flag==1))//按键按下并已稳定
		{
			if(key_temp==5)//如果是S8,S9组合按键
			{
				if(long_wait_flag==0)//如果是第一次检测到S8,S9组合按键长按
				{
					long_wait_flag=1;//S8,S9组合长按等待检测标志位置1
					long_wait_flag_now=currenttim;//记录当前S8,S9组合按键时间戳
				}
				else//已经开始检测S8,S9组合按键长按
				{
					if((long_wait_flag==1)&&(currenttim-long_wait_flag_now>2000))//已经开始检测S8,S9组合按键长按并且延时达到2000ms
					{
						long_wait_flag=0;//清除S8,S9组合按键长按检测标志位，便于下一次检测
						key_flag=0;//清除按键按下已稳定标志位，方便下次读取
						key_value=5;//按键值5标志S8,S9组合按键长按
					}
					if(martix_scan()!=5)//已经开始检测S8,S9组合按键长按，延时没有到2s，检测到按键变化（非S8,S9组合按键长按）
					{
						long_wait_flag=0;//清除S8,S9组合按键长按检测标志位，便于下一次检测
						key_flag=0;//清除按键按下已稳定标志位，方便下次读取
					}
				}
			}
			else//S4 S5 S8 S9
			{
				if(!martix_scan())//按键已经放下
				{
					key_flag=0;//清除按键按下已稳定标志位，方便下次读取
					key_value=key_temp;//按键缓冲区的值赋给按键最终值
				}
			}
		}
		//按键失能等待延时
		if((key_ena==0)&&(currenttim-key_ena_now>6000))//从按键开始失能起计时，按键失能已经达到6s
		{
			distance_ask_ok=0;//清除距离获取指令发送完成标志位（退出可能打断距离获取）
			key_ena=1;//重新使能按键
		}
		//按键动作
		//默认按键动作
		if((key_ena==1)&&(key_value==1))//在任何界面下，S4为页面切换界面
		{
			if(seg_mode1==0)//当前的界面为测距界面，默认把单位切为cm
			{
				seg_mode2=0;//距离cm显示模式
			}
			if(seg_mode1==1)//当前的界面为参数设置界面，默认把参数切为距离参数
			{
				seg_mode3=0;//距离参数模式
			}
			if(seg_mode1==2)//当前的界面为工厂模式界面，默认切会标准值设定页面
			{
				seg_mode4=0;//标准值设定页面
			}
			seg_mode1++;//显示模式自增
			if(seg_mode1==3)//边界校验 0 - 1 - 2 测距-参数-工厂
			{
				seg_mode1=0;//回到测距页面
			}
			key_value=0;//清除按键值，防止重复触发
		}
		if((key_ena==1)&&(key_value==1))//在任何界面下，S8,S9组合按键为恢复出厂模式按键
		{
			//恢复出厂模式
			Set_temperature=30.0f;//温度参数恢复出厂模式
			Set_distance=40;//距离参数恢复出厂模式
			distance_correct=0;//距离修正值恢复出厂模式
			distance_correct_flag=0;//清除距离校正值符号标志位
			Trans_V=340;//默认介质声音传播速度恢复出厂模式
			V_min=1.0f;//默认最小电压输出恢复出厂模式
			key_value=0;//清除按键值，防止重复触发
		}
		//测距界面按键动作 
		if(seg_mode1==0)//当前位于测距界面
		{
			if((key_ena==1)&&(key_value==2))//S5按下乒乓切换距离显示单位
			{
				if(seg_mode2==0)//seg_mode2 cm-m来回乒乓切换
				{
					seg_mode2=1;
				}
				else//非法情况，回到cm显示模式
				{
					seg_mode2=0;
				}
				key_value=0;//清除按键值，防止重复触发
			}
			if((key_ena==1)&&(key_value==3))
			{
				key_ena=0;//按键失能，开始距离数据记录
				key_ena_now=currenttim;//记录当前按键失能开始的时间戳
				//按键失能，按键初始化
				key_first_ok=0;//清除按键第一次扫描完成标志位
				key_flag=0;//清除按键按下已稳定标志位
				long_wait_flag=0;//清除S8,S9长按等待标志位
				key_temp=0;//清除按键值存储缓冲区
				key_value=0;//清除按键值，防止重复触发
			}
			if((key_ena==1)&&(key_value==4))
			{
				V_ena=1;//使能电压转换输出
				key_value=0;//清除按键值，防止重复触发
			}
		}
		//参数界面按键动作 
		if(seg_mode1==1)//当前位于参数设置总界面
		{
			if((key_ena==1)&&(key_value==2))//S5按下乒乓切换参数设置模式
			{
				if(seg_mode3==0)//参数设置界面来回切换 距离温度参数
				{
					seg_mode3=1;
				}
				else//非法情况，回到距离参数设置模式
				{
					seg_mode3=0;
				}
				key_value=0;//清除按键值，防止重复触发
			}
			if(seg_mode3==0)//当前在距离参数设置界面
			{
				if((key_ena==1)&&(key_value==3))//在距离参数界面下，S8增加距离参数值
				{
					Set_distance=Set_distance+10;//距离参数增加10cm
					if(Set_distance==100)//距离参数上界校验
					{
						Set_distance=90;//如果达到100，报持上界90cm
					}
					key_value=0;//清除按键值，防止重复触发
				}
				if((key_ena==1)&&(key_value==4))//在距离参数界面下，S9减少距离参数值
				{
					Set_distance=Set_distance-10;//距离参数减少10cm
					if(Set_distance==0)//距离参数下界校验
					{
						Set_distance=10;//如果达到0，报持下界10cm
					}
					key_value=0;//清除按键值，防止重复触发
				}
			}
			if(seg_mode3==1)//当前在温度参数显示界面
			{
				if((key_ena==1)&&(key_value==3))//在温度参数界面下，S8增加温度参数值
				{
					Set_temperature=Set_temperature+1.0f;//温度参数加1.0C
					if(Set_temperature==81.0f)//温度参数上界校验
					{
						Set_temperature=80.0f;//如果达到81.0f，报持上界80.0f
					}
					key_value=0;//清除按键值，防止重复触发
				}
				if((key_ena==1)&&(key_value==4))//在温度参数界面下，S9减少温度参数值
				{
					Set_temperature=Set_temperature-1.0f;//温度参数减1.0C
					if(Set_temperature==-1.0f)//温度参数下界校验
					{
						Set_temperature=0.0f;//如果达到-1.0f，报持下界0.0f
					}
					key_value=0;//清除按键值，防止重复触发
				}
			}
		}
		//工厂模式界面按键动作 
		if(seg_mode1==2)//当前位于工厂模式总界面
		{
			if((key_ena==1)&&(key_value==2))//S5 切换工厂模式子界面
			{
				seg_mode4++;//工厂模式子界面自增
				if(seg_mode4==3)//如果到达边界 0 - 1 - 2 标准值设置，介质设置，DAC输出设置
				{
					seg_mode4=0;
				}
				key_value=0;//清除按键值，防止重复触发
			}
			if(seg_mode4==0)//位于标准值设置子界面
			{
				if((key_ena==1)&&(key_value==3))//位于标准值设置子界面，S8增加校正值
				{
					distance_correct=distance_correct+5;//校正值加5cm
					if(distance_correct==95)//校正值上界检验
					{
						distance_correct=90;//校正值达到95cm，保持上界90cm
					}
					//判断校正值符号
					if(distance_correct<0)//校正值为负
					{
						distance_correct_flag=1;//校正值为负标志位置1
					}
					else//非负
					{
						distance_correct_flag=0;//清除校正置位负标志位
					}
					key_value=0;//清除按键值，防止重复触发
				}
				if((key_ena==1)&&(key_value==4))//位于标准值设置子界面，S9减少校正值
				{
					distance_correct=distance_correct-5;//校正值减5cm
					if(distance_correct==-95)//校正值下界检验
					{
						distance_correct=-90;//校正值达到-95cm，保持下界-90cm
					}
					//判断校正值符号
					if(distance_correct<0)//校正值为负
					{
						distance_correct_flag=1;//校正值为负标志位置1
					}
					else//非负
					{
						distance_correct_flag=0;//清除校正置位负标志位
					}
					key_value=0;//清除按键值，防止重复触发
				}
			}
			if(seg_mode4==1)//位于介质速度设置子界面
			{
				if((key_ena==1)&&(key_value==3))//位于介质速度设置子界面,S8增加介质速度
				{
					Trans_V=Trans_V+10;//介质速度加10m/s
					if(Trans_V==10000)//介质速度上界检验
					{
						Trans_V=9990;//如果介质速度到达10000m/s，保持上界9990m/s
					}
					key_value=0;//清除按键值，防止重复触发
				}
				if((key_ena==1)&&(key_value==4))//位于介质速度设置子界面,，S9减少介质速度
				{
					Trans_V=Trans_V-10;//介质速度减10m/s
					if(Trans_V==0)//介质速度下界检验
					{
						Trans_V=10;//如果介质速度到达0m/s，保持下界10m/s
					}
					key_value=0;//清除按键值，防止重复触发
				}
			}
			if(seg_mode4==2)//位于DAC输出设置界面
			{
				if((key_ena==1)&&(key_value==3))//位于DAC输出设置界面,S8增加电压输出下界
				{
					V_min=V_min+0.1f;//电压输出下界增加0.1v
					if(V_min==2.1f)//电压输出上界检验
					{
						V_min=2.0f;//电压输出到2.1f,保持上界2.0f
					}
					key_value=0;//清除按键值，防止重复触发
				}
				if((key_ena==1)&&(key_value==4))//位于DAC输出设置界面,S9减少电压输出下界
				{
					V_min=V_min-0.1f;//电压输出下界减少0.1v
					if(V_min==-1.0f)//电压输出下界检验
					{
						V_min=0.0f;//电压输出到-1.0f,保持下界0.0f
					}
					key_value=0;//清除按键值，防止重复触发
				}
			}
		}
		//距离测量
		if((key_ena==0)&&(distance_ask_ok==0))//当触发距离采集，并且距离获取请求未发送的情况下
		{
			Get_distance_ask();//发送距离获取请求
			distance_ask_ok=1;//距离获取请求发送完毕
		}
		if((key_ena==0)&&(distance_ask_ok==1)&&((P11==0)|(TF0==1)))//当触发距离采集，并且距离获取请求已发送的情况下（检测是否收到回波获取溢出信号）
		{
			distance=Get_distance_receive(Trans_V);//获取距离原始数据
			distance_processed=distance+distance_correct;//最终距离数据=原始数据+修正值
			if(distance_processed<0)//距离合法性判断（距离非负）
			{
				distance_processed=0;//如果遇到负数，最终距离数据清零
			}
			distance_ask_ok=0;//清除距离获取请求发送完毕标志位，便于下一次读取
		}
		//电压转换输出
		if(V_ena==1)//电压更新输出使能
		{
			if(distance_processed<10)//小于10cm
			{
				V_temp=(unsigned char)((255.0f*V_min)/5.0f);//电压转换=255*（V_min/V_Full）
			}
			else if(distance_processed<90)//10cm-90cm
			{
				V_temp=(unsigned char)((255.0f*V_min)/5.0f+((distance_processed-10)*(255.0f*(5.0f-V_min)/5.0f))/80.0f);//电压=V_min+(distance_processed-10)*(255*(5.0-V_min))/80
			}
			else//大于90cm
			{
				V_temp=255;//电压输出5V
			}
			V_Trans(V_temp);//电压转换DAC
			V_ena=0;//清除电压转换标志位，等待下一次转换
		}
		//温度测量
		if(temperature_ask_ok==0)//当温度转换指令没有发送
		{
			Get_Temperature_ask();//发送温度转换指令
			temperature_ask_ok=1;//温度转换指令发送完成标志位置1
			temperature_now=currenttim;//记录温度转换时间戳
		}
		if((temperature_ask_ok==1)&&(currenttim-temperature_now>750))//温度转换指令发送完成，等待750ms获取温度
		{
			temperature=Get_temperature_receive();//获取当前温度
			temperature_ask_ok=0;//清除温度转换指令发送完成标志位，便于下一次读取温度
		}
		if(((unsigned int)(distance_processed)>=Set_distance-5)&&((unsigned int)(distance_processed)<=Set_distance+5)&&(temperature<=Set_temperature))//距离参数-5cm<=当前采集处理的距离<=距离参数+5cm,同时当前采集到的温度<=温度参数
		{
			relay_on=1;//继电器吸合
		}
		else
		{
			relay_on=0;//继电器断开
		}
		//继电器动作
		Relay_control(relay_on);
		//LED灯动作
		//LED震荡源动作
		if(currenttim-Led_now>100)//0.1秒切换一次状态，实现LED震荡源变换
		{
			if(Led_temp==0)//Led_temp,0-1乒乓切换
			{
				Led_temp=1;
			}
			else//Led_temp不是0一律回归0，防止非法状态
			{
				Led_temp=0;
			}
			Led_now=currenttim;//更新LED时间戳
		}
		if(seg_mode1==0)//当前如果在测距页面
		{
			Led1=((unsigned char)(distance_processed))&0x01;//LED1显示 距离第一位
			Led2=((unsigned char)(distance_processed))&0x02;//LED2显示 距离第二位
			Led3=((unsigned char)(distance_processed))&0x04;//LED3显示 距离第三位
			Led4=((unsigned char)(distance_processed))&0x08;//LED4显示 距离第四位
			Led5=((unsigned char)(distance_processed))&0x10;//LED5显示 距离第五位
			Led6=((unsigned char)(distance_processed))&0x20;//LED6显示 距离第六位
			Led7=((unsigned char)(distance_processed))&0x40;//LED7显示 距离第七位
			Led8=((unsigned char)(distance_processed))&0x80;//LED8显示 距离第八位
		}
		else if(seg_mode1==1)//当前如果在参数设置界面
		{
			Led1=0;//LED1熄灭
			Led2=0;//LED2熄灭
			Led3=0;//LED3熄灭
			Led4=0;//LED4熄灭
			Led5=0;//LED5熄灭
			Led6=0;//LED6熄灭
			Led7=0;//LED7熄灭
			Led8=1;//LED8点亮
		}
		else
		{
			Led1=Led_temp;//LED1以0.1s间隔闪烁
			Led2=0;//LED2熄灭
			Led3=0;//LED3熄灭
			Led4=0;//LED4熄灭
			Led5=0;//LED5熄灭
			Led6=0;//LED6熄灭
			Led7=0;//LED7熄灭
			Led8=0;//LED8熄灭
		}
		LED_display((Led1&0x01)|(Led2&(0x01<<1))|(Led3&(0x01<<2))|(Led4&(0x01<<3))|(Led5&(0x01<<4))|(Led6&(0x01<<5))|(Led7&(0x01<<6))|(Led8&(0x01<<7)));//LED显示
		//数码管显示
		switch(seg_mode1)//判断系统工作总界面
		{
			case 0://测距工作模式
			{
				Seg_Display(1,((unsigned char)(temperature))/10,0);//获取实时温度十位
				Seg_Display(2,((unsigned char)(temperature))%10,1);//获取实时温度个位+小数点
				Seg_Display(3,((unsigned char)(temperature*10))%10,0);//获取实时温度小数点第一位
				Seg_Display(4,10,0);//-
				if(seg_mode2==0)//选择距离显示单位 0-cm,1-m
				{
					if((distance_processed_zero==0)&&((distance_processed/1000)%10==0))//如果千位数字为0，并且高位没有数
					{
						Seg_Display(5,13,0);//熄灭
					}
					else
					{
						distance_processed_zero=1;//表示高位有数，以后数位上的0不能省略
						Seg_Display(5,(distance_processed/1000)%10,0);//显示千位
					}
					if((distance_processed_zero==0)&&((distance_processed/100)%10==0))//如果百位数字为0，并且高位没有数
					{
						Seg_Display(6,13,0);//熄灭
					}
					else
					{
						distance_processed_zero=1;//表示高位有数，以后数位上的0不能省略
						Seg_Display(6,(distance_processed/100)%10,0);//显示百位
					}
					if((distance_processed_zero==0)&&((distance_processed/10)%10==0))//如果十位数字为0，并且高位没有数
					{
						Seg_Display(7,13,0);//熄灭
					}
					else
					{
						distance_processed_zero=1;//表示高位有数，以后数位上的0不能省略
						Seg_Display(7,(distance_processed/10)%10,0);//显示十位
					}
					//无论如何最低位都要显示
					Seg_Display(8,distance_processed%10,0);//显示个位
					distance_processed_zero=0;//清除最终处理cm数据高位灭零标志位,便于下一次显示
				}
				else//m
				{//采用移动小数点法，转换单位直接使用距离cm数据
					if((distance_processed_zero_m==0)&&(distance_processed/1000)==0)//如果千位数字为0，并且高位没有数
					{
						Seg_Display(5,13,0);//熄灭
					}
					else
					{
						distance_processed_zero_m=1;//表示高位有数，以后数位上的0不能省略
						Seg_Display(5,(distance_processed/1000)%10,0);//显示十位
					}
					//后面的数位无论如何都要显示
					Seg_Display(6,(distance_processed/100)%10,1);//显示个位+小数点
					Seg_Display(7,(distance_processed/10)%10,0);//显示小数点后第一位
					Seg_Display(8,distance_processed%10,0);//显示小数点后第二位
					distance_processed_zero_m=0;//清除最终处理m数据高位灭零标志位,便于下一次显示
				}
				break;
			}
			case 1://参数设置模式
			{
				if(seg_mode3==0)//距离参数设置模式
				{
					Seg_Display(1,11,0);//P
					Seg_Display(2,1,0);//1
					Seg_Display(3,13,0);//熄灭
					Seg_Display(4,13,0);//熄灭
					Seg_Display(5,13,0);//熄灭
					Seg_Display(6,13,0);//熄灭
					Seg_Display(7,Set_distance/10,0);//距离参数十位
					Seg_Display(8,Set_distance%10,0);//距离参数个位
				}
				else//温度参数设置模式
				{
					Seg_Display(1,11,0);//P
					Seg_Display(2,2,0);//2
					Seg_Display(3,13,0);//熄灭
					Seg_Display(4,13,0);//熄灭
					Seg_Display(5,13,0);//熄灭
					Seg_Display(6,13,0);//熄灭
					Seg_Display(7,((unsigned char)(Set_temperature))/10,0);//温度参数十位
					Seg_Display(8,((unsigned char)(Set_temperature))%10,0);//温度参数个位
				}
				break;
			}
			case 2://工厂模式
			{
				if(seg_mode4==0)//校正置设置模式
				{
					Seg_Display(1,12,0);//F
					Seg_Display(2,1,0);//1
					Seg_Display(3,13,0);//熄灭
					Seg_Display(4,13,0);//熄灭
					Seg_Display(5,13,0);//熄灭
					if(distance_correct_flag==0)//距离校正值非负
					{
						if((distance_correct_zero==0)&&((distance_correct/100)%10==0))//距离校正值百位数为0，而且高位没有数
						{
							Seg_Display(6,13,0);//熄灭
						}
						else
						{
							distance_correct_zero=1;//表示此位有数，后面数位上的0不能省略
							Seg_Display(6,(distance_correct/100)%10,0);//显示百位数据
						}
						if((distance_correct_zero==0)&&((distance_correct/10)%10==0))//距离校正值十位数为0，而且高位没有数
						{
							Seg_Display(7,13,0);//熄灭
						}
						else
						{
							distance_correct_zero=1;//表示此位有数，后面数位上的0不能省略
							Seg_Display(7,(distance_correct/10)%10,0);//显示十位数据
						}
						Seg_Display(8,distance_correct%10,0);//显示个位数据
						distance_correct_zero=0;//清除距离校正值高位灭零标志位
					}
					else//距离校正值位负（显示绝对值）
					{
						Seg_Display(8,(-distance_correct)%10,0);//显示个位数据
						if(((-distance_correct)/10)%10==0)//如果距离校正值绝对值十位数据为0
						{
							Seg_Display(6,13,0);//百位熄灭
							Seg_Display(7,10,0);//-
						}
						else//原本百位显示符号，十位显示校正值绝对值十位数据
						{
							Seg_Display(6,10,0);//-
							Seg_Display(7,((-distance_correct)/10)%10,0);//显示距离校正值绝对值十位数据
						}
					}
				}
				else if(seg_mode4==1)//介质设置界面
				{
					Seg_Display(1,12,0);//F
					Seg_Display(2,2,0);//2
					Seg_Display(3,13,0);//熄灭
					Seg_Display(4,13,0);//熄灭
					if((Trans_V_zero==0)&&((Trans_V/1000)%10==0))//介质速度千位为0，而且高位也没有任何数
					{
						Seg_Display(5,13,0);//熄灭
					}
					else
					{
						Trans_V_zero=1;//表示此位有数，此位后面的数位上的0不能省略
						Seg_Display(5,(Trans_V/1000)%10,0);//显示千位上的数
					}
					if((Trans_V_zero==0)&&((Trans_V/100)%10==0))//介质速度百位为0，而且高位也没有任何数
					{
						Seg_Display(6,13,0);//熄灭
					}
					else
					{
						Trans_V_zero=1;//表示此位有数，此位后面的数位上的0不能省略
						Seg_Display(6,(Trans_V/100)%10,0);//显示百位上的数
					}
					if((Trans_V_zero==0)&&((Trans_V/10)%10==0))//介质速度十位为0，而且高位也没有任何数
					{
						Seg_Display(7,13,0);//熄灭
					}
					else
					{
						Trans_V_zero=1;//表示此位有数，此位后面的数位上的0不能省略
						Seg_Display(7,(Trans_V/10)%10,0);//显示十位上的数
					}
					//无论如何，个位的数都要显示
					Seg_Display(8,Trans_V%10,0);//显示个位
					Trans_V_zero=0;//清除介质声波传播速度高位灭零标志位，便于下一个数据显示
				}
				else//DAC输出设置界面
				{
					Seg_Display(1,12,0);//F
					Seg_Display(2,3,0);//3
					Seg_Display(3,13,0);//熄灭
					Seg_Display(4,13,0);//熄灭
					Seg_Display(5,13,0);//熄灭
					Seg_Display(6,13,0);//熄灭
					Seg_Display(7,((unsigned char)(V_min))%10,1);//显示V_min个位+小数点
					Seg_Display(8,((unsigned char)(V_min*10))%10,0);//显示V_min小数点后第一位
				}
				break;
			}
			default:
			{
				seg_mode1=0;//出现非法状况，工作总模式回到测距显示模式
				break;
			}
		}
	}
}