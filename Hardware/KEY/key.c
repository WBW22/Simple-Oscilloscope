#include "key.h"
#include "timer.h"
#include "tft.h"
#include "delay.h"
#include "adc.h"

extern volatile struct Oscilloscope oscilloscope;

/*
*  函数功能：初始化按键GPIO (KEY1/2/3 下降沿中断)
*  入口参数：无
*  返回值：无
*/
void Init_Key_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  // 配置NVIC中断
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  // KEY1: PB13 下降沿触发
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // KEY2: PB15 下降沿触发
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // KEY3: PB14 下降沿触发
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*
*  函数功能：初始化EC11编码器GPIO
*  入口参数：无
*  返回值：无
*/
void Init_EC11_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  // 释放PB3/PB4(禁用JTAG,保留SWD)
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  // 配置NVIC中断
  HAL_NVIC_SetPriority(EXTI4_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  // KEYA: PB4 (编码器A相) 上升沿触发
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // KEYB: PB3 (编码器B相) 普通输入
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // KEYD: PB9 (编码器按键) 上升沿触发
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*
*  函数功能：外部中断回调(所有GPIO EXTI共用一个回调)
*  入口参数：GPIO_Pin--触发中断的引脚
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint8_t clockwiseNum = 0;
  static uint8_t anticlockwiseNum = 0;

  // 编码器A相(PB4)
  if(GPIO_Pin == GPIO_PIN_4)
  {
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET)   // 顺时针旋转方向
    {
      delay_ms(5);
      anticlockwiseNum = 0;
      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET)
      {
        clockwiseNum++;
        if(clockwiseNum >= 1)
        {
          clockwiseNum = 0;
          oscilloscope.keyValue = KEYB;
        }
      }
    }
    else                                                        // 逆时针旋转方向
    {
      delay_ms(5);
      clockwiseNum = 0;
      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_SET)
      {
        anticlockwiseNum++;
        if(anticlockwiseNum >= 1)
        {
          anticlockwiseNum = 0;
          oscilloscope.keyValue = KEYA;
        }
      }
    }
  }
  // 编码器按键(PB9)
  else if(GPIO_Pin == GPIO_PIN_9)
  {
    delay_ms(5);
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_RESET)
    {
      oscilloscope.keyValue = KEYD;
    }
  }
  // KEY1/2/3 (PB13/14/15)
  else if(GPIO_Pin & (GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15))
  {
    if(GPIO_Pin == GPIO_PIN_13)
    {
      delay_ms(5);
      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET)
        oscilloscope.keyValue = KEY1;
    }
    if(GPIO_Pin == GPIO_PIN_15)
    {
      delay_ms(5);
      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET)
        oscilloscope.keyValue = KEY2;
    }
    if(GPIO_Pin == GPIO_PIN_14)
    {
      delay_ms(5);
      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET)
        oscilloscope.keyValue = KEY3;
    }
  }
}

/*
*  函数功能：按键处理函数
*  入口参数：Oscilloscope *value 指向Oscilloscope结构体,value值为按键的当前状态
*  返回值：无
*/
void Key_Handle(volatile struct Oscilloscope *value)
{
  float tempValue = 0;
  switch((*value).keyValue)
  {
    // KEY1: 增加占空比
    case KEY1:
      // 每次按下占空比5%增加
      (*value).pwmOut = ((*value).timerPeriod * 0.05f) + (*value).pwmOut;
      // 占空比大于周期
      if((*value).pwmOut > (*value).timerPeriod)
      {
        // 占空比为0
        (*value).pwmOut = 0;
      }
      // 设置PWM占空比输出
      Set_Output_PWMComparex((*value).pwmOut);
      break;

    // KEY2: 开启/关闭PWM输出
    case KEY2:
      // PWM输出标志位为0
      if((*value).ouptputbit == 0)
      {
        // 输出标志位置1
        (*value).ouptputbit = 1;
        // 启动PWM
        Start_PWM_Output();
        // 设置PWM占空比
        Set_Output_PWMComparex((*value).pwmOut);
      }
      // 标志位不为0表示PWM输出已打开
      else
      {
        // PWM功能标志位置0
        (*value).ouptputbit = 0;
        // 关闭PWM(占空比设为0)
        Set_Output_PWMComparex(0);
        delay_ms(10);
      }
      break;

    // KEY3: 改变输出频率(周期减半)
    case KEY3:
      // 计算当前占空比
      tempValue = (*value).pwmOut / ((*value).timerPeriod + 0.0f);
      // 计算当前输出频率(周期减半)
      (*value).timerPeriod = (*value).timerPeriod / 2.0f;
      if((*value).timerPeriod < 250)
      {
        (*value).timerPeriod = 1000;
      }
      (*value).outputFreq = 1000000 / (*value).timerPeriod;
      (*value).pwmOut = (*value).timerPeriod * tempValue;
      Set_Output_PWMComparex((*value).pwmOut);
      Set_Output_Freq((*value).timerPeriod - 1);
      tempValue = 0;
      break;

    // KEYA: 顺时针旋转(采样时间增大)
    case KEYA:
      switch((*value).sampletime)
      {
        case ADC_SAMPLETIME_28CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_41CYCLES_5;
          break;
        case ADC_SAMPLETIME_41CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_55CYCLES_5;
          break;
        case ADC_SAMPLETIME_55CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_71CYCLES_5;
          break;
        case ADC_SAMPLETIME_71CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_239CYCLES_5;
          break;
        case ADC_SAMPLETIME_239CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_239CYCLES_5;
          break;
        default:
          (*value).sampletime = ADC_SAMPLETIME_239CYCLES_5;
          break;
      }
      ADC_SetSampleTime((*value).sampletime);
      break;

    // KEYB: 逆时针旋转(采样时间减小)
    case KEYB:
      switch((*value).sampletime)
      {
        case ADC_SAMPLETIME_239CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_71CYCLES_5;
          break;
        case ADC_SAMPLETIME_71CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_55CYCLES_5;
          break;
        case ADC_SAMPLETIME_55CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_41CYCLES_5;
          break;
        case ADC_SAMPLETIME_41CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_28CYCLES_5;
          break;
        case ADC_SAMPLETIME_28CYCLES_5:
          (*value).sampletime = ADC_SAMPLETIME_28CYCLES_5;
          break;
        default:
          (*value).sampletime = ADC_SAMPLETIME_28CYCLES_5;
          break;
      }
      ADC_SetSampleTime((*value).sampletime);
      break;

    case KEYD:
      break;

    default:
      break;
  }
  (*value).keyValue = 0;
  // 刷新显示UI
  TFT_ShowUI(value);
}
