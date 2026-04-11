代码：
计数器0初始化：
void Timer0Init(void)//配置为P34计数器模式
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//保持定时器1的配置不变
	TMOD |= 0x05;		//设置定时器0为16位不可重装载计数器模式
	TF0 = 0;		//清除TF0标志
	TL0=0x00;       //清除计数器不确定的值
	TH0=0x00;       //清除计数器不确定的值
}

频率获取：
//获取频率
void Get_Fequence_ask(void)
{
	TL0=0x00;//清除计数器的值，从零开始
	TH0=0x00;//清除计数器的值，从零开始
	TF0=0;//清除计数器0溢出标志
	TR0=1;//计数器0开始计数
}
	
unsigned int Get_Fequence_receive(void)
{
	unsigned int Feq=0;
	TR0=0;//计数器0停止计数
	Feq=(TH0<<8)|TL0;//在1s内触发，频率就是计数器的值
	TF0=0;//清除计数器0溢出标志
	TL0=0x00;//清除计数器的值，从零开始
	TH0=0x00;//清除计数器的值，从零开始
	return(Feq);
}
主函数调用示例：
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