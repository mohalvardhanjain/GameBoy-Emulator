#include <ram.h>

class ram_context{
    public:
        uint8_t wram[0x2000];
        uint8_t hram[0x80];
};

static ram_context ctx;

uint8_t hram_read(uint16_t address) {
    address -= 0xFF80;
    return ctx.hram[address];
}


void hram_write(uint16_t address, uint8_t value) {
    address -= 0xFF80;
    ctx.hram[address] = value;
}


uint8_t wram_read(uint16_t address) {
    address -= 0xC000;
    if(address >= 0x2000) {
        std::cout << "INVALID WRAM ADDRESS : " << address + 0xC000 << "\n";
        exit(-1);
    }

    return ctx.wram[address];
}


void wram_write(uint16_t address, uint8_t value) {
    address -= 0xC000;
    ctx.wram[address] = value;
}