#pragma once 

#include <common.h>

static const int LINES_PER_FRAME = 154;
static const int TICKS_PER_LINE = 456;
static const int YRES = 144;
static const int XRES = 160;


enum fetch_state{
    FS_TILE = 0,
    FS_DATA0 = 1,
    FS_DATA1 = 2,
    FS_IDLE = 3,
    FS_PUSH = 4
};

class fifo_entry {
    public:
        fifo_entry* next;

        uint32_t value; // 32-bit color value
};

class fifo {
    public:
        fifo_entry *head;
        fifo_entry *tail;
        uint32_t size;
};

class pixel_fifo_context {
    public:
        fetch_state curr_fetch_state;
        fifo pixel_fifo;
        uint8_t line_x;
        uint8_t pushed_x;
        uint8_t fetch_x;
        uint8_t bgw_fetch_data[3];
        uint8_t fetch_entry_data[6]; //oam data..
        uint8_t map_y;
        uint8_t map_x;
        uint8_t tile_y;
        uint8_t fifo_x;
};

class oam_entry {
    public:
        uint8_t y;
        uint8_t x;
        uint8_t tile;
        
        uint8_t f_cgb_pn : 3;
        uint8_t f_cgb_vram : 1;
        uint8_t f_pn : 1;
        uint8_t f_x_flip : 1;
        uint8_t f_y_flip : 1;
        uint8_t f_bgp : 1;
};

/*
Bit 7 : BG and Window over OBJ : Priority: 0 = No, 1 = BG and Window color indices 1–3 are drawn over this OBJ
Bit 6 : Y flip: 0 = Normal, 1 = Entire OBJ is vertically mirrored
Bit 5 : X flip: 0 = Normal, 1 = Entire OBJ is horizontally mirrored
Bit 4 : Pallette number : DMG palette [Non CGB Mode only]: 0 = OBP0, 1 = OBP1
Bit 3 : Bank [CGB Mode Only]: 0 = Fetch tile from VRAM bank 0, 1 = Fetch tile from VRAM bank 1
Bit 2-0 : CGB palette [CGB Mode Only]: Which of OBP0–7 to use
*/


class oam_line_entry {
    public:
        oam_entry entry;
        oam_line_entry* next;
};

class ppu_context{
    public:
        oam_entry oam_ram[40];
        uint8_t vram[0x2000];

        uint8_t line_sprite_count; // 0 t0 10 sprites;
        oam_line_entry *line_sprites; //linked list of current sprites on line.
        oam_line_entry line_entry_array[10]; //memory to use for list.

        uint8_t fetched_entry_count;
        oam_entry fetched_entries[3]; //entries fetched during pipeline.

        pixel_fifo_context pfc;

        uint32_t current_frame;
        uint32_t line_ticks;
        uint32_t *video_buffer;
};

void ppu_init();
void ppu_tick();

uint8_t ppu_oam_read(uint16_t address);
void ppu_oam_write(uint16_t address, uint8_t value);

uint8_t ppu_vram_read(uint16_t address);
void ppu_vram_write(uint16_t address, uint8_t value);

ppu_context* ppu_get_context();

void pipeline_fifo_reset();
void pipeline_process();