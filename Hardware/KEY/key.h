#ifndef __KEY_H
#define __KEY_H

#include "main.h"

// 按键编号定义
enum
{
    KEY1 = 1,   // PB13
    KEY2 = 2,   // PB15
    KEY3 = 3,   // PB14
    KEYD = 4,   // PB9  EC11编码器按键
    KEYA = 5,   // PB4  EC11编码器A相
    KEYB = 6,   // PB3  EC11编码器B相
};

void Init_Key_GPIO(void);                              // 初始化按键GPIO
void Init_EC11_GPIO(void);                             // 初始化EC11编码器GPIO
void Key_Handle(volatile struct Oscilloscope *value);  // 按键处理

#endif
