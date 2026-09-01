#pragma once

#include <common.h>

class timer_context {
    public:
        uint16_t div;
        uint8_t tima;
        uint8_t tma;
        uint8_t tac;
};

void timer_init();
void timer_tick();

void timer_write(uint16_t address, uint8_t value);
uint8_t timer_read(uint16_t address);

timer_context* timer_get_context();