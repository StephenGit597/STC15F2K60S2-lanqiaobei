//基本命令
//其实发送地址阶段
//0x90写
//0x91读
//独立模式下
//0x41 读光敏电阻
//0x43 读电位器

//读光敏电阻
unsigned char PCF8591_ADC(void)
{
	unsigned char temp=0;
	I2C_Start();
	I2C_SendByte(0x90);
	I2C_WaitAck();
	
	I2C_SendByte(0x41);
	I2C_WaitAck();
	I2C_Stop();
	
	I2C_Start();
	I2C_SendByte(0x91);
	I2C_WaitAck();
	
	temp=I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();
	return(temp);
}
//读电位器
unsigned char PCF8591_ADC(void)
{
	unsigned char temp=0;
	I2C_Start();
	I2C_SendByte(0x90);
	I2C_WaitAck();
	
	I2C_SendByte(0x43);
	I2C_WaitAck();
	I2C_Stop();
	
	I2C_Start();
	I2C_SendByte(0x91);
	I2C_WaitAck();
	
	temp=I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();
	return(temp);
}
//综合函数，可自行拓展mode的功能采集更多通道
unsigned char PCF8591_ADC(unsigned char mode)//mode0光敏电阻，mode1电位器
{
	unsigned char temp=0;
    unsigned char cmd=0;
    switch(mode)
    {
        case 0:
        {
            cmd=0x41;
            break;
        }
        case 1:
        {
            cmd=0x43;
            break;
        }
        default:
        {
            return(0);
        }
    }
	I2C_Start();
	I2C_SendByte(0x90);
	I2C_WaitAck();
	
	I2C_SendByte(cmd);
	I2C_WaitAck();
	I2C_Stop();
	
	I2C_Start();
	I2C_SendByte(0x91);
	I2C_WaitAck();
	
	temp=I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();
	return(temp);
}
//数字模拟转换
void PCF8591_DAC(unsigned char V)//结果需要万用表测量
{
	I2C_Start();
	I2C_SendByte(0x90);
	I2C_Waitack();
	
	I2C_SendByte(0x41);
	I2C_Waitack();
	
	I2C_SendByte(V);
	I2C_Waitack();
	I2C_Stop();
}