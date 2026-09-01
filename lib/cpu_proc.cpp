#include <cpu.h>
#include <emu.h>
#include <bus.h>
#include <common.h>
#include <stack.h>

//processes CPU instructions...

static bool is_16_bit(reg_type rt) {
    return rt >= RT_AF;
}

static bool check_cond(cpu_context* ctx) {
    // bool z = ctx->regs.f & CPU_FLAG_Z; maybe this was wrong
    // bool c = ctx->regs.f & CPU_FLAG_C;

    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch(ctx->curr_inst->condition) {
        case CT_NONE: return true;
        case CT_C:    return c;
        case CT_NC:   return !c;
        case CT_Z:    return z;
        case CT_NZ:   return !z;
    }

    return false;
}

void cpu_set_flags(cpu_context* ctx, uint8_t z, uint8_t n, uint8_t h, uint8_t c) {
    if (z != -1) {
        BIT_SET(ctx->regs.f, 7, z);
    }

    if (n != -1) {
        BIT_SET(ctx->regs.f, 6, n);
    }

    if (h != -1) {
        BIT_SET(ctx->regs.f, 5, h);
    }

    if (c != -1) {
        BIT_SET(ctx->regs.f, 4, c);
    }
}

reg_type rt_lookup[] = {
    RT_B,
    RT_C,
    RT_D,
    RT_E,
    RT_H,
    RT_L,
    RT_HL,
    RT_A
};

reg_type decode_reg(uint8_t reg) {
    if(reg > 0b111) {
        return RT_NONE;
    }
    return rt_lookup[reg];
}

static void proc_cb(cpu_context* ctx) {
    uint8_t op = ctx->fetched_data;
    reg_type reg = decode_reg(op & 0b111);

    uint8_t bit = (op >> 3) & 0b111;
    uint8_t bit_op = (op >> 6) & 0b11;

    uint8_t reg_val = cpu_read_reg8(reg);
    emu_cycles(1);

    if(reg == RT_HL) {
        emu_cycles(2);
    }

    switch(bit_op) {
        
        case 1 : 
            //BIT
            cpu_set_flags(ctx, !(reg_val & (1 << bit)), 0, 1, -1);
            return;

        case 2 : 
            //RST
            reg_val &= ~(1 << bit);
            cpu_set_reg8(reg, reg_val);
            return;

        case 3 : 
            //SET
            reg_val |= (1 << bit);
            cpu_set_reg8(reg, reg_val);
            return;
    }



        bool flagC = CPU_FLAG_C;

        switch(bit) {
            case 0: {
                //RLC
                bool setC = false;
                uint8_t result = (reg_val << 1) & 0xFF;

                if ((reg_val & (1 << 7)) != 0) {
                    result |= 1;
                    setC = true;
                }

                cpu_set_reg8(reg, result);
                cpu_set_flags(ctx, result == 0, false, false, setC);
            } return;

            case 1: {
                //RRC
                uint8_t old = reg_val;
                reg_val >>= 1;
                reg_val |= (old << 7);

                cpu_set_reg8(reg, reg_val);
                cpu_set_flags(ctx, !reg_val, false, false, old & 1);
            } return;

            case 2: {
                //RL
                uint8_t old = reg_val;
                reg_val <<= 1;
                reg_val |= flagC;

                cpu_set_reg8(reg, reg_val);
                cpu_set_flags(ctx, !reg_val, false, false, !!(old & 0x80));
            } return;

            case 3: {
                //RR
                uint8_t old = reg_val;
                reg_val >>= 1;

                reg_val |= (flagC << 7);

                cpu_set_reg8(reg, reg_val);
                cpu_set_flags(ctx, !reg_val, false, false, old & 1);
            } return;

            case 4: {
                //SLA
                uint8_t old = reg_val;
                reg_val <<= 1;

                cpu_set_reg8(reg, reg_val);
                cpu_set_flags(ctx, !reg_val, false, false, !!(old & 0x80));
            } return;

            case 5: {
                //SRA
                uint8_t u = (int8_t)reg_val >> 1;
                cpu_set_reg8(reg, u);
                cpu_set_flags(ctx, !u, 0, 0, reg_val & 1);
            } return;

            case 6: {
                //SWAP
                reg_val = ((reg_val & 0xF0) >> 4) | ((reg_val & 0xF) << 4);
                cpu_set_reg8(reg, reg_val);
                cpu_set_flags(ctx, reg_val == 0, false, false, false);
            } return;

            case 7: {
                //SRL
                uint8_t u = reg_val >> 1;
                cpu_set_reg8(reg, u);
                cpu_set_flags(ctx, !u, 0, 0, reg_val & 1);
            } return;
        }
}

