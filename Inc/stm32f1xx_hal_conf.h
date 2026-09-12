/**
  ******************************************************************************
  * @file    stm32f1xx_hal_conf.h
  * @brief   HAL库配置头文件 (STM32F1)
  ******************************************************************************
  */
#ifndef __STM32F1xx_HAL_CONF_H
#define __STM32F1xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* 外部晶振频率(Hz)。野火F103开发板默认8MHz;若为12MHz请改为12000000 */
#if !defined  (HSE_VALUE)
  #define HSE_VALUE               8000000U
#endif

#if !defined  (HSI_VALUE)
  #define HSI_VALUE               8000000U
#endif

/* HSE_STARTUP_TIMEOUT */
#if !defined  (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT     100U
#endif

/* 内部低速晶振(LSI)与外部低速晶振(LSE)频率。
   本项目未使用 LSE(PC14/PC15 被当作 LED 使用),但 HAL 的 RCC 驱动
   需要这几个宏才能编译通过 */
#if !defined  (LSI_VALUE)
  #define LSI_VALUE               40000U
#endif

#if !defined  (LSE_VALUE)
  #define LSE_VALUE               32768U
#endif

#if !defined  (LSE_STARTUP_TIMEOUT)
  #define LSE_STARTUP_TIMEOUT     5000U
#endif

/* SysTick 时基(1ms) */
#if !defined  (TICK_INT_PRIORITY)
  #define TICK_INT_PRIORITY       15U
#endif

/* 使能需要的HAL模块 */
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED

/* 包含对应头文件 */
#ifdef HAL_MODULE_ENABLED
  #include "stm32f1xx_hal.h"
#endif

/* 注意:包含顺序必须与 ST 官方模板一致,不能按字母序排列。
   stm32f1xx_hal_adc.h 里用到了 DMA_HandleTypeDef,因此 DMA 必须排在 ADC 之前。 */
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32f1xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32f1xx_hal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32f1xx_hal_dma.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32f1xx_hal_cortex.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
  #include "stm32f1xx_hal_adc.h"
  #include "stm32f1xx_hal_adc_ex.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32f1xx_hal_flash.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32f1xx_hal_pwr.h"
#endif

#ifdef HAL_SPI_MODULE_ENABLED
  #include "stm32f1xx_hal_spi.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32f1xx_hal_tim.h"
  #include "stm32f1xx_hal_tim_ex.h"
#endif

/* 断言 */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_HAL_CONF_H */
