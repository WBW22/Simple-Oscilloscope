#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void DMA1_Channel1_IRQHandler(void);   // ADC采集DMA
void DMA1_Channel3_IRQHandler(void);   // SPI1 TX DMA
void TIM3_IRQHandler(void);            // 频率测量输入捕获
void EXTI4_IRQHandler(void);           // 编码器A相 PB4
void EXTI9_5_IRQHandler(void);         // 编码器按键 PB9
void EXTI15_10_IRQHandler(void);       // KEY1/2/3 PB13/14/15

#endif
