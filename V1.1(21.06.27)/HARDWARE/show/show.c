#include "show.h"
#include "math.h"
#include "control.h"

void oled_show(void)
{	
	  OLED_ShowString(0,0,"Vx:",12);       //��������ת�ٶ���ʾ
	  if(Encoder_x>=0){OLED_ShowString(20,0,"+",12);OLED_ShowNum(27,0,Encoder_x,3,12);}
    else          {OLED_ShowString(20,0,"-",12);OLED_ShowNum(27,0,-Encoder_x,3,12);}
   
    OLED_ShowString(0,1,"voltage:",12);  //��ص�����ʾ
		OLED_ShowNum(60,1,(int)(Voltage*1000),4,12);OLED_ShowString(85,1,"mV",12);
		
		OLED_ShowString(60,2,"GR:",12);      //ROLL����ٶ���ʾ
    if(Gyro_Balance_x>=0){OLED_ShowString(91,2,"+",12);OLED_ShowNum(98,2,Gyro_Balance_x,4,12);}
		else                  {OLED_ShowString(91,2,"-",12);OLED_ShowNum(98,2,-Gyro_Balance_x,4,12);}
		
		OLED_ShowString(0,4,"Rol:",12);     //ROLL��Ƕ���ʾ
    if(Angle_Balance_x>=0)
      {
			  OLED_ShowString(30,4,"+",12);
			  OLED_ShowNum(37,4,Angle_Balance_x,3,12);
				OLED_ShowString(55,4,".",12);
				OLED_ShowNum(60,4,(int)(Angle_Balance_x*10)%10,1,12);
				OLED_ShowString(68,4,"deg",12);
			}
		else                  
      {
			  OLED_ShowString(30,4,"-",12);
			  OLED_ShowNum(37,4,-Angle_Balance_x,3,12);
				OLED_ShowString(55,4,".",12);
				OLED_ShowNum(60,4,(int)(-Angle_Balance_x*10)%10,1,12);
				OLED_ShowString(68,4,"deg",12);
			}
			OLED_ShowString(0,5,"remot_moto:",12);     //ң���ٶ���ʾ
		if(remot_moto>=0)
			{
			  OLED_ShowString(90,5,"+",12);
			  OLED_ShowNum(97,5,remot_moto,3,12);
			}
		else 
			{
			  OLED_ShowString(90,5,"-",12);
			  OLED_ShowNum(97,5,-remot_moto,3,12);
			}
			
		OLED_ShowString(0,6,"Cen_SET:",12);    //�����У���̬��е��ֵ��ʾ
   OLED_ShowNum(77,6,Center_Gra_Sart,2,12);
		OLED_ShowString(89,6,".",12);
		OLED_ShowNum(94,6,(int)(Center_Gra_Sart*10)%10,1,12);
		OLED_ShowString(102,6,"deg",12);
	}
void fill_picture(unsigned char fill_Data)    //OLED���ͼƬ����
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);	//�ӵ�0ҳ����7ҳ
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)OLED_WR_Byte(fill_Data,1);	
	}
}



