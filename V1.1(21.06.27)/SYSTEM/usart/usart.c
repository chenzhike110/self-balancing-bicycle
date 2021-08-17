#include "usart.h"	 
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
//int fputc(int ch, FILE *f)
//{      
	
//	while((USART1->SR&0X40)==0);//Flag_Show!=0  使用串口1   
//	USART1->DR = (u8) ch;      
//	return ch;
//}
#endif 
//end
void usart1_send(u8 data) /*usart1发送一个字节****/
{
	USART1->DR = data;
	while((USART1->SR&0x40)==0);	
}
void usart2_send(u8 data) /*usart1发送一个字节****/
{
	USART2->DR = data;
	while((USART2->SR&0x40)==0);	
}
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F;//IO状态设置
	GPIOA->CRH|=0X000008B0;//IO状态设置
		  
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
	
//使能接收中断
	USART1->CR1|=1<<8;    //PE中断使能
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(1,1,USART1_IRQn,2);//组2，最低优先级 
}
void uart2_init(u32 pclk2,u32 bound)
{    
    float temp;
    u16 mantissa;
    u16 fraction;      
    temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
    mantissa=temp;               //得到整数部分
    fraction=(temp-mantissa)*16; //得到小数部分    
    mantissa<<=4;
    mantissa+=fraction;  
    RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  //RCC->APB2ENR 见中文参考手册 p70  port A - G的时钟使能 分别在第2-8位，USART1在14位
    RCC->APB1ENR|=1<<17;  //使能串口时钟 //RCC->APB1ENR 见中文参考手册 p72  USART2-5的时钟使能 分别在第17-20位
    
    /*CRL & CRH
    中文参考手册p113
     每个io口4位，
    低两位是mode：
    00输入，01输出10MHz，10输出2MHz，11输出50MHz，
    高两位是CNF：00模拟输入，01浮空输入 。10上拉下拉输入，11保留  ，
    00：通用推挽输出模式
01：通用开漏输出模式
10：复用功能推挽输出模式
11：复用功能开漏输出模式

串口配置时，RX为1000，TX为1011
    */
    GPIOA->CRL&=0XFFFF00FF;  
    GPIOA->CRL|=0X00008B00;//IO状态设置
    
    /*ODR p115
    似乎可以不用配置
    端口输出数据（可用来配置上拉下拉）
    串口配置时，RX为上拉输入，对应位配置为1
    */
    GPIOA->ODR|=1<<3;     
    /*
    p67
    APB1RSTR 的 第17-20分别是串口2-5的复位
    */
    RCC->APB1RSTR|=1<<17;   //复位串口2
    RCC->APB1RSTR&=~(1<<17);//停止复位         
    //波特率设置
    USART2->BRR=mantissa; // 波特率设置   
    USART2->CR1|=0X200C;  //1位停止,无校验位.
    //使能接收中断
    USART2->CR1|=1<<8;    //PE中断使能
    USART2->CR1|=1<<5;    //接收缓冲区非空中断使能         
    MY_NVIC_Init(1,1,USART2_IRQn,2);//组2，最低优先级 
}
u8 USART_RX_BUF[20];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记
s8 remot_moto=0;
s8 remot_angle=0;
void USART1_IRQHandler(void)
{
	
	u8 res=0;	
	u8 i = 0;
	if(USART1->SR&(1<<5))//接收到数据
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
 		else if((USART_RX_STA&0x8000)==0)//接收未完成
 		{
			USART_RX_BUF[USART_RX_STA&0X3FFF]=res;		
 			USART_RX_STA++;
		}
	/*
			
 			if(USART_RX_STA&0x4000)//接收到了0x0d
 			{
				usart1_send(USART_RX_STA&0x00ff);
				usart1_send(USART_RX_STA&0xff00);
				usart1_send(res);
				usart1_send(USART_RX_BUF[0]);
				usart1_send(USART_RX_BUF[1]);
				USART_RX_STA=0;
			
				
				
				
 				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
 				else  //接收完成了
        {
         USART_RX_STA|=0x8000;
				 //数据就在USART_RX_BUF中，用户可以在这里为所欲为
				 remot_angle=USART_RX_BUF[0];
				 remot_moto=USART_RX_BUF[1];
				 //usart1_send(USART_RX_BUF[0]);
         USART_RX_STA=0;
        }	
 			}
 			else //还没收到0X0d
 			{	
 				if(res==0x0d)USART_RX_STA|=0x4000;
 				else
				
 				{
 					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					
 					USART_RX_STA++;
					//usart1_send(USART_RX_STA&0x00ff);
					//usart1_send(USART_RX_STA&0xff00);
 					if(USART_RX_STA>30)USART_RX_STA=0;//接收数据错误,重新开始接收	  
 				}	
 			}
 		} 
*/
	}
} 

char USART2_RX_BUF[20];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART2_RX_STA=0;       //接收状态标记
//s8 remot_moto=0;
//s8 remot_angle=0;
void USART2_IRQHandler(void)   //串口接收中断
{
	u8 res;	
	if(USART2->SR&(1<<5))//接收到数据
	{	 
		res=USART2->DR; 
 		if((USART2_RX_STA&0x8000)==0)//接收未完成
 		{
 			if(USART2_RX_STA&0x4000)//接收到了0x0d
 			{
 				if(res!=0xa5)USART2_RX_STA=0;//接收错误,重新开始
 				else  //接收完成了
        {
         USART2_RX_STA|=0x8000;//数据就在USART_RX_BUF中，用户可以在这里为所欲为
				 
				 remot_angle=USART2_RX_BUF[0];
				 remot_moto=USART2_RX_BUF[1];
         USART2_RX_STA=0;
				 usart2_send(USART2_RX_BUF[0]);
        }	
 			}
 			else //还没收到0X0d
 			{	
 				if(res==0x5a)USART2_RX_STA|=0x4000;
 				else
 				{
 					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=res;
 					USART2_RX_STA++;
 					if(USART2_RX_STA>30)USART2_RX_STA=0;//接收数据错误,重新开始接收	  
 				}		 
 			}
 		}  
		
		
	}
} 

