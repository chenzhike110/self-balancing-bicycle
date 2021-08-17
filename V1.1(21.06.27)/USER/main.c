#include "sys.h"
#include "show.h"
#include "control.h"
#include "stmflash.h"
#include "key.h"
//���� Lee Young 
//2021��6�h27�գ���һ�θ��£�������ͨ������������е��ֵ�Ĺ��ܡ�

u16 datatemp[4];

int main(void)
{ 
	Stm32_Clock_Init(9);            //ϵͳʱ������
	delay_init(72);                 //��ʱ��ʼ��
	uart_init(72,19200);             //��ʼ������1
	uart2_init(72,19200);           //��ʼ������2
	JTAG_Set(JTAG_SWD_DISABLE);     //�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);           //��SWD�ӿ� �������������SWD�ӿڵ���
	MY_NVIC_PriorityGroupConfig(2);
	KEY_Init();
	MiniBalance_PWM_Init(7199,0);   //��ʱ��3��ʼ��PWM 10KHZ������������� 
	Server_PWM_Init(1999,720);        //�����ʼ��PWM 
	OLED_Init();                    //OLED��ʼ��       
  fill_picture(0x00);	            //OLED����
  Baterry_Adc_Init();             //�������Adc��ʼ��
	OLED_ShowString(25,3,"MPU6050",12);
	OLED_ShowString(27,4,"init...",12);
	delay_ms(1000);
	while(MPU_Init());              //MPU6050��ʼ��
	fill_picture(0x00);
  OLED_ShowString(25,3,"MPU6050",12);
	OLED_ShowString(5,4,"init success!!!",12);
	delay_ms(500);
	fill_picture(0x00);
	LED_Init();                     //LED��ʼ��
	Encoder_Init_TIM4();            //��ʼ����������TIM4�ı������ӿ�ģʽ�� 
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,4);   //��ȡflash�е���ֵ����
	if(datatemp[3]==86)             //����Ϊ��һ�ο���
	{
   Center_Gra_Sart=datatemp[0]*10+datatemp[1]+((float)datatemp[2])/10;
	}
	else                            //��һ�ο��� 
	{ 
		datatemp[3]=86;  
    datatemp[0]=((int)(Center_Gra_Sart*10))/100;
		datatemp[1]=(((int)(Center_Gra_Sart*10))%100)/10;
		datatemp[2]=((int)(Center_Gra_Sart*10))%10;
		
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)datatemp,7);
	}
  Center_Gravity=Center_Gra_Sart;
	Angle_Balance_x=Center_Gra_Sart;   //����Ƕ�	
	Timer1_Init(99,7199);           //��ʱ��1�жϳ�ʼ�� 
	while(1)oled_show();
}
