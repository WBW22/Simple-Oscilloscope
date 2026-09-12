#include "led.h"

/*
*  函数功能：初始化LED引脚 (PC14/PC15)
*  入口参数：无
*  返回值：无
*/
void Init_LED_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;       // 通用推挽输出
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // 默认不点亮
  CLose_LED(led2);
  CLose_LED(led3);
}

/*
*  函数功能：打开对应LED
*  入口参数：value--LED编号
*  返回值：无
*/
void Open_LED(uint8_t value)
{
    switch(value)
    {
        case 1:
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

/*
*  函数功能：关闭对应LED
*  入口参数：value--LED编号
*  返回值：无
*/
void CLose_LED(uint8_t value)
{
    switch(value)
    {
        case 1:
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
            break;
        default:
            break;
    }
}
