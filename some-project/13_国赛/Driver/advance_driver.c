#include"advance_driver.h"
//I2C 驱动函数
static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();		
    }
    while(n--);      	
}

void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}
//AT24C02驱动
//单字节写入
void AT24C02_Write(unsigned char addr,unsigned char Byte)
{
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	
	I2CSendByte(addr);
	I2CWaitAck();
	
	I2CSendByte(Byte);
	I2CWaitAck();
	I2CStop();
	MS_Delay(5);
}
//单字节读取
unsigned char AT24C02_Read(unsigned char addr)
{
	unsigned char Byte=0;
	I2CStart();
	I2CSendByte(0xA0);
	I2CWaitAck();
	
	I2CSendByte(addr);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xA0|0x01);
	I2CWaitAck();
	
	I2CSendByte(addr);
	I2CWaitAck();
	
	Byte=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return(Byte);
}
//PCF8591 电压转换
//获取电压
void Get_V_Ask(void)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	
	I2CSendByte(0x43);
	I2CWaitAck();
	I2CStop();
}

unsigned Get_V_Receive(void)
{
	unsigned char result=0;
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	
	result=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return(result);
}
//DA转换
void V_Transfer(unsigned char V)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	
	I2CSendByte(0x41);
	I2CWaitAck();
	
	I2CSendByte(V);
	I2CWaitAck();
	I2CStop();
}