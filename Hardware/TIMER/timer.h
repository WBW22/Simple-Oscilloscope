#ifndef __TIMER_H
#define __TIMER_H

#include "main.h"

#define PI 3.1415926

// TIM2 输出比较通道3 (PA2) 输出PWM
#define GENERAL_TIM2            TIM2
#define GENERAL_TIM2_Period     9
#define GENERAL_TIM2_Prescaler  71     // 72MHz/(71+1) = 1MHz

extern TIM_HandleTypeDef htim2;

void Init_PWM_Output(uint32_t period, uint32_t pulse); // 初始化PWM(不启动)
void Start_PWM_Output(void);                           // 启动PWM输出
void Set_Output_PWMComparex(uint16_t value);           // 设置占空比
void Set_Output_Freq(uint32_t value);                  // 设置周期(对应频率)

#endif
