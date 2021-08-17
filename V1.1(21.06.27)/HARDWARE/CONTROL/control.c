#include "control.h"	
#include "show.h"
#include "usart.h"
#include "stmflash.h"
float Center_Gravity;               //��е��ֵx�᷽���е��ֵ  ���Խ�С��PWM�ر�Ȼ��۲�С����x��ƽ��ʱ��Ļ��ʾ�ĽǶȣ���Ϊ��е��ֵ����ÿ��С�����ڵ�غ͹����ְ�װλ�ò�ͬ����Щ�����
float Center_Gra_Sart=90.0;         //�����趨��е��ֵ

int Encoder_x=0;            //����������������
int Moto_x,Moto_y;          //���PWM����	
float Voltage;              //��ص�ѹ����
float Angle_Balance_x; //����Ƕ�
float Gyro_Balance_x=0;     //����Ǽ��ٶ�
int Balance_Pwm_x=0,velocity_Pwm_x=0; //������PWM����
int TIM1_UP_IRQHandler(void)          //���еĿ��ƴ��붼�������� TIM1���Ƶ�10ms��ʱ�ж�  
{    
	if(TIM1->SR&0X0001)
	{   
		  u8 key_value;
		  TIM1->SR&=~(1<<0);      //�����ʱ��1�жϱ�־λ	
      Get_Angle();                              //��ȡ����Ƕ�
		  Voltage=Get_battery_volt(); 		          //��ȡ��ǰ��ص�ѹ
			Encoder_x=Read_Encoder(4);                //��ȡ������ֵ
      Balance_Pwm_x=balance_x(Angle_Balance_x,Gyro_Balance_x-40);  //�Ƕȱջ�����
      velocity_Pwm_x=velocity_x(Encoder_x);				                 //�ٶȱջ�����
			if(Angle_Balance_x<120&&Angle_Balance_x>60)Moto_x=Balance_Pwm_x+velocity_Pwm_x; //����������PWM
      else 	Moto_x=0;	
    
		  Xianfu_Pwm();           Moto_y=remot_moto*40;		              //ǰ�������ֵ
		  PWM_SERVER=2000-((remot_angle/2)+125);//�����ֵ                //PWM�޷�
      //Set_Pwm(0,0);                       //��ֵ��PWM�Ĵ���*/						
			Set_Pwm(Moto_x,Moto_y);               //��ֵ��PWM�Ĵ���*/	
      key_value=key_read();
			if(key_value==3)                         //��������ͬʱ�����£������������״̬
			{
				Set_Pwm(0,0);                           //�رյ��
				while(key_value==3)key_value=key_read();
				fill_picture(0x00);	            //OLED����
				OLED_ShowString(5,10,"SET_UP",12);
			  OLED_ShowString(10,4,"Entering...",12);					
				delay_ms(1000);
				fill_picture(0x00);	            //OLED����
				while(1)
				{
					Get_Angle();                           //��ȡ�Ƕ�
		      Voltage=Get_battery_volt();            //��ȡ��ѹ
          Encoder_x=Read_Encoder(4);             //���±�����λ����Ϣ
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
							fill_picture(0x00);	            //OLED����
							OLED_ShowString(5,3,"Parameters",12);
							OLED_ShowString(10,4,"Saving...",12);
							datatemp[0]=((int)(Center_Gra_Sart*10))/100;
							datatemp[1]=(((int)(Center_Gra_Sart*10))%100)/10;
							datatemp[2]=((int)(Center_Gra_Sart*10))%10;
							STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)datatemp,4);						
							delay_ms(1000);delay_ms(1000);
							fill_picture(0x00);	            //OLED����
							break;
						}
					oled_show(); 
					delay_ms(5);
				}
			}		
	}
	 return 0;	  
} 
int balance_x(float Angle,float gyro)//���PD���� ��ڲ������Ƕ� ����  ֵ����ǿ���PWM
{  
	 //float Balance_KP=350,Balance_KI=0,Balance_KD=2;
	 float Balance_KP=750,Balance_KI=0,Balance_KD=4;
   float Bias;                                        //���ƫ��
	 static float D_Bias,Integral_bias;                 //PID��ر���
	 int balance;                                       //PWM����ֵ 
	 Bias=Angle-Center_Gravity;                                   //���ƽ��ĽǶ���ֵ �ͻ�е���
	 Integral_bias+=Bias;	
	 if(Integral_bias>30000)Integral_bias=30000;
	 if(Integral_bias<-30000)Integral_bias=-30000;
   D_Bias=gyro;	                                      //���ƫ���΢�� ����΢�ֿ���
	 balance=Balance_KP*Bias+Balance_KI*Integral_bias+D_Bias*Balance_KD;   //===������ǿ��Ƶĵ��PWM  PD����
	 return balance;
}
int velocity_x(int encoder)   //λ��ʽPID������ ��ڲ���������������λ����Ϣ��Ŀ��λ��  ����  ֵ�����PWM
{ 	
	 float Position_KP=50,Position_KI=0.02,Position_KD=0;
	 static float Pwm,Integral_bias,Last_Bias,Encoder;
	 Encoder *= 0.65;		                                       //һ�׵�ͨ�˲���       
	 Encoder += encoder*0.35;	                                 //һ�׵�ͨ�˲���    
	 Integral_bias+=Encoder;	                                 //���ƫ��Ļ���
	 if(Integral_bias>20000)Integral_bias=20000;
	 if(Integral_bias<-20000)Integral_bias=-20000;
	 Pwm=Position_KP*Encoder+Position_KI*Integral_bias+Position_KD*(Encoder-Last_Bias);       //λ��ʽPID������
	 Last_Bias=Encoder;                                       //������һ��ƫ�� 
	 return Pwm;                                              //�������
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
	  int Amplitude_x=6900,Amplitude_y=6900;    //===PWM������7200 ������6900
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
void Yijielvbo_X(float angle_m, float gyro_m) //һ�׻����˲�  ��ڲ��������ٶȡ����ٶ�
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
		  
	Gyro_Y=sensor.gyro.origin.y;    //��ȡX��������
	Gyro_Z=sensor.gyro.origin.z;    //��ȡZ��������
	Accel_X=sensor.acc.origin.x;    //��ȡY����ٶȼ�
	Accel_Z=sensor.acc.origin.z;    //��ȡZ����ٶȼ�
	if(Gyro_Y>32768)  Gyro_Y-=65536;                       //��������ת��  Ҳ��ͨ��shortǿ������ת��
	if(Gyro_Z>32768)  Gyro_Z-=65536;                       //��������ת��
	if(Accel_X>32768) Accel_X-=65536;                      //��������ת��
	if(Accel_Z>32768) Accel_Z-=65536;                      //��������ת��	
//	Gyro_Balance_y=-Gyro_Y;                //����ƽ����ٶ�		
	Accel_Y=atan2(Accel_X,Accel_Z)*180/PI; //�������	
  Gyro_Y=Gyro_Y/16.4;                    //����������ת��

			
	Gyro_X=sensor.gyro.origin.x;       //��ȡX��������
	Accel_Y=sensor.acc.origin.y;       //��ȡY����ٶȼ�
	if(Gyro_X>32768)  Gyro_X-=65536;   //��������ת��  Ҳ��ͨ��shortǿ������ת��
	if(Accel_Y>32768) Accel_Y-=65536;  //��������ת��
	Gyro_Balance_x=-Gyro_X;            //����ƽ����ٶ�
	Accel_X= (atan2(Accel_Z , Accel_Y)) * 180 / PI; //�������	
  Gyro_X=Gyro_X/16.4;                //����������ת��			
	Yijielvbo_X(Accel_X,-Gyro_X);
}

