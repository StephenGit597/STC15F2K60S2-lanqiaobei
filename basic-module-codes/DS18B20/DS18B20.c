//DS18B20常用命令
//0xCC跳过地址
//0x44启用温度转换
//0xBE读取DS18B20的温度传感器的值

//DS18B20返回值有12位数据 被符号拓展到16位int型
//SSSS Szzz zzzz zzzz
//最后四位表示小数部分，分辨率0.0625
//剩下八位表示整数部分

//如果需要对符号的判断
static unsigned char tempature_flag=0;
//正0，负1

//注意 这个DS18B20在12位模式下温度转换时间较长 如果使用需要单片机不断扫描动态显示的数码管或者点阵屏，建议采用触发方式（按键等）否则数码管显示会跳动

float Get_tempature(void)//这个函数直接返回温度的绝对值
{
	unsigned char low=0;//存放低位
	unsigned char high=0;//存放高位
	unsigned int row=0;//存放温度传感器传回来的原始数据
	float temp=0;//存放返回温度
	
	DS18B20_Init();//初始化
	DS18B20_WriteByte(0xCC);//跳过地址模式
	DS18B20_WriteByte(0x44);//启用温度转化
	MY_Delay(750);//等待转换完成
	DS18B20_Init();//再次读取需要再次初始化
	DS18B20_WriteByte(0xCC);//跳过地址模式
	DS18B20_WriteByte(0xBE);//发出读命令
	
	low=DS18B20_ReadByte();//LSB原则，先接收第八位
	high=DS18B20_ReadByte();//LSB原则，先接收第八位
	row=(high<<8)|low;
	
	if(row&0x0800)//如果是负温度
	{
		row=~row+1;//取反+1求绝对值
        tempature_flag=1;//标志位值1
	}
	temp=(row>>4)+(row&0x0000F)*0.0625;//读出绝对值
	return(temp);
}