/**
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines (HAL版本)
  */
#include "main.h"
#include "stm32f1xx_it.h"
#include "adc.h"
#include "dma.h"
#include "freq.h"

/******************************************************************************/
/*           Cortex-M3 Processor Exceptions Handlers                          */
/******************************************************************************/

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/******************************************************************************/

/**
  * @brief  ADC采集DMA中断 (DMA1通道1)
  */
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_adc1);
}

/**
  * @brief  SPI1 TX DMA中断 (DMA1通道3)
  */
void DMA1_Channel3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
}

/**
  * @brief  TIM3中断(频率测量输入捕获)
  */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim3);
}

/**
  * @brief  EXTI4中断(编码器A相 PB4)
  */
void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

/**
  * @brief  EXTI9_5中断(编码器按键 PB9)
  */
void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

/**
  * @brief  EXTI15_10中断(KEY1/2/3 PB13/14/15)
  */
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}
