代码实现
单字节写入：
void AT24C02(unsigned char addr,unsigned char write_addr,unsigned char Byte)
{
	IIC_Start();
	IIC_SendByte(addr);
	IIC_WaitAck();

	IIC_SendByte(write_addr)
	IIC_WaitAck();

	IIC_SendByte(Byte);
	IIC_WaitAck();
	IIC_Stop();
	Delay5ms();
}

多字节写入
 void AT2402(unsigned char addr,unsigned char write addr,unsigned char length,unsigned char *Byte)
{
	unsigned char i;

	IIC_Start();
	IIC_SendByte(addr);
	IIC_WaitAck();

	IIC_SendByte(write addr);
	IIC_WaitAck();

	for(i=0;i<length;i++)
	{
		IIC_SendByte(*Byte++);
		IIC_WaitAck();
	}
	IIC_Stop();
	Delay5ms();
}

单字节读取
unsigned char AT24C02_Read(unsigned char addr,unsigned char Read_addr)
{
	unsigned char Byte=0x00;

	IIC_Start();
	IIC_SendByte(addr);
	IIC_WaitAck();

	IIC_SendByte(Read_addr);
	IIC_WaitAck();

	IIC_Start();
	IIC_SendByte(addr|0x01);
	IIC_WaitAck();

	Byte=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return(Byte);
}

多字节读取
unsigned char AT24C02_Read(unsigned char addr,unsigned char read_addr,unsigned char *Byte)
{
	unsigned char byte=0x00;
	unsigned char i;
	
	IIC_Start();
	IIC_SendByte(addr);
	IIC_WaitAck();

	IIC_SendByte(Read_addr);
	IIC_WaitAck();

	IIC_Start();
	IIC_SendByte(read_addr|0x01);
	IIC_WaitAck();

	for(i=0;i<length;i++)
	{
		byte=IIC_RecByte();
		if(i!=length-1)
		{
			IIC_SendAck(0);
		}
		else
		{
			IIC_SendAck(1);
		}
		*Byte=byte;
		*Byte++;
	}
	IIC_Stop();
}