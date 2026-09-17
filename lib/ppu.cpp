#include <ppu.h>
#include <lcd.h>
#include <string.h>
#include <ppu_sm.h>

// void pipeline_fifo_reset();
// void pipeline_process();


static ppu_context ctx;

ppu_context* ppu_get_context() {
    return &ctx;
}

void ppu_init() {
    ctx.current_frame = 0;
    ctx.line_ticks = 0;
    ctx.video_buffer = new uint32_t[YRES * XRES]{};

    ctx.pfc.line_x = 0;
    ctx.pfc.pushed_x = 0;
    ctx.pfc.fetch_x = 0;
    ctx.pfc.pixel_fifo.size = 0;
    ctx.pfc.pixel_fifo.head = ctx.pfc.pixel_fifo.tail = NULL;
    ctx.pfc. curr_fetch_state = FS_TILE;

    ctx.line_sprites = 0;
    ctx.fetched_entry_count = 0;

    lcd_init();
    LCDS_MODE_SET(MODE_OAM);

    memset(ctx.oam_ram, 0, sizeof(ctx.oam_ram));
    memset(ctx.video_buffer, 0, sizeof(YRES * XRES * sizeof(uint32_t)));
}

void ppu_tick() {
    ctx.line_ticks++;

    switch(LCDS_MODE) {
        case MODE_OAM :
            ppu_mode_oam();
            break;
        case MODE_XFER :
            ppu_mode_xfer();
            break;
        case MODE_VBLANK :
            ppu_mode_vblank();
            break;
        case MODE_HBLANK :
            ppu_mode_hblank();
            break;
    }
}

uint8_t ppu_oam_read(uint16_t address) {
    if(address >= 0xFE00) {
        address -= 0xFE00;
    }

    uint8_t *p = (uint8_t *)ctx.oam_ram;
    return p[address];
}

void ppu_oam_write(uint16_t address, uint8_t value) {
    if(address >= 0xFE00) {
        address -= 0xFE00;
    }

    uint8_t *p = (uint8_t *)ctx.oam_ram;
    p[address] = value;
}

uint8_t ppu_vram_read(uint16_t address) {
    return ctx.vram[address - 0x8000];
}

void ppu_vram_write(uint16_t address, uint8_t value) {
    ctx.vram[address - 0x8000] = value;
}