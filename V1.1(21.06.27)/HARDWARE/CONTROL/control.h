#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
#define PI 3.14159265
int TIM1_UP_IRQHandler(void);
int balance_x(float angle,float gyro);
int velocity_x(int Encoder);
int balance_y(float Angle,float Gyro);//ֱ��PD���� ��ڲ������Ƕȡ����ٶ�
int velocity_y(int encoder);
void Set_Pwm(int motox,int motoy);
void Xianfu_Pwm(void);
int myabs(int a);
void Get_Angle(void);


extern float Center_Gravity;          //��̬��е��ֵ
extern float Center_Gra_Sart;         //�����趨��е��ֵ�����Խ�С��PWM�ر�Ȼ��۲�С����x��ƽ��ʱ��Ļ��ʾ�ĽǶȣ���Ϊ��е��ֵ����ÿ��С�����ڵ�غ͹����ְ�װλ�ò�ͬ����Щ�����
#endif
