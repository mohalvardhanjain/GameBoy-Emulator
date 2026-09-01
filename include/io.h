#pragma once

#include <common.h>

uint8_t io_read(uint16_t address);

void io_write(uint16_t address, uint8_t value);