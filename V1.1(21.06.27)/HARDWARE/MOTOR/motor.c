#include "motor.h"
void MiniBalance_Motor_Init(void)  //电机方向控制引脚
{
	RCC->APB2ENR|=1<<2;       //PORTA时钟使能 
	RCC->APB2ENR|=1<<3;       //PORTB时钟使能 
	GPIOA->CRL&=0XF00FFFFF;   //PORTA,5,6推挽输出 
	GPIOA->CRL|=0X02200000;   //PORTA,5,6推挽输出
	
	GPIOB->CRH&=0XFFFFFF0F;   //PORTB9推挽输出 
	GPIOB->CRH|=0X00000020;   //PORTB9推挽输出
	
	GPIOB->CRL&=0XFFFFFF0F;   //PORTB1推挽输出 
	GPIOB->CRL|=0X00000020;   //PORTB1推挽输出

	EN=1;                     //使能惯量轮电机
}
void MiniBalance_PWM_Init(u16 arr,u16 psc)  //电机PWM引脚初始化
{		 					 
  MiniBalance_Motor_Init(); //初始化电机控制所需IO
	RCC->APB1ENR|=1<<1;       //TIM3时钟使能    
	RCC->APB2ENR|=1<<3;       //PORTB时钟使能 
  RCC->APB2ENR|=1<<2;       //PORTA时钟使能 
 	
	GPIOB->CRL&=0XFFFFFFF0;   //PORTB0 复用输出
	GPIOB->CRL|=0X0000000B;   //PORTB0 复用输出
	
	GPIOA->CRL&=0X0FFFFFFF;   //PORTA  7复用输出
	GPIOA->CRL|=0XB0000000;   //PORTA  7复用输出
	
	TIM3->ARR=arr;            //设定计数器自动重装值 
	TIM3->PSC=psc;            //预分频器不分频
	
	TIM3->CCMR2|=6<<12;       //CH4 PWM1模式	
	TIM3->CCMR2|=6<<4;        //CH3 PWM1模式
  TIM3->CCMR1|=7<<12;       //CH2 PWM1模式
	
  TIM3->CCMR1|=1<<11;       //CH2预装载使能
	
	TIM3->CCMR2|=1<<11;       //CH4预装载使能	 
	TIM3->CCMR2|=1<<3;        //CH3预装载使能	 

  TIM3->CCER|=1<<4;         //CH2输出使能

	TIM3->CCER|=1<<12;        //CH4输出使能	   
	TIM3->CCER|=1<<8;         //CH3输出使能	
	TIM3->BDTR |= 1<<15;
	TIM3->CR1=0x80;           //ARPE使能 
	TIM3->CR1|=0x01;          //使能定时器3 
} 
void Server_PWM_Init(u16 arr,u16 psc)  //电机PWM引脚初始化
{		 					 
	RCC->APB1ENR|=1<<0;         //TIM2时钟使能   
  RCC->APB2ENR|=1<<2;       //PORTA时钟使能 
 	
	GPIOA->CRL&=0XFFFFFF0F;   //PORTA1 复用输出
	GPIOA->CRL|=0X000000B0;   //PORTA1 复用输出
	
	TIM2->ARR=arr;            //设定计数器自动重装值 
	TIM2->PSC=psc;            //预分频器不分频
	
  TIM2->CCMR1|=7<<12;       //CH2 PWM1模式
	
  TIM2->CCMR1|=1<<11;       //CH2预装载使能

  TIM2->CCER|=1<<4;         //CH2输出使能

	TIM2->BDTR |= 1<<15;
	TIM2->CR1=0x80;           //ARPE使能 
	TIM2->CR1|=0x01;          //使能定时器2
} 

