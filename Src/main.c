#include "main.h"
#include "tft_init.h"
#include "tft.h"
#include "led.h"
#include "adc.h"
#include "timer.h"
#include "key.h"
#include "freq.h"
#include "delay.h"

volatile struct Oscilloscope oscilloscope = {0};

void SystemClock_Config(void);
void Init_Oscilloscope(volatile struct Oscilloscope *value);
void Error_Handler(void);

int main(void)
{
  uint16_t i = 0;

  // 中间值
  float median = 0;

  // 峰值
  float voltage = 0, vpp = 0;

  // 参考电压值
  float max_data = 1.0f;

  // 波形放大倍数
  float gainFactor = 0;

  // 触发点编号
  uint16_t Trigger_number = 0;

  HAL_Init();              // 初始化HAL库(内部配置SysTick为1ms)
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);  // 2位抢占 + 2位子优先级
  SystemClock_Config();    // 配置系统时钟 8MHz -> 72MHz

  // 初始化示波器结构体
  Init_Oscilloscope(&oscilloscope);

  // LED初始化
  Init_LED_GPIO();

  // 屏幕初始化
  TFT_Init();

  // 初始化ADC\DMA采集
  ADC_DMA_Start();

  // 初始化EC11编码器
  Init_EC11_GPIO();

  // 初始化按键
  Init_Key_GPIO();

  // 初始化PWM输出
  Init_PWM_Output(oscilloscope.timerPeriod - 1, oscilloscope.pwmOut);

  // 初始化频率定时器
  Init_FreqTimer();

  // 清屏
  TFT_Fill(0, 0, 160, 128, BLACK);

  // 初始化静态UI
  TFT_StaticUI();
  while(1)
  {
    // 按键扫描处理
    Key_Handle(&oscilloscope);
    // 数据采集电压值完成，开始刷新
    if(oscilloscope.showbit == 1)
    {
      oscilloscope.showbit = 0;
      oscilloscope.vpp = 0;
      // 转换电压值
      for(i = 0; i < 300; i++)
      {
        oscilloscope.voltageValue[i] = (Get_ADC_Value(i) * 3.3f) / 4096.0f;

        // 获取采集数据的峰峰电压
        vpp = (5 - (2.0f * oscilloscope.voltageValue[i]));
        if((oscilloscope.vpp) < vpp)
        {
          oscilloscope.vpp = vpp;
        }
        if(oscilloscope.vpp <= 0.3)
        {
          oscilloscope.gatherFreq = 0;
        }
      }

      // 重新启动DMA采集,同时采集下一组电压值
      ADC_DMA_Restart();

      // 寻找触发点
      for(i = 0; i < 200; i++)
      {
        // 找第一个小于参考电压的采集点
        if(oscilloscope.voltageValue[i] < max_data)
        {
          for(; i < 200; i++)
          {
            // 从小于参考电压的点找到下一个大于参考电压的采集点,以此点为触发电平相位参考
            if(oscilloscope.voltageValue[i] > max_data)
            {
              Trigger_number = i;
              break;
            }
          }
          break;
        }
      }

      // 根据峰值大小来适当放大,防止波形显示异常情况
      if(oscilloscope.vpp > 0.3)
      {
        // 计算中间电平，因为电压都是2.5V以内的(5-vpp)/2,中间电平就等于峰值减去峰值/2
        median = (2.5 - (5 - oscilloscope.vpp) / 2.0f) / 2.0f;

        // 放大倍数，需要确保放大之后的波形，将波形固定显示在:18.75~41.25行，即(41.25-18.75)/2=11.25f
        gainFactor = 11.25f / median;

        // 最低的减去中间电平得到中间值
        median = 2.5 - median;
      }

      // 每屏显示100个数据点，减少点以防止波形过密
      for(i = Trigger_number; i < Trigger_number + 100; i++)
      {
        if(oscilloscope.keyValue == KEYD)
        {
          oscilloscope.keyValue = 0;
          do
          {
            if(oscilloscope.keyValue == KEYD)
            {
              oscilloscope.keyValue = 0;
              break;
            }
          } while(1);
        }
        voltage = oscilloscope.voltageValue[i];

        if(voltage >= median)
        {
          voltage = 30 - (voltage - median) * gainFactor;
        }
        else
        {
          voltage = 30 + (median - voltage) * gainFactor;
        }
        drawCurve(80, voltage);
      }
    }
  }
}

/*
*  函数功能：初始化示波器显示结构体
*  入口参数：volatile struct Oscilloscope *value--示波器数据结构体指针
*  返回值：无
*/
void Init_Oscilloscope(volatile struct Oscilloscope *value)
{
    (*value).showbit    = 0;                          // 数据刷新标志位
    (*value).sampletime = ADC_SAMPLETIME_239CYCLES_5; // adc采样时间
    (*value).keyValue   = 0;                          // 按键值
    (*value).ouptputbit = 0;                          // 输出标志位
    (*value).gatherFreq = 0;                          // 采集频率
    (*value).outputFreq = 1000;                       // 输出频率
    (*value).pwmOut     = 500;                        // PWM高电平输出(PWM占空比)
    (*value).timerPeriod= 1000;                       // PWM输出定时器周期
    (*value).vpp        = 0.0f;                       // 峰峰值
}

/**
  * @brief  系统时钟配置: HSE(8MHz) -> PLLx9 -> 72MHz
  *         APB1 = 36MHz, APB2 = 72MHz
  * @note   若开发板使用12MHz晶振,请将 PLLMUL 改为 RCC_PLL_MUL6,
  *         并把 HSE_VALUE(在 stm32f1xx_hal_conf.h 中)改为 12000000。
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while(1)
  {
  }
}
