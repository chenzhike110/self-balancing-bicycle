#include "i2c.h"
#include "delay.h"
void I2C_Delay(void)
{
	delay_us(2);
}
void I2C_init(void) //��ʼ��IIC
{					     
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;	 // �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO 
	
  GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4);						 //PB10,PB11 �����	
}
void I2C_Start(void) //����IIC��ʼ�ź�
{
	MPU_SDA_OUT();     //sda�����
	I2C_SDA=1;	  	  
	I2C_SCL=1;
	I2C_Delay();
 	I2C_SDA=0;//START:when CLK is high,DATA change form high to low 
	I2C_Delay();
	I2C_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
void I2C_Stop(void) //����IICֹͣ�ź�
{
	MPU_SDA_OUT();//sda�����
	I2C_SCL=0;
	I2C_SDA=0;//STOP:when CLK is high DATA change form low to high
 	I2C_Delay();
	I2C_SCL=1;  
	I2C_SDA=1;//����I2C���߽����ź�
	I2C_Delay();							   	
}
u8 I2C_Wait_Ack(void)  //�ȴ�Ӧ���źŵ���  ����ֵ��1������Ӧ��ʧ��    0������Ӧ��ɹ�
{
	u8 ucErrTime=0;
	MPU_SDA_IN();      //SDA����Ϊ����  
	I2C_SDA=1;I2C_Delay();	   
	I2C_SCL=1;I2C_Delay();	 
	while(MPU_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			return 1;
		}
	}
	I2C_SCL=0;//ʱ�����0 	   
	return 0;  
} 
void I2C_Ack(void)  //����ACKӦ��
{
	I2C_SCL=0;
	MPU_SDA_OUT();
	I2C_SDA=0;
	I2C_Delay();
	I2C_SCL=1;
	I2C_Delay();
	I2C_SCL=0;
}		    
void I2C_NAck(void) //������ACKӦ��
{
	I2C_SCL=0;
	MPU_SDA_OUT();
	I2C_SDA=1;
	I2C_Delay();
	I2C_SCL=1;
	I2C_Delay();
	I2C_SCL=0;
}					 				     	  
void I2C_Send_Byte(u8 txd)   //IIC����һ���ֽ�  ���شӻ�����Ӧ��  1����Ӧ��  0����Ӧ��		
{                        
    u8 t;   
	  MPU_SDA_OUT(); 	    
    I2C_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        I2C_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		I2C_SCL=1;
		I2C_Delay(); 
		I2C_SCL=0;	
		I2C_Delay();
    }	 
} 	    
u8 I2C_Read_Byte(unsigned char ack)   //��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
{
	unsigned char i,receive=0;
	MPU_SDA_IN();//SDA����Ϊ����
  for(i=0;i<8;i++ )
	{
		I2C_SCL=0; 
		I2C_Delay();
		I2C_SCL=1;
		receive<<=1;
		if(MPU_READ_SDA)receive++;   
		I2C_Delay(); 
   }					 
   if (!ack)I2C_NAck();//����nACK
   else I2C_Ack(); //����ACK   
   return receive;
}
u8 Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)	 //���ֽ�д��	    
{
  	I2C_Start();
    I2C_Send_Byte((SlaveAddress<<1)|0);//����������ַ+д����
    I2C_Wait_Ack();
    I2C_Send_Byte(REG_Address );   //���üĴ�����ַ      
    I2C_Wait_Ack();	
    I2C_Send_Byte(REG_data);
    I2C_Wait_Ack();
    I2C_Stop(); 
    return 0;
}
u8 Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)  //���ֽڶ�ȡ
{  
	u8 REG_data;     	
	I2C_Start();
	I2C_Send_Byte((SlaveAddress<<1)|0);//����������ַ+д���� 
	I2C_Wait_Ack();
	I2C_Send_Byte(REG_Address);   //���üĴ�����ַ      
	I2C_Wait_Ack();
	I2C_Start();
	I2C_Send_Byte((SlaveAddress<<1)|1);//����������ַ+������
	I2C_Wait_Ack();
	REG_data= I2C_Read_Byte(0);//��ȡ����,����nACK 
	I2C_Stop();
	return REG_data;
}	
//IIC����д
//addr:������ַ reg:�Ĵ�����ַ len:д�볤�� buf:������  ����ֵ:0,����   ����,�������
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 i; 
    I2C_Start(); 
	I2C_Send_Byte((addr<<1)|0);//����������ַ+д����	
	I2C_Wait_Ack();
    I2C_Send_Byte(reg);	//д�Ĵ�����ַ
    I2C_Wait_Ack();		//�ȴ�Ӧ��
	for(i=0;i<len;i++)
	{
		I2C_Send_Byte(buf[i]);	//��������
		I2C_Wait_Ack();
	}    
    I2C_Stop();	 
	return 0;	
} 

//IIC������
//addr:������ַ  reg:Ҫ��ȡ�ļĴ�����ַ  len:Ҫ��ȡ�ĳ���  buf:��ȡ�������ݴ洢��
//����ֵ:0,����    ����,�������
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
 	I2C_Start(); 
	I2C_Send_Byte((addr<<1)|0);//����������ַ+д����	
	I2C_Wait_Ack();
	I2C_Send_Byte(reg);	//д�Ĵ�����ַ
	I2C_Wait_Ack();		//�ȴ�Ӧ��
	I2C_Start();
	I2C_Send_Byte((addr<<1)|1);//����������ַ+������	
  I2C_Wait_Ack();		//�ȴ�Ӧ�� 
	while(len)
	{
		if(len==1)*buf=I2C_Read_Byte(0);//������,����nACK 
		else *buf=I2C_Read_Byte(1);		//������,����ACK  
		len--;
		buf++; 
	}    
    I2C_Stop();	//����һ��ֹͣ���� 
	return 0;	
}




