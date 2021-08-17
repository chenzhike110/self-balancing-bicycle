#ifndef __MOTOR_H
#define __MOTOR_H
#include <sys.h>	 
#define PWM    TIM3->CCR4   //预留

#define PWMX   TIM3->CCR2   //惯量轮电机PWM引脚
#define DIR   PBout(9)      //惯量轮电机方向控制引脚
#define EN    PBout(8)      //惯量轮电机使能引脚

#define PWMY   TIM3->CCR3   //驱动轮电机PWM控制引脚
#define DIRA   PAout(5)     //驱动轮电机方向引脚1
#define DIRB   PAout(6)     //驱动轮电机方向引脚2
#define SERVER PAout(1)
#define PWM_SERVER TIM2->CCR2    //舵机PWM引脚

void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MiniBalance_Motor_Init(void);
void Server_PWM_Init(u16 arr,u16 psc);  //电机PWM引脚初始化

#endif
