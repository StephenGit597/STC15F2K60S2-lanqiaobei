void Display_line(unsigned char row,unsigned char Byte)
{
	if((row==0)|(row>8))//只有八列，对于超出的数据要做溢出判断
	{
		return;
	}
	P2=(P2&0x1F)|0x00;//先让控制列选的D触发器关闭数据传输
	P0=(0x01)<<(row-1);//准备列选信号
	US_delay(1);//等待电平稳定后
	P2=(P2&0x1F)|0xC0;//选择控制列选的D触发器
	US_delay(5);//等待D触发器动作
	P2=(P2&0x1F)|0x00;//控制列选的D触发器锁存
	P0=~Byte;//准备显示数据信号
	US_delay(1);//等待电平稳定后
	P2=(P2&0x1F)|0xE0;/选择控制数据的D触发器
	MS_delay(1);//延时给人视觉停留
	P0=0xFF;//清除这一次的显示数据，消隐
	P2=(P2&0x1F)|0x00;//锁存数据
}
//显示长图函数
void display_picture(len)
{
	unsigned char i=0;//扫屏循环变量
	unsigned char offset=0;//偏移量
	while(1)
	{
		now=currenttim;//等待变量
		while(1)
		{
			for(i=0;i<8;i++)
			{
				Display_line(i+1,Display[(i+offset)%len]);
			}
			if(currenttim-now>100)//每一帧图像静止显示0.1S
			{
				break;
			}
		}
		offset++;//显示下一帧
		if(offset+1>len)//基准点显示图像完成，清0进行下一轮显示
		{
			offset=0;
		}
	}
}
//特别注意，这个函数需要不断扫描屏幕显示图案，而且有两层循环
//因此，做按键判断（跳出）时，应该在内层循环判断（在cpu内层循环时间占比很大），break内存循环后，还应该设置标志位，在外层循环判断是否是因为按键按下导致
//内层循环跳出，如果是要跳出外层循环，才能结束显示函数

//显示静止图片
void static_display(void)
{
	unsigned char i=0;
	for(i=0;i<8;i++)
	{
		Display_line(i+1,picture[i]);
	}
}
//需要在循环中不断扫描，此函数只扫描一次