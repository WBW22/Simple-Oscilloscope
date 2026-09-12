#ifndef __FREQ_H
#define __FREQ_H

#include "main.h"

// 通用定时器TIM3, 输入捕获通道1(PA6)
#define GENERAL_TIM3            TIM3
#define GENERAL_TIM3_PERIOD     0xFFFF
#define GENERAL_TIM3_PSC        (72-1)   // 72MHz/(71+1) = 1MHz, 即1us计数

// 定时器输入捕获用户自定义结构体定义
typedef struct
{
    uint8_t   Capture_FinishFlag;   // 捕获完成标志位
    uint8_t   Capture_StartFlag;    // 捕获开始标志位
    uint16_t  Capture_CcrValue;     // 捕获寄存器数值
    uint16_t  Capture_Period;       // 自动重装载寄存器更新标志
} TIM_ICUserValueTypeDef;

extern TIM_HandleTypeDef htim3;
extern TIM_ICUserValueTypeDef TIM_ICUserValueStructure;

void Init_FreqTimer(void);   // 初始化输入捕获定时器

#endif
