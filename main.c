// for accessing uint16_t
#include <stdint.h>

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

// enum for conditional flags
// lC3 has 3 confitional flags
enum
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};