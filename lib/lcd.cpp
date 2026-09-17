#include <lcd.h>
#include <ppu.h>
#include <dma.h>

static lcd_context ctx;

static unsigned long colors_default[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void lcd_init() {
    ctx.lcdc = 0x91;
    ctx.scroll_x = 0;
    ctx.scroll_y = 0;
    ctx.ly = 0;
    ctx.ly_compare = 0;
    ctx.bg_palette = 0xFC;
    ctx.obj_palette[0] = 0xFF;
    ctx.obj_palette[1] = 0xFF;
    ctx.win_y = 0;
    ctx.win_x = 0;

    for(int i = 0; i < 4; i++){
        ctx.bg_colors[i] = colors_default[i];
        ctx.sp1_colors[i] = colors_default[i];
        ctx.sp2_colors[i] = colors_default[i];
    }
}

lcd_context* lcd_get_context() {
    return &ctx;
}


uint8_t lcd_read(uint16_t address) {
    switch(address) {
        case 0xFF40: return ctx.lcdc;
        case 0xFF41: return ctx.lcds;
        case 0xFF42: return ctx.scroll_y;
        case 0xFF43: return ctx.scroll_x;
        case 0xFF44: return ctx.ly;
        case 0xFF45: return ctx.ly_compare;
        case 0xFF47: return ctx.bg_palette;
        case 0xFF48: return ctx.obj_palette[0];
        case 0xFF49: return ctx.obj_palette[1];
        case 0xFF4A: return ctx.win_y;
        case 0xFF4B: return ctx.win_x;
    }

    return 0xFF;
}


void update_palette(uint8_t palette_data, uint8_t pal){
    uint32_t *p_colors = ctx.bg_colors;

    switch(pal) {
        case 1 :
            p_colors = ctx.sp1_colors;
            break;
        case 2 :
            p_colors = ctx.sp2_colors;
            break;
    }

    p_colors[0] = colors_default[palette_data & 0b11];
    p_colors[1] = colors_default[(palette_data >> 2) & 0b11];
    p_colors[2] = colors_default[(palette_data >> 4) & 0b11];
    p_colors[3] = colors_default[(palette_data >> 6) & 0b11];
}

void lcd_write(uint16_t address, uint8_t value) {
    switch (address) {
        case 0xFF40: // LCDC
            ctx.lcdc = value;
            break;

        case 0xFF41: // STAT
            // Only bits 3-6 are writable on DMG.
            ctx.lcds = (ctx.lcds & 0x07) | (value & 0x78);
            break;

        case 0xFF42: // SCY
            ctx.scroll_y = value;
            break;

        case 0xFF43: // SCX
            ctx.scroll_x = value;
            break;

        case 0xFF44: // LY
            // LY is read-only; writes reset it to 0.
            ctx.ly = 0;
            break;

        case 0xFF45: // LYC
            ctx.ly_compare = value;
            break;

        case 0xFF46: // DMA
            dma_start(value);
            break;

        case 0xFF47: // BGP
            ctx.bg_palette = value;
            update_palette(value, 0);
            break;

        case 0xFF48: // OBP0
            ctx.obj_palette[0] = value;
            update_palette(value & 0xFC, 1);
            break;

        case 0xFF49: // OBP1
            ctx.obj_palette[1] = value;
            update_palette(value & 0xFC, 2);
            break;

        case 0xFF4A: // WY
            ctx.win_y = value;
            break;

        case 0xFF4B: // WX
            ctx.win_x = value;
            break;

        default:
            break;
    }
}