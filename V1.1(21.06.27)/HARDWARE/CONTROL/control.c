#include "control.h"	
#include "show.h"
#include "usart.h"
#include "stmflash.h"
float Center_Gravity;               //机械中值x轴方向机械中值  可以将小车PWM关闭然后观察小车的x轴平衡时屏幕显示的角度（即为机械中值），每个小车由于电池和惯量轮安装位置不同会有些许差异
float Center_Gra_Sart=90.0;         //开机设定机械中值

int Encoder_x=0;            //横向电机编码器变量
int Moto_x,Moto_y;          //电机PWM变量	
float Voltage;              //电池电压变量
float Angle_Balance_x; //横向角度
float Gyro_Balance_x=0;     //横向角加速度
int Balance_Pwm_x=0,velocity_Pwm_x=0; //横向电机PWM分量
int TIM1_UP_IRQHandler(void)          //所有的控制代码都在这里面 TIM1控制的10ms定时中断  
{    
	if(TIM1->SR&0X0001)
	{   
		  u8 key_value;
		  TIM1->SR&=~(1<<0);      //清除定时器1中断标志位	
      Get_Angle();                              //获取侧向角度
		  Voltage=Get_battery_volt(); 		          //获取当前电池电压
			Encoder_x=Read_Encoder(4);                //读取编码器值
      Balance_Pwm_x=balance_x(Angle_Balance_x,Gyro_Balance_x-40);  //角度闭环控制
      velocity_Pwm_x=velocity_x(Encoder_x);				                 //速度闭环控制
			if(Angle_Balance_x<120&&Angle_Balance_x>60)Moto_x=Balance_Pwm_x+velocity_Pwm_x; //计算电机最终PWM
      else 	Moto_x=0;	
    
		  Xianfu_Pwm();           Moto_y=remot_moto*40;		              //前进电机赋值
		  PWM_SERVER=2000-((remot_angle/2)+125);//舵机赋值                //PWM限幅
      //Set_Pwm(0,0);                       //赋值给PWM寄存器*/						
			Set_Pwm(Moto_x,Moto_y);               //赋值给PWM寄存器*/	
      key_value=key_read();
			if(key_value==3)                         //两个按键同时被按下，进入参数调节状态
			{
				Set_Pwm(0,0);                           //关闭电机
				while(key_value==3)key_value=key_read();
				fill_picture(0x00);	            //OLED清屏
				OLED_ShowString(5,10,"SET_UP",12);
			  OLED_ShowString(10,4,"Entering...",12);					
				delay_ms(1000);
				fill_picture(0x00);	            //OLED清屏
				while(1)
				{
					Get_Angle();                           //获取角度
		      Voltage=Get_battery_volt();            //获取电压
          Encoder_x=Read_Encoder(4);             //更新编码器位置信息
					key_value=key_read();	
						if(key_value==1)
						{
							Center_Gra_Sart=Center_Gra_Sart-0.1;
			        if(Center_Gra_Sart<70)Center_Gra_Sart=70;
							oled_show(); 
							delay_ms(200);				
						}
						else if(key_value==2)
						{
							Center_Gra_Sart=Center_Gra_Sart+0.1;
							if(Center_Gra_Sart>110)Center_Gra_Sart=110;
							oled_show(); 
							delay_ms(200);
						}
						else if(key_value==3)
						{
							fill_picture(0x00);	            //OLED清屏
							OLED_ShowString(5,3,"Parameters",12);
							OLED_ShowString(10,4,"Saving...",12);
							datatemp[0]=((int)(Center_Gra_Sart*10))/100;
							datatemp[1]=(((int)(Center_Gra_Sart*10))%100)/10;
							datatemp[2]=((int)(Center_Gra_Sart*10))%10;
							STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)datatemp,4);						
							delay_ms(1000);delay_ms(1000);
							fill_picture(0x00);	            //OLED清屏
							break;
						}
					oled_show(); 
					delay_ms(5);
				}
			}		
	}
	 return 0;	  
} 
int balance_x(float Angle,float gyro)//倾角PD控制 入口参数：角度 返回  值：倾角控制PWM
{  
	 //float Balance_KP=350,Balance_KI=0,Balance_KD=2;
	 float Balance_KP=750,Balance_KI=0,Balance_KD=4;
   float Bias;                                        //倾角偏差
	 static float D_Bias,Integral_bias;                 //PID相关变量
	 int balance;                                       //PWM返回值 
	 Bias=Angle-Center_Gravity;                                   //求出平衡的角度中值 和机械相关
	 Integral_bias+=Bias;	
	 if(Integral_bias>30000)Integral_bias=30000;
	 if(Integral_bias<-30000)Integral_bias=-30000;
   D_Bias=gyro;	                                      //求出偏差的微分 进行微分控制
	 balance=Balance_KP*Bias+Balance_KI*Integral_bias+D_Bias*Balance_KD;   //===计算倾角控制的电机PWM  PD控制
	 return balance;
}
int velocity_x(int encoder)   //位置式PID控制器 入口参数：编码器测量位置信息，目标位置  返回  值：电机PWM
{ 	
	 float Position_KP=50,Position_KI=0.02,Position_KD=0;
	 static float Pwm,Integral_bias,Last_Bias,Encoder;
	 Encoder *= 0.65;		                                       //一阶低通滤波器       
	 Encoder += encoder*0.35;	                                 //一阶低通滤波器    
	 Integral_bias+=Encoder;	                                 //求出偏差的积分
	 if(Integral_bias>20000)Integral_bias=20000;
	 if(Integral_bias<-20000)Integral_bias=-20000;
	 Pwm=Position_KP*Encoder+Position_KI*Integral_bias+Position_KD*(Encoder-Last_Bias);       //位置式PID控制器
	 Last_Bias=Encoder;                                       //保存上一次偏差 
	 return Pwm;                                              //增量输出
}
void Set_Pwm(int motox,int motoy)
{
    	if(motox<0)			DIR=0;
			else 	          DIR=1;
			PWMX=myabs(motox)+100;
	
	    if(motoy<0){DIRA=1;PWMY=7200-myabs(motoy);}
			else 	{DIRA=0;PWMY=myabs(motoy);}
}
void Xianfu_Pwm(void)
{	
	  int Amplitude_x=6900,Amplitude_y=6900;    //===PWM满幅是7200 限制在6900
	  if(Moto_x<-Amplitude_x) Moto_x=-Amplitude_x;	
		if(Moto_x>Amplitude_x)  Moto_x=Amplitude_x;	

    if(Moto_y<-Amplitude_y) Moto_y=-Amplitude_y;	
		if(Moto_y>Amplitude_y)  Moto_y=Amplitude_y;		
}
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}
void Yijielvbo_X(float angle_m, float gyro_m) //一阶互补滤波  入口参数：加速度、角速度
{
	Angle_Balance_x = 0.1 * angle_m+ (1-0.1) * (Angle_Balance_x + gyro_m * 0.005);
}
void Get_Angle(void)
{ 
  u8 bufa[6],bufg[6];	
	float Accel_Y,Accel_X,Accel_Z,Gyro_Y,Gyro_X,Gyro_Z;
	MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,bufa);
	MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,bufg);
	
