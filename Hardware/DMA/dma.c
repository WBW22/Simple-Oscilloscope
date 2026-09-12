#include "dma.h"
#include "tft.h"
#include "tft_init.h"

DMA_HandleTypeDef hdma_spi1_tx;

static uint16_t lastX = 0, lastY = 0;
static uint8_t firstPoint = 1;

/*
*  函数功能：配置 SPI1 TX 的 DMA 通道(DMA1_Channel3)
*  入口参数：无
*  返回值：无
*/
static void MYDMA_Config(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  // 内存 -> 外设(SPI1->DR), 字节, 普通模式
  hdma_spi1_tx.Instance = DMA1_Channel3;
  hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_spi1_tx.Init.Mode = DMA_NORMAL;
  hdma_spi1_tx.Init.Priority = DMA_PRIORITY_HIGH;
  HAL_DMA_Init(&hdma_spi1_tx);

  // 关联DMA到SPI1的TX
  __HAL_LINKDMA(&hspi1, hdmatx, hdma_spi1_tx);

  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

/*
*  函数功能：SPI DMA 发送完成回调(拉高片选)
*/
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if(hspi->Instance == SPI1)
  {
    TFT_CS_HIGH();
  }
}

/*
*  函数功能：DMA填充指定区域颜色
*  说明：原代码用16bit SPI+DMA发送固定颜色(存在字节序bug);
*        这里改为按字节阻塞发送,语义等价且正确。
*/
void DMA_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{
    uint8_t color_bytes[2];
    uint32_t num = (uint32_t)(xend - xsta) * (yend - ysta);
    uint32_t i;

    color_bytes[0] = (uint8_t)(color >> 8);
    color_bytes[1] = (uint8_t)(color);

    TFT_Address_Set(xsta, ysta, xend - 1, yend - 1);  // 设置显示范围

    TFT_CS_LOW();   // 拉低片选

    for(i = 0; i < num; i++)
    {
      HAL_SPI_Transmit(&hspi1, color_bytes, 2, 10);
    }

    TFT_CS_HIGH();
}

/*
*  函数功能：DMA显示图片(把内存中的像素缓冲用SPI DMA发送到屏幕)
*  入口参数：showData--像素数据指针(每像素2字节RGB565)
*            num--像素个数
*/
void DMA_ShowBMP(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t num,volatile uint8_t* showData)
{
    MYDMA_Config();

    TFT_Address_Set(xsta, ysta, xend - 1, yend - 1);  // 设置显示范围

    TFT_CS_LOW();   // 拉低片选

    // SPI1 8bit模式,DMA发送 num*2 字节
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)showData, (uint16_t)(num * 2));
}

/*
*  函数功能：初始化显示数据
*  入口参数：uint8_t *showData--数据指针
*  返回值：无
*/
void Init_ShowData(volatile uint8_t *showData)
{
    uint16_t i = 0, j = 0;
    for(i = 0; i <= 40; i++)
    {
        for(j = 0; j <= 50; j++)
        {
            if(((i % 10) == 0) || (i == 0))
            {
              *showData++ = (uint8_t)(BLUE >> 8);
              *showData++ = (uint8_t)(BLUE);
            }
            else
            {
              *showData++ = (uint8_t)(BLACK >> 8);
              *showData++ = (uint8_t)(BLACK);
            }
        }
    }
}

/*
*  函数功能：画线
*  入口参数：x1,y1 起始坐标
*            x2,y2 终止坐标
*            color 要填充的颜色
*/
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color,volatile uint8_t* showData)
{
    uint16_t i = 0;
    uint16_t t = 0;
    int xerr = 0, yerr = 0, delta_x = 0, delta_y = 0, distance = 0;
    int incx = 0, incy = 0, uRow = 0, uCol = 0;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0){
        incx = 1;
    }
    else if (delta_x == 0){
        incx = 0;
    }
    else {
        incx = -1;
        delta_x = -delta_x;
    }
    if(delta_y > 0){
        incy = 1;
    }
    else if (delta_y == 0){
        incy = 0;
    }
    else {
        incy = -1;
        delta_y = -delta_y;
    }
    if(delta_x > delta_y){
        distance = delta_x;
    }
    else {
        distance = delta_y;
    }
    for(t = 0; t < distance + 1; t++)
    {
        i = uRow * 2 + (uCol - 40) * 51 * 2;

        if((i % 2) == 0)
        {
            *(showData + i) = (uint8_t)(color >> 8);
            *(showData + i + 1) = (uint8_t)(color);
        }
        else
        {
            *(showData + i - 1) = (uint8_t)(color >> 8);
            *(showData + i) = (uint8_t)(color);
        }
        i = 0;

        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/*
*  函数功能：画曲线(DMA版本)
*  入口参数：short int rawValue--Y方向的值
*  返回值：无
*/
void DMAdrawCurve(uint8_t yOffset,short int rawValue,volatile uint8_t *showData)
{
    uint16_t i = 0;
    uint16_t x = 0, y = 0;
    y = yOffset - rawValue;   // data processing code
    if(firstPoint)
    {
        i = 0 + (y - 40) * 51 * 2;
        if((i % 2) == 0)
        {
            *(showData + i) = (uint8_t)(RED >> 8);
            *(showData + i + 1) = (uint8_t)(RED);
        }
        else
        {
            *(showData + i - 1) = (uint8_t)(RED >> 8);
            *(showData + i) = (uint8_t)(RED);
        }
        i = 0;

        lastX = 0;
        lastY = y;
        firstPoint = 0;
    }
    else
    {
        x = lastX + 1;

        if(x < 50)
        {
            LCD_DrawLine(lastX, lastY, x, y, RED, showData);
            lastX = x;
            lastY = y;
        }
        else
        {
            LCD_DrawLine(lastX, lastY, x, y, RED, showData);
            lastX = 0;
            lastY = y;
        }
    }
}
