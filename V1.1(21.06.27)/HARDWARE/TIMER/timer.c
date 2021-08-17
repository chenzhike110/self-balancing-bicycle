#include "timer.h"
#include "led.h"
#include "motor.h"

int T=0;
u8 server_pwm=125;

void Timer1_Init(u16 arr,u16 psc)  //入口参数：arr：自动重装值  psc：时钟预分频数 
{  
	RCC->APB2ENR|=1<<11;//TIM1时钟使能    
 	TIM1->ARR=arr;      //设定计数器自动重装值   
	TIM1->PSC=psc;      //预分频器7200,得到10Khz的计数时钟
	TIM1->DIER|=1<<0;   //允许更新中断				
	TIM1->DIER|=1<<6;   //允许触发中断	   
	TIM1->CR1|=0x01;    //使能定时器
	MY_NVIC_Init(2,2,TIM1_UP_IRQn,2);
}  

void Timer2_Init(u16 arr,u16 psc)  //入口参数：arr：自动重装值  psc：时钟预分频数 
{  
	RCC->APB1ENR|=1<<0; //TIM2时钟使能
 	TIM2->ARR=arr;      //设定计数器自动重装值   
	TIM2->PSC=psc;      //预分频器7200,得到10Khz的计数时钟
	TIM2->DIER|=1<<0;   //允许更新中断				
	//TIM2->DIER|=1<<6;   //允许触发中断	   
	TIM2->CR1|=0x01;    //使能定时器
	MY_NVIC_Init(2,2,TIM2_IRQn,2);
}  

void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//溢出中断
	{	
		//TIM2->SR&=~(1<<0);//清除中断标志位 
    T++;
    if(T>server_pwm)SERVER=0;
    else 	SERVER=1;
    if(T>2000)T=0;		
	}				   
	TIM2->SR&=~(1<<0);//清除中断标志位 	    
}



 

