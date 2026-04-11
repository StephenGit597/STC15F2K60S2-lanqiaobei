基本驱动函数
void Seg_display(unsigned char sel,unsigned char Byte,unsigned char dot)
{
	if((sel==0)|(sel>8)|(Byte>16)|(dot>1))
	{
		return;
	}
	P2=(P2&0x1F)|0x00;
	P0=0x01<<(sel-1);
	US_Delay(1);
	P2=(P2&0x1F)|0xC0;
	US_Delay(3);
	P2=(P2&0x1F)|0x00;
	P0=segcode[Byte];
	if(dot==1)
	{
		P07=0;
	}
	US_Delay(1);
	P2=(P2&0x1F)|0xE0;
	MS_Delay(1);
	P0=0xFF;
	P2=(P2&0x1F)|0x00;
}
主循环调用模板(不同界面显示)
while(1)
{
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