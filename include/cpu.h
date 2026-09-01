#pragma once

#include <instructions.h>
#include <common.h>

struct cpu_registers {
    public:
        uint8_t a;
        uint8_t f;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t h;
        uint8_t l;

        uint16_t pc;
        uint16_t sp;
};

class cpu_context {
    public:
        cpu_registers regs;
        uint16_t fetched_data;
        uint16_t memory_dest;
        bool is_dest_mem;
        uint8_t curr_opcode;
        instruction *curr_inst;

        bool halted;
        bool stepping;

        bool int_master_enabled;
        bool enabling_ime;
        uint8_t ie_register;
        uint8_t int_flags;
};

void init_instructions();

void inst_to_str(cpu_context* ctx, char *str);

void cpu_init();
bool cpu_step();

uint16_t cpu_read_reg(reg_type rt);
void cpu_set_reg(reg_type rt, uint16_t val);

void fetch_data();

uint8_t cpu_get_ie_register();
void cpu_set_ie_register(uint8_t n);

uint8_t cpu_read_reg8(reg_type rt);
void cpu_set_reg8(reg_type rt, uint8_t val);

cpu_registers * cpu_get_regs();

using IN_PROC = void (*)(cpu_context*);

IN_PROC inst_get_processor(inst_type type);

#define CPU_FLAG_Z BIT(ctx->regs.f, 7)
#define CPU_FLAG_N BIT(ctx->regs.f, 6)
#define CPU_FLAG_H BIT(ctx->regs.f, 5)
#define CPU_FLAG_C BIT(ctx->regs.f, 4)

uint8_t cpu_get_int_flags();
void cpu_set_int_flags(uint8_t value);