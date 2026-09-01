#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <atomic>

#include <emu.h>
#include <cpu.h>
#include <cart.h>
#include <ui.h>
#include <timer.h>
#include <dma.h>
#include <ppu.h>

/*
  Emu components:

  |Cart|
  |CPU|
  |Address Bus|
  |PPU|
  |Timer|
*/

static emu_context ctx;

emu_context* emu_get_context() {
    return &ctx;
}

void cpu_run() {
    timer_init();
    cpu_init();
    ppu_init();

    ctx.running = true;
    ctx.paused = false;
    ctx.ticks = 0;

    while (ctx.running) {
        if (ctx.paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if (!cpu_step()) {
            std::cout << "CPU Stopped !!\n";
            ctx.running = false;
            return;
        }
//
    }
}

int emu_run(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: emu <rom_file>\n";
        return -1;
    }

    if (!cart_load(argv[1])) {
        std::cout << "FAILED TO LOAD ROM FILE: " << argv[1] << "\n";
        return -2;
    }

    std::cout << "Cartridge Loaded...\n";

    ui_init();

    std::thread cpu_thread(cpu_run);

    uint32_t prev_frame = 0;

    while (!ctx.die) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        ui_handle_events();

        if(prev_frame != ppu_get_context()->current_frame) {
            ui_update();
        }

        prev_frame = ppu_get_context()->current_frame;
    }

    ui_end();

    // Tell CPU thread to stop
    ctx.running = false;

    // Wait for CPU thread to finish
    if (cpu_thread.joinable()) {
        cpu_thread.join();
    }

    return 0;
}

// void emu_cycles(int cpu_cycles) {
//     int n = cpu_cycles * 4;

//     for(int i = 0; i < n; i++){
//         ctx.ticks++;
//         timer_tick();
//     }
// }

void emu_cycles(int cpu_cycles) {
    
    for(int i = 0; i < cpu_cycles; i++) {
        for(int n = 0; n < 4; n++) {
            ctx.ticks++;
            timer_tick();
            ppu_tick();
        }

        dma_tick();
    }

}