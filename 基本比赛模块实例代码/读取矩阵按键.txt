单词扫描矩阵按键
unsigned char Scan_Martix_Key(void)
{
	unsigned char temp=0;
	unsigned char value=0;
	P44=0;
	P42=1;
	temp=(P3&0x0C)>>2;
	P44=1;
	P42=0;
	temp=~(0xF0|(temp<<2)|((P3&0x0C)>>2));
	switch(temp)
	{
		case 0x08:
		{
			value=1;
			break;
		}
		case 0x04:
		{
			value=2;
			break;
		}
		case 0x02:
		{
			value=3;
			break;
		}
		case 0x01:
		{
			value=4;
			break;
		}
		default:
		{
			value=0;
			break;
		}
	}
	return(value);
}
循环执行按键操作 与按键界面切换与功能设置
标志位设置
//矩阵按键读取
unsigned long xdata key_now=0;//按键获取时间戳
unsigned char xdata key_1=0;//按键第一次读取到的值
unsigned char xdata key_2=0;//按键第二次读取到的值
unsigned char xdata key_first_ok=0;//按键第一次扫描完成标志位
unsigned char xdata key_flag=0;//按键按下并且已稳定标志位
unsigned char xdata key_temp=0;//按键缓冲区
unsigned char xdata key_value=0;//最终扫描得到的按键值
//界面变量
unsigned char xdata seg_mode1=0;//总的界面显示模式
unsigned char xdata seg_mode2=0;//参数子界面显示模式
unsigned char xdata seg_mode3=0;//回显子界面显示模式
while(1)
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
}
