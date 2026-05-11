# 蓝桥杯串口学习
#  STC15F2K60S2关键寄存器配置
#   AUXR寄存器
bit0 S1ST2 0：定时器1作为串口1波特率发生器 定时器2作为串口1波特率发生器
bit1 EXTRAM 0：允许使用拓展RAM 1：禁止使用拓展RAM
bit2 T2x12 0：定时器2为8051主频12分频 1：定时器2为8051主频
bit3 T2 C/T 定时器2用做定时器 定时器2用作计数器 P3.1 引脚
bit4 T2R 0：不允许定时器2运行 1：允许定时器2运行
bit5 UART 0：串口1 12分频 1：串口1 2分频
bit6 T1x12 0：定时器1 12分频 1：定时器1不分频
bit7 T0x12 0：定时器0 12分频 1：定时器0不分频

#   SCON 串口控制寄存器
TL 串行接口数据发送完成中断标志位 手动软件清除
Rl 串行接口数据接收完成中断标志位 手动软件清除
RB8 发送数据的第八位
TB8 接收数据的第八位
REN=1 允许接收 REN=0禁止接收
SM2 模式2和模式3才可以使用
SM0 SM1 模式
00 同步移位寄存器 （波特率fos/12）
01 8位的UART（波特率可变）
10 9位的UART (波特率可变 fos/32或fos/64)
11 9位的UART (波特率可变)

#  1.串口的初始化步骤：
串口的初始化配置：
STC_IPS软件初始化页面配置：

![串口标准配置][fig1]

[fig1]: https://i.postimg.cc/5XTNg1BQ/ping-mu-jie-tu-2026-05-11-172929.png

生成的代码：
void Uart1_Init(void)	//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xC7;			//设置定时初始值
	T2H = 0xFE;			//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
	ES = 1;             //使能串口
	EA = 1;             //使能全局中断
}

#  2.基础代码构建：
-------------------------------------------------------------------
//单片机向上位机发送一个字节
void SendByte(unsigned char dat)
{
	SBUF = dat;
	while(!TI);//死循环，等待字节发送
	TI = 0;//字节发送完成后TI会置1，计时清0
}

//单片机向上位机发送一个数组(字符串)：循环写法
//写法一：如果你刚开始学习C语言，写法一比较好理解
void SendString(unsigned char *dat, unsigned char i)
{
	unsigned char j;
    for (j = 0; j < i; j++) // 遍历数组中的所有元素
    {
        SendByte(dat[j]); // 发送当前字节
    }
}

//单片机向上位机发送一个数组(字符串)：地址写法
void SendString(unsigned char *dat)
{
	while(*dat != '\0')//当发送数组未到达终止符时
		SendByte(*dat++);//发送该字节，发送完成后数组往后移位
}

#  3.中断服务函数写法：
-------------------------------------------------------------------

//全局变量定义
idata unsigned char uart_rec[10];//数据缓存数组
idata unsigned char uart_rec_index; //数据缓存数组索引

void Uart1Server() interrupt 4
{
	if(RI == 1)//串口数据接收完成
	{
		uart_rec[uart_rec_index] = SBUF;//读取数据缓存区的数据
		uart_rec_index++;//移位
		RI = 0;//重新接收
	}
    if(TI == 1)//串口数据单字节发送完成
    {
        //业务逻辑

    }
}

#  4.串口解析与比赛实际使用：
-------------------------------------------------------------------
#   题目描述：
![1][fig2]

[fig2]: https://i.postimg.cc/K1hy8NyD/ping-mu-jie-tu-2026-05-11-174859.png
![2][fig2]

[fig2]: https://i.postimg.cc/hXRqtrqb/ping-mu-jie-tu-2026-05-11-174908.png
