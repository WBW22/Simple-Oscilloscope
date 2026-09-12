#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f1xx_hal.h"

struct Oscilloscope
{
   uint8_t showbit;          // 波形刷新标志位
   uint8_t keyValue;         // 按键值
   uint8_t ouptputbit;       // 输出标志位
   uint16_t outputFreq;      // 信号输出频率
   uint16_t pwmOut;          // PWM输出高电平时间(占空比)
   uint32_t sampletime;      // 电压采集时间(ADC采样周期)
   uint32_t timerPeriod;     // 定时器周期(重装载值)
   float gatherFreq;         // 示波器采集频率
   float vpp;                // 峰峰值
   float voltageValue[300];  // ADC采集电压值
};

#endif
