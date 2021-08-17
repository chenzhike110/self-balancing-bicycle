#include "key.h"
void KEY_Init(void)
{
	RCC->APB2ENR|=1<<3;    //使能PORTB时钟	   	 
	GPIOB->CRH&=0X00FFFFFF; 
	GPIOB->CRH|=0X88000000;
	GPIOB->ODR|=3<<14; //PB14 15  上拉	
}

u8 key_read(void) /*按键扫描入口参数：双击等待时间返回  值：按键状态 0：无动作 1：单击 2：双击 */
{
	  if(KEY1==1&&KEY2==0)return 1;        //长按标志位未置1
    else if(KEY1==0&&KEY2==1)return 2;
	  else if(KEY1==0&&KEY2==0)return 3;
		else return 0;
}

