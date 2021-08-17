#include "usart.h"
#include <stdarg.h>
 
 
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
 
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); 
 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
	  
 
	USART_InitStructure.USART_BaudRate = 115200;	
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	
	USART_InitStructure.USART_Parity = USART_Parity_No ;  
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);  
	USART_Cmd(USART1, ENABLE);
}
 
 
 void UART1SendByte(unsigned char SendData)
{	   
        USART_SendData(USART1,SendData);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	    
}  
 
 
unsigned char UART1GetByte(unsigned char* GetData)
{   	   
        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
        {  return 0;
		}
        *GetData = USART_ReceiveData(USART1); 
        return 1;
}
 
void UART1Test(void)
{
       unsigned char i = 0;
 
       while(1)
       {    
		 while(UART1GetByte(&i))
        {
         USART_SendData(USART1,i);
        }      
       }     
}
