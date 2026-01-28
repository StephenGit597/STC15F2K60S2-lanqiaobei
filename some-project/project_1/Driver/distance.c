#include"distance.h"

float Get_distance(void)
{
	unsigned char i=0;
	float distance=0.0f;
	TL1=0;
	TH1=0;
	P11=1;
	TR1=1;
	for(i=0;i<3;i++)
	{
		P10=1;
		US_Delay(13);
		P10=0;
		US_Delay(13);
	}
	while((TF1==0)&&(P11==1));
	TR1=0;
	if(TF1)
	{
		TF1=0;
		distance=65535*0.0014f;
	}
	else
	{
		distance=((TH1<<8)|TL1)*0.0014f;
		TL1=0;
		TH1=0;
	}
	return(distance);
}