//	sensor.acc.origin.y = ((((int16_t)bufa[0]) << 8) | bufa[1]);
//	sensor.acc.origin.x = ((((int16_t)bufa[2]) << 8) | bufa[3]);
//	sensor.acc.origin.z = ((((int16_t)bufa[4]) << 8) | bufa[5]);

//	sensor.gyro.origin.y = ((((int16_t)bufg[0]) << 8)| bufg[1]);
//	sensor.gyro.origin.x = ((((int16_t)bufg[2]) << 8)| bufg[3]);
//	sensor.gyro.origin.z = ((((int16_t)bufg[4]) << 8)| bufg[5]);
	
	sensor.acc.origin.x = ((((int16_t)bufa[0]) << 8) | bufa[1]);
	sensor.acc.origin.y = ((((int16_t)bufa[2]) << 8) | bufa[3]);
	sensor.acc.origin.z = ((((int16_t)bufa[4]) << 8) | bufa[5]);

	sensor.gyro.origin.x = ((((int16_t)bufg[0]) << 8)| bufg[1]);
	sensor.gyro.origin.y = ((((int16_t)bufg[2]) << 8)| bufg[3]);
	sensor.gyro.origin.z = ((((int16_t)bufg[4]) << 8)| bufg[5]);
		  
	Gyro_Y=sensor.gyro.origin.y;    //读取X轴陀螺仪
	Gyro_Z=sensor.gyro.origin.z;    //读取Z轴陀螺仪
	Accel_X=sensor.acc.origin.x;    //读取Y轴加速度计
	Accel_Z=sensor.acc.origin.z;    //读取Z轴加速度计
	if(Gyro_Y>32768)  Gyro_Y-=65536;                       //数据类型转换  也可通过short强制类型转换
	if(Gyro_Z>32768)  Gyro_Z-=65536;                       //数据类型转换
	if(Accel_X>32768) Accel_X-=65536;                      //数据类型转换
	if(Accel_Z>32768) Accel_Z-=65536;                      //数据类型转换	
//	Gyro_Balance_y=-Gyro_Y;                //更新平衡角速度		
	Accel_Y=atan2(Accel_X,Accel_Z)*180/PI; //计算倾角	
  Gyro_Y=Gyro_Y/16.4;                    //陀螺仪量程转换

			
	Gyro_X=sensor.gyro.origin.x;       //读取X轴陀螺仪
	Accel_Y=sensor.acc.origin.y;       //读取Y轴加速度计
	if(Gyro_X>32768)  Gyro_X-=65536;   //数据类型转换  也可通过short强制类型转换
	if(Accel_Y>32768) Accel_Y-=65536;  //数据类型转换
	Gyro_Balance_x=-Gyro_X;            //更新平衡角速度
	Accel_X= (atan2(Accel_Z , Accel_Y)) * 180 / PI; //计算倾角	
  Gyro_X=Gyro_X/16.4;                //陀螺仪量程转换			
	Yijielvbo_X(Accel_X,-Gyro_X);
}

