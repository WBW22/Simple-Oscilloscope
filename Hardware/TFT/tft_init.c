#include "tft_init.h"
#include "delay.h"

SPI_HandleTypeDef hspi1;

/*
*  函数功能：初始化SPI1及GPIO
*  入口参数：无
*  返回值：无
*/
static void Init_SPI0_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 使能SPI/端口时钟 */
  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  /************引脚配置************/
  // CS (软件片选, 普通推挽输出)
  GPIO_InitStruct.Pin = TFT_SPI_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TFT_SPI_CS_PORT, &GPIO_InitStruct);
  TFT_CS_HIGH();  // 拉高CS电平

  // SCK
  GPIO_InitStruct.Pin = TFT_SPI_SCK_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TFT_SPI_SCK_PORT, &GPIO_InitStruct);

  // MISO (输入)
  GPIO_InitStruct.Pin = TFT_SPI_MISO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TFT_SPI_MISO_PORT, &GPIO_InitStruct);

  // MOSI
  GPIO_InitStruct.Pin = TFT_SPI_MOSI_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TFT_SPI_MOSI_PORT, &GPIO_InitStruct);

  /* SPI 模式配置 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;              // 主模式
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;    // 两线全双工
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;        // 8bit数据位
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;     // 时钟极性高
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;          // 时钟第2个边沿采样
  hspi1.Init.NSS = SPI_NSS_SOFT;                  // 软件片选
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;  // 分频
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;         // 高位在前
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  HAL_SPI_Init(&hspi1);
}

/*
*  函数功能：SPI1发送一个字节(直接操作寄存器,速度更快)
*  入口参数：TxData--待发送数据
*  返回值：无
*/
static void SPI0_Write(uint8_t TxData)
{
    // 等待发送缓冲区空
    while(!(TFT_SPIx->SR & SPI_SR_TXE));
    TFT_SPIx->DR = TxData;  // 通过寄存器SPIx发送一个byte数据
}

/*
*  函数功能：初始化REST、BLK、DC引脚
*  入口参数：无
*  返回值：无
*/
static void TFT_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();

  // REST
  GPIO_InitStruct.Pin = REST_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(REST_GPIO_PORT, &GPIO_InitStruct);

  // BLK
  GPIO_InitStruct.Pin = BLK_GPIO_PIN;
  HAL_GPIO_Init(BLK_GPIO_PORT, &GPIO_InitStruct);

  // DC
  GPIO_InitStruct.Pin = DC_GPIO_PIN;
  HAL_GPIO_Init(DC_GPIO_PORT, &GPIO_InitStruct);
}

/*
*  函数功能：TFT发送单字节数据
*  入口参数：data--待发送数据
*  返回值：无
*/
void TFT_WR_DATA8(uint8_t data)
{
    TFT_CS_LOW();   // 拉低片选信号

    SPI0_Write(data);

    TFT_CS_HIGH();  // 拉高片选信号
}

/*
*  函数功能：TFT发送2字节数据
*  入口参数：data--待发送数据
*  返回值：无
*/
void TFT_WR_DATA(uint16_t data)
{
    TFT_CS_LOW();   // 拉低片选信号

    SPI0_Write(data >> 8);

    SPI0_Write(data);

    TFT_CS_HIGH();  // 拉高片选信号
}

/*
*  函数功能：TFT发送命令
*  入口参数：reg--命令
*  返回值：无
*/
void TFT_WR_REG(uint8_t reg)
{
    TFT_CS_LOW();   // 拉低片选信号
    TFT_DC_LOW();   // 拉低命令/数据选择(命令)

    SPI0_Write(reg);

    TFT_DC_HIGH();  // 拉高命令/数据选择(数据)
    TFT_CS_HIGH();  // 拉高片选信号
}

void TFT_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	if(USE_HORIZONTAL==0)
	{
		TFT_WR_REG(0x2a);//列地址设置
		TFT_WR_DATA(x1);
		TFT_WR_DATA(x2);
		TFT_WR_REG(0x2b);//行地址设置
		TFT_WR_DATA(y1);
		TFT_WR_DATA(y2);
		TFT_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		TFT_WR_REG(0x2a);
		TFT_WR_DATA(x1);
		TFT_WR_DATA(x2);
		TFT_WR_REG(0x2b);
		TFT_WR_DATA(y1);
		TFT_WR_DATA(y2);
		TFT_WR_REG(0x2c);
	}
	else if(USE_HORIZONTAL==2)
	{
		TFT_WR_REG(0x2a);
		TFT_WR_DATA(x1);
		TFT_WR_DATA(x2);
		TFT_WR_REG(0x2b);
		TFT_WR_DATA(y1);
		TFT_WR_DATA(y2);
		TFT_WR_REG(0x2c);
	}
	else
	{
		TFT_WR_REG(0x2a);
		TFT_WR_DATA(x1);
		TFT_WR_DATA(x2);
		TFT_WR_REG(0x2b);
		TFT_WR_DATA(y1);
		TFT_WR_DATA(y2);
		TFT_WR_REG(0x2c);
	}
}

