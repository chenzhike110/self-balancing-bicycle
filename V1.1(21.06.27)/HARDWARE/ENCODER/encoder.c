#include "encoder.h"
void Encoder_Init_TIM2(void)
{
	RCC->APB1ENR|=1<<0;         //TIM2时钟使能
	RCC->APB2ENR|=1<<2;         //使能PORTA时钟
	GPIOA->CRL&=0XFFFFFF00;     //PA0 PA1
	GPIOA->CRL|=0X00000044;     //浮空输入
	
	TIM2->DIER|=1<<0;           //配置DMA/中断使能寄存器，允许更新中断				
	TIM2->DIER|=1<<6;           //允许触发中断
	
	MY_NVIC_Init(1,3,TIM2_IRQn,1);    //中断分组
	
	TIM2->PSC = 0x0;                  //预分频器 不分频
	TIM2->ARR = ENCODER_TIM_PERIOD;   //设定计数器自动重装值（65535） 
	
	TIM2->CR1 &=~(3<<8);              //配置通用定时器控制寄存器 选择时钟分频：不分频
	TIM2->CR1 &=~(3<<5);              //选择计数模式:边沿对齐模式，计数器依据方向位(DIR)向上或向下计数
	
	TIM2->CCMR1 |= 1<<0;              //配置捕获/比较模式寄存器CCMR CC1S='01' CC1通道被配置为输入 IC1FP1映射到TI1（输入引脚1）
	TIM2->CCMR1 |= 1<<8;              //CC2S='01' IC2FP2映射到TI2（输入引脚2）
	
	/*通过设置捕获/比较使能寄存器CCER中的CC1P和CC2P位，可以选择TL1和TL2的极性*/
	TIM2->CCER &= ~(1<<1);	          //CC1P='0'	 IC1FP1不反相，IC1FP1=TI1  捕获发生在IC1的上升沿
	TIM2->CCER &= ~(1<<5);	          //CC2P='0'	 IC2FP2不反相，IC2FP2=TI2
	
	/*根据两个输入信号的跳变顺序，产生了计数脉冲和方向信号，依据两个输入信号的跳变顺序，计数器向上或向下计数。*/
	TIM2->CCMR1 |= 3<<4;              //配置捕获/比较模式寄存器CCMR1	 IC1F='1000'定义了TL1的输入采样频率及数字滤波器的长度(有公式) 输入捕获1滤波器
	
	/*SMS='001'，只在TL2的边沿计数，SMS='010'，只在TL1的边沿计数*/
	TIM2->SMCR |= 3<<0;	              //配置从模式控制寄存器SMCR SMS='011'（编码器模式3）所有的输入均在上升沿和下降沿有效（计数器同时在TL1和TL2边沿计数）
	                                  //（编码器模式3：根据另一信号的输入电平，计数器在TL1FP1和TL2FP2的边沿向上/向下计数）
	
	TIM2->CNT =0;                     //给计数器寄存器附初值
	TIM2->CR1 |= 0x01;                //CEN=1，使能定时器
}
void Encoder_Init_TIM4(void)
{
	RCC->APB1ENR|=1<<2;     //TIM4时钟使能
	RCC->APB2ENR|=1<<3;    //使能PORTb时钟
	GPIOB->CRL&=0X00FFFFFF;//PB6 PB7
	GPIOB->CRL|=0X44000000;//浮空输入
	TIM4->PSC = 0x0;//预分频器
	TIM4->ARR = ENCODER_TIM_PERIOD-1;//设定计数器自动重装值 
  TIM4->CCMR1 |= 1<<0;          //输入模式，IC1FP1映射到TI1上
  TIM4->CCMR1 |= 1<<8;          //输入模式，IC2FP2映射到TI2上
  TIM4->CCER |= 0<<1;           //IC1不反向
  TIM4->CCER |= 0<<5;           //IC2不反向
	TIM4->SMCR |= 3<<0;	          //SMS='011' 所有的输入均在上升沿和下降沿有效
	TIM4->CR1 |= 0x01;    //CEN=1，使能定时器
}
int Read_Encoder(u8 TIMX)//单位时间读取编码器计数 入口参数：定时器 返回  值：速度值
{
   int Encoder_TIM;    
   switch(TIMX)
	 {
	   case 2:  Encoder_TIM= (short)TIM2 -> CNT; TIM2 -> CNT=0;break;                //读取一次后不清零，在位置闭环中使用
		 case 3:  Encoder_TIM= (short)TIM3 -> CNT; TIM3 -> CNT=0;break;	 //每读取一次后将计数器寄存器清零，在速度闭环中使用。
		 case 4:  Encoder_TIM= (short)TIM4 -> CNT; TIM4 -> CNT=0;break;	
		 default:  Encoder_TIM=0;
	 }
		return Encoder_TIM;
}

