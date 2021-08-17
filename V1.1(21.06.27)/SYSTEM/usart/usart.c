#include "usart.h"	 
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
//int fputc(int ch, FILE *f)
//{      
	
//	while((USART1->SR&0X40)==0);//Flag_Show!=0  ʹ�ô���1   
//	USART1->DR = (u8) ch;      
//	return ch;
//}
#endif 
//end
void usart1_send(u8 data) /*usart1����һ���ֽ�****/
{
	USART1->DR = data;
	while((USART1->SR&0x40)==0);	
}
void usart2_send(u8 data) /*usart1����һ���ֽ�****/
{
	USART2->DR = data;
	while((USART2->SR&0x40)==0);	
}
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F;//IO״̬����
	GPIOA->CRH|=0X000008B0;//IO״̬����
		  
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
	
//ʹ�ܽ����ж�
	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(1,1,USART1_IRQn,2);//��2��������ȼ� 
}
void uart2_init(u32 pclk2,u32 bound)
{    
    float temp;
    u16 mantissa;
    u16 fraction;      
    temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
    mantissa=temp;               //�õ���������
    fraction=(temp-mantissa)*16; //�õ�С������    
    mantissa<<=4;
    mantissa+=fraction;  
    RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  //RCC->APB2ENR �����Ĳο��ֲ� p70  port A - G��ʱ��ʹ�� �ֱ��ڵ�2-8λ��USART1��14λ
    RCC->APB1ENR|=1<<17;  //ʹ�ܴ���ʱ�� //RCC->APB1ENR �����Ĳο��ֲ� p72  USART2-5��ʱ��ʹ�� �ֱ��ڵ�17-20λ
    
    /*CRL & CRH
    ���Ĳο��ֲ�p113
     ÿ��io��4λ��
    ����λ��mode��
    00���룬01���10MHz��10���2MHz��11���50MHz��
    ����λ��CNF��00ģ�����룬01�������� ��10�����������룬11����  ��
    00��ͨ���������ģʽ
01��ͨ�ÿ�©���ģʽ
10�����ù����������ģʽ
11�����ù��ܿ�©���ģʽ

��������ʱ��RXΪ1000��TXΪ1011
    */
    GPIOA->CRL&=0XFFFF00FF;  
    GPIOA->CRL|=0X00008B00;//IO״̬����
    
    /*ODR p115
    �ƺ����Բ�������
    �˿�������ݣ���������������������
    ��������ʱ��RXΪ�������룬��Ӧλ����Ϊ1
    */
    GPIOA->ODR|=1<<3;     
    /*
    p67
    APB1RSTR �� ��17-20�ֱ��Ǵ���2-5�ĸ�λ
    */
    RCC->APB1RSTR|=1<<17;   //��λ����2
    RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ         
    //����������
    USART2->BRR=mantissa; // ����������   
    USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.
    //ʹ�ܽ����ж�
    USART2->CR1|=1<<8;    //PE�ж�ʹ��
    USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��         
    MY_NVIC_Init(1,1,USART2_IRQn,2);//��2��������ȼ� 
}
u8 USART_RX_BUF[20];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���
s8 remot_moto=0;
s8 remot_angle=0;
void USART1_IRQHandler(void)
{
	
	u8 res=0;	
	u8 i = 0;
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		//usart1_send(0);
 		res=USART1->DR; 
		if (res == 0x0a) USART_RX_STA = 0;
		else if (res == 0x0d) {
			USART_RX_STA|=0x8000;
			usart1_send(USART_RX_BUF[0]);
			usart1_send(USART_RX_BUF[1]);
			remot_angle=USART_RX_BUF[0];
		  remot_moto=USART_RX_BUF[1];
			
			for(i=0;i<20;++i){
				USART_RX_BUF[i] = 0;
			}
		}
 		else if((USART_RX_STA&0x8000)==0)//����δ���
 		{
			USART_RX_BUF[USART_RX_STA&0X3FFF]=res;		
 			USART_RX_STA++;
		}
	/*
			
 			if(USART_RX_STA&0x4000)//���յ���0x0d
 			{
				usart1_send(USART_RX_STA&0x00ff);
				usart1_send(USART_RX_STA&0xff00);
				usart1_send(res);
				usart1_send(USART_RX_BUF[0]);
				usart1_send(USART_RX_BUF[1]);
				USART_RX_STA=0;
			
				
				
				
 				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
 				else  //���������
        {
         USART_RX_STA|=0x8000;
				 //���ݾ���USART_RX_BUF�У��û�����������Ϊ����Ϊ
				 remot_angle=USART_RX_BUF[0];
				 remot_moto=USART_RX_BUF[1];
				 //usart1_send(USART_RX_BUF[0]);
         USART_RX_STA=0;
        }	
 			}
 			else //��û�յ�0X0d
 			{	
 				if(res==0x0d)USART_RX_STA|=0x4000;
 				else
				
 				{
 					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					
 					USART_RX_STA++;
					//usart1_send(USART_RX_STA&0x00ff);
					//usart1_send(USART_RX_STA&0xff00);
 					if(USART_RX_STA>30)USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
 				}	
 			}
 		} 
*/
	}
} 

char USART2_RX_BUF[20];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART2_RX_STA=0;       //����״̬���
//s8 remot_moto=0;
//s8 remot_angle=0;
void USART2_IRQHandler(void)   //���ڽ����ж�
{
	u8 res;	
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		res=USART2->DR; 
 		if((USART2_RX_STA&0x8000)==0)//����δ���
 		{
 			if(USART2_RX_STA&0x4000)//���յ���0x0d
 			{
 				if(res!=0xa5)USART2_RX_STA=0;//���մ���,���¿�ʼ
 				else  //���������
        {
         USART2_RX_STA|=0x8000;//���ݾ���USART_RX_BUF�У��û�����������Ϊ����Ϊ
				 
				 remot_angle=USART2_RX_BUF[0];
				 remot_moto=USART2_RX_BUF[1];
         USART2_RX_STA=0;
				 usart2_send(USART2_RX_BUF[0]);
        }	
 			}
 			else //��û�յ�0X0d
 			{	
 				if(res==0x5a)USART2_RX_STA|=0x4000;
 				else
 				{
 					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=res;
 					USART2_RX_STA++;
 					if(USART2_RX_STA>30)USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
 				}		 
 			}
 		}  
		
		
	}
} 

