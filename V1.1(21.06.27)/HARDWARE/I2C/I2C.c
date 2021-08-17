#include "i2c.h"
#include "delay.h"
void I2C_Delay(void)
{
	delay_us(2);
}
void I2C_init(void) //初始化IIC
{					     
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;	 // 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIO 
	
  GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4);						 //PB10,PB11 输出高	
}
void I2C_Start(void) //产生IIC起始信号
{
	MPU_SDA_OUT();     //sda线输出
	I2C_SDA=1;	  	  
	I2C_SCL=1;
	I2C_Delay();
 	I2C_SDA=0;//START:when CLK is high,DATA change form high to low 
	I2C_Delay();
	I2C_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
void I2C_Stop(void) //产生IIC停止信号
{
	MPU_SDA_OUT();//sda线输出
	I2C_SCL=0;
	I2C_SDA=0;//STOP:when CLK is high DATA change form low to high
 	I2C_Delay();
	I2C_SCL=1;  
	I2C_SDA=1;//发送I2C总线结束信号
	I2C_Delay();							   	
}
u8 I2C_Wait_Ack(void)  //等待应答信号到来  返回值：1，接收应答失败    0，接收应答成功
{
	u8 ucErrTime=0;
	MPU_SDA_IN();      //SDA设置为输入  
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
	I2C_SCL=0;//时钟输出0 	   
	return 0;  
} 
void I2C_Ack(void)  //产生ACK应答
{
	I2C_SCL=0;
	MPU_SDA_OUT();
	I2C_SDA=0;
	I2C_Delay();
	I2C_SCL=1;
	I2C_Delay();
	I2C_SCL=0;
}		    
void I2C_NAck(void) //不产生ACK应答
{
	I2C_SCL=0;
	MPU_SDA_OUT();
	I2C_SDA=1;
	I2C_Delay();
	I2C_SCL=1;
	I2C_Delay();
	I2C_SCL=0;
}					 				     	  
void I2C_Send_Byte(u8 txd)   //IIC发送一个字节  返回从机有无应答：  1，有应答  0，无应答		
{                        
    u8 t;   
	  MPU_SDA_OUT(); 	    
    I2C_SCL=0;//拉低时钟开始数据传输
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
u8 I2C_Read_Byte(unsigned char ack)   //读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
{
	unsigned char i,receive=0;
	MPU_SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
		I2C_SCL=0; 
		I2C_Delay();
		I2C_SCL=1;
		receive<<=1;
		if(MPU_READ_SDA)receive++;   
		I2C_Delay(); 
   }					 
   if (!ack)I2C_NAck();//发送nACK
   else I2C_Ack(); //发送ACK   
   return receive;
}
u8 Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)	 //单字节写入	    
{
  	I2C_Start();
    I2C_Send_Byte((SlaveAddress<<1)|0);//发送器件地址+写命令
    I2C_Wait_Ack();
    I2C_Send_Byte(REG_Address );   //设置寄存器地址      
    I2C_Wait_Ack();	
    I2C_Send_Byte(REG_data);
    I2C_Wait_Ack();
    I2C_Stop(); 
    return 0;
}
u8 Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)  //单字节读取
{  
	u8 REG_data;     	
	I2C_Start();
	I2C_Send_Byte((SlaveAddress<<1)|0);//发送器件地址+写命令 
	I2C_Wait_Ack();
	I2C_Send_Byte(REG_Address);   //设置寄存器地址      
	I2C_Wait_Ack();
	I2C_Start();
	I2C_Send_Byte((SlaveAddress<<1)|1);//发送器件地址+读命令
	I2C_Wait_Ack();
	REG_data= I2C_Read_Byte(0);//读取数据,发送nACK 
	I2C_Stop();
	return REG_data;
}	
//IIC连续写
//addr:器件地址 reg:寄存器地址 len:写入长度 buf:数据区  返回值:0,正常   其他,错误代码
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 i; 
    I2C_Start(); 
	I2C_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	I2C_Wait_Ack();
    I2C_Send_Byte(reg);	//写寄存器地址
    I2C_Wait_Ack();		//等待应答
	for(i=0;i<len;i++)
	{
		I2C_Send_Byte(buf[i]);	//发送数据
		I2C_Wait_Ack();
	}    
    I2C_Stop();	 
	return 0;	
} 

//IIC连续读
//addr:器件地址  reg:要读取的寄存器地址  len:要读取的长度  buf:读取到的数据存储区
//返回值:0,正常    其他,错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
 	I2C_Start(); 
	I2C_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	I2C_Wait_Ack();
	I2C_Send_Byte(reg);	//写寄存器地址
	I2C_Wait_Ack();		//等待应答
	I2C_Start();
	I2C_Send_Byte((addr<<1)|1);//发送器件地址+读命令	
  I2C_Wait_Ack();		//等待应答 
	while(len)
	{
		if(len==1)*buf=I2C_Read_Byte(0);//读数据,发送nACK 
		else *buf=I2C_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
    I2C_Stop();	//产生一个停止条件 
	return 0;	
}




