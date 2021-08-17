#include "sys.h"
#include "show.h"
#include "control.h"
#include "stmflash.h"
#include "key.h"
//作者 Lee Young 
//2021年6玥27日，第一次更新：增加了通过按键调整机械中值的功能。

u16 datatemp[4];

int main(void)
{ 
	Stm32_Clock_Init(9);            //系统时钟设置
	delay_init(72);                 //延时初始化
	uart_init(72,19200);             //初始化串口1
	uart2_init(72,19200);           //初始化串口2
	JTAG_Set(JTAG_SWD_DISABLE);     //关闭JTAG接口
	JTAG_Set(SWD_ENABLE);           //打开SWD接口 可以利用主板的SWD接口调试
	MY_NVIC_PriorityGroupConfig(2);
	KEY_Init();
	MiniBalance_PWM_Init(7199,0);   //定时器3初始化PWM 10KHZ，用于驱动电机 
	Server_PWM_Init(1999,720);        //舵机初始化PWM 
	OLED_Init();                    //OLED初始化       
  fill_picture(0x00);	            //OLED清屏
  Baterry_Adc_Init();             //电量检测Adc初始化
	OLED_ShowString(25,3,"MPU6050",12);
	OLED_ShowString(27,4,"init...",12);
	delay_ms(1000);
	while(MPU_Init());              //MPU6050初始化
	fill_picture(0x00);
  OLED_ShowString(25,3,"MPU6050",12);
	OLED_ShowString(5,4,"init success!!!",12);
	delay_ms(500);
	fill_picture(0x00);
	LED_Init();                     //LED初始化
	Encoder_Init_TIM4();            //初始化编码器（TIM4的编码器接口模式） 
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,4);   //读取flash中的中值数据
	if(datatemp[3]==86)             //若不为第一次开机
	{
   Center_Gra_Sart=datatemp[0]*10+datatemp[1]+((float)datatemp[2])/10;
	}
	else                            //第一次开机 
	{ 
		datatemp[3]=86;  
    datatemp[0]=((int)(Center_Gra_Sart*10))/100;
		datatemp[1]=(((int)(Center_Gra_Sart*10))%100)/10;
		datatemp[2]=((int)(Center_Gra_Sart*10))%10;
		
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)datatemp,7);
	}
  Center_Gravity=Center_Gra_Sart;
	Angle_Balance_x=Center_Gra_Sart;   //横向角度	
	Timer1_Init(99,7199);           //定时器1中断初始化 
	while(1)oled_show();
}
