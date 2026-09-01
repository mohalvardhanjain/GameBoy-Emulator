#pragma once
#include <atomic>
#include <cstdint>

#include <common.h>

class emu_context{
    public:
        std::atomic<bool> running;
        std::atomic<bool> paused;
        std::atomic<bool> die;

        std::atomic<uint64_t> ticks;
};

int emu_run(int argc, char ** argv);

emu_context *emu_get_context();

void emu_cycles(int cpu_cycles);