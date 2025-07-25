/*
 *
 *      6502.h
 *      6502 processor header file
 *
 *      2024/12/13 By MicroFish
 *      Based on MIT open source agreement
 *      Copyright © 2020 ViudiraTech, based on the MIT agreement.
 *
 */

#ifndef INCLUDE_6502_H_
#define INCLUDE_6502_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CPU_FREQ      4e6    // Processor frequency
#define STEP_DURATION 10e6   // Duration of each clock cycle
#define ONE_SECOND    1e9    // Number of nanoseconds in a second
#define NUM_MODES     14     // Number of instruction modes
#define NMI_VEC       0xFFFA // Non-maskable interrupt vector address
#define RST_VEC       0xFFFC // Reset interrupt vector address
#define IRQ_VEC       0xFFFE // Maskable interrupt vector address

/* Processor Status Bits */
struct StatusBits {
        bool carry     : 1;
        bool zero      : 1;
        bool interrupt : 1;
        bool decimal   : 1;
        bool brk       : 1;
        bool unused    : 1;
        bool overflow  : 1;
        bool sign      : 1;
};

/* Processor Status Register  */
union StatusReg {
        struct StatusBits bits;
        uint8_t           byte;
};

/* Instruction addressing modes */
typedef enum { ACC, ABS, ABSX, ABSY, IMM, IMPL, IND, XIND, INDY, REL, ZP, ZPX, ZPY, JMP_IND_BUG } Mode;

/* Instruction structure */
typedef struct {
        const char *mnemonic;
        void (*function)();
        Mode    mode;
        uint8_t cycles;
} Instruction;

/* CPU structure */
typedef struct {
        uint8_t         memory[1 << 16];
        uint8_t         A;
        uint8_t         X;
        uint8_t         Y;
        uint16_t        PC;
        uint8_t         SP;
        uint8_t         extra_cycles;
        uint64_t        total_cycles;
        union StatusReg SR;
} CPUMAP;

/* Addressing mode length */
static const int lengths[NUM_MODES] = {[ACC] = 1,  [ABS] = 3,  [ABSX] = 3, [ABSY] = 3, [IMM] = 2, [IMPL] = 1, [IND] = 3,
                                       [XIND] = 2, [INDY] = 2, [REL] = 2,  [ZP] = 2,   [ZPX] = 2, [ZPY] = 2,  [JMP_IND_BUG] = 3};

#ifndef INCLUDE

/* Access memory according to different addressing modes */
static uint8_t *get_ACC(void);
static uint8_t *get_ABS(void);
static uint8_t *get_ABSX(void);
static uint8_t *get_ABSY(void);
static uint8_t *get_IMM(void);
static uint8_t *get_IMPL(void);
static uint8_t *get_IND(void);
static uint8_t *get_XIND(void);
static uint8_t *get_INDY(void);
static uint8_t *get_REL(void);
static uint8_t *get_ZP(void);
static uint8_t *get_ZPX(void);
static uint8_t *get_ZPY(void);
static uint8_t *get_JMP_IND_BUG(void);

/* 6502 instruction set */
static void inst_ADC(void);
static void inst_AND(void);
static void inst_ASL(void);
static void inst_BCC(void);
static void inst_BCS(void);
static void inst_BEQ(void);
static void inst_BIT(void);
static void inst_BMI(void);
static void inst_BNE(void);
static void inst_BPL(void);
static void inst_BRK(void);
static void inst_BVC(void);
static void inst_BVS(void);
static void inst_CLC(void);
static void inst_CLD(void);
static void inst_CLI(void);
static void inst_CLV(void);
static void inst_CMP(void);
static void inst_CPX(void);
static void inst_CPY(void);
static void inst_DEC(void);
static void inst_DEX(void);
static void inst_DEY(void);
static void inst_EOR(void);
static void inst_INC(void);
static void inst_INX(void);
static void inst_INY(void);
static void inst_JMP(void);
static void inst_JSR(void);
static void inst_LDA(void);
static void inst_LDX(void);
static void inst_LDY(void);
static void inst_LSR(void);
static void inst_NOP(void);
static void inst_ORA(void);
static void inst_PHA(void);
static void inst_PHP(void);
static void inst_PLA(void);
static void inst_PLP(void);
static void inst_ROL(void);
static void inst_ROR(void);
static void inst_RTI(void);
static void inst_RTS(void);
static void inst_SBC(void);
static void inst_SEC(void);
static void inst_SED(void);
static void inst_SEI(void);
static void inst_STA(void);
static void inst_STX(void);
static void inst_STY(void);
static void inst_TAX(void);
static void inst_TAY(void);
static void inst_TSX(void);
static void inst_TXA(void);
static void inst_TXS(void);
static void inst_TYA(void);

