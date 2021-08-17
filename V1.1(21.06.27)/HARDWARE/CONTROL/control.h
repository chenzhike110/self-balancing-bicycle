#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
#define PI 3.14159265
int TIM1_UP_IRQHandler(void);
int balance_x(float angle,float gyro);
int velocity_x(int Encoder);
int balance_y(float Angle,float Gyro);//直立PD控制 入口参数：角度、角速度
int velocity_y(int encoder);
void Set_Pwm(int motox,int motoy);
void Xianfu_Pwm(void);
int myabs(int a);
void Get_Angle(void);


extern float Center_Gravity;          //动态机械中值
extern float Center_Gra_Sart;         //开机设定机械中值，可以将小车PWM关闭然后观察小车的x轴平衡时屏幕显示的角度（即为机械中值），每个小车由于电池和惯量轮安装位置不同会有些许差异
#endif
