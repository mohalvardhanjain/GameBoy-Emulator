#pragma once

#include <cpu.h>

enum interrupt_type {
    IT_VBLANK = 1,
    IT_LCD_STAT = 2,
    IT_TIMER = 4,
    IT_SERIAL = 8,
    IT_JOYPAD = 16
};

void cpu_request_interrupt(interrupt_type t);

void cpu_handle_interrupts(cpu_context *ctx);