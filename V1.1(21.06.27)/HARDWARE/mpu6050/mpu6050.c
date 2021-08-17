#include "mpu6050.h"
struct _sensor sensor;
u8 MPU_Init(void)  //初始化MPU6050  返回值:0,成功  其他,错误代码
{ 
	u8 res; 
	I2C_init();//初始化IIC总线
	Single_Write(MPU_ADDR,MPU_PWR_MGMT1_REG,0X80);//复位MPU6050
  delay_ms(100);
	Single_Write(MPU_ADDR,MPU_PWR_MGMT1_REG,0X00);//唤醒MPU6050 
  Single_Write(MPU_ADDR,MPU_SAMPLE_RATE_REG,0x01);//设置采样率为500Hz
  Single_Write(MPU_ADDR,MPU_CFG_REG,0x04);  //设置20Hz低通滤波
	Single_Write(MPU_ADDR,MPU_GYRO_CFG_REG,0x10);//设置陀螺仪量程+-1000
	Single_Write(MPU_ADDR,MPU_ACCEL_CFG_REG,0x09);//设置加速度计量程+-4g
	res=Single_Read(MPU_ADDR,MPU_DEVICE_ID_REG); 
	if(res==MPU_ADDR)//器件ID正确
	{
		Single_Write(MPU_ADDR,MPU_PWR_MGMT2_REG,0X00);//加速度与陀螺仪都工作
 	}
	else return 1;
	delay_ms(45);//等待数值稳定
	return 0;
}

void MPU_Getdata(void)
{
	u8 bufa[6],bufg[6];
	
	MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,bufa);
	MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,bufg);
	
	sensor.acc.origin.x = ((((int16_t)bufa[0]) << 8) | bufa[1]);
	sensor.acc.origin.y = ((((int16_t)bufa[2]) << 8) | bufa[3]);
	sensor.acc.origin.z = ((((int16_t)bufa[4]) << 8) | bufa[5]);

	sensor.gyro.origin.x = ((((int16_t)bufg[0]) << 8)| bufg[1]);
	sensor.gyro.origin.y = ((((int16_t)bufg[2]) << 8)| bufg[3]);
	sensor.gyro.origin.z = ((((int16_t)bufg[4]) << 8)| bufg[5]);
	
	sensor.acc.offset.x=sensor.acc.origin.x-sensor.acc.quiet.x;
	sensor.acc.offset.y=sensor.acc.origin.y-sensor.acc.quiet.y;
  
	sensor.gyro.offset.x = sensor.gyro.origin.x - sensor.gyro.quiet.x;
	sensor.gyro.offset.y = sensor.gyro.origin.y - sensor.gyro.quiet.y;
	sensor.gyro.offset.z = sensor.gyro.origin.z - sensor.gyro.quiet.z;
}

void MPU_Offset(void)
{
	static u8 over_flag=0;
	u8  i,cnt_g = 0;
	struct _int16 last;
	int Integral[3] = {0,0,0};
	s32 tempg[3]={0,0,0},tempa[3]={0,0,0};
	//s16 gx_last=0,gy_last=0,gz_last=0;
	while(!over_flag)	//此循环是确保MPU6050芯片处于完全静止状态
	{
		if(cnt_g < 200)
		{
			MPU_Getdata();

			tempg[0] += sensor.gyro.origin.x;
			tempg[1] += sensor.gyro.origin.y;
			tempg[2] += sensor.gyro.origin.z;
			
			tempa[0] += sensor.acc.origin.x;
			tempa[1] += sensor.acc.origin.y;
			tempa[2] += sensor.acc.origin.z;
			
			Integral[0] += abs(last.x - sensor.gyro.origin.x);
			Integral[1] += abs(last.y - sensor.gyro.origin.y);
			Integral[2] += abs(last.z - sensor.gyro.origin.z);

			last.x = sensor.gyro.origin.x;
			last.y = sensor.gyro.origin.y;
			last.z = sensor.gyro.origin.z;
		}
		else 
			{			
			if(Integral[0]>=60|| Integral[1]>=60|| Integral[2]>= 60) // 未校准成功
				{
				 cnt_g = 0;
				 for(i=0;i<3;i++)tempa[i]=tempg[i]=Integral[i]=0;
			  }
			else // 校准成功 
				{				
				  sensor.acc.quiet.x = tempa[0]/200;
					sensor.acc.quiet.y = tempa[1]/200;
					sensor.acc.quiet.z = tempa[2]/200;
				
				  sensor.gyro.quiet.x = tempg[0]/200;
	        sensor.gyro.quiet.y = tempg[1]/200;
	        sensor.gyro.quiet.z = tempg[2]/200;
				  over_flag = 1;
			  }
		}
		cnt_g++;
	}
}




