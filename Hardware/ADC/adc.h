#ifndef __ADC_H
#define __ADC_H
#include "main.h"

// 注意：此处ADC采集的IO没有复用，方便采集电压对波形的影响
/********************ADC1采集通道(PA3 = 通道3)**************************/
#define ADC_VALUE_NUM   300     // DMA采集点数

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

void ADC_DMA_Start(void);                       // ADC+DMA初始化并启动采集
void ADC_DMA_Restart(void);                     // 重新启动DMA采集
void ADC_SetSampleTime(uint32_t sampletime);    // 动态修改ADC采样周期
uint16_t Get_ADC_Value(uint16_t value);         // 读取ADC值

#endif
