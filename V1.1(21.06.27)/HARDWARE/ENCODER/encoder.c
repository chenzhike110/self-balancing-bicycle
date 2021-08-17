#include "encoder.h"
void Encoder_Init_TIM2(void)
{
	RCC->APB1ENR|=1<<0;         //TIM2ʱ��ʹ��
	RCC->APB2ENR|=1<<2;         //ʹ��PORTAʱ��
	GPIOA->CRL&=0XFFFFFF00;     //PA0 PA1
	GPIOA->CRL|=0X00000044;     //��������
	
	TIM2->DIER|=1<<0;           //����DMA/�ж�ʹ�ܼĴ�������������ж�				
	TIM2->DIER|=1<<6;           //�������ж�
	
	MY_NVIC_Init(1,3,TIM2_IRQn,1);    //�жϷ���
	
	TIM2->PSC = 0x0;                  //Ԥ��Ƶ�� ����Ƶ
	TIM2->ARR = ENCODER_TIM_PERIOD;   //�趨�������Զ���װֵ��65535�� 
	
	TIM2->CR1 &=~(3<<8);              //����ͨ�ö�ʱ�����ƼĴ��� ѡ��ʱ�ӷ�Ƶ������Ƶ
	TIM2->CR1 &=~(3<<5);              //ѡ�����ģʽ:���ض���ģʽ�����������ݷ���λ(DIR)���ϻ����¼���
	
	TIM2->CCMR1 |= 1<<0;              //���ò���/�Ƚ�ģʽ�Ĵ���CCMR CC1S='01' CC1ͨ��������Ϊ���� IC1FP1ӳ�䵽TI1����������1��
	TIM2->CCMR1 |= 1<<8;              //CC2S='01' IC2FP2ӳ�䵽TI2����������2��
	
	/*ͨ�����ò���/�Ƚ�ʹ�ܼĴ���CCER�е�CC1P��CC2Pλ������ѡ��TL1��TL2�ļ���*/
	TIM2->CCER &= ~(1<<1);	          //CC1P='0'	 IC1FP1�����࣬IC1FP1=TI1  ��������IC1��������
	TIM2->CCER &= ~(1<<5);	          //CC2P='0'	 IC2FP2�����࣬IC2FP2=TI2
	
	/*�������������źŵ�����˳�򣬲����˼�������ͷ����źţ��������������źŵ�����˳�򣬼��������ϻ����¼�����*/
	TIM2->CCMR1 |= 3<<4;              //���ò���/�Ƚ�ģʽ�Ĵ���CCMR1	 IC1F='1000'������TL1���������Ƶ�ʼ������˲����ĳ���(�й�ʽ) ���벶��1�˲���
	
	/*SMS='001'��ֻ��TL2�ı��ؼ�����SMS='010'��ֻ��TL1�ı��ؼ���*/
	TIM2->SMCR |= 3<<0;	              //���ô�ģʽ���ƼĴ���SMCR SMS='011'��������ģʽ3�����е�������������غ��½�����Ч��������ͬʱ��TL1��TL2���ؼ�����
	                                  //��������ģʽ3��������һ�źŵ������ƽ����������TL1FP1��TL2FP2�ı�������/���¼�����
	
	TIM2->CNT =0;                     //���������Ĵ�������ֵ
	TIM2->CR1 |= 0x01;                //CEN=1��ʹ�ܶ�ʱ��
}
void Encoder_Init_TIM4(void)
{
	RCC->APB1ENR|=1<<2;     //TIM4ʱ��ʹ��
	RCC->APB2ENR|=1<<3;    //ʹ��PORTbʱ��
	GPIOB->CRL&=0X00FFFFFF;//PB6 PB7
	GPIOB->CRL|=0X44000000;//��������
	TIM4->PSC = 0x0;//Ԥ��Ƶ��
	TIM4->ARR = ENCODER_TIM_PERIOD-1;//�趨�������Զ���װֵ 
  TIM4->CCMR1 |= 1<<0;          //����ģʽ��IC1FP1ӳ�䵽TI1��
  TIM4->CCMR1 |= 1<<8;          //����ģʽ��IC2FP2ӳ�䵽TI2��
  TIM4->CCER |= 0<<1;           //IC1������
  TIM4->CCER |= 0<<5;           //IC2������
	TIM4->SMCR |= 3<<0;	          //SMS='011' ���е�������������غ��½�����Ч
	TIM4->CR1 |= 0x01;    //CEN=1��ʹ�ܶ�ʱ��
}
int Read_Encoder(u8 TIMX)//��λʱ���ȡ���������� ��ڲ�������ʱ�� ����  ֵ���ٶ�ֵ
{
   int Encoder_TIM;    
   switch(TIMX)
	 {
	   case 2:  Encoder_TIM= (short)TIM2 -> CNT; TIM2 -> CNT=0;break;                //��ȡһ�κ����㣬��λ�ñջ���ʹ��
		 case 3:  Encoder_TIM= (short)TIM3 -> CNT; TIM3 -> CNT=0;break;	 //ÿ��ȡһ�κ󽫼������Ĵ������㣬���ٶȱջ���ʹ�á�
		 case 4:  Encoder_TIM= (short)TIM4 -> CNT; TIM4 -> CNT=0;break;	
		 default:  Encoder_TIM=0;
	 }
		return Encoder_TIM;
}

