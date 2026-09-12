#ifndef __LED_H
#define __LED_H

#include "main.h"

enum
{
    led2 = 1,
    led3 = 2,
};

void Init_LED_GPIO(void);
void Open_LED(uint8_t value);
void CLose_LED(uint8_t value);

#endif
