#include"intrins.h"
#include"STC12C5A60S2.h"

//变量声明
volatile unsigned long currenttim=0;//定时器初始化完成后的时间
static unsigned long now=0;//用于记录某一时刻的时间给TIM
static unsigned long led_now=0;//用于记录某一时刻的时间给LED
static unsigned long led_DIS=0;//用于记录某一时刻的时间给Display

volatile unsigned char hour=0;//小时
volatile unsigned char minute=0;//分钟
volatile unsigned char second=0;//秒
//闹钟设置
static unsigned char prehour=0;//小时
static unsigned char preminute=0;//分钟
static unsigned char presecond=0;//秒
unsigned char clkflag=0;//闹钟开启关闭状态
unsigned int j=0;//闹铃翻转次数

unsigned char pretim[6]={0,0,0,0,0,0};//供设置时间
unsigned char sel=0;//选择数字
unsigned char displayMode=0;//显示模式

//led功能变量
unsigned char mode_led=0;//用于存储当前LED显示的功能
unsigned char temp=0;
unsigned char led1=0xAA;
unsigned char led2=0x01;
unsigned char led3=0x00;
unsigned char LH=0;//持续时间
unsigned char flag=0;//指示延时时间变化方向

//按键值接收变量
unsigned char key=0;

const unsigned char music[47]={6,0,6,0,6,0,6,0,6,0,6,0,6,0,7,0,3,7,0,3,7,7,0,2,0,7,0,6,0,6,0,6,0,6,0,6,0,6,0,1,3,6,5,4,7,0,3};

//函数声明
//时间类函数
void MS_Delay(unsigned int ms);
void US_Delay(unsigned int us);
void Timer0Init(void);


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


//LCD控制函数
void LCD1602_WriteCmd(unsigned char cmd);
void LCD1602_Writedata(unsigned char Byte);
void LCD1602_Init(void);
void LCD1602_Single_Display(unsigned char row,unsigned char col,unsigned char ch);
void LCD1602_Str_Display(unsigned char row,unsigned char col,unsigned char *str);
void LCD1602_num_Display(unsigned char row,unsigned char col,unsigned char num);

//读取按键
unsigned char Key_Read(void);
unsigned char Get_Key(void);

//LED
void LED_Display(unsigned char LED);

//蜂鸣器
void Buffer(void);
void Music_Display(unsigned char i);//简单音乐播放函数
void Carry(void);//Hello World