static void proc_none(cpu_context* ctx) {
    std::cout << "INVALID INSTRUCTION! \n";
    exit(-7);
}

static void proc_nop(cpu_context* ctx) {

}

static void proc_di(cpu_context* ctx) {
    ctx->int_master_enabled = false;
}

static void proc_ei(cpu_context* ctx) {
    ctx->enabling_ime = true;
}

static void proc_ld(cpu_context* ctx) {
    if(ctx->is_dest_mem) {
        // LD (BC), A for instance...

        if(is_16_bit(ctx->curr_inst->reg2)) {
            //if 16 bit register...
            emu_cycles(1);
            bus_write16(ctx->memory_dest, ctx->fetched_data);
        } else {
            bus_write(ctx->memory_dest, ctx->fetched_data);
        }

        emu_cycles(1);

        return;
    }

    if(ctx->curr_inst->mode == AM_HL_SPR) {
        uint8_t hflag = (cpu_read_reg(ctx->curr_inst->reg2) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;

        uint8_t cflag = (cpu_read_reg(ctx->curr_inst->reg2) & 0xFF) + (ctx->fetched_data & 0xFF) >= 0x100;

        cpu_set_flags(ctx, 0, 0, hflag, cflag);
        cpu_set_reg(ctx->curr_inst->reg1, cpu_read_reg(ctx->curr_inst->reg2) + (int8_t)ctx->fetched_data);

        return;
    }

    cpu_set_reg(ctx->curr_inst->reg1, ctx->fetched_data);
}

static void proc_ldh(cpu_context* ctx) {//
    if(ctx->curr_opcode == 0xE0) {
        bus_write(ctx->memory_dest, ctx->regs.a);
    } else if(ctx->curr_opcode == 0xF0) {
        cpu_set_reg(ctx->curr_inst->reg1 , bus_read(0xFF00 | ctx->fetched_data));
    }

    emu_cycles(1);
}

static void proc_and(cpu_context* ctx) {
    ctx->regs.a &= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 1, 0);
}

static void proc_or(cpu_context* ctx) {
    ctx->regs.a |= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
}

static void proc_xor(cpu_context* ctx) {
    ctx->regs.a ^= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
}

static void proc_cp(cpu_context* ctx) {
    int n = (int)ctx->regs.a - (int)ctx->fetched_data;
    cpu_set_flags(ctx, n==0, 1, ((int) ctx->regs.a & 0x0F) - ((int)ctx->fetched_data & 0x0F) < 0, n < 0);
}

/// Goto address

static void goto_addr(cpu_context* ctx, uint16_t addr, bool pushpc) {
    if(check_cond(ctx)) {
        if(pushpc) {
            emu_cycles(2);
            stack_push16(ctx->regs.pc);
        }

        ctx->regs.pc = addr;
        emu_cycles(1);
    }
}

static void proc_jp(cpu_context* ctx) {
    goto_addr(ctx, ctx->fetched_data, false);
}

// static void proc_jr(cpu_context* ctx) {

//     bool should_jump = true;

//     switch (ctx->curr_inst->cond) {

//         case COND_NONE:
//             should_jump = true;
//             break;

//         case COND_NZ:
//             should_jump = !(ctx->regs.f & FLAG_Z);
//             break;

//         case COND_Z:
//             should_jump = (ctx->regs.f & FLAG_Z);
//             break;

//         case COND_NC:
//             should_jump = !(ctx->regs.f & FLAG_C);
//             break;

//         case COND_C:
//             should_jump = (ctx->regs.f & FLAG_C);
//             break;
//     }

//     if (!should_jump) {
//         return;
//     }

//     int8_t rel = static_cast<int8_t>(ctx->fetched_data & 0xFF);

//     uint16_t addr = ctx->regs.pc + rel;

