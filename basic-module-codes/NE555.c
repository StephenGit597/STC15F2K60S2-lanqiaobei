void Get_Fequence_ask(void)
{
	TL0=0x00;//�����������ֵ�����㿪ʼ
	TH0=0x00;//�����������ֵ�����㿪ʼ
	TF0=0;//���������0�����־
	TR0=1;//������0��ʼ����
}
	
unsigned int Get_Fequence_receive(void)
{
	unsigned int Feq=0;
	TR0=0;//������0ֹͣ����
	Feq=(TH0<<8)|TL0;//��1s�ڴ�����Ƶ�ʾ��Ǽ�������ֵ
	TF0=0;//���������0�����־
	TL0=0x00;//�����������ֵ�����㿪ʼ
	TH0=0x00;//�����������ֵ�����㿪ʼ
	return(Feq);
}