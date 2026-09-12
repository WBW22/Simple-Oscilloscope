#include "adc.h"
#include "main.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

// ADC采集缓冲区
static uint16_t adc_value[ADC_VALUE_NUM];

extern volatile struct Oscilloscope oscilloscope;

/**
  * @brief  ADC 时钟/GPIO 底层初始化(由 HAL_ADC_Init 调用)
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(hadc->Instance == ADC1)
  {
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    // PA3 配置为模拟输入
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

/*
*  函数功能：ADC_DMA采集初始化
*  入口参数：无
*  返回值：无
*/
void ADC_DMA_Start(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  // ADC1 初始化
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;          // 单通道,禁止扫描
  hadc1.Init.ContinuousConvMode = ENABLE;              // 连续转换模式
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;    // 软件触发
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;          // 右对齐
  hadc1.Init.NbrOfConversion = 1;                      // 转换通道数
  HAL_ADC_Init(&hadc1);

  // ADC 校准
  HAL_ADCEx_Calibration_Start(&hadc1);

  // 配置通道3,采样时间239.5周期
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  // DMA1_Channel1 初始化(ADC1 -> 内存, 半字, 循环模式)
  hdma_adc1.Instance = DMA1_Channel1;
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc1.Init.Mode = DMA_CIRCULAR;
  hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
  HAL_DMA_Init(&hdma_adc1);

  // 关联DMA到ADC
  __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

  // 使能DMA1通道1中断
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

  // 启动ADC+DMA
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, ADC_VALUE_NUM);
}

/*
*  函数功能：重新启动DMA采集(在主循环刷新后调用)
*/
void ADC_DMA_Restart(void)
{
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, ADC_VALUE_NUM);
}

/*
*  函数功能：动态修改ADC采样周期(按键调整采样率时调用)
*/
void ADC_SetSampleTime(uint32_t sampletime)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = sampletime;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

/*
*  函数功能：得到ADC值
*  入口参数：value--数据下标
*  返回值：对应下标的ADC值
*/
uint16_t Get_ADC_Value(uint16_t value)
{
    uint16_t returnValue = 0;
    if(value >= ADC_VALUE_NUM)
    {
        value = 0;
    }
    returnValue = adc_value[value];
    adc_value[value] = 0;
    return returnValue;
}

/*
*  函数功能：ADC DMA 采集完成回调(每采满300点触发一次)
*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if(hadc->Instance == ADC1)
  {
    // 停止DMA,等待主循环读取数据
    HAL_ADC_Stop_DMA(hadc);
    oscilloscope.showbit = 1;
  }
}