//     goto_addr(ctx, addr, false);
// }

static void proc_jr(cpu_context* ctx) {
    int8_t rel = static_cast<int8_t>(ctx->fetched_data & 0xFF);
    uint16_t addr = ctx->regs.pc + rel;

    goto_addr(ctx, addr, false);
}

//OR MAYBE THIS WAS THE PROBLEM NEVER MIND WHATEVER IT WAS IT IS FINALLY SOLVED AFTER NOT SLEEPING THE WHOLE NIGHT
// static void proc_jr(cpu_context* ctx) {
//     int8_t rel = static_cast<int8_t>(ctx->fetched_data & 0xFF);
//     uint16_t addr = ctx->regs.pc + rel;
//     printf("%02X %04X", rel, addr);

//     goto_addr(ctx, addr, false);
// }

static void proc_call(cpu_context* ctx) {
    goto_addr(ctx, ctx->fetched_data, true);
}

static void proc_rst(cpu_context* ctx) {
    goto_addr(ctx, ctx->curr_inst->param, true);
}

static void proc_ret(cpu_context* ctx) {
    if(ctx->curr_inst->condition != CT_NONE) {
        emu_cycles(1);
    }

    if(check_cond(ctx)) {
        uint16_t lo = stack_pop();
        emu_cycles(1);
        uint16_t hi = stack_pop();
        emu_cycles(1);

        uint16_t n = (hi << 8) | lo;
        ctx->regs.pc = n;

        emu_cycles(1);
    }
}

static void proc_reti(cpu_context* ctx) {
    ctx->int_master_enabled = true;
    proc_ret(ctx);
}

static void proc_pop(cpu_context* ctx) {
    uint16_t lo = stack_pop();
    emu_cycles(1);
    uint16_t hi = stack_pop();
    emu_cycles(1);

    uint16_t n = (hi << 8) | lo;

    cpu_set_reg(ctx->curr_inst->reg1, n);

    if(ctx->curr_inst->reg1 == RT_AF) {
        cpu_set_reg(ctx->curr_inst->reg1, n & 0xFFF0);
    }
}

static void proc_push(cpu_context* ctx) {
    uint16_t hi = (cpu_read_reg(ctx->curr_inst->reg1) >> 8) & 0xFF;
    emu_cycles(1);
    stack_push(hi);

    uint16_t lo = cpu_read_reg(ctx->curr_inst->reg1) & 0xFF;
    emu_cycles(1);
    stack_push(lo);

    emu_cycles(1);
}

static void proc_inc(cpu_context* ctx) {
    uint16_t val = cpu_read_reg(ctx->curr_inst->reg1) + 1;

    if(is_16_bit(ctx->curr_inst->reg1)) {
        emu_cycles(1);
    }

    if(ctx->curr_inst->reg1 == RT_HL && ctx->curr_inst->mode == AM_MR) {
        val = bus_read(cpu_read_reg(RT_HL)) + 1;
        val &= 0xFF;
        bus_write(cpu_read_reg(RT_HL), val);
    } else {
        cpu_set_reg(ctx->curr_inst->reg1, val);
        val = cpu_read_reg(ctx->curr_inst->reg1);
    }

    if((ctx->curr_opcode & 0x03) == 0x03) {
        return;
    }

    cpu_set_flags(ctx, val == 0, 0, (val & 0x0F) == 0, -1);
}

static void proc_dec(cpu_context* ctx) {
    uint16_t val = cpu_read_reg(ctx->curr_inst->reg1) - 1;

    if(is_16_bit(ctx->curr_inst->reg1)) {
        emu_cycles(1);
    }

    if(ctx->curr_inst->reg1 == RT_HL && ctx->curr_inst->mode == AM_MR) {
        val = bus_read(cpu_read_reg(RT_HL)) - 1;
        bus_write(cpu_read_reg(RT_HL), val);
    } else {
        cpu_set_reg(ctx->curr_inst->reg1, val);
        val = cpu_read_reg(ctx->curr_inst->reg1);
    }

    if ((ctx->curr_opcode & 0x0B) == 0x0B) {
        return;
    }

    cpu_set_flags(ctx, val == 0, 1, (val & 0x0F) == 0x0F, -1);
}

