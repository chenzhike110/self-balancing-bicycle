#ifndef __I2C_H
#define __I2C_H
#include "sys.h"
#include "delay.h"
	   		   
//IO方向设置
#define MPU_SDA_IN()  {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=8<<12;}
#define MPU_SDA_OUT() {GPIOB->CRL&=0XFFFF0FFF;GPIOB->CRL|=3<<12;}

//IO操作函数	 
#define I2C_SCL    PBout(4) 		//SCL
#define I2C_SDA    PBout(3) 		//SDA	 
#define MPU_READ_SDA   PBin(3) 		//输入SDA 

//IIC所有操作函数
void I2C_Delay(void);				//MPU IIC延时函数
void I2C_init(void);                //初始化IIC的IO口				 
void I2C_Start(void);				//发送IIC开始信号
void I2C_Stop(void);	  			//发送IIC停止信号
void I2C_Send_Byte(u8 txd);			//IIC发送一个字节
u8 I2C_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 I2C_Wait_Ack(void); 				//IIC等待ACK信号
void I2C_Ack(void);					//IIC发送ACK信号
void I2C_NAck(void);				//IIC不发送ACK信号

u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);//IIC连续写
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf); //IIC连续读 

u8 Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);
u8 Single_Read(unsigned char SlaveAddress,unsigned char REG_Address);

  
#endif
















