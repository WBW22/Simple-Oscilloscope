#ifndef __TFT_H
#define __TFT_H

#include "main.h"
#include <stdio.h>

// 常用颜色
#define WHITE          0xFFFF
#define BLACK          0x0000
#define BLUE           0x001F
#define BRED           0XF81F
#define GRED           0XFFE0
#define GBLUE          0X07FF
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0
#define PURPLE         0x780F
#define BROWN          0XBC40
#define BRRED          0XFC07
#define GRAY           0X8430
#define DARKBLUE       0X01CF
#define LIGHTBLUE      0X7D7C
#define GRAYBLUE       0X5458
#define LIGHTGREEN     0X841F
#define LGRAY          0XC618
#define LGRAYBLUE      0XA651
#define LBBLUE         0X2B12

void TFT_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
void TFT_DrawPoint(uint16_t x,uint16_t y, uint16_t color);
void TFT_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void TFT_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);
void drawCurve(uint8_t yOffset,short int rawValue);
void TFT_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void TFT_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void TFT_ShowChinese(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void TFT_ShowChinese12x12(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void TFT_ShowChinese16x16(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void TFT_ShowChinese24x24(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void TFT_StaticUI(void);
void TFT_ShowUI(volatile const struct Oscilloscope *value);

#endif
