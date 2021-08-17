#include "motor.h"
void MiniBalance_Motor_Init(void)  //��������������
{
	RCC->APB2ENR|=1<<2;       //PORTAʱ��ʹ�� 
	RCC->APB2ENR|=1<<3;       //PORTBʱ��ʹ�� 
	GPIOA->CRL&=0XF00FFFFF;   //PORTA,5,6������� 
	GPIOA->CRL|=0X02200000;   //PORTA,5,6�������
	
	GPIOB->CRH&=0XFFFFFF0F;   //PORTB9������� 
	GPIOB->CRH|=0X00000020;   //PORTB9�������
	
	GPIOB->CRL&=0XFFFFFF0F;   //PORTB1������� 
	GPIOB->CRL|=0X00000020;   //PORTB1�������

	EN=1;                     //ʹ�ܹ����ֵ��
}
void MiniBalance_PWM_Init(u16 arr,u16 psc)  //���PWM���ų�ʼ��
{		 					 
  MiniBalance_Motor_Init(); //��ʼ�������������IO
	RCC->APB1ENR|=1<<1;       //TIM3ʱ��ʹ��    
	RCC->APB2ENR|=1<<3;       //PORTBʱ��ʹ�� 
  RCC->APB2ENR|=1<<2;       //PORTAʱ��ʹ�� 
 	
	GPIOB->CRL&=0XFFFFFFF0;   //PORTB0 �������
	GPIOB->CRL|=0X0000000B;   //PORTB0 �������
	
	GPIOA->CRL&=0X0FFFFFFF;   //PORTA  7�������
	GPIOA->CRL|=0XB0000000;   //PORTA  7�������
	
	TIM3->ARR=arr;            //�趨�������Զ���װֵ 
	TIM3->PSC=psc;            //Ԥ��Ƶ������Ƶ
	
	TIM3->CCMR2|=6<<12;       //CH4 PWM1ģʽ	
	TIM3->CCMR2|=6<<4;        //CH3 PWM1ģʽ
  TIM3->CCMR1|=7<<12;       //CH2 PWM1ģʽ
	
  TIM3->CCMR1|=1<<11;       //CH2Ԥװ��ʹ��
	
	TIM3->CCMR2|=1<<11;       //CH4Ԥװ��ʹ��	 
	TIM3->CCMR2|=1<<3;        //CH3Ԥװ��ʹ��	 

  TIM3->CCER|=1<<4;         //CH2���ʹ��

	TIM3->CCER|=1<<12;        //CH4���ʹ��	   
	TIM3->CCER|=1<<8;         //CH3���ʹ��	
	TIM3->BDTR |= 1<<15;
	TIM3->CR1=0x80;           //ARPEʹ�� 
	TIM3->CR1|=0x01;          //ʹ�ܶ�ʱ��3 
} 
void Server_PWM_Init(u16 arr,u16 psc)  //���PWM���ų�ʼ��
{		 					 
	RCC->APB1ENR|=1<<0;         //TIM2ʱ��ʹ��   
  RCC->APB2ENR|=1<<2;       //PORTAʱ��ʹ�� 
 	
	GPIOA->CRL&=0XFFFFFF0F;   //PORTA1 �������
	GPIOA->CRL|=0X000000B0;   //PORTA1 �������
	
	TIM2->ARR=arr;            //�趨�������Զ���װֵ 
	TIM2->PSC=psc;            //Ԥ��Ƶ������Ƶ
	
  TIM2->CCMR1|=7<<12;       //CH2 PWM1ģʽ
	
  TIM2->CCMR1|=1<<11;       //CH2Ԥװ��ʹ��

  TIM2->CCER|=1<<4;         //CH2���ʹ��

	TIM2->BDTR |= 1<<15;
	TIM2->CR1=0x80;           //ARPEʹ�� 
	TIM2->CR1|=0x01;          //ʹ�ܶ�ʱ��2
} 

