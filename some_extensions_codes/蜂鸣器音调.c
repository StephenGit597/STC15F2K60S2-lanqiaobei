//用于按键提示音
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

//发出7种不同的音调 音调由低到高 1，2，3，4，5，6，7
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