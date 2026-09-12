#include "timer.h"
#include "main.h"

TIM_HandleTypeDef htim2;

// TIM2_CH3(PA2) GPIO 初始化
static void GENERAL_TIM_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_TIM2_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      // 复用推挽输出
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* ----------------   PWM信号 周期和占空比的计算 --------------- */
// ARR 自动重装载寄存器值 = period
// CLK_cnt：计数器时钟，等于 Fck_int / (psc+1) = 72M/(71+1) = 1MHz
// PWM 信号的周期 T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// 占空比P = CCR/(ARR+1)
/*
*  函数功能：初始化定时器产生PWM信号
*  入口参数：
*           period--定时器周期(ARR)
*           pulse --占空比(CCR)
*  返回值：无
*/
void Init_PWM_Output(uint32_t period, uint32_t pulse)
{
  TIM_OC_InitTypeDef sConfigOC = {0};

  // 初始化PWM GPIO
  GENERAL_TIM_GPIO_Config();

  // 定时器基本配置: 向上计数, 1MHz
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = GENERAL_TIM2_Prescaler;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = period;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_PWM_Init(&htim2);

  // 输出比较通道3配置: PWM模式2, 极性低
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = pulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
}

/*
*  函数功能：启动PWM输出(按KEY2时调用)
*/
void Start_PWM_Output(void)
{
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}

/*
*  函数功能：设置PWM占空比
*  入口参数：value--比较值(CCR)
*/
void Set_Output_PWMComparex(uint16_t value)
{
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, value);
}

/*
*  函数功能：设置周期，对应输出频率
*  入口参数：value--自动重装载值(ARR)
*/
void Set_Output_Freq(uint32_t value)
{
  __HAL_TIM_SET_AUTORELOAD(&htim2, value);
}
