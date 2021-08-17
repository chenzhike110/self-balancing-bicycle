#ifndef __I2C_H
#define __I2C_H
#include "sys.h"
#include "delay.h"
	   		   
//IO��������
#define MPU_SDA_IN()  {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=8<<12;}
#define MPU_SDA_OUT() {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=3<<12;}

//IO��������	 
#define I2C_SCL    PBout(4) 		//SCL
#define I2C_SDA    PBout(3) 		//SDA	 
#define MPU_READ_SDA   PBin(3) 		//����SDA 

//IIC���в�������
void I2C_Delay(void);				//MPU IIC��ʱ����
void I2C_init(void);                //��ʼ��IIC��IO��				 
void I2C_Start(void);				//����IIC��ʼ�ź�
void I2C_Stop(void);	  			//����IICֹͣ�ź�
void I2C_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 I2C_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 I2C_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void I2C_Ack(void);					//IIC����ACK�ź�
void I2C_NAck(void);				//IIC������ACK�ź�

u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);//IIC����д
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf); //IIC������ 

u8 Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);
u8 Single_Read(unsigned char SlaveAddress,unsigned char REG_Address);

  
#endif
















