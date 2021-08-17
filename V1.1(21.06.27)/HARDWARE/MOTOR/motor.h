#ifndef __MOTOR_H
#define __MOTOR_H
#include <sys.h>	 
#define PWM    TIM3->CCR4   //Ԥ��

#define PWMX   TIM3->CCR2   //�����ֵ��PWM����
#define DIR   PBout(9)      //�����ֵ�������������
#define EN    PBout(8)      //�����ֵ��ʹ������

#define PWMY   TIM3->CCR3   //�����ֵ��PWM��������
#define DIRA   PAout(5)     //�����ֵ����������1
#define DIRB   PAout(6)     //�����ֵ����������2
#define SERVER PAout(1)
#define PWM_SERVER TIM2->CCR2    //���PWM����

void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MiniBalance_Motor_Init(void);
void Server_PWM_Init(u16 arr,u16 psc);  //���PWM���ų�ʼ��

#endif
