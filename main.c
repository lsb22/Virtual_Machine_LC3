#include <stdint.h>          // for accessing uint16_t
#define MEMORY_MAX (1 << 16) // this will be equal to 2 ^ 16

enum
{
    R_R0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, // Program counter
    R_COND,
    R_COUNT
};

// creating the memory, uint16_t -> unsigned int of 16 bits
uint16_t memory[MEMORY_MAX];

// declaring the registers
// LC3 has 10 registers, each can hold 16 bit unsigned integer
uint16_t reg[R_COUNT];