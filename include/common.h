#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <iostream>
#include <stdlib.h>

#define BIT(a, n) ((a & (1 << n)) ? 1 : 0)

#define BIT_SET(a, n, on) \
    do { \
        if (on) \
            (a) |= (1 << n); \
        else \
            (a) &= ~(1 << n); \
    } while (0)

#define BETWEEN(a, b, c) ((a >= b) && (a <= c))

uint32_t get_ticks();
void delay(uint32_t ms);