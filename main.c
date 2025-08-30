// for accessing uint16_t
#include <stdint.h>
#include <stdio.h>

// this will be equal to 2 ^ 16
#define MEMORY_MAX (1 << 16)

// enum for registers, LC3 has 10 registers, each can hold 16 bit unsigned integer
enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,   /* Program counter */
    R_COND, /* Conditional register */
    R_COUNT
};

// creating the memory
// uint16_t -> unsigned int of 16 bits
// LC3 has 65,536(2^16) memory locations each of size 16 bits
uint16_t memory[MEMORY_MAX];

// storing registers in array
uint16_t reg[R_COUNT];

// enum for conditional flags
// lC3 has 3 confitional flags
enum
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

// enum for opcodes
// LC3 has 16 opcodes
// instruction(16 bits) -> opcode(first 4 bits) + parameters(rest 12 bits)
enum
{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

// extends lower bit integer to 16 bits for performing task
uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1)
    {
        x |= (0xFFFF << bit_count);
    }

    return x;
}

void update_flags(uint16_t r)
{
    if (reg[r] == 0)
    {
        reg[R_COND] = FL_ZRO;
    }
    else if (reg[r] >> 15)
    {
        reg[R_COND] = FL_NEG;
    }
    else
    {
        reg[R_COND] = FL_POS;
    }
}

int main(int argc, const char *argv[])
{
    // loading arguments
    if (argc < 2)
    {
        /* showcase the correct usage*/
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }
    else
    {
        // check if all the files can be accessed
        for (int i = 1; i < argc; i++)
        {
            if (!read_image(argv[i]))
            {
                printf("failed to load image: %s\n", argv[i]);
                exit(1);
            }
        }
    }

    /* since exactly one condition flag should be set at any given time, set the Z flag */
    reg[R_COND] = FL_ZRO;

    // In LC3, all the user programs are by default
    // stored from the memory location 0x3000 and
    // onwards. Initial memory loactions are used
    // to store os, system related stuff
    enum
    {
        PC_START = 0x3000 /* hexadecimal value */
    };
    reg[R_PC] = PC_START;

    int running = 1;

    while (running)
    {
        // fetch the instruction
        uint16_t instr = mem_read(reg[R_PC]++);
        // get the opcode
        // instruction 16 bits,
        // opcode will be first 4 bits
        uint16_t op = instr >> 12;

        switch (op)
        {
        case OP_ADD:
        {
            // get the destination register
            uint16_t r0 = (instr >> 9) & 0x7;
            // get the first operand register
            uint16_t r1 = (instr >> 6) & 0x7;

            // check the mode (register or immediate)
            if ((instr >> 5) & 0x1)
            {
                // extract the embedded value
                uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                // perform the operation
                reg[r0] = reg[r1] + imm5;
            }
            else
            {
                // get the second register
                uint16_t r2 = instr & 0x7;
                reg[0] = reg[r1] + reg[r2];
            }

            update_flags(r0);
            break;
        }
        case OP_AND:
        {
            // fetch destination register
            uint16_t r0 = (instr >> 9) & 0x7;
            // fetch the 1st operand's register
            uint16_t r1 = (instr >> 6) & 0x7;
            // fetch the mode
            uint16_t mode = (instr >> 5) & 0x1;

            if (mode)
            {
                uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                reg[r0] = reg[r1] & imm5;
            }
            else
            {
                uint16_t r2 = instr & 0x7;
                reg[r0] = reg[r1] & reg[r2];
            }

            update_flags(r0);
            break;
        }
        case OP_NOT:
        {
            // desttination register
            uint16_t r0 = (instr >> 9) & 0x7;
            // source register
            uint16_t r1 = (instr >> 6) & 0x7;

            reg[r0] = ~reg[r1];
            update_flags(r0);
            break;
        }
        case OP_BR:
        {
            // for conditional branching
            uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
            uint16_t cond_flag = (instr >> 9) & 0x7;

            if (cond_flag & reg[R_COND])
            {
                // update pc to fetch instructions from the new loaction
                reg[R_PC] += pc_offset;
            }
            break;
        }
        case OP_JMP:
        {
            // to jump unconditionally to the location specified by base register
            // bits -> [8:6] represent base register
            // this code also handle RET opcode
            // fetch base register
            uint16_t r1 = (instr >> 6) & 0x7;
            reg[R_PC] = reg[r1];
            break;
        }
        case OP_JSR:
        {
            // for handling subroutines(other functions) calls
            // first store the current pc value to R7(register 7)
            // then if the base register (bit[11]) if set, get the
            // address of subroutine from the instruction (bits[10:0])
            // else get it from the base register(bits[8:6])
            uint16_t flag = (instr >> 11) & 1;
            reg[R_R7] = reg[R_PC];

            if (flag)
            {
                uint16_t pc_offset = sign_extend(instr & 0x7FF, 11);
                reg[R_PC] += pc_offset; /* JSR */
            }
            else
            {
                uint16_t r1 = (instr >> 6) & 0x7;
                reg[R_PC] = reg[r1]; /* JSRR */
            }
            break;
        }
        case OP_LD:
        {
            // load the destination register
            uint16_t r0 = (instr >> 9) & 0x7;
            // get the pc offset
            uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
            // read the content from the memory by adding current pc + pc_offset
            reg[r0] = mem_read(reg[R_PC] + pc_offset);
            update_flags(r0);
            break;
        }
        case OP_LDI:
        {
            // fetch the destination register
            uint16_t r0 = (instr >> 9) & 0x7;
            // fetch the PCoffset9
            uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);

            // add the pc_offset with the PC value to get
            // the intermediate address that holds the final
            // address
            reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
            update_flags(r0);
            break;
        }
        case OP_LDR:
        {
            // LDR -> load register
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t r1 = (instr >> 6) & 0x7;
            uint16_t offset = sign_extend(instr & 0x3F, 6);
            reg[r0] = mem_read(reg[r1] + offset);

            update_flags(r0);
            break;
        }
        case OP_LEA:
        {
            // LEA -> Load Effective Address
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
            reg[r0] = reg[R_PC] + pc_offset;
            update_flags(r0);
            break;
        }
        case OP_ST:
        {
            // ST -> Store
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
            mem_write(reg[R_PC] + pc_offset, reg[r0]);
            break;
        }
        case OP_STI:
        {
            // STI -> store indirect
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
            mem_write(mem_read(reg[R_PC] + pc_offset), reg[r0]);
            break;
        }
        case OP_STR:
            break;
        case OP_TRAP:
            break;
        case OP_RES:
        case OP_RTI:
            abort();
        default:
            break;
        }
    }

    return 0;
}