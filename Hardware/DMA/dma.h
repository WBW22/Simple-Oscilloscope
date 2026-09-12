#ifndef __DMA_H
#define __DMA_H

#include "main.h"

// 注意: 本模块是原项目中的 SPI+DMA 快速刷屏实验代码,
// 主程序 main.c 实际并未调用这里的函数(实际绘图走 tft.c 的软件方式),
// 这里仅按 HAL 库等价改写,保留原接口。

extern DMA_HandleTypeDef hdma_spi1_tx;

void DMA_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);

void DMA_ShowBMP(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t num,volatile uint8_t* showData);

void Init_ShowData(volatile uint8_t *showData);

void DMAdrawCurve(uint8_t yOffset,short int rawValue,volatile uint8_t *showData);

#endif
