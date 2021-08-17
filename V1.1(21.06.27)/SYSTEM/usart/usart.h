#ifndef __USART_H
#define __USART_H

#include "sys.h"
#include "stdio.h"	 
void usart1_send(u8 data);
void usart2_send(u8 data); /*usart1发送一个字节****/
void uart_init(u32 pclk2,u32 bound);
void uart2_init(u32 pclk2,u32 bound);
#endif	   
















