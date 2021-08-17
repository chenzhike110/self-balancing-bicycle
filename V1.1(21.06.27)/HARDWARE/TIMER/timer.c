#include "timer.h"
#include "led.h"
#include "motor.h"

int T=0;
u8 server_pwm=125;

void Timer1_Init(u16 arr,u16 psc)  //��ڲ�����arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ�� 
{  
	RCC->APB2ENR|=1<<11;//TIM1ʱ��ʹ��    
 	TIM1->ARR=arr;      //�趨�������Զ���װֵ   
	TIM1->PSC=psc;      //Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	TIM1->DIER|=1<<0;   //��������ж�				
	TIM1->DIER|=1<<6;   //�������ж�	   
	TIM1->CR1|=0x01;    //ʹ�ܶ�ʱ��
	MY_NVIC_Init(2,2,TIM1_UP_IRQn,2);
}  

void Timer2_Init(u16 arr,u16 psc)  //��ڲ�����arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ�� 
{  
	RCC->APB1ENR|=1<<0; //TIM2ʱ��ʹ��
 	TIM2->ARR=arr;      //�趨�������Զ���װֵ   
	TIM2->PSC=psc;      //Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	TIM2->DIER|=1<<0;   //��������ж�				
	//TIM2->DIER|=1<<6;   //�������ж�	   
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��
	MY_NVIC_Init(2,2,TIM2_IRQn,2);
}  

void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//����ж�
	{	
		//TIM2->SR&=~(1<<0);//����жϱ�־λ 
    T++;
    if(T>server_pwm)SERVER=0;
    else 	SERVER=1;
    if(T>2000)T=0;		
	}				   
	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
}



 

