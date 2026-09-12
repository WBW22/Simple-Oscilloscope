# Simple-Oscilloscope

基于 **STM32F103C8T6** 的简易数字示波器,使用 **STM32 HAL 库** 编写,
配套 1.8 寸 **ST7735S** SPI 液晶屏(160×128)。

本工程由标准库(StdPeriph)版本的简易示波器**移植到 HAL 库**,
并用 STM32CubeMX 生成工程骨架、用 Keil MDK-ARM 组织编译。

---

## 功能

- **波形采集**:ADC1 通道 3(PA3) + DMA1 通道 1 循环搬运,一次采 300 点
- **软件触发**:在 200 点内寻找「先低于参考电平、再高于参考电平」的跳变点作为触发点,
  从触发点起取 100 点绘制,使波形在屏幕上稳定不滚动
- **自动增益**:按峰峰值 Vpp 计算放大倍数,把波形固定显示在屏幕 18.75~41.25 行
- **信号输出**:TIM2 通道 3(PA2)输出 PWM 方波,可调频率与占空比
- **频率测量**:TIM3 通道 1(PA6)输入捕获测频
- **人机交互**:3 个独立按键 + EC11 旋转编码器(带按键)
- **界面**:实时显示波形、峰峰值、采集/输出频率等参数

## 硬件引脚

| 引脚 | 功能 | 说明 |
|------|------|------|
| PA2  | TIM2_CH3 | PWM 信号输出 |
| PA3  | ADC1_IN3 | 模拟信号输入(被测信号) |
| PA5  | SPI1_SCK | TFT 时钟 |
| PA6  | TIM3_CH1 | 频率测量输入捕获 |
| PA7  | SPI1_MOSI | TFT 数据 |
| PB3  | GPIO 输入 | EC11 编码器 B 相 |
| PB4  | EXTI4 | EC11 编码器 A 相 |
| PB5  | GPIO 输出 | TFT_RES 复位 |
| PB6  | GPIO 输出 | TFT_DC 命令/数据 |
| PB7  | GPIO 输出 | TFT_CS 片选(软件控制) |
| PB8  | GPIO 输出 | TFT_BLK 背光 |
| PB9  | EXTI9 | EC11 编码器按键 |
| PB13 | EXTI13 | KEY1 |
| PB14 | EXTI14 | KEY3 |
| PB15 | EXTI15 | KEY2 |
| PC14 | GPIO 输出 | LED1 |
| PC15 | GPIO 输出 | LED2 |
| PA13 / PA14 | SWDIO / SWCLK | 调试下载 |

> PA6 同时被 `tft_init.h` 声明为 SPI MISO。因为本屏只做**半双工写**,
> MISO 实际不使用,且 `Init_FreqTimer()` 在 `TFT_Init()` 之后执行,
> 会把 PA6 重新配成 TIM3 输入捕获,所以两者不冲突。
> 但如果你要改用 SPI 读操作,PA6 这个复用需要重新分配。

## 操作说明

| 操作 | 功能 |
|------|------|
| KEY1 (PB13) | 增大 PWM 占空比,每次 +5%,超过周期后归 0 |
| KEY2 (PB15) | 开启 / 关闭 PWM 输出 |
| KEY3 (PB14) | 切换输出频率(周期减半,小于 250 时回到 1000) |
| EC11 顺时针 | 增大 ADC 采样时间(降低采样率) |
| EC11 逆时针 | 减小 ADC 采样时间(提高采样率) |
| EC11 按下 | 暂停波形刷新(在绘图循环中等待再次按下) |

## 目录结构

```
├── Simple-Oscilloscope.ioc      CubeMX 工程文件(引脚分配以此为准)
├── Src/  Inc/                   CubeMX 生成的骨架 + 应用层代码
│   ├── main.c                   主逻辑:HAL 初始化、时钟、采集与绘图主循环
│   ├── main.h                   struct Oscilloscope 数据结构定义
│   ├── stm32f1xx_it.c/.h        中断服务函数
│   └── stm32f1xx_hal_conf.h     HAL 库裁剪配置(见下方说明)
├── Drivers/                     ST 官方库(已裁剪)
│   ├── CMSIS/                   Cortex-M3 内核头文件 + F103 器件头文件
│   └── STM32F1xx_HAL_Driver/    STM32F1xx HAL 驱动
├── Hardware/                    外设驱动
│   ├── ADC/                     ADC1 + DMA1 采集
│   ├── DMA/                     SPI+DMA 刷屏(实验代码,主程序未调用)
│   ├── FREQ/                    TIM3 输入捕获测频
│   ├── KEY/                     按键与 EC11 编码器
│   ├── LED/                     LED(主程序未调用)
│   ├── TFT/                     ST7735S 屏驱动 + 字库
│   └── TIMER/                   TIM2 PWM 输出
├── SYSTEM/delay/                DWT 实现的 us/ms 延时
└── MDK-ARM/                     Keil 工程
```

## 编译

用 **Keil MDK-ARM V5**(ARMCC V5.06)打开 [MDK-ARM/Simple-Oscilloscope.uvprojx](MDK-ARM/Simple-Oscilloscope.uvprojx)
直接编译即可,无需额外配置。

命令行编译:

```bash
UV4.exe -b MDK-ARM/Simple-Oscilloscope.uvprojx -j0 -o build.log
```

当前编译结果:

```
Program Size: Code=20742  RO-data=8738  RW-data=36  ZI-data=3876
0 Error(s), 0 Warning(s)
```

即 **Flash ≈ 28.8 KB / 64 KB**,**RAM ≈ 3.8 KB / 20 KB**,资源余量充足。

芯片选择 **STM32F103C8**,预定义宏 `USE_HAL_DRIVER, STM32F103xB`。