//主函数
void main(void)
{
	Timer0Init();//必须开始就初始化完成，否则中断函数有问题
	EA=1;//所有有关于中断配置完成后才可以打开
	LCD1602_Init();
	while(1)
	{
		LCD1602_Str_Display(1,1,"&*Hello World!*&");
		LCD1602_Str_Display(2,1,"{[(=<MCU_  >=)]}");
		LCD1602_num_Display(2,10,5);
		LCD1602_num_Display(2,11,1);
		Music_Display(1);
		MS_Delay(10);
		Music_Display(2);
		MS_Delay(10);
		Music_Display(3);
		MS_Delay(10);
		Music_Display(4);
		MS_Delay(10);
		Music_Display(5);
		MS_Delay(10);
		Music_Display(6);
		MS_Delay(10);
		Music_Display(7);
		MS_Delay(10);
		LCD1602_WriteCmd(0x01);
		LCD1602_Str_Display(2,1,"Time:");
		LCD1602_Single_Display(2,8,'-');
		LCD1602_Single_Display(2,11,'-');
		LCD1602_Str_Display(2,16,"-");
		while(1)//显示时间主循环(功能1)
		{
			if(currenttim-now>1000)//更新显示时间
			{
				LCD1602_num_Display(2,6,hour/10);
				LCD1602_num_Display(2,7,hour%10);
				
				LCD1602_num_Display(2,9,minute/10);
				LCD1602_num_Display(2,10,minute%10);
				
				LCD1602_num_Display(2,12,second/10);
				LCD1602_num_Display(2,13,second%10);
				now=currenttim;//记录这次更新时间的时间
			}
			if(currenttim-led_now>10)
			{
				switch(mode_led)
				{
					case 0:
					{
						temp++;
						LED_Display(temp);
						led_now=currenttim;
						break;
					}
					case 1:
					{
						led1=(led1<<1)|((led1&0x80)>>7);
						LED_Display(led1);
						led_now=currenttim;
						break;
					}
					case 2:
					{
						led2=(led2<<1)|((led2&0x80)>>7);
						LED_Display(led2);
						led_now=currenttim;
						break;
					}
					case 3:
					{
						if(P11==1)
						{
							led3=led3>>1;
						    led3|=0x80;
							LED_Display(led3);
						}
						else
						{
							led3=led3>>1;
							LED_Display(led3);
						}
						led_now=currenttim;
						break;
					}
					case 4:
					{
						LED_Display(0xFF);
						MS_Delay(10-LH);
						LED_Display(0x00);
						MS_Delay(LH);
						if(flag==0)
						{
						    LH++;
						}
						else
						{
							LH--;
						}
						if(LH==10)
						{
							flag=1;
						}
						if(LH==0)
						{
							flag=0;
						}
					}
					default:
					{
						led_now=currenttim;
						break;
					}
				}
			}
			if(currenttim-led_DIS>1000)
			{
				switch(displayMode)
				{
					case 0:
					{
						LCD1602_Str_Display(1,1,"Stephen Flanker");
						break;
					}
					case 1:
					{
						LCD1602_Str_Display(1,1,"Wish you best!!!");
						break;
					}
					case 2:
					{
						LCD1602_Str_Display(1,1,"*Happy new year*");
						break;
					}
					case 3:
					{
						LCD1602_Str_Display(1,1,"Welcome to ");
						LCD1602_num_Display(1,12,2);
						LCD1602_num_Display(1,13,0);
						LCD1602_num_Display(1,14,2);
						LCD1602_num_Display(1,15,6);
						break;
					}
					case 4:
					{
						LCD1602_Str_Display(1,1,"Hello,");
						LCD1602_num_Display(1,7,2);
						LCD1602_num_Display(1,8,0);
						LCD1602_Str_Display(1,10,"I am...");
						break;
					}
					case 5:
					{
						LCD1602_Str_Display(1,1,"I am Youth.@STC");
						break;
					}
					case 6:
					{
						LCD1602_Str_Display(1,1,"PCLK:");
						LCD1602_Single_Display(1,8,'-');
						LCD1602_Single_Display(1,11,'-');
						LCD1602_Single_Display(1,14,'%');
						LCD1602_num_Display(1,6,prehour/10);
						LCD1602_num_Display(1,7,prehour%10);
						LCD1602_num_Display(1,9,preminute/10);
						LCD1602_num_Display(1,10,preminute%10);
						LCD1602_num_Display(1,12,presecond/10);
						LCD1602_num_Display(1,13,presecond%10);
						break;
					}
					default:
					{
						break;
					}
				}
			}
			key=Get_Key();
			if(key==3)//LED显示切换
			{
				mode_led++;
				if(mode_led==5)
				{
					mode_led=0;
				}
				key=0;
			}
			if(key==1)//设置时间
			{
				pretim[0]=hour/10;
				pretim[1]=hour%10;
				pretim[2]=minute/10;
				pretim[3]=minute%10;
				pretim[4]=second/10;
				pretim[5]=second%10;
				LED_Display(0x00);
				while(1)
				{
					key=Get_Key();
					if(key==4)
					{
						hour=pretim[0]*10+pretim[1];
						minute=pretim[2]*10+pretim[3];
						second=pretim[4]*10+pretim[5];
						key=0;
						break;
					}
					if(key==1)
					{
						sel++;
						if(sel==6)
						{
							sel=0;
						}
					}
					if(key==2)
					{
						pretim[sel]++;
						if(pretim[sel]==10)
						{
							pretim[sel]=0;
						}
					}
					LCD1602_num_Display(2,6,pretim[0]);
					LCD1602_num_Display(2,7,pretim[1]);
					LCD1602_num_Display(2,9,pretim[2]);
					LCD1602_num_Display(2,10,pretim[3]);
					LCD1602_num_Display(2,12,pretim[4]);
					LCD1602_num_Display(2,13,pretim[5]);
					LED_Display((0x80)>>(sel));
				}
			}
			if(key==2)//显示模式设置
			{
				LCD1602_Str_Display(1,1,"                ");//第一行清屏
				displayMode++;
				if(displayMode==7)
				{
					displayMode=0;
				}
				key=0;
			}
			if(key==4)
			{
				LCD1602_Str_Display(2,1,"PCLK:");
				pretim[0]=prehour/10;
				pretim[1]=prehour%10;
				pretim[2]=preminute/10;
				pretim[3]=preminute%10;
				pretim[4]=presecond/10;
				pretim[5]=presecond%10;
				LED_Display(0x00);
				while(1)
				{
					key=Get_Key();
					if(key==4)
					{
						prehour=pretim[0]*10+pretim[1];
						preminute=pretim[2]*10+pretim[3];
						presecond=pretim[4]*10+pretim[5];
						LCD1602_Str_Display(2,1,"Time:");
						key=0;
						break;
					}
					if(key==1)
					{
						sel++;
						if(sel==6)
						{
							sel=0;
						}
					}
					if(key==2)
					{
						pretim[sel]++;
						if(pretim[sel]==10)
						{
							pretim[sel]=0;
						}
					}
					if(key==3)
					{
						clkflag=!clkflag;
						LCD1602_Str_Display(2,14,"   ");
						if(clkflag)
						{
							LCD1602_Str_Display(2,16,"O");
						}
						else
						{
							LCD1602_Str_Display(2,16,"-");
						}
					}
					LCD1602_num_Display(2,6,pretim[0]);
					LCD1602_num_Display(2,7,pretim[1]);
					LCD1602_num_Display(2,9,pretim[2]);
					LCD1602_num_Display(2,10,pretim[3]);
					LCD1602_num_Display(2,12,pretim[4]);
					LCD1602_num_Display(2,13,pretim[5]);
					LED_Display((0x80)>>(sel));
				}
			}
			if((clkflag)&(hour==prehour)&(minute==preminute)&(second==presecond))
			{
				LCD1602_Str_Display(1,1,"<<<Time is UP>>>");
				Carry();
			}
		}
	}
}

