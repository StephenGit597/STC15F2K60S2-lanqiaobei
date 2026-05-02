//LCD1602常用命令

//屏幕清理类
//清屏与光标归位 0x01 需要1.64ms
//光标归位 0x02 需要1.64ms

//显示模式类
//开显示 关光标 关闪烁 0x0C 需要40us（常用）
//开显示 开光标 关闪烁 0x0E 需要40us（常用有输入设置的场景）
//开显示 关光标 关闪烁 0x0F 需要40us（常用有输入设置的场景，而且闪烁提醒）
//关显示 关光标 关闪烁 0x08 需要40us（待机）省电不丢失数据

//输入设置类
//写数据，光标自动后移 显示内容不动 0x06 需要40us
//写数据，光标自动前移 显示内容不动 0x04 需要40us
//写数据，光标自动后移 显示内容整体左移 0x07 需要40us（一行要显示很长的数据的情况）

//总线模式类
//8位总线 2行显示 5*8点阵 0x38 常用 需要40us
//4位总线 2行显示 5*8点阵 0x28 常用 需要40us

//地址设置类 0x80+addr
//第一行 0x00
//第二行 0x40

//LCD 函数定义
void LCD1602_WriteCmd(unsigned char cmd)
{
	P05=0;//EN=0,先不使能，等待电平稳定和准备数据
	US_Delay(5);//延时5微秒
	P07=0;//RS=0,选择指令寄存器
	US_Delay(3);//延时3微秒
	P06=0;//RW=0,设置为写模式
	US_Delay(3);//延时3微秒
	
	P2=cmd;//P2给出指令
	US_Delay(5);//延时5微秒
	P05=1;//EN=1,使能
	US_Delay(5);//延时5微秒
	P05=0;//EN=0,下降沿，锁存数据
	US_Delay(5);//延时5微秒
	
	P06=0;//函数执行完成，引脚回归默认状态
	P07=0;//函数执行完成，引脚回归默认状态
}

void LCD1602_Writedata(unsigned char Byte)
{
	P05=0;//EN=0,先不使能，等待电平稳定和准备数据
	US_Delay(5);//延时5微秒
	P07=1;//RS=1,选择数据寄存器
	US_Delay(3);//延时3微秒
	P06=0;//RW=0,设置为写模式
	US_Delay(3);//延时3微秒
	
	P2=Byte;//P2给出数据
	US_Delay(5);//延时5微秒
	P05=1;//EN=1,使能
	US_Delay(5);//延时5微秒
	P05=0;//EN=0,下降沿，锁存数据
	US_Delay(5);//延时5微秒
	
	P06=0;//函数执行完成，引脚回归默认状态
	P07=0;//函数执行完成，引脚回归默认状态
}

void LCD1602_Init(void)//延时比较充足，没有比这最低延时来
{
	MS_Delay(50);//上电延时50ms
	//Must Be First
	LCD1602_WriteCmd(0x38);//8位总线 2行显示 5*8点阵
	MS_Delay(3);//延时3ms
	
	LCD1602_WriteCmd(0x0C);//开显示 关光标 关闪烁
	MS_Delay(3);//延时3ms
	
	LCD1602_WriteCmd(0x06);//写数据，光标自动后移 显示内容不动
	MS_Delay(3);//延时3ms
	//最后才清屏
	LCD1602_WriteCmd(0x01);//清屏与光标归位
	MS_Delay(3);//延时3ms
}

void LCD1602_Single_Display(unsigned char row,unsigned char col,unsigned char ch)//显示单个字符
{
	unsigned char addr=0;//存放最终显存地址
	
	if((row<1)|(row>2)|(col<1)|(col>16))//对于LCD1602显示超出2*16范围直接返回
	{
		return;
	}
	if(row==1)
	{
		addr=0x00+(col-1);//注意地址是0开始的，如果函数设置第一个为1，那么要减1
	}
	if(row==2)
	{
		addr=0x40+(col-1);//注意地址是0开始的，如果函数设置第一个为1，那么要减1
	}
	LCD1602_WriteCmd(0x80+addr);
	MS_Delay(3);//延时3ms
	LCD1602_Writedata(ch);
	MS_Delay(1);//延时1ms
}

void LCD1602_Str_Display(unsigned char row,unsigned char col,unsigned char *str)//显示字符串,注意范围
{
	unsigned char addr=0;//存放最终显存地址
	unsigned char i=0;//显示字符串循环变量
	
	if((row<1)|(row>2)|(col<1)|(col>16))//对于LCD1602显示超出2*16范围直接返回
	{
		return;
	}
	if(row==1)
	{
		addr=0x00+(col-1);//注意地址是0开始的，如果函数设置第一个为1，那么要减1
	}
	if(row==2)
	{
		addr=0x40+(col-1);//注意地址是0开始的，如果函数设置第一个为1，那么要减1
	}
	LCD1602_WriteCmd(0x80+addr);//计算第一个字符的地址
	MS_Delay(3);//延时3ms
	//由于设置为显存自增模式，不必写一个字符设置一个显存地址
	while(str[i]!='\0')
	{
		LCD1602_Writedata(str[i]);//写显存
		MS_Delay(1);//延时1ms
		i++;
	}
}

void LCD1602_num_Display(unsigned char row,unsigned char col,unsigned char num)//显示单个数字
{
	unsigned char addr=0;//存放最终显存地址
	
	if((row<1)|(row>2)|(col<1)|(col>16))//对于LCD1602显示超出2*16范围直接返回
	{
		return;
	}
	if(row==1)
	{
		addr=0x00+(col-1);//注意地址是0开始的，如果函数设置第一个为1，那么要减1
	}
	if(row==2)
	{
		addr=0x40+(col-1);//注意地址是0开始的，如果函数设置第一个为1，那么要减1
	}
	LCD1602_WriteCmd(0x80+addr);
	MS_Delay(3);//延时3ms
	LCD1602_Writedata(0x30+num);//num对应的ASCII码：0-9，0x30-0x39
	MS_Delay(1);//延时1ms
}