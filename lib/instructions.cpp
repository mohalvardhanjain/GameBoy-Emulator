#include <instructions.h>
#include <cpu.h>
#include <vector>

#include <cstdio>
#include <cstdint>

#include <bus.h>

std::vector<instruction> instructions = {
    {IN_NOP, AM_IMP},
    {IN_LD, AM_R_D16, RT_BC},
    {IN_LD, AM_MR_R, RT_BC, RT_A},
    {IN_INC, AM_R, RT_BC},
    {IN_INC, AM_R, RT_B},
    {IN_DEC, AM_R, RT_B},
    {IN_LD, AM_R_D8, RT_B},
    {IN_RLCA},
    {IN_LD, AM_A16_R, RT_NONE, RT_SP},
    {IN_ADD, AM_R_R, RT_HL, RT_BC},
    {IN_LD, AM_R_MR, RT_A, RT_BC},
    {IN_DEC, AM_R, RT_BC},
    {IN_INC, AM_R, RT_C},
    {IN_DEC, AM_R, RT_C},
    {IN_LD, AM_R_D8, RT_C},
    {IN_RRCA},

    {IN_STOP},
    {IN_LD, AM_R_D16, RT_DE},
    {IN_LD, AM_MR_R, RT_DE, RT_A},
    {IN_INC, AM_R, RT_DE},
    {IN_INC, AM_R, RT_D},
    {IN_DEC, AM_R, RT_D},
    {IN_LD, AM_R_D8, RT_D},
    {IN_RLA},
    {IN_JR, AM_D8},
    {IN_ADD, AM_R_R, RT_HL, RT_DE},
    {IN_LD, AM_R_MR, RT_A, RT_DE},
    {IN_DEC, AM_R, RT_DE},
    {IN_INC, AM_R, RT_E},
    {IN_DEC, AM_R, RT_E},
    {IN_LD, AM_R_D8, RT_E},
    {IN_RRA},

    {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ},
    {IN_LD, AM_R_D16, RT_HL},
    {IN_LD, AM_HLI_R, RT_HL, RT_A},
    {IN_INC, AM_R, RT_HL},
    {IN_INC, AM_R, RT_H},
    {IN_DEC, AM_R, RT_H},
    {IN_LD, AM_R_D8, RT_H},
    {IN_DAA},
    {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_Z},
    {IN_ADD, AM_R_R, RT_HL, RT_HL},
    {IN_LD, AM_R_HLI, RT_A, RT_HL},
    {IN_DEC, AM_R, RT_HL},
    {IN_INC, AM_R, RT_L},
    {IN_DEC, AM_R, RT_L},
    {IN_LD, AM_R_D8, RT_L},
    {IN_CPL},

    {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_NC},
    {IN_LD, AM_R_D16, RT_SP},
    {IN_LD, AM_HLD_R, RT_HL,  RT_A},
    {IN_INC, AM_R, RT_SP},
    {IN_INC, AM_MR, RT_HL},
    {IN_DEC, AM_MR, RT_HL},
    {IN_LD,  AM_MR_D8, RT_HL},
    {IN_SCF},
    {IN_JR, AM_D8, RT_NONE, RT_NONE, CT_C},
    {IN_ADD, AM_R_R,  RT_HL,  RT_SP},
    {IN_LD,  AM_R_HLD, RT_A,  RT_HL},
    {IN_DEC, AM_R, RT_SP},
    {IN_INC, AM_R, RT_A},
    {IN_DEC, AM_R, RT_A},
    {IN_LD,  AM_R_D8, RT_A},
    {IN_CCF},

    {IN_LD, AM_R_R, RT_B, RT_B},
    {IN_LD, AM_R_R, RT_B, RT_C},
    {IN_LD, AM_R_R, RT_B, RT_D},
    {IN_LD, AM_R_R, RT_B, RT_E},
    {IN_LD, AM_R_R, RT_B, RT_H},
    {IN_LD, AM_R_R, RT_B, RT_L},
    {IN_LD, AM_R_MR, RT_B, RT_HL},
    {IN_LD, AM_R_R, RT_B, RT_A},
    {IN_LD, AM_R_R, RT_C, RT_B},
    {IN_LD, AM_R_R, RT_C, RT_C},
    {IN_LD, AM_R_R, RT_C, RT_D},
    {IN_LD, AM_R_R, RT_C, RT_E},
    {IN_LD, AM_R_R, RT_C, RT_H},
    {IN_LD, AM_R_R, RT_C, RT_L},
    {IN_LD, AM_R_MR, RT_C, RT_HL},
    {IN_LD, AM_R_R, RT_C, RT_A},

    {IN_LD, AM_R_R, RT_D, RT_B},
    {IN_LD, AM_R_R, RT_D, RT_C},
    {IN_LD, AM_R_R, RT_D, RT_D},
    {IN_LD, AM_R_R, RT_D, RT_E},
    {IN_LD, AM_R_R, RT_D, RT_H},
    {IN_LD, AM_R_R, RT_D, RT_L},
    {IN_LD, AM_R_MR, RT_D, RT_HL},
    {IN_LD, AM_R_R, RT_D, RT_A},
    {IN_LD, AM_R_R, RT_E, RT_B},
    {IN_LD, AM_R_R, RT_E, RT_C},
    {IN_LD, AM_R_R, RT_E, RT_D},
    {IN_LD, AM_R_R, RT_E, RT_E},
    {IN_LD, AM_R_R, RT_E, RT_H},
    {IN_LD, AM_R_R, RT_E, RT_L},
    {IN_LD, AM_R_MR, RT_E, RT_HL},
    {IN_LD, AM_R_R, RT_E, RT_A},

    {IN_LD, AM_R_R, RT_H, RT_B},
    {IN_LD, AM_R_R, RT_H, RT_C},
    {IN_LD, AM_R_R, RT_H, RT_D},
    {IN_LD, AM_R_R, RT_H, RT_E},
    {IN_LD, AM_R_R, RT_H, RT_H},
    {IN_LD, AM_R_R, RT_H, RT_L},
    {IN_LD, AM_R_MR, RT_H, RT_HL},
    {IN_LD, AM_R_R, RT_H, RT_A},
    {IN_LD, AM_R_R, RT_L, RT_B},
    {IN_LD, AM_R_R, RT_L, RT_C},
    {IN_LD, AM_R_R, RT_L, RT_D},
    {IN_LD, AM_R_R, RT_L, RT_E},
    {IN_LD, AM_R_R, RT_L, RT_H},
    {IN_LD, AM_R_R, RT_L, RT_L},
    {IN_LD, AM_R_MR, RT_L, RT_HL},
    {IN_LD, AM_R_R, RT_L, RT_A},

    {IN_LD, AM_MR_R, RT_HL, RT_B},
    {IN_LD, AM_MR_R, RT_HL, RT_C},
    {IN_LD, AM_MR_R, RT_HL, RT_D},
    {IN_LD, AM_MR_R, RT_HL, RT_E},
    {IN_LD, AM_MR_R, RT_HL, RT_H},
    {IN_LD, AM_MR_R, RT_HL, RT_L},
    {IN_HALT},
    {IN_LD, AM_MR_R, RT_HL, RT_A},
    {IN_LD, AM_R_R, RT_A, RT_B},
    {IN_LD, AM_R_R, RT_A, RT_C},
    {IN_LD, AM_R_R, RT_A, RT_D},
    {IN_LD, AM_R_R, RT_A, RT_E},
    {IN_LD, AM_R_R, RT_A, RT_H},
    {IN_LD, AM_R_R, RT_A, RT_L},
    {IN_LD, AM_R_MR, RT_A, RT_HL},
    {IN_LD, AM_R_R, RT_A, RT_A},

    {IN_ADD, AM_R_R, RT_A, RT_B},
    {IN_ADD, AM_R_R, RT_A, RT_C},
    {IN_ADD, AM_R_R, RT_A, RT_D},
    {IN_ADD, AM_R_R, RT_A, RT_E},
    {IN_ADD, AM_R_R, RT_A, RT_H},
    {IN_ADD, AM_R_R, RT_A, RT_L},
    {IN_ADD, AM_R_MR, RT_A, RT_HL},
    {IN_ADD, AM_R_R, RT_A, RT_A},
    {IN_ADC, AM_R_R, RT_A, RT_B},
    {IN_ADC, AM_R_R, RT_A, RT_C},
    {IN_ADC, AM_R_R, RT_A, RT_D},
    {IN_ADC, AM_R_R, RT_A, RT_E},
    {IN_ADC, AM_R_R, RT_A, RT_H},
    {IN_ADC, AM_R_R, RT_A, RT_L},
    {IN_ADC, AM_R_MR, RT_A, RT_HL},
    {IN_ADC, AM_R_R, RT_A, RT_A},

    {IN_SUB, AM_R_R, RT_A, RT_B},
    {IN_SUB, AM_R_R, RT_A, RT_C},
    {IN_SUB, AM_R_R, RT_A, RT_D},
    {IN_SUB, AM_R_R, RT_A, RT_E},
    {IN_SUB, AM_R_R, RT_A, RT_H},
    {IN_SUB, AM_R_R, RT_A, RT_L},
    {IN_SUB, AM_R_MR, RT_A, RT_HL},
    {IN_SUB, AM_R_R, RT_A, RT_A},
    {IN_SBC, AM_R_R, RT_A, RT_B},
    {IN_SBC, AM_R_R, RT_A, RT_C},
    {IN_SBC, AM_R_R, RT_A, RT_D},
    {IN_SBC, AM_R_R, RT_A, RT_E},
    {IN_SBC, AM_R_R, RT_A, RT_H},
    {IN_SBC, AM_R_R, RT_A, RT_L},
    {IN_SBC, AM_R_MR, RT_A, RT_HL},
    {IN_SBC, AM_R_R, RT_A, RT_A},

    {IN_AND, AM_R_R, RT_A, RT_B},
    {IN_AND, AM_R_R, RT_A, RT_C},
    {IN_AND, AM_R_R, RT_A, RT_D},
    {IN_AND, AM_R_R, RT_A, RT_E},
    {IN_AND, AM_R_R, RT_A, RT_H},
    {IN_AND, AM_R_R, RT_A, RT_L},
    {IN_AND, AM_R_MR, RT_A, RT_HL},
    {IN_AND, AM_R_R, RT_A, RT_A},
    {IN_XOR, AM_R_R, RT_A, RT_B},
    {IN_XOR, AM_R_R, RT_A, RT_C},
    {IN_XOR, AM_R_R, RT_A, RT_D},
    {IN_XOR, AM_R_R, RT_A, RT_E},
    {IN_XOR, AM_R_R, RT_A, RT_H},
    {IN_XOR, AM_R_R, RT_A, RT_L},
    {IN_XOR, AM_R_MR, RT_A, RT_HL},
    {IN_XOR, AM_R_R, RT_A, RT_A},

    {IN_OR, AM_R_R, RT_A, RT_B},
    {IN_OR, AM_R_R, RT_A, RT_C},
    {IN_OR, AM_R_R, RT_A, RT_D},
    {IN_OR, AM_R_R, RT_A, RT_E},
    {IN_OR, AM_R_R, RT_A, RT_H},
    {IN_OR, AM_R_R, RT_A, RT_L},
    {IN_OR, AM_R_MR, RT_A, RT_HL},
    {IN_OR, AM_R_R, RT_A, RT_A},
    {IN_CP, AM_R_R, RT_A, RT_B},
    {IN_CP, AM_R_R, RT_A, RT_C},
    {IN_CP, AM_R_R, RT_A, RT_D},
    {IN_CP, AM_R_R, RT_A, RT_E},
    {IN_CP, AM_R_R, RT_A, RT_H},
    {IN_CP, AM_R_R, RT_A, RT_L},
    {IN_CP, AM_R_MR, RT_A, RT_HL},
    {IN_CP, AM_R_R, RT_A, RT_A},

    {IN_RET,  AM_IMP, RT_NONE, RT_NONE, CT_NZ},
    {IN_POP,  AM_R,   RT_BC},
    {IN_JP,   AM_D16, RT_NONE, RT_NONE, CT_NZ},
    {IN_JP,   AM_D16},
    {IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_NZ},
    {IN_PUSH, AM_R,   RT_BC},
    {IN_ADD,  AM_R_D8, RT_A},
    {IN_RST,  AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x00},
    {IN_RET,  AM_IMP, RT_NONE, RT_NONE, CT_Z},
    {IN_RET},
    {IN_JP,   AM_D16, RT_NONE, RT_NONE, CT_Z},
    {IN_CB,   AM_D8},
    {IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_Z},
    {IN_CALL, AM_D16},
    {IN_ADC,  AM_R_D8, RT_A},
    {IN_RST,  AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x08},

    {IN_RET,  AM_IMP, RT_NONE, RT_NONE, CT_NC},
    {IN_POP,  AM_R,   RT_DE},
    {IN_JP,   AM_D16, RT_NONE, RT_NONE, CT_NC},
    {IN_NONE},
    {IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_NC},
    {IN_PUSH, AM_R,   RT_DE},
    {IN_SUB,  AM_R_D8, RT_A},
    {IN_RST,  AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x10},
    {IN_RET,  AM_IMP, RT_NONE, RT_NONE, CT_C},
    {IN_RETI},
    {IN_JP,   AM_D16, RT_NONE, RT_NONE, CT_C},
    {IN_NONE},
    {IN_CALL, AM_D16, RT_NONE, RT_NONE, CT_C},
    {IN_NONE},
    {IN_SBC,  AM_R_D8, RT_A},
    {IN_RST,  AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x18},

    {IN_LDH,  AM_A8_R, RT_NONE, RT_A},
    {IN_POP, AM_R, RT_HL},
    {IN_LD, AM_MR_R, RT_C, RT_A},
    {IN_NONE},
    {IN_NONE},
    {IN_PUSH, AM_R, RT_HL},
    {IN_AND, AM_R_D8, RT_A},
    {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x20},
    {IN_ADD, AM_R_D8, RT_SP},//
    {IN_JP, AM_R, RT_HL},
    {IN_LD, AM_A16_R, RT_NONE, RT_A},
    {IN_NONE},
    {IN_NONE},
    {IN_NONE},
    {IN_XOR, AM_R_D8, RT_A},
    {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x28},
    
    {IN_LDH, AM_R_A8, RT_A},
    {IN_POP, AM_R, RT_AF},
    {IN_LD, AM_R_MR, RT_A, RT_C},
    {IN_DI},
    {IN_NONE},
    {IN_PUSH, AM_R, RT_AF},
    {IN_OR, AM_R_D8, RT_A},
    {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x30},
    {IN_LD, AM_HL_SPR, RT_HL, RT_SP},
    {IN_LD, AM_R_R, RT_SP, RT_HL},
    {IN_LD, AM_R_A16, RT_A},
    {IN_EI},
    {IN_NONE},
    {IN_NONE},
    {IN_CP, AM_R_D8, RT_A},
    {IN_RST, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0x38}
};

