#include "key.h"
void KEY_Init(void)
{
	RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��	   	 
	GPIOB->CRH&=0X00FFFFFF; 
	GPIOB->CRH|=0X88000000;
	GPIOB->ODR|=3<<14; //PB14 15  ����	
}

u8 key_read(void) /*����ɨ����ڲ�����˫���ȴ�ʱ�䷵��  ֵ������״̬ 0���޶��� 1������ 2��˫�� */
{
	  if(KEY1==1&&KEY2==0)return 1;        //������־λδ��1
    else if(KEY1==0&&KEY2==1)return 2;
	  else if(KEY1==0&&KEY2==0)return 3;
		else return 0;
}