static void proc_sub(cpu_context* ctx) {//
    uint16_t val = cpu_read_reg(ctx->curr_inst->reg1) - ctx->fetched_data;

    int z = val == 0;
    int h = ((int)cpu_read_reg(ctx->curr_inst->reg1) & 0xF) - ((int)ctx->fetched_data & 0xF) < 0;//
    int c = ((int)cpu_read_reg(ctx->curr_inst->reg1)) - ((int)ctx->fetched_data) < 0;

    cpu_set_reg(ctx->curr_inst->reg1, val);
    cpu_set_flags(ctx, z, 1, h, c);
}

static void proc_sbc(cpu_context *ctx) {
    uint8_t val = ctx->fetched_data + CPU_FLAG_C;

    int z = cpu_read_reg(ctx->curr_inst->reg1) - val == 0;

    int h = ((int)cpu_read_reg(ctx->curr_inst->reg1) & 0xF) 
        - ((int)ctx->fetched_data & 0xF) - ((int)CPU_FLAG_C) < 0;
    int c = ((int)cpu_read_reg(ctx->curr_inst->reg1)) 
        - ((int)ctx->fetched_data) - ((int)CPU_FLAG_C) < 0;

    cpu_set_reg(ctx->curr_inst->reg1, cpu_read_reg(ctx->curr_inst->reg1) - val);
    cpu_set_flags(ctx, z, 1, h, c);
}

static void proc_adc(cpu_context *ctx) {
    uint16_t u = ctx->fetched_data;
    uint16_t a = ctx->regs.a;
    uint16_t c = CPU_FLAG_C;

    ctx->regs.a = (a + u + c) & 0xFF;

    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 
        (a & 0xF) + (u & 0xF) + c > 0xF,
        a + u + c > 0xFF);
}

