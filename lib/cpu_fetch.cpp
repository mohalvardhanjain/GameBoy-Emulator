#include <cpu.h>
#include <bus.h>
#include <emu.h>

extern cpu_context ctx;

void fetch_data() {
    ctx.memory_dest = 0;
    ctx.is_dest_mem = false;

    if(ctx.curr_inst == NULL) {
        return;
    }

    switch(ctx.curr_inst->mode) {
        case AM_IMP : return;

        case AM_R : 
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->reg1);
            return;

        case AM_R_R : 
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->reg2);
            return;

        case AM_R_D8 : 
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;
        
        case AM_R_D16: {
            uint16_t lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            uint16_t hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);

            ctx.fetched_data = lo + (hi << 8);
            ctx.regs.pc += 2;
            return;
        }

        case AM_D16 : {
            uint16_t lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            uint16_t hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);

            ctx.fetched_data = lo + (hi << 8);
            ctx.regs.pc += 2;
            return;
        }

        case AM_MR_R : 
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->reg2);
            ctx.memory_dest = cpu_read_reg(ctx.curr_inst->reg1);
            ctx.is_dest_mem = true;

            if(ctx.curr_inst->reg1 == RT_C) {
                ctx.memory_dest |= 0xFF00;
            } 
            
            return;

        case AM_R_MR : {
            uint16_t addr = cpu_read_reg(ctx.curr_inst->reg2);

            if(ctx.curr_inst->reg2 == RT_C) {
                addr |= 0xFF00;
            }

            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);
        } return;

        case AM_R_HLI : 
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.curr_inst->reg2));
            emu_cycles(1);
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;

        case AM_R_HLD : 
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.curr_inst->reg2));
            emu_cycles(1);
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;

        case AM_HLI_R : 
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->reg2);
            ctx.memory_dest = cpu_read_reg(ctx.curr_inst->reg1);
            ctx.is_dest_mem = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;

        case AM_HLD_R : 
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->reg2);
            ctx.memory_dest = cpu_read_reg(ctx.curr_inst->reg1);
            ctx.is_dest_mem = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;

        case AM_R_A8 : 
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case AM_A8_R : 
            ctx.memory_dest = bus_read(ctx.regs.pc) | 0xFF00;
            ctx.is_dest_mem = true;
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case AM_HL_SPR : 
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case AM_D8 : 
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case AM_A16_R : {
            uint16_t lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            uint16_t hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);

            ctx.memory_dest = lo | (hi << 8);
            ctx.is_dest_mem = true;

            ctx.regs.pc += 2;
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->reg2);
        } return;

        case AM_D16_R : {

            uint16_t lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            uint16_t hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);

            ctx.memory_dest = lo | (hi << 8);
            ctx.is_dest_mem = true;

            ctx.regs.pc += 2;
            ctx.fetched_data = cpu_read_reg(ctx.curr_inst->reg2);
        } return;

        case AM_MR_D8 : 
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            ctx.memory_dest = cpu_read_reg(ctx.curr_inst->reg1);
            ctx.is_dest_mem = true;
            return;

        case AM_MR : 
            ctx.memory_dest = cpu_read_reg(ctx.curr_inst->reg1);
            ctx.is_dest_mem = true;
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.curr_inst->reg1));
            emu_cycles(1);
            return;

        case AM_R_A16 : {
            uint16_t lo = bus_read(ctx.regs.pc);
            emu_cycles(1);

            uint16_t hi = bus_read(ctx.regs.pc + 1);
            emu_cycles(1);

            uint16_t addr = lo | (hi << 8);

            ctx.regs.pc += 2;
            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);
        } return;

        default : 
            std::cout << "Unknown Addressing MOde !! " << ctx.curr_inst->mode << " (" << ctx.curr_opcode << ") \n";
            exit(-7);
            return;


    }


}