## 系统时钟

`main.c` 中的 `SystemClock_Config()` 把时钟配置为
**HSE 8MHz → PLL×9 → 72MHz**,APB1 = 36MHz,APB2 = 72MHz。

> **注意**:`Simple-Oscilloscope.ioc` 里 CubeMX 记录的却是 36MHz
> (`RCC.SYSCLKFreq_VALUE=36000000`),因为 CubeMX 默认认为 HSE 先二分频
> (`RCC_HSE_PREDIV_DIV2`)。
> **实际运行的固件以 `main.c` 为准,是 72MHz**,`.ioc` 里的这一项与代码不一致。
> 旁证:`Hardware/TIMER/timer.h` 和 `Hardware/FREQ/freq.h` 里的预分频注释都写着
> `72MHz/(71+1) = 1MHz`,即代码是按 72MHz 写的。
> (`.ioc` 仅用于引脚分配参考;`SystemClock_Config()` 已被移植代码整体覆盖。)
>
> 如果你的板子是 12MHz 晶振,请把 `SystemClock_Config()` 里的
> `RCC_PLL_MUL9` 改成 `RCC_PLL_MUL6`,同时把
> [Inc/stm32f1xx_hal_conf.h](Inc/stm32f1xx_hal_conf.h) 里的 `HSE_VALUE` 改成 `12000000`。

## 移植说明

从标准库移植到 HAL 库时遵循以下约定,**改代码前请先读这一节**:

### 1. 各外设的 MspInit 写在各自的驱动模块里

HAL 库本来会把 GPIO 与时钟初始化集中放在 `stm32f1xx_hal_msp.c` 里,
本工程刻意**不保留**这个文件,而是把每个外设的底层初始化直接写在它的驱动中:

- `Hardware/ADC/adc.c` → `HAL_ADC_MspInit()`
- 其余外设的 GPIO/时钟初始化写在各自的 `Init_xxx_GPIO()` 里

**因此不要再加回 CubeMX 生成的 `stm32f1xx_hal_msp.c`**,
否则会出现函数重定义冲突。

### 2. `stm32f1xx_hal_conf.h` 的两处坑

- **头文件包含顺序不能按字母序排**。`stm32f1xx_hal_adc.h` 里用到了
  `DMA_HandleTypeDef`,所以 `dma.h` 必须排在 `adc.h` 之前。
  当前顺序已按 ST 官方模板的依赖关系排好。
- **必须定义 `LSI_VALUE` / `LSE_VALUE` / `LSE_STARTUP_TIMEOUT`**。
  本工程虽未使用 LSE(PC14/PC15 拿去当 LED 了),
  但 HAL 的 RCC 驱动需要这几个宏才能编译通过。

### 3. 延时用 DWT 而不是 SysTick

`SYSTEM/delay/delay.c` 用 Cortex-M3 的 **DWT 周期计数器**实现 `delay_us` / `delay_ms`,
不占用 SysTick,因此**可以在中断服务函数里安全调用**
(标准库版本的 delay 直接操作 SysTick 寄存器,在中断里调用会出问题)。
采用的是首次调用时懒初始化。

### 4. CubeMX 重新生成代码会覆盖 `main.c`

本工程是用 CubeMX 生成骨架后,**用移植代码整体替换掉**生成的 `Src/main.c` 的。
替换后的 `main.c` 里**没有 `USER CODE BEGIN/END` 保护段**,
所以**直接点 CubeMX 的 "Generate Code" 会冲掉主逻辑**。

如果需要改动引脚:改 `.ioc` → 生成到**另一个空目录** → 手工把
`MDK-ARM/*.uvprojx` 的差异(分组、包含路径)和 `Src/stm32f1xx_it.c`
的差异搬过来。不要直接对着本目录生成。

## 注意事项 / 已知问题

### ⚠️ `Hardware/TFT/tft.c` 与 `Hardware/TFT/font.h` 必须保持 GBK 编码

这两个文件是 **GBK(ISO-8859 检测)编码**,仓库里其余文件都是 UTF-8。
**请不要把它们转成 UTF-8。**

原因:`font.h` 里的汉字字模用 `unsigned char Index[2]` 存索引,
即一个汉字占 **2 字节**;`TFT_ShowChinese()` 是按 `s += 2` 逐字推进、
拿这 2 字节去表里查字模的。
UTF-8 的汉字是 **3 字节**,一旦转换,汉字就**全部查不到**,屏幕上只剩空白
(编译不会报任何错)。

编辑器在保存时如果默认用 UTF-8,会静默破坏这个文件。改 `tft.c` 或 `font.h` 前
请确认编辑器编码设为 GBK / GB2312。

### 其余问题

- **`Hardware/LED/led.c` 的 `Open_LED()` / `CLose_LED()` 有缺陷**:
  `case 1` 和 `case 2` 写的是同一个引脚
  (两个 case 都操作 PC14 点亮、都操作 PC15 熄灭),
  导致 LED2 无法独立点亮。这是**从原标准库版本继承下来的问题,移植时按原样保留**,
  未作修改。主程序当前没有调用这两个函数。
- **`Hardware/DMA/dma.c` 未被使用**:那是原项目 SPI+DMA 快速刷屏的实验代码,
  实际绘图走 `tft.c` 的软件方式。为保持接口完整按 HAL 等价改写后一并保留,
  目前只是被编译进去,没有被调用。

## 许可

本工程代码采用 MIT 许可,见 [LICENSE](LICENSE)。

`Drivers/` 下的 CMSIS 与 STM32F1xx HAL 驱动版权归 STMicroelectronics 所有,
按其原始许可分发(详见 [Drivers/CMSIS/LICENSE.txt](Drivers/CMSIS/LICENSE.txt))。
