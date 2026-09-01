#pragma once

#include <common.h>

void dma_start(uint8_t start);
void dma_tick();

bool dma_transferring();