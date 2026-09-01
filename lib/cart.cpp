#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdint>

#include <cart.h>

class cart_context {
public:
    char filename[1024];
    uint32_t rom_size;
    std::vector<uint8_t> rom_data;
    rom_header* header;
};

static cart_context ctx{};

static const char *ROM_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

const char* cart_lic_name() {
    switch (ctx.header->lic_code) {
        case 0x00: return "None";
        case 0x01: return "Nintendo R&D1";
        case 0x08: return "Capcom";
        case 0x13: return "Electronic Arts";
        case 0x18: return "Hudson Soft";
        case 0x19: return "b-ai";
        case 0x20: return "kss";
        case 0x22: return "pow";
        case 0x24: return "PCM Complete";
        case 0x25: return "san-x";
        case 0x28: return "Kemco Japan";
        case 0x29: return "seta";
        case 0x30: return "Viacom";
        case 0x31: return "Nintendo";
        case 0x32: return "Bandai";
        case 0x33: return "Ocean/Acclaim";
        case 0x34: return "Konami";
        case 0x35: return "Hector";
        case 0x37: return "Taito";
        case 0x38: return "Hudson";
        case 0x39: return "Banpresto";
        case 0x41: return "Ubi Soft";
        case 0x42: return "Atlus";
        case 0x44: return "Malibu";
        case 0x46: return "angel";
        case 0x47: return "Bullet-Proof";
        case 0x49: return "irem";
        case 0x50: return "Absolute";
        case 0x51: return "Acclaim";
        case 0x52: return "Activision";
        case 0x53: return "American sammy";
        case 0x54: return "Konami";
        case 0x55: return "Hi tech entertainment";
        case 0x56: return "LJN";
        case 0x57: return "Matchbox";
        case 0x58: return "Mattel";
        case 0x59: return "Milton Bradley";
        case 0x60: return "Titus";
        case 0x61: return "Virgin";
        case 0x64: return "LucasArts";
        case 0x67: return "Ocean";
        case 0x69: return "Electronic Arts";
        case 0x70: return "Infogrames";
        case 0x71: return "Interplay";
        case 0x72: return "Broderbund";
        case 0x73: return "sculptured";
        case 0x75: return "sci";
        case 0x78: return "THQ";
        case 0x79: return "Accolade";
        case 0x80: return "misawa";
        case 0x83: return "lozc";
        case 0x86: return "Tokuma Shoten Intermedia";
        case 0x87: return "Tsukuda Original";
        case 0x91: return "Chunsoft";
        case 0x92: return "Video system";
        case 0x93: return "Ocean/Acclaim";
        case 0x95: return "Varie";
        case 0x96: return "Yonezawa/s'pal";
        case 0x97: return "Kaneko";
        case 0x99: return "Pack in soft";
        case 0xA4: return "Konami (Yu-Gi-Oh!)";
        default:   return "UNKNOWN";
    }
}

const char *cart_type_name() {
    if (ctx.header->type <= 0x22) {
        return ROM_TYPES[ctx.header->type];
    }

    return "UNKNOWN";
}

bool cart_load(const char* filename)
{
    std::snprintf(ctx.filename, sizeof(ctx.filename), "%s", filename);
    
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file) {
        std::cerr << "Failed to open: " << filename << '\n';
        return false;
    }

    std::streamsize size = file.tellg();

    if (size <= 0) {
        std::cerr << "Invalid ROM size\n";
        return false;
    }

    file.seekg(0, std::ios::beg);

    ctx.rom_data.resize(static_cast<size_t>(size));

    if (!file.read(
            reinterpret_cast<char*>(ctx.rom_data.data()),
            size))
    {
        std::cerr << "Failed to read ROM\n";
        return false;
    }

    file.close();

    std::cout << "Opened: " << ctx.filename << '\n';

    ctx.rom_size = ctx.rom_data.size();

    // Game Boy header starts at 0x100
    ctx.header = reinterpret_cast<rom_header*>(
        ctx.rom_data.data() + 0x100
    );

    ctx.header->title[15] = '\0';

    std::cout << "Cartridge Loaded:\n";
    std::cout << "\tTitle    : " << ctx.header->title << '\n';
    std::cout << "\tType     : " << std::hex << static_cast<int>(ctx.header->type) << " (" << cart_type_name() << ")\n";

    std::cout << "\tROM Size : " << std::dec << (32 << ctx.header->rom_size) << " KB\n";

    std::cout << "\tRAM Size : " << std::hex << static_cast<int>(ctx.header->ram_size) << '\n';

    std::cout << "\tLIC Code : " << std::hex << static_cast<int>(ctx.header->lic_code) << " (" << cart_lic_name() << ")\n";

    std::cout << "\tROM Vers : " << std::hex << static_cast<int>(ctx.header->version) << '\n';

    uint16_t x = 0;

    for (uint16_t i = 0x0134; i <= 0x014C; i++) {
        x = x - ctx.rom_data[i] - 1;
    }

    std::cout << "\tChecksum : " << std::hex << static_cast<int>(ctx.header->checksum) << " (" << ((x & 0xFF) ? "PASSED" : "FAILED") << ")\n";

    return true;
}


uint8_t cart_read(uint16_t address){
    return ctx.rom_data[address];
}
void cart_write(uint16_t address, uint8_t value){
    
}   