void TFT_Init(void)
{
    TFT_GPIO_Init();
    Init_SPI0_GPIO();

    TFT_REST_LOW();   // 复位
    delay_ms(100);
    TFT_REST_HIGH();
    delay_ms(100);

    TFT_BLK_HIGH();   // 打开背光
    delay_ms(100);

	//************* Start Initial Sequence **********//
	TFT_WR_REG(0x11); //Sleep out
	delay_ms(120);              //Delay 120ms
	//------------------------------------ST7735S Frame Rate-----------------------------------------//
	TFT_WR_REG(0xB1);
	TFT_WR_DATA8(0x05);
	TFT_WR_DATA8(0x3C);
	TFT_WR_DATA8(0x3C);
	TFT_WR_REG(0xB2);
	TFT_WR_DATA8(0x05);
	TFT_WR_DATA8(0x3C);
	TFT_WR_DATA8(0x3C);
	TFT_WR_REG(0xB3);
	TFT_WR_DATA8(0x05);
	TFT_WR_DATA8(0x3C);
	TFT_WR_DATA8(0x3C);
	TFT_WR_DATA8(0x05);
	TFT_WR_DATA8(0x3C);
	TFT_WR_DATA8(0x3C);
	//------------------------------------End ST7735S Frame Rate---------------------------------//
	TFT_WR_REG(0xB4); //Dot inversion
	TFT_WR_DATA8(0x03);
	//------------------------------------ST7735S Power Sequence---------------------------------//
	TFT_WR_REG(0xC0);
	TFT_WR_DATA8(0x28);
	TFT_WR_DATA8(0x08);
	TFT_WR_DATA8(0x04);
	TFT_WR_REG(0xC1);
	TFT_WR_DATA8(0XC0);
	TFT_WR_REG(0xC2);
	TFT_WR_DATA8(0x0D);
	TFT_WR_DATA8(0x00);
	TFT_WR_REG(0xC3);
	TFT_WR_DATA8(0x8D);
	TFT_WR_DATA8(0x2A);
	TFT_WR_REG(0xC4);
	TFT_WR_DATA8(0x8D);
	TFT_WR_DATA8(0xEE);
	//---------------------------------End ST7735S Power Sequence-------------------------------------//
	TFT_WR_REG(0xC5); //VCOM
	TFT_WR_DATA8(0x1A);
	TFT_WR_REG(0x36); //MX, MY, RGB mode
	if(USE_HORIZONTAL==0){
        TFT_WR_DATA8(0x00);
    }
	else if(USE_HORIZONTAL==1){
        TFT_WR_DATA8(0xC0);
    }
	else if(USE_HORIZONTAL==2){
        TFT_WR_DATA8(0x70);
    }
	else {
        TFT_WR_DATA8(0xA0);
    }
	//------------------------------------ST7735S Gamma Sequence---------------------------------//
	TFT_WR_REG(0xE0);
	TFT_WR_DATA8(0x04);
	TFT_WR_DATA8(0x22);
	TFT_WR_DATA8(0x07);
	TFT_WR_DATA8(0x0A);
	TFT_WR_DATA8(0x2E);
	TFT_WR_DATA8(0x30);
	TFT_WR_DATA8(0x25);
	TFT_WR_DATA8(0x2A);
	TFT_WR_DATA8(0x28);
	TFT_WR_DATA8(0x26);
	TFT_WR_DATA8(0x2E);
	TFT_WR_DATA8(0x3A);
	TFT_WR_DATA8(0x00);
	TFT_WR_DATA8(0x01);
	TFT_WR_DATA8(0x03);
	TFT_WR_DATA8(0x13);
	TFT_WR_REG(0xE1);
	TFT_WR_DATA8(0x04);
	TFT_WR_DATA8(0x16);
	TFT_WR_DATA8(0x06);
	TFT_WR_DATA8(0x0D);
	TFT_WR_DATA8(0x2D);
	TFT_WR_DATA8(0x26);
	TFT_WR_DATA8(0x23);
	TFT_WR_DATA8(0x27);
	TFT_WR_DATA8(0x27);
	TFT_WR_DATA8(0x25);
	TFT_WR_DATA8(0x2D);
	TFT_WR_DATA8(0x3B);
	TFT_WR_DATA8(0x00);
	TFT_WR_DATA8(0x01);
	TFT_WR_DATA8(0x04);
	TFT_WR_DATA8(0x13);
	//------------------------------------End ST7735S Gamma Sequence-----------------------------//
	TFT_WR_REG(0x3A); //65k mode
	TFT_WR_DATA8(0x05);
	TFT_WR_REG(0x29); //Display on
}
