#ifndef __TFT_INIT_H
#define __TFT_INIT_H
#include "main.h"

#include <stdio.h>

#define USE_HORIZONTAL 2  // 屏幕方向设置 0/1为竖屏 2/3为横屏

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 128
#endif

/* SPI 接口定义 (SPI1) */
#define      TFT_SPIx                        SPI1

// CS(NSS)引脚 片选选择通过GPIO控制
#define      TFT_SPI_CS_PORT                 GPIOB
#define      TFT_SPI_CS_PIN                  GPIO_PIN_7

// SCK引脚
#define      TFT_SPI_SCK_PORT                GPIOA
#define      TFT_SPI_SCK_PIN                 GPIO_PIN_5

// MISO引脚(示波器只使用半双工)
#define      TFT_SPI_MISO_PORT               GPIOA
#define      TFT_SPI_MISO_PIN                GPIO_PIN_6

// MOSI引脚
#define      TFT_SPI_MOSI_PORT               GPIOA
#define      TFT_SPI_MOSI_PIN                GPIO_PIN_7

// 背光接口
#define      BLK_GPIO_PORT      GPIOB
#define      BLK_GPIO_PIN       GPIO_PIN_8

// REST接口
#define      REST_GPIO_PORT     GPIOB
#define      REST_GPIO_PIN      GPIO_PIN_5

// DC接口(此接口为TFT屏幕的命令接口)
#define      DC_GPIO_PORT       GPIOB
#define      DC_GPIO_PIN        GPIO_PIN_6

#define TFT_CS_LOW()        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)   // CS
#define TFT_CS_HIGH()       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)

#define TFT_REST_LOW()      HAL_GPIO_WritePin(REST_GPIO_PORT, REST_GPIO_PIN, GPIO_PIN_RESET)
#define TFT_REST_HIGH()     HAL_GPIO_WritePin(REST_GPIO_PORT, REST_GPIO_PIN, GPIO_PIN_SET)

#define TFT_DC_LOW()        HAL_GPIO_WritePin(DC_GPIO_PORT, DC_GPIO_PIN, GPIO_PIN_RESET)   // DC
#define TFT_DC_HIGH()       HAL_GPIO_WritePin(DC_GPIO_PORT, DC_GPIO_PIN, GPIO_PIN_SET)

#define TFT_BLK_LOW()       HAL_GPIO_WritePin(BLK_GPIO_PORT, BLK_GPIO_PIN, GPIO_PIN_RESET) // BLK
#define TFT_BLK_HIGH()      HAL_GPIO_WritePin(BLK_GPIO_PORT, BLK_GPIO_PIN, GPIO_PIN_SET)

extern SPI_HandleTypeDef hspi1;

void TFT_WR_DATA8(uint8_t TxData);
void TFT_WR_DATA(uint16_t data);
void TFT_WR_REG(uint8_t reg);
void TFT_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void TFT_Init(void);

#endif