static void proc_add(cpu_context *ctx) {//
    uint32_t val = cpu_read_reg(ctx->curr_inst->reg1) + ctx->fetched_data;

    bool is_16bit = is_16_bit(ctx->curr_inst->reg1);

    if (is_16bit) {
        emu_cycles(1);
    }

    if (ctx->curr_inst->reg1 == RT_SP) {
        val = cpu_read_reg(ctx->curr_inst->reg1) + (uint8_t)ctx->fetched_data;//
    }

    int z = (val & 0xFF) == 0;
    int h = (cpu_read_reg(ctx->curr_inst->reg1) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
    int c = (int)(cpu_read_reg(ctx->curr_inst->reg1) & 0xFF) + (int)(ctx->fetched_data & 0xFF) >= 0x100;

    if (is_16bit) {
        z = -1;
        h = (cpu_read_reg(ctx->curr_inst->reg1) & 0xFFF) + (ctx->fetched_data & 0xFFF) >= 0x1000;
        uint32_t n = ((uint32_t)cpu_read_reg(ctx->curr_inst->reg1)) + ((uint32_t)ctx->fetched_data);
        c = n >= 0x10000;
    }

    if (ctx->curr_inst->reg1 == RT_SP) {
        z = 0;
        h = (cpu_read_reg(ctx->curr_inst->reg1) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
        c = (int)(cpu_read_reg(ctx->curr_inst->reg1) & 0xFF) + (int)(ctx->fetched_data & 0xFF) >= 0x100;
    }

    cpu_set_reg(ctx->curr_inst->reg1, val & 0xFFFF);
    cpu_set_flags(ctx, z, 0, h, c);
}

static void proc_rlca(cpu_context *ctx) {
    uint8_t u = ctx->regs.a;
    bool c = (u >> 7) & 1;
    u = (u << 1) | c;
    ctx->regs.a = u;

    cpu_set_flags(ctx, 0, 0, 0, c);
}

static void proc_rrca(cpu_context *ctx) {
    uint8_t b = ctx->regs.a & 1;
    ctx->regs.a >>= 1;
    ctx->regs.a |= (b << 7);

    cpu_set_flags(ctx, 0, 0, 0, b);
}


static void proc_rla(cpu_context *ctx) {
    uint8_t u = ctx->regs.a;
    uint8_t cf = CPU_FLAG_C;
    uint8_t c = (u >> 7) & 1;

    ctx->regs.a = (u << 1) | cf;
    cpu_set_flags(ctx, 0, 0, 0, c);
}

static void proc_stop(cpu_context *ctx) {
    fprintf(stderr, "STOPPING!\n");
    exit(-2);
}

static void proc_daa(cpu_context *ctx) {
    uint8_t u = 0;
    int fc = 0;

    if (CPU_FLAG_H || (!CPU_FLAG_N && (ctx->regs.a & 0xF) > 9)) {
        u = 6;
    }

    if (CPU_FLAG_C || (!CPU_FLAG_N && ctx->regs.a > 0x99)) {
        u |= 0x60;
        fc = 1;
    }

    ctx->regs.a += CPU_FLAG_N ? -u : u;

    cpu_set_flags(ctx, ctx->regs.a == 0, -1, 0, fc);
}

static void proc_cpl(cpu_context *ctx) {
    ctx->regs.a = ~ctx->regs.a;
    cpu_set_flags(ctx, -1, 1, 1, -1);
}

static void proc_scf(cpu_context *ctx) {
    cpu_set_flags(ctx, -1, 0, 0, 1);
}

static void proc_ccf(cpu_context *ctx) {
    cpu_set_flags(ctx, -1, 0, 0, CPU_FLAG_C ^ 1);
}

static void proc_halt(cpu_context *ctx) {
    ctx->halted = true;
}

static void proc_rra(cpu_context *ctx) {
    uint8_t carry = CPU_FLAG_C;
    uint8_t new_c = ctx->regs.a & 1;

    ctx->regs.a >>= 1;
    ctx->regs.a |= (carry << 7);

    cpu_set_flags(ctx, 0, 0, 0, new_c);
}

static IN_PROC INSTRUCTIONS[256];

void init_instructions() {
    INSTRUCTIONS[IN_NONE] = proc_none;
    INSTRUCTIONS[IN_NOP] = proc_nop;
    INSTRUCTIONS[IN_LD] = proc_ld;
    INSTRUCTIONS[IN_LDH] = proc_ldh;
    INSTRUCTIONS[IN_JP] = proc_jp;
    INSTRUCTIONS[IN_DI] = proc_di;
    INSTRUCTIONS[IN_POP] = proc_pop;
    INSTRUCTIONS[IN_PUSH] = proc_push;
    INSTRUCTIONS[IN_JR] = proc_jr;
    INSTRUCTIONS[IN_CALL] = proc_call;
    INSTRUCTIONS[IN_RET] = proc_ret;
    INSTRUCTIONS[IN_RST] = proc_rst;
    INSTRUCTIONS[IN_DEC] = proc_dec;
    INSTRUCTIONS[IN_INC] = proc_inc;
    INSTRUCTIONS[IN_ADD] = proc_add;
    INSTRUCTIONS[IN_ADC] = proc_adc;
    INSTRUCTIONS[IN_SUB] = proc_sub;
    INSTRUCTIONS[IN_SBC] = proc_sbc;
    INSTRUCTIONS[IN_AND] = proc_and;
    INSTRUCTIONS[IN_XOR] = proc_xor;
    INSTRUCTIONS[IN_OR] = proc_or;
    INSTRUCTIONS[IN_CP] = proc_cp;
    INSTRUCTIONS[IN_CB] = proc_cb;
    INSTRUCTIONS[IN_RRCA] = proc_rrca;
    INSTRUCTIONS[IN_RLCA] = proc_rlca;
    INSTRUCTIONS[IN_RRA] = proc_rra;
    INSTRUCTIONS[IN_RLA] = proc_rla;
    INSTRUCTIONS[IN_STOP] = proc_stop;
    INSTRUCTIONS[IN_HALT] = proc_halt;
    INSTRUCTIONS[IN_DAA] = proc_daa;
    INSTRUCTIONS[IN_CPL] = proc_cpl;
    INSTRUCTIONS[IN_SCF] = proc_scf;
    INSTRUCTIONS[IN_CCF] = proc_ccf;
    INSTRUCTIONS[IN_EI] = proc_ei;
    INSTRUCTIONS[IN_RETI] = proc_reti;
}

IN_PROC inst_get_processor(inst_type type) {
    return INSTRUCTIONS[type];
}