/* Functions for getting memory addresses in different addressing modes */
static uint8_t *(*const get_ptr[NUM_MODES])() = {[ACC] = get_ACC,   [ABS] = get_ABS,
                                                 [ABSX] = get_ABSX, [ABSY] = get_ABSY,
                                                 [IMM] = get_IMM,   [IMPL] = get_IMPL,
                                                 [IND] = get_IND,   [XIND] = get_XIND,
                                                 [INDY] = get_INDY, [REL] = get_REL,
                                                 [ZP] = get_ZP,     [ZPX] = get_ZPX,
                                                 [ZPY] = get_ZPY,   [JMP_IND_BUG] = get_JMP_IND_BUG};

/* Instruction List */
static const Instruction instructions[0x100] = {
    [0x00] = {"BRK impl",  inst_BRK, IMPL,        7},
    [0x01] = {"ORA X,ind", inst_ORA, XIND,        6},
    [0x02] = {"???",       inst_NOP, IMPL,        2},
    [0x03] = {"???",       inst_NOP, IMPL,        8},
    [0x04] = {"???",       inst_NOP, ZP,          3},
    [0x05] = {"ORA zpg",   inst_ORA, ZP,          3},
    [0x06] = {"ASL zpg",   inst_ASL, ZP,          5},
    [0x07] = {"???",       inst_NOP, IMPL,        5},
    [0x08] = {"PHP impl",  inst_PHP, IMPL,        3},
    [0x09] = {"ORA #",     inst_ORA, IMM,         2},
    [0x0A] = {"ASL A",     inst_ASL, ACC,         2},
    [0x0B] = {"???",       inst_NOP, IMPL,        2},
    [0x0C] = {"???",       inst_NOP, ABS,         4},
    [0x0D] = {"ORA abs",   inst_ORA, ABS,         4},
    [0x0E] = {"ASL abs",   inst_ASL, ABS,         6},
    [0x0F] = {"???",       inst_NOP, IMPL,        6},
    [0x10] = {"BPL rel",   inst_BPL, REL,         2},
    [0x11] = {"ORA ind,Y", inst_ORA, INDY,        5},
    [0x12] = {"???",       inst_NOP, IMPL,        2},
    [0x13] = {"???",       inst_NOP, IMPL,        8},
    [0x14] = {"???",       inst_NOP, ZP,          4},
    [0x15] = {"ORA zpg,X", inst_ORA, ZPX,         4},
    [0x16] = {"ASL zpg,X", inst_ASL, ZPX,         6},
    [0x17] = {"???",       inst_NOP, IMPL,        6},
    [0x18] = {"CLC impl",  inst_CLC, IMPL,        2},
    [0x19] = {"ORA abs,Y", inst_ORA, ABSY,        4},
    [0x1A] = {"???",       inst_NOP, IMPL,        2},
    [0x1B] = {"???",       inst_NOP, IMPL,        7},
    [0x1C] = {"???",       inst_NOP, ABSX,        4},
    [0x1D] = {"ORA abs,X", inst_ORA, ABSX,        4},
    [0x1E] = {"ASL abs,X", inst_ASL, ABSX,        7},
    [0x1F] = {"???",       inst_NOP, IMPL,        7},
    [0x20] = {"JSR abs",   inst_JSR, ABS,         6},
    [0x21] = {"AND X,ind", inst_AND, XIND,        6},
    [0x22] = {"???",       inst_NOP, IMPL,        2},
    [0x23] = {"???",       inst_NOP, IMPL,        8},
    [0x24] = {"BIT zpg",   inst_BIT, ZP,          3},
    [0x25] = {"AND zpg",   inst_AND, ZP,          3},
    [0x26] = {"ROL zpg",   inst_ROL, ZP,          5},
    [0x27] = {"???",       inst_NOP, IMPL,        5},
    [0x28] = {"PLP impl",  inst_PLP, IMPL,        4},
    [0x29] = {"AND #",     inst_AND, IMM,         2},
    [0x2A] = {"ROL A",     inst_ROL, ACC,         2},
    [0x2B] = {"???",       inst_NOP, IMPL,        2},
    [0x2C] = {"BIT abs",   inst_BIT, ABS,         4},
    [0x2D] = {"AND abs",   inst_AND, ABS,         4},
    [0x2E] = {"ROL abs",   inst_ROL, ABS,         6},
    [0x2F] = {"???",       inst_NOP, IMPL,        6},
    [0x30] = {"BMI rel",   inst_BMI, REL,         2},
    [0x31] = {"AND ind,Y", inst_AND, INDY,        5},
    [0x32] = {"???",       inst_NOP, IMPL,        2},
    [0x33] = {"???",       inst_NOP, IMPL,        8},
    [0x34] = {"???",       inst_NOP, ZP,          4},
    [0x35] = {"AND zpg,X", inst_AND, ZPX,         4},
    [0x36] = {"ROL zpg,X", inst_ROL, ZPX,         6},
    [0x37] = {"???",       inst_NOP, IMPL,        6},
    [0x38] = {"SEC impl",  inst_SEC, IMPL,        2},
    [0x39] = {"AND abs,Y", inst_AND, ABSY,        4},
    [0x3A] = {"???",       inst_NOP, IMPL,        2},
    [0x3B] = {"???",       inst_NOP, IMPL,        7},
    [0x3C] = {"???",       inst_NOP, ABSX,        4},
    [0x3D] = {"AND abs,X", inst_AND, ABSX,        4},
    [0x3E] = {"ROL abs,X", inst_ROL, ABSX,        7},
    [0x3F] = {"???",       inst_NOP, IMPL,        7},
    [0x40] = {"RTI impl",  inst_RTI, IMPL,        6},
    [0x41] = {"EOR X,ind", inst_EOR, XIND,        6},
    [0x42] = {"???",       inst_NOP, IMPL,        2},
    [0x43] = {"???",       inst_NOP, IMPL,        8},
    [0x44] = {"???",       inst_NOP, ZP,          3},
    [0x45] = {"EOR zpg",   inst_EOR, ZP,          3},
    [0x46] = {"LSR zpg",   inst_LSR, ZP,          5},
    [0x47] = {"???",       inst_NOP, IMPL,        5},
    [0x48] = {"PHA impl",  inst_PHA, IMPL,        3},
    [0x49] = {"EOR #",     inst_EOR, IMM,         2},
    [0x4A] = {"LSR A",     inst_LSR, ACC,         2},
    [0x4B] = {"???",       inst_NOP, IMPL,        2},
    [0x4C] = {"JMP abs",   inst_JMP, ABS,         3},
    [0x4D] = {"EOR abs",   inst_EOR, ABS,         4},
    [0x4E] = {"LSR abs",   inst_LSR, ABS,         6},
    [0x4F] = {"???",       inst_NOP, IMPL,        6},
    [0x50] = {"BVC rel",   inst_BVC, REL,         2},
    [0x51] = {"EOR ind,Y", inst_EOR, INDY,        5},
    [0x52] = {"???",       inst_NOP, IMPL,        2},
    [0x53] = {"???",       inst_NOP, IMPL,        8},
    [0x54] = {"???",       inst_NOP, ZP,          4},
    [0x55] = {"EOR zpg,X", inst_EOR, ZPX,         4},
    [0x56] = {"LSR zpg,X", inst_LSR, ZPX,         6},
    [0x57] = {"???",       inst_NOP, IMPL,        6},
    [0x58] = {"CLI impl",  inst_CLI, IMPL,        2},
    [0x59] = {"EOR abs,Y", inst_EOR, ABSY,        4},
    [0x5A] = {"???",       inst_NOP, IMPL,        2},
    [0x5B] = {"???",       inst_NOP, IMPL,        7},
    [0x5C] = {"???",       inst_NOP, ABSX,        4},
    [0x5D] = {"EOR abs,X", inst_EOR, ABSX,        4},
    [0x5E] = {"LSR abs,X", inst_LSR, ABSX,        7},
    [0x5F] = {"???",       inst_NOP, IMPL,        7},
    [0x60] = {"RTS impl",  inst_RTS, IMPL,        6},
    [0x61] = {"ADC X,ind", inst_ADC, XIND,        6},
    [0x62] = {"???",       inst_NOP, IMPL,        2},
    [0x63] = {"???",       inst_NOP, IMPL,        8},
    [0x64] = {"???",       inst_NOP, ZP,          3},
    [0x65] = {"ADC zpg",   inst_ADC, ZP,          3},
    [0x66] = {"ROR zpg",   inst_ROR, ZP,          5},
    [0x67] = {"???",       inst_NOP, IMPL,        5},
    [0x68] = {"PLA impl",  inst_PLA, IMPL,        4},
    [0x69] = {"ADC #",     inst_ADC, IMM,         2},
    [0x6A] = {"ROR A",     inst_ROR, ACC,         2},
    [0x6B] = {"???",       inst_NOP, IMPL,        2},
    [0x6C] = {"JMP ind",   inst_JMP, JMP_IND_BUG, 5},
    [0x6D] = {"ADC abs",   inst_ADC, ABS,         4},
    [0x6E] = {"ROR abs",   inst_ROR, ABS,         6},
    [0x6F] = {"???",       inst_NOP, IMPL,        6},
    [0x70] = {"BVS rel",   inst_BVS, REL,         2},
    [0x71] = {"ADC ind,Y", inst_ADC, INDY,        5},
    [0x72] = {"???",       inst_NOP, IMPL,        2},
    [0x73] = {"???",       inst_NOP, IMPL,        8},
    [0x74] = {"???",       inst_NOP, ZP,          4},
    [0x75] = {"ADC zpg,X", inst_ADC, ZPX,         4},
    [0x76] = {"ROR zpg,X", inst_ROR, ZPX,         6},
    [0x77] = {"???",       inst_NOP, IMPL,        6},
    [0x78] = {"SEI impl",  inst_SEI, IMPL,        2},
    [0x79] = {"ADC abs,Y", inst_ADC, ABSY,        4},
    [0x7A] = {"???",       inst_NOP, IMPL,        2},
    [0x7B] = {"???",       inst_NOP, IMPL,        7},
    [0x7C] = {"???",       inst_NOP, ABSX,        4},
    [0x7D] = {"ADC abs,X", inst_ADC, ABSX,        4},
    [0x7E] = {"ROR abs,X", inst_ROR, ABSX,        7},
    [0x7F] = {"???",       inst_NOP, IMPL,        7},
    [0x80] = {"???",       inst_NOP, IMM,         2},
    [0x81] = {"STA X,ind", inst_STA, XIND,        6},
    [0x82] = {"???",       inst_NOP, IMPL,        2},
    [0x83] = {"???",       inst_NOP, IMPL,        6},
    [0x84] = {"STY zpg",   inst_STY, ZP,          3},
    [0x85] = {"STA zpg",   inst_STA, ZP,          3},
    [0x86] = {"STX zpg",   inst_STX, ZP,          3},
    [0x87] = {"???",       inst_NOP, IMPL,        3},
    [0x88] = {"DEY impl",  inst_DEY, IMPL,        2},
    [0x89] = {"???",       inst_NOP, IMPL,        2},
    [0x8A] = {"TXA impl",  inst_TXA, IMPL,        2},
    [0x8B] = {"???",       inst_NOP, IMPL,        2},
    [0x8C] = {"STY abs",   inst_STY, ABS,         4},
    [0x8D] = {"STA abs",   inst_STA, ABS,         4},
    [0x8E] = {"STX abs",   inst_STX, ABS,         4},
    [0x8F] = {"???",       inst_NOP, IMPL,        4},
    [0x90] = {"BCC rel",   inst_BCC, REL,         2},
    [0x91] = {"STA ind,Y", inst_STA, INDY,        6},
    [0x92] = {"???",       inst_NOP, IMPL,        2},
    [0x93] = {"???",       inst_NOP, IMPL,        6},
    [0x94] = {"STY zpg,X", inst_STY, ZPX,         4},
    [0x95] = {"STA zpg,X", inst_STA, ZPX,         4},
    [0x96] = {"STX zpg,Y", inst_STX, ZPY,         4},
    [0x97] = {"???",       inst_NOP, IMPL,        4},
    [0x98] = {"TYA impl",  inst_TYA, IMPL,        2},
    [0x99] = {"STA abs,Y", inst_STA, ABSY,        5},
    [0x9A] = {"TXS impl",  inst_TXS, IMPL,        2},
    [0x9B] = {"???",       inst_NOP, IMPL,        5},
    [0x9C] = {"???",       inst_NOP, IMPL,        5},
    [0x9D] = {"STA abs,X", inst_STA, ABSX,        5},
    [0x9E] = {"???",       inst_NOP, IMPL,        5},
    [0x9F] = {"???",       inst_NOP, IMPL,        5},
    [0xA0] = {"LDY #",     inst_LDY, IMM,         2},
    [0xA1] = {"LDA X,ind", inst_LDA, XIND,        6},
    [0xA2] = {"LDX #",     inst_LDX, IMM,         2},
    [0xA3] = {"???",       inst_NOP, IMPL,        6},
    [0xA4] = {"LDY zpg",   inst_LDY, ZP,          3},
    [0xA5] = {"LDA zpg",   inst_LDA, ZP,          3},
    [0xA6] = {"LDX zpg",   inst_LDX, ZP,          3},
    [0xA7] = {"???",       inst_NOP, IMPL,        3},
    [0xA8] = {"TAY impl",  inst_TAY, IMPL,        2},
    [0xA9] = {"LDA #",     inst_LDA, IMM,         2},
    [0xAA] = {"TAX impl",  inst_TAX, IMPL,        2},
    [0xAB] = {"???",       inst_NOP, IMPL,        2},
    [0xAC] = {"LDY abs",   inst_LDY, ABS,         4},
    [0xAD] = {"LDA abs",   inst_LDA, ABS,         4},
    [0xAE] = {"LDX abs",   inst_LDX, ABS,         4},
    [0xAF] = {"???",       inst_NOP, IMPL,        4},
    [0xB0] = {"BCS rel",   inst_BCS, REL,         2},
    [0xB1] = {"LDA ind,Y", inst_LDA, INDY,        5},
    [0xB2] = {"???",       inst_NOP, IMPL,        2},
    [0xB3] = {"???",       inst_NOP, IMPL,        5},
    [0xB4] = {"LDY zpg,X", inst_LDY, ZPX,         4},
    [0xB5] = {"LDA zpg,X", inst_LDA, ZPX,         4},
    [0xB6] = {"LDX zpg,Y", inst_LDX, ZPY,         4},
    [0xB7] = {"???",       inst_NOP, IMPL,        4},
    [0xB8] = {"CLV impl",  inst_CLV, IMPL,        2},
    [0xB9] = {"LDA abs,Y", inst_LDA, ABSY,        4},
    [0xBA] = {"TSX impl",  inst_TSX, IMPL,        2},
    [0xBB] = {"???",       inst_NOP, IMPL,        4},
    [0xBC] = {"LDY abs,X", inst_LDY, ABSX,        4},
    [0xBD] = {"LDA abs,X", inst_LDA, ABSX,        4},
    [0xBE] = {"LDX abs,Y", inst_LDX, ABSY,        4},
    [0xBF] = {"???",       inst_NOP, IMPL,        4},
    [0xC0] = {"CPY #",     inst_CPY, IMM,         2},
    [0xC1] = {"CMP X,ind", inst_CMP, XIND,        6},
    [0xC2] = {"???",       inst_NOP, IMPL,        2},
    [0xC3] = {"???",       inst_NOP, IMPL,        8},
    [0xC4] = {"CPY zpg",   inst_CPY, ZP,          3},
    [0xC5] = {"CMP zpg",   inst_CMP, ZP,          3},
    [0xC6] = {"DEC zpg",   inst_DEC, ZP,          5},
    [0xC7] = {"???",       inst_NOP, IMPL,        5},
    [0xC8] = {"INY impl",  inst_INY, IMPL,        2},
    [0xC9] = {"CMP #",     inst_CMP, IMM,         2},
    [0xCA] = {"DEX impl",  inst_DEX, IMPL,        2},
    [0xCB] = {"???",       inst_NOP, IMPL,        2},
    [0xCC] = {"CPY abs",   inst_CPY, ABS,         4},
    [0xCD] = {"CMP abs",   inst_CMP, ABS,         4},
    [0xCE] = {"DEC abs",   inst_DEC, ABS,         6},
    [0xCF] = {"???",       inst_NOP, IMPL,        6},
    [0xD0] = {"BNE rel",   inst_BNE, REL,         2},
    [0xD1] = {"CMP ind,Y", inst_CMP, INDY,        5},
    [0xD2] = {"???",       inst_NOP, IMPL,        2},
    [0xD3] = {"???",       inst_NOP, IMPL,        8},
    [0xD4] = {"???",       inst_NOP, ZP,          4},
    [0xD5] = {"CMP zpg,X", inst_CMP, ZPX,         4},
    [0xD6] = {"DEC zpg,X", inst_DEC, ZPX,         6},
    [0xD7] = {"???",       inst_NOP, IMPL,        6},
    [0xD8] = {"CLD impl",  inst_CLD, IMPL,        2},
    [0xD9] = {"CMP abs,Y", inst_CMP, ABSY,        4},
    [0xDA] = {"???",       inst_NOP, IMPL,        2},
    [0xDB] = {"???",       inst_NOP, IMPL,        7},
    [0xDC] = {"???",       inst_NOP, ABSX,        4},
    [0xDD] = {"CMP abs,X", inst_CMP, ABSX,        4},
    [0xDE] = {"DEC abs,X", inst_DEC, ABSX,        7},
    [0xDF] = {"???",       inst_NOP, IMPL,        7},
    [0xE0] = {"CPX #",     inst_CPX, IMM,         2},
    [0xE1] = {"SBC X,ind", inst_SBC, XIND,        6},
    [0xE2] = {"???",       inst_NOP, IMPL,        2},
    [0xE3] = {"???",       inst_NOP, IMPL,        8},
    [0xE4] = {"CPX zpg",   inst_CPX, ZP,          3},
    [0xE5] = {"SBC zpg",   inst_SBC, ZP,          3},
    [0xE6] = {"INC zpg",   inst_INC, ZP,          5},
    [0xE7] = {"???",       inst_NOP, IMPL,        5},
    [0xE8] = {"INX impl",  inst_INX, IMPL,        2},
    [0xE9] = {"SBC #",     inst_SBC, IMM,         2},
    [0xEA] = {"NOP impl",  inst_NOP, IMPL,        2},
    [0xEB] = {"???",       inst_NOP, IMPL,        2},
    [0xEC] = {"CPX abs",   inst_CPX, ABS,         4},
    [0xED] = {"SBC abs",   inst_SBC, ABS,         4},
    [0xEE] = {"INC abs",   inst_INC, ABS,         6},
    [0xEF] = {"???",       inst_NOP, IMPL,        6},
    [0xF0] = {"BEQ rel",   inst_BEQ, REL,         2},
    [0xF1] = {"SBC ind,Y", inst_SBC, INDY,        5},
    [0xF2] = {"???",       inst_NOP, IMPL,        2},
    [0xF3] = {"???",       inst_NOP, IMPL,        8},
    [0xF4] = {"???",       inst_NOP, ZP,          4},
    [0xF5] = {"SBC zpg,X", inst_SBC, ZPX,         4},
    [0xF6] = {"INC zpg,X", inst_INC, ZPX,         6},
    [0xF7] = {"???",       inst_NOP, IMPL,        6},
    [0xF8] = {"SED impl",  inst_SED, IMPL,        2},
    [0xF9] = {"SBC abs,Y", inst_SBC, ABSY,        4},
    [0xFA] = {"???",       inst_NOP, IMPL,        2},
    [0xFB] = {"???",       inst_NOP, IMPL,        7},
    [0xFC] = {"???",       inst_NOP, ABSX,        4},
    [0xFD] = {"SBC abs,X", inst_SBC, ABSX,        4},
    [0xFE] = {"INC abs,X", inst_INC, ABSX,        7},
    [0xFF] = {"???",       inst_NOP, IMPL,        7}
};

#endif // INCLUDE

extern CPUMAP CPU;
extern void  *read_addr;
extern void  *write_addr;

/* Reset CPU state */
void reset_cpu(int _a, int _x, int _y, int _sp, int _sr, int _pc);

/* Load ROM file into memory */
int load_rom(char *filename, int load_addr);

/* Execute an instruction */
int step_cpu(int verbose);

/* Memory dump */
void save_memory(const char *filename);

#endif // INCLUDE_6502_H_
