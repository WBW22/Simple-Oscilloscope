#include "freq.h"
#include "main.h"

TIM_HandleTypeDef htim3;

// 定时器输入捕获没有自动定义的结构体定义
TIM_ICUserValueTypeDef TIM_ICUserValueStructure = {0, 0, 0, 0};

extern volatile struct Oscilloscope oscilloscope;

// TIM3_CH1(PA6) GPIO 初始化
static void GENERAL_TIM_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;   // 输入捕获,普通输入模式
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*
*  函数功能：初始化频率测量定时器(输入捕获)
*  入口参数：无
*  返回值：无
*/
void Init_FreqTimer(void)
{
  TIM_IC_InitTypeDef sConfigIC = {0};

  // 初始化GPIO
  GENERAL_TIM_GPIO_Config();

  // 定时器基本配置: 1MHz计数, 最大周期0xFFFF
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = GENERAL_TIM3_PSC;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = GENERAL_TIM3_PERIOD;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_IC_Init(&htim3);

  // 输入捕获通道1: 直接映射, 上升沿, 不分频, 不滤波
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1);

  // 使能中断
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);

  // 使能更新中断(用于统计溢出周期数)
  __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
  // 启动输入捕获中断
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
}

/*
*  函数功能：定时器更新(溢出)中断回调
*  说明：输入信号周期大于定时器最长周期时,定时器会不断产生更新中断,
*        这里累加溢出次数,最后加到信号周期里去
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM3)
  {
    TIM_ICUserValueStructure.Capture_Period++;
  }
}

/*
*  函数功能：输入捕获中断回调
*  说明：捕获两个上升沿(即一个完整信号周期),计算频率
*        1M的计数频率,计数1次 = 1us
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    // 第一次捕获
    if(TIM_ICUserValueStructure.Capture_StartFlag == 0)
    {
      // 计数器清零
      __HAL_TIM_SET_COUNTER(&htim3, 0);
      // 自动重装载寄存器更新标志清0
      TIM_ICUserValueStructure.Capture_Period = 0;
      // 捕获比较寄存器值的变量值清0
      TIM_ICUserValueStructure.Capture_CcrValue = 0;
      // 开始捕获标志置1
      TIM_ICUserValueStructure.Capture_StartFlag = 1;
    }
    // 第二次捕获
    else if(TIM_ICUserValueStructure.Capture_StartFlag == 1)
    {
      // 读取捕获比较寄存器值,这个值就是被测信号周期的时间
      TIM_ICUserValueStructure.Capture_CcrValue =
          HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);

      // 自动重装载寄存器更新标志*定时器周期时间 + 定时器剩余时间,
      // 1M的计数频率,即计数1次=1us,换算成频率需要乘以1000000
      oscilloscope.gatherFreq =
          (1.0f / (TIM_ICUserValueStructure.Capture_Period * (GENERAL_TIM3_PERIOD + 1) +
                   (TIM_ICUserValueStructure.Capture_CcrValue + 1))) * 1000000.0f;

      // 开始捕获标志清0
      TIM_ICUserValueStructure.Capture_StartFlag = 0;
      // 捕获完成标志置1
      TIM_ICUserValueStructure.Capture_FinishFlag = 1;
    }
  }
}
