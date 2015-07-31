#include "i2c-ctrl.h"

void iDelay(unsigned long n)
{
  volatile unsigned long i;
  for(i = n;i > 0;i--)
    asm("NOP");
}

void Start_I2c(void)
{
	P0DIR |= 0xC0;   /* P0_6, P0_7��� */
	//SCL=0;
	SDA=1;   /*������ʼ�����������ź�*/
	iDelay(50);
	SCL=1;
	iDelay(200);    /*��ʼ��������ʱ�����4.7us,��ʱ*/ 
	SDA=0;   /*������ʼ�ź�*/
	iDelay(200);    /* ��ʼ��������ʱ�����4��s*/    
	SCL=0;   /*ǯסI2C���ߣ�׼�����ͻ�������� */
	iDelay(50);
}

void Stop_I2c(void)
{
	P0DIR |= 0x40;   /* P0_6��� */
	//SCL=0;
	//iDelay(200);
	SDA=0;  /*���ͽ��������������ź�*/
	iDelay(200);   /*���ͽ���������ʱ���ź�*/
	SCL=1;  /*������������ʱ�����4��s*/
	iDelay(200);
	SDA=1;  /*����I2C���߽����ź�*/
	iDelay(50);
}


/* Ӧ���Ӻ��� */
void Ack_I2c(uint8 a)
{
	P0DIR |= 0x40;   /* P0_6��� */
	if(a==0)SDA=0;     /*�ڴ˷���Ӧ����Ӧ���ź� */
	else SDA=1;
	iDelay(50);      
	SCL=1;
	iDelay(200);               /*ʱ�ӵ͵�ƽ���ڴ���4��s*/

	SCL=0;                /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
	iDelay(50);    
}


/* �ֽ����ݴ��ͺ��� */
void  SendByte(uint8 c)
{
	uint8 i;

	P0DIR |= 0x40;   /* P0_6��� */
	for (i=0x80;i>0;i/=2)             //shift bit for masking
	{  
		if (i & c) SDA=1;
		else SDA=0; 

		iDelay(50);
		SCL=1;               /*��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ*/
		iDelay(200);               /*��֤ʱ�Ӹߵ�ƽ���ڴ���4��s*/     
		SCL=0; 
	}  

	iDelay(50);
	SDA=1;               /*8λ��������ͷ������ߣ�׼������Ӧ��λ*/
	iDelay(50);   
	SCL=1;
	iDelay(50);
	P0DIR &= (~0x40);/* P0_6���� */
	if(SDA == 1)ack=0;     
	else ack=1;  

	SCL=0;
	iDelay(50);
}


/* �ֽ����ݽ��պ���  */
uint8  RecvByte(void)
{
	uint8 retc;
	uint8 i;

	P0DIR |= 0x40;   /* P0_6��� */
	retc=0; 
	SDA=1; 
	//LY_GPIO_Ctrl(LY_DEV_LED2,LY_LED_ON);
	P0DIR &= (~0x40);/* P0_6���� *//*��������Ϊ���뷽ʽ*/
	iDelay(200);
	for (i=0x80;i>0;i/=2)             
	{           
		SCL=1;       /*��ʱ����Ϊ��ʹ��������������Ч*/
		iDelay(100);
		//retc=retc<<1;
		if (SDA==1)   
			retc=(retc | i);
		SCL=0;       /*��ʱ����Ϊ�ͣ�׼����������λ*/
		iDelay(200);         /*ʱ�ӵ͵�ƽ���ڴ���4.7��s*/  
	}
	SCL=0;    
	iDelay(50);
	return(retc);
}


/* �����ӵ�ַ�������ֽ����ݺ���  */
uint8 IRecvByte(uint16 sla)
{
   uint8 c;
   
   uint8 slaveHighAddr = (sla>>8);
   uint8 slaveLowAddr = (sla&0xff);
   
   Start_I2c();                /*��������*/
   SendByte(IIC_WRITE_SLAVE_ADDR);  /*����������ַ*/
     if(ack==0)return(0);
   SendByte(slaveHighAddr);  /*��������16λ�ӵ�ַ��λ�ֽ�*/
     if(ack==0)return(0);
   SendByte(slaveLowAddr);  /*��������16λ�ӵ�ַ��λ�ֽ�*/
     if(ack==0)return(0);
   Start_I2c();   
   SendByte(IIC_READ_SLAVE_ADDR); /*����������ַ*/
     if(ack==0)return(0); 
   c = RecvByte();               /*��ȡ����*/
     Ack_I2c(1);               /*���ͷǾʹ�λ*/
  Stop_I2c();                  /*��������*/ 

  return c;
}


/* �����ӵ�ַ���������ֽ����ݺ���   */
uint8 ISendByte(uint16 sla, uint8 c)
{
   uint8 slaveHighAddr = (sla>>8);
   uint8 slaveLowAddr = (sla&0xff);
   Start_I2c();               /*��������*/
   SendByte(IIC_WRITE_SLAVE_ADDR);  /*����������ַ*/
     if(ack==0)return(0);
   SendByte(slaveHighAddr);  /*��������16λ�ӵ�ַ��λ�ֽ�*/
     if(ack==0)return(0);
   SendByte(slaveLowAddr);  /*��������16λ�ӵ�ַ��λ�ֽ�*/
      if(ack==0)return(0);
   SendByte(c);               /*��������*/
     if(ack==0)return(0);
   Stop_I2c();                 /*��������*/ 
   
   return(1);
}