instruction* instruction_by_opcode(uint8_t opcode){
    return &instructions[opcode];
}


static const char *inst_lookup[] = {
    "<NONE>",
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RLCA",
    "ADD",
    "RRCA",
    "STOP",
    "RLA",
    "JR",
    "RRA",
    "DAA",
    "CPL",
    "SCF",
    "CCF",
    "HALT",
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "XOR",
    "OR",
    "CP",
    "POP",
    "JP",
    "PUSH",
    "RET",
    "CB",
    "CALL",
    "RETI",
    "LDH",
    "JPHL",
    "DI",
    "EI",
    "RST",
    "IN_ERR",
    "IN_RLC",
    "IN_RRC",
    "IN_RL",
    "IN_RR",
    "IN_SLA",
    "IN_SRA",
    "IN_SWAP",
    "IN_SRL",
    "IN_BIT",
    "IN_RES",
    "IN_SET"
};

const char *inst_name(inst_type t) {
    const std::size_t index = static_cast<std::size_t>(t);

    if (index >= sizeof(inst_lookup) / sizeof(inst_lookup[0])) {
        return "<INVALID>";
    }

    return inst_lookup[index];
}


static const char *rt_lookup[] = {
    "<NONE>",
    "A",
    "F",
    "B",
    "C",
    "D",
    "E",
    "H",
    "L",
    "AF",
    "BC",
    "DE",
    "HL",
    "SP",
    "PC"
};

