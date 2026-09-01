#pragma once
#include <common.h>

enum addr_mode {
    AM_IMP = 0,
    AM_R_D16 = 1,
    AM_R_R = 2,
    AM_MR_R = 3,
    AM_R = 4,
    AM_R_D8 = 5,
    AM_R_MR = 6,
    AM_R_HLI = 8,
    AM_R_HLD = 9,
    AM_HLI_R = 10,
    AM_HLD_R = 11,
    AM_R_A8 = 12,
    AM_A8_R = 13,
    AM_HL_SPR = 14,
    AM_D16 = 15,
    AM_D8 = 16,
    AM_D16_R = 17,
    AM_MR_D8 = 18,
    AM_MR = 19,
    AM_A16_R = 20,
    AM_R_A16 = 21
};

enum reg_type {
    RT_NONE = 0,
    RT_A = 1,
    RT_F = 2,
    RT_B = 3,
    RT_C = 4,
    RT_D = 5,
    RT_E = 6,
    RT_H = 7,
    RT_L = 8,
    RT_AF = 9,
    RT_BC = 10,
    RT_DE = 11,
    RT_HL = 12,
    RT_SP = 13,
    RT_PC = 14
};

enum inst_type {
    IN_NONE = 0,
    IN_NOP = 1,
    IN_LD = 2,
    IN_INC = 3,
    IN_DEC = 4,
    IN_RLCA = 5,
    IN_ADD = 6,
    IN_RRCA = 7,
    IN_STOP = 8,
    IN_RLA = 9,
    IN_JR = 10,
    IN_RRA = 11,
    IN_DAA = 12,
    IN_CPL = 13,
    IN_SCF = 14,
    IN_CCF = 15,
    IN_HALT = 16,
    IN_ADC = 17,
    IN_SUB = 18,
    IN_SBC = 19,
    IN_AND = 20,
    IN_XOR = 21,
    IN_OR = 22,
    IN_CP = 23,
    IN_POP = 24,
    IN_JP = 25,
    IN_PUSH = 26,
    IN_RET = 27, 
    IN_CB = 28,
    IN_CALL = 29,
    IN_RETI = 30,
    IN_LDH = 31,
    IN_JPHL = 32,
    IN_DI = 33,
    IN_EI = 34,
    IN_RST = 35,
    IN_ERR = 36,
    //CB instructions...
    IN_RLC = 37, 
    IN_RRC = 38,
    IN_RL = 39, 
    IN_RR = 40,
    IN_SLA = 41, 
    IN_SRA = 42,
    IN_SWAP = 43, 
    IN_SRL = 44,
    IN_BIT = 45,
    IN_RES = 46,
    IN_SET = 47
};

enum condition_type {
    CT_NONE = 0,
    CT_NZ = 1,
    CT_Z = 2,
    CT_NC = 3,
    CT_C = 4
};

class instruction {
    public:
        inst_type type;
        addr_mode mode;
        reg_type reg1;
        reg_type reg2;
        condition_type condition;
        uint8_t param;
};

instruction *instruction_by_opcode(uint8_t opcode);

const char *inst_name(inst_type t);