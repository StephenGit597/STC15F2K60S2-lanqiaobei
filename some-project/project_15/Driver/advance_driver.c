#include"advance_driver.h"

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

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

void DS1302_Set_Time(unsigned char hour,unsigned char minute,unsigned char second)
{
	unsigned char temp=0;
	Write_Ds1302_Byte(0x8E,0x00);
	temp=((hour/10)<<4)|((hour%10)&0x0F);
	Write_Ds1302_Byte(0x84,temp);
	temp=((minute/10)<<4)|((minute%10)&0x0F);
	Write_Ds1302_Byte(0x82,temp);
	temp=((second/10)<<4)|((second%10)&0x0F);
	Write_Ds1302_Byte(0x80,temp);
	Write_Ds1302_Byte(0x8E,0x80);
}

void DS1302_Get_Time(void)
{
	unsigned char temp=0;
	temp=Read_Ds1302_Byte(0x85);
	hour=10*(temp>>4)+(temp&0x0F);
	temp=Read_Ds1302_Byte(0x83);
	minute=10*(temp>>4)+(temp&0x0F);
	temp=Read_Ds1302_Byte(0x81);
	second=10*(temp>>4)+(temp&0x0F);
}

void PCF8591_Trans_V(unsigned char V)
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