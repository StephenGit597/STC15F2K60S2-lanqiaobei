#include"advance.h"
//I2CÍ¨Ñ¶¹ÙÇýº¯Êý
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

void AT24C20_Write(unsigned char addr,unsigned char *dat,unsigned char len)
{
	unsigned char i=0;
	I2CStart();
	I2CSendByte(AT24C02_Address);
	I2CWaitAck();
	
	I2CSendByte(addr);
	I2CWaitAck();
	
	for(i=0;i<len;i++)
	{
		I2CSendByte(dat[i]);
		I2CWaitAck();
	}
	I2CStop();
	MS_Delay(5);
}

void AT24C02_Read(unsigned char addr,unsigned char *dat,unsigned char len)
{
	unsigned char i=0;
	I2CStart();
	I2CSendByte(AT24C02_Address);
	I2CWaitAck();
	
	I2CSendByte(addr);
	I2CWaitAck();
	I2CStop();
	
	I2CStart();
	I2CSendByte(AT24C02_Address|0x01);
	I2CWaitAck();
	
	for(i=0;i<len;i++)
	{
		dat[i]=I2CReceiveByte();
		if(i==len-1)
		{
			I2CSendAck(1);
		}
		else
		{
			I2CSendAck(0);
		}
	}
	I2CStop();
}