//定时器初始化

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x7F;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
}

void Time0(void) interrupt 1
{
	currenttim++;
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	if(currenttim%1000==0)
	{
		second++;
	}
	if(second>=60)
	{
		second=0;
		minute++;
	}
	if(minute>=60)
	{
		minute=0;
		hour++;
	}
	if(hour>=24)
	{
		hour=0;
	}
}

void MS_Delay(unsigned int ms)//毫秒级延时函数
{
	unsigned long prev=currenttim+ms;
	while(prev>currenttim);
}

void US_Delay(unsigned int us)//微秒初略级延时函数，晶振频率11.0592MHz
{
	unsigned int i=0;//外部循环变量
	while(i<us)
	{
		i++;
	}
}
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

//独立按键读取函数，一次仅仅读取1ms
unsigned char Key_Read(void)
{
	P32=1;//先把引脚置高，低电平读取有效
	P33=1;//先把引脚置高，低电平读取有效
	P34=1;//先把引脚置高，低电平读取有效
	P35=1;//先把引脚置高，低电平读取有效
	US_Delay(1);//延时1us,等待稳定
	if(P35==0)//S1
	{
		return(1);
	}
	if(P34==0)//S2
	{
		return(2);
	}
	if(P33==0)//S3
	{
		return(3);
	}
	if(P32==0)//S4
	{
		return(4);
	}
	return(0);//没有任何按键按下
}

unsigned char Get_Key(void)//具有消抖和阻塞延时（按下阻塞等待放下）的函数
{
	unsigned char V1=0;//存储第一次读取的值
	unsigned char V2=0;//存储第二次读取的值
	unsigned char V3=0;//存储返回值
	V1=Key_Read();//读第一次
	MS_Delay(5);//等待
	V2=Key_Read();//读第二次
	P37=1;
	if(V1==V2)
	{
		if(V1==0)
		{
			return(0);
		}
		V3=V1;//保持读取到的按键值
		while(1)
		{
			V1=Key_Read();//读取按键
			MS_Delay(10);//等待
			V2=Key_Read();//读取按键
			if((V1==V2)&(V1==0))//等待按键放下
			{
				Buffer();
				return(V3);//返回按键值
			}
		}
	}
	return(0);//如果读到的是杂波,视作按键没有按下
}

void LED_Display(unsigned char LED)//操控P1引脚实现
{
	P1=~LED;
}

void Buffer(void)
{
	unsigned char i=0;
	for(i=0;i<10;i++)
	{
		P36=~P36;
		MS_Delay(5);
		P36=0;
	}
}

void Music_Display(unsigned char i)
{
	unsigned char j=0;//发音时间
	switch(i)
	{
		case 1:
		{
			for(j=0;j<67;j++)
			{
				P36=~P36;
				US_Delay(50);
			}
			break;
		}
		case 2:
		{
			for(j=0;j<50;j++)
			{
				P36=~P36;
				US_Delay(42);
			}
			break;
		}
		case 3:
		{
			for(j=0;j<36;j++)
			{
				P36=~P36;
				US_Delay(37);
			}
			break;
		}
		case 4:
		{
			for(j=0;j<45;j++)
			{
				P36=~P36;
				US_Delay(31);
			}
			break;
		}
		case 5:
		{
			for(j=0;j<49;j++)
			{
				P36=~P36;
				US_Delay(24);
			}
			break;
		}
		case 6:
		{
			for(j=0;j<50;j++)
			{
				P36=~P36;
				US_Delay(19);
			}
			break;
		}
		case 7:
		{
			for(j=0;j<55;j++)
			{
				P36=~P36;
				US_Delay(13);
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void Carry(void)
{
	unsigned char i=0;
	for(i=0;i<47;i++)
	{
		Music_Display(music[i]);
		MS_Delay(60);
	}
}
