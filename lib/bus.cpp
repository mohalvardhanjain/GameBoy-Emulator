#include <bus.h>
#include <cart.h>
#include <ram.h>
#include <cpu.h>
#include <io.h>
#include <ppu.h>
#include <dma.h>

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page



uint8_t bus_read(uint16_t address) {

    if(address < 0x8000) {
        //ROM DATA
        return cart_read(address);
    } else if (address < 0xA000) {
        //Char/ Map Data / VRAM
        return ppu_vram_read(address);
    } else if(address < 0xC000) {
        //Cartridge RAM

        return cart_read(address);
    } else if(address < 0xE000) {
        //WRAM (Working RAM)

        return wram_read(address);
    } else if(address < 0xFE00) {
        //reserved echo ram...
        return 0;
    } else if(address < 0xFEA0) {
        //OAM
        if(dma_transferring()) {
            return 0xFF;
        }

        return ppu_oam_read(address);
    } else if(address < 0xFF00) {
        //reserved unusable..
        return 0;
    } else if(address < 0xFF80) {
        //IO registers..
        //todo...
        return io_read(address);
        
    } else if(address == 0xFFFF) {
        //CPU ENABLE REGISTER
        //todo...
        return cpu_get_ie_register();

    }

    return hram_read(address);

}


void bus_write(uint16_t address, uint8_t value) {

    if (address < 0x8000) {
        //ROM Data
        cart_write(address, value);
    } else if (address < 0xA000) {
        //Char/Map Data
        ppu_vram_write(address, value);

    } else if (address < 0xC000) {
        //EXT-RAM
        cart_write(address, value);
    } else if (address < 0xE000) {
        //WRAM
        wram_write(address, value);
    } else if (address < 0xFE00) {
        //reserved echo ram
    } else if (address < 0xFEA0) {
        //OAM

        if(dma_transferring()) {
            return;
        }

        ppu_oam_write(address, value);
    } else if (address < 0xFF00) {
        //unusable reserved
    } else if (address < 0xFF80) {
        //IO Registers...
        return io_write(address, value);
        
    } else if (address == 0xFFFF) {
        //CPU SET ENABLE REGISTER
        
        cpu_set_ie_register(value);
    } else {
        hram_write(address, value);
    }
}

uint16_t bus_read16(uint16_t address) {
    uint16_t lo = bus_read(address);
    uint16_t hi = bus_read(address + 1);

    return lo | (hi << 8);
}

void bus_write16(uint16_t address, uint16_t value) {
    bus_write(address + 1, (value >> 8) & 0xFF);
    bus_write(address, value & 0xFF);
}