void inst_to_str(cpu_context *ctx, char *str) {
    instruction *inst = ctx->curr_inst;

    switch (inst->mode)
    {
        case AM_IMP:
            std::snprintf(
                str,
                64,
                "%s",
                inst_name(inst->type)
            );
            return;

        case AM_R_D16:
        case AM_R_A16:
            std::snprintf(
                str,
                64,
                "%s %s,$%04X",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                ctx->fetched_data
            );
            return;

        case AM_R:
            std::snprintf(
                str,
                64,
                "%s %s",
                inst_name(inst->type),
                rt_lookup[inst->reg1]
            );
            return;

        case AM_R_R:
            std::snprintf(
                str,
                64,
                "%s %s,%s",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                rt_lookup[inst->reg2]
            );
            return;

        case AM_MR_R:
            std::snprintf(
                str,
                64,
                "%s (%s),%s",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                rt_lookup[inst->reg2]
            );
            return;

        case AM_MR:
            std::snprintf(
                str,
                64,
                "%s (%s)",
                inst_name(inst->type),
                rt_lookup[inst->reg1]
            );
            return;

        case AM_R_MR:
            std::snprintf(
                str,
                64,
                "%s %s,(%s)",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                rt_lookup[inst->reg2]
            );
            return;

        case AM_R_D8:
        case AM_R_A8:
            std::snprintf(
                str,
                64,
                "%s %s,$%02X",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                ctx->fetched_data & 0xFF
            );
            return;

        case AM_R_HLI:
            std::snprintf(
                str,
                64,
                "%s %s,(%s+)",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                rt_lookup[inst->reg2]
            );
            return;

        case AM_R_HLD:
            std::snprintf(
                str,
                64,
                "%s %s,(%s-)",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                rt_lookup[inst->reg2]
            );
            return;

        case AM_HLI_R:
            std::snprintf(
                str,
                64,
                "%s (%s+),%s",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                rt_lookup[inst->reg2]
            );
            return;

        case AM_HLD_R:
            std::snprintf(
                str,
                64,
                "%s (%s-),%s",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                rt_lookup[inst->reg2]
            );
            return;

        case AM_A8_R:
            std::snprintf(
                str,
                64,
                "%s $%02X,%s",
                inst_name(inst->type),
                bus_read(ctx->regs.pc - 1),
                rt_lookup[inst->reg2]
            );
            return;

        case AM_HL_SPR:
            std::snprintf(
                str,
                64,
                "%s (%s),SP+%d",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                ctx->fetched_data & 0xFF
            );
            return;

        case AM_D8:
            std::snprintf(
                str,
                64,
                "%s $%02X",
                inst_name(inst->type),
                ctx->fetched_data & 0xFF
            );
            return;

        case AM_D16:
            std::snprintf(
                str,
                64,
                "%s $%04X",
                inst_name(inst->type),
                ctx->fetched_data
            );
            return;

        case AM_MR_D8:
            std::snprintf(
                str,
                64,
                "%s (%s),$%02X",
                inst_name(inst->type),
                rt_lookup[inst->reg1],
                ctx->fetched_data & 0xFF
            );
            return;

        case AM_A16_R:
            std::snprintf(
                str,
                64,
                "%s ($%04X),%s",
                inst_name(inst->type),
                ctx->fetched_data,
                rt_lookup[inst->reg2]
            );
            return;

        default:
            std::fprintf(
                stderr,
                "INVALID AM: %d\n",
                static_cast<int>(inst->mode)
            );

            return;
    }
}