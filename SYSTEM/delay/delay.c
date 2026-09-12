#include "delay.h"

// 使用DWT(数据观察点)计数器实现延时,不依赖SysTick,
// 可在中断服务函数中安全使用(原标准库版本的delay直接操作SysTick寄存器)
static uint32_t fac_us = 0;

/*
*  函数功能：初始化延时(懒初始化,首次调用时自动执行)
*/
static void delay_init(void)
{
    fac_us = SystemCoreClock / 1000000;   // 1us对应的计数值
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  // 使能DWT
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;   // 使能周期计数器
}

/*
*  函数功能：微秒延时
*  入口参数：nus--延时时间(us)
*/
void delay_us(uint32_t nus)
{
    uint32_t ticks, told;

    if(fac_us == 0)
    {
        delay_init();
    }

    ticks = nus * fac_us;
    told = DWT->CYCCNT;
    while((DWT->CYCCNT - told) < ticks);
}

/*
*  函数功能：毫秒延时
*  入口参数：nms--延时时间(ms)
*/
void delay_ms(uint16_t nms)
{
    delay_us(nms * 1000);
}
