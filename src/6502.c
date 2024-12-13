/*
 *
 *		6502.c
 *		6502处理器
 *
 *		2024/12/13 By MicroFish
 *		基于 MIT 开源协议
 *		Copyright © 2020 ViudiraTech，保留最终解释权。
 *
 */

#include "6502.h"

CPUMAP CPU;
Instruction inst;
int jumping;
void *read_addr;
void *write_addr;

/* 设置处理器状态寄存器中的符号 */
static inline void N_flag(int8_t val)
{
	CPU.SR.bits.sign = val < 0;
}

/* 设置处理器状态寄存器中的零 */
static inline void Z_flag(uint8_t val)
{
	CPU.SR.bits.zero = val == 0;
}

/* 数据入栈 */
static inline void stack_push(uint8_t val)
{
	CPU.memory[0x100+(CPU.SP--)] = val;
}

/* 数据出栈 */
static inline uint8_t stack_pull(void)
{
	return CPU.memory[0x100+(++CPU.SP)];
}

/* 获取当前指令的读取指针 */
static inline uint8_t *read_ptr(void)
{
	return (uint8_t*) (read_addr = get_ptr[inst.mode]());
}

/* 获取当前指令的写入指针 */
static inline uint8_t *write_ptr(void)
{
	return (uint8_t*) (write_addr = get_ptr[inst.mode]());
}

/* 处理条件分支跳转 */
static inline void take_branch(void)
{
	uint16_t oldPC;
	oldPC = CPU.PC + 2;
	CPU.PC = read_ptr() - CPU.memory;
	if ((CPU.PC ^ oldPC) & 0xff00) CPU.extra_cycles += 1;
	CPU.extra_cycles += 1;
}

/* ↓指令集实现↓ */

static void inst_ADC(void)
{
	uint8_t operand = *read_ptr();
	unsigned int tmp = CPU.A + operand + (CPU.SR.bits.carry & 1);
	if (CPU.SR.bits.decimal) {
		tmp = (CPU.A & 0x0f) + (operand & 0x0f) + (CPU.SR.bits.carry & 1);
		if (tmp >= 10) tmp = (tmp - 10) | 0x10;
		tmp += (CPU.A & 0xf0) + (operand & 0xf0);
		if (tmp > 0x9f) tmp += 0x60;
	}
	CPU.SR.bits.carry = tmp > 0xFF;
	CPU.SR.bits.overflow =  ((CPU.A^tmp)&(operand^tmp)&0x80) != 0;
	CPU.A = tmp & 0xFF;
	N_flag(CPU.A);
	Z_flag(CPU.A);
}

static void inst_AND(void)
{
	CPU.A &= *read_ptr();
	N_flag(CPU.A);
	Z_flag(CPU.A);
}

static void inst_ASL(void)
{
	uint8_t tmp = *read_ptr();
	CPU.SR.bits.carry = (tmp & 0x80) != 0;
	tmp <<= 1;
	N_flag(tmp);
	Z_flag(tmp);
	*write_ptr() = tmp;
}

static void inst_BCC(void)
{
	if (!CPU.SR.bits.carry) {
		take_branch();
	}
}

static void inst_BCS(void)
{
	if (CPU.SR.bits.carry) {
		take_branch();
	}
}

static void inst_BEQ(void)
{
	if (CPU.SR.bits.zero) {
		take_branch();
	}
}

static void inst_BIT(void)
{
	uint8_t tmp = *read_ptr();
	N_flag(tmp);
	Z_flag(tmp & CPU.A);
	CPU.SR.bits.overflow = (tmp & 0x40) != 0;
}

static void inst_BMI(void)
{
	if (CPU.SR.bits.sign) {
		take_branch();
	}
}

static void inst_BNE(void)
{
	if (!CPU.SR.bits.zero) {
		take_branch();
	}
}

static void inst_BPL(void)
{
	if (!CPU.SR.bits.sign) {
		take_branch();
	}
}

static void inst_BRK(void)
{
	uint16_t newPC;
	memcpy(&newPC, &CPU.memory[IRQ_VEC], sizeof(newPC));
	CPU.PC += 2;
	stack_push(CPU.PC >> 8);
	stack_push(CPU.PC & 0xFF);
	CPU.SR.bits.brk = 1;
	stack_push(CPU.SR.byte);
	CPU.SR.bits.interrupt = 1;
	CPU.PC = newPC;
	jumping = 1;
}

static void inst_BVC(void)
{
	if (!CPU.SR.bits.overflow) {
		take_branch();
	}
}

static void inst_BVS(void)
{
	if (CPU.SR.bits.overflow) {
		take_branch();
	}
}

static void inst_CLC(void)
{
	CPU.SR.bits.carry = 0;
}

static void inst_CLD(void)
{
	CPU.SR.bits.decimal = 0;
}

static void inst_CLI(void)
{
	CPU.SR.bits.interrupt = 0;
}

static void inst_CLV(void)
{
	CPU.SR.bits.overflow = 0;
}

static void inst_CMP(void)
{
	uint8_t operand = *read_ptr();
	uint8_t tmpDiff = CPU.A - operand;
	N_flag(tmpDiff);
	Z_flag(tmpDiff);
	CPU.SR.bits.carry = CPU.A >= operand;
}

static void inst_CPX(void)
{
	uint8_t operand = *read_ptr();
	uint8_t tmpDiff = CPU.X - operand;
	N_flag(tmpDiff);
	Z_flag(tmpDiff);
	CPU.SR.bits.carry = CPU.X >= operand;
}

static void inst_CPY(void)
{
	uint8_t operand = *read_ptr();
	uint8_t tmpDiff = CPU.Y - operand;
	N_flag(tmpDiff);
	Z_flag(tmpDiff);
	CPU.SR.bits.carry = CPU.Y >= operand;
}

static void inst_DEC(void)
{
	uint8_t tmp = *read_ptr();
	tmp--;
	N_flag(tmp);
	Z_flag(tmp);
	*write_ptr() = tmp;
}

static void inst_DEX(void)
{
	CPU.X--;
	N_flag(CPU.X);
	Z_flag(CPU.X);
}

static void inst_DEY(void)
{
	CPU.Y--;
	N_flag(CPU.Y);
	Z_flag(CPU.Y);
}

static void inst_EOR(void)
{
	CPU.A ^= *read_ptr();
	N_flag(CPU.A);
	Z_flag(CPU.A);
}

static void inst_INC(void)
{
	uint8_t tmp = *read_ptr();
	tmp++;
	N_flag(tmp);
	Z_flag(tmp);
	*write_ptr() = tmp;
}

static void inst_INX(void)
{
	CPU.X++;
	N_flag(CPU.X);
	Z_flag(CPU.X);
}

static void inst_INY(void)
{
	CPU.Y++;
	N_flag(CPU.Y);
	Z_flag(CPU.Y);
}

static void inst_JMP(void)
{
	CPU.PC = read_ptr() - CPU.memory;
	jumping = 1;
}

static void inst_JSR(void)
{
	uint16_t newPC = read_ptr() - CPU.memory;
	CPU.PC += 2;
	stack_push(CPU.PC >> 8);
	stack_push(CPU.PC & 0xFF);
	CPU.PC = newPC;
	jumping = 1;
}

static void inst_LDA(void)
{
	CPU.A = *read_ptr();
	N_flag(CPU.A);
	Z_flag(CPU.A);
}

static void inst_LDX(void)
{
	CPU.X = *read_ptr();
	N_flag(CPU.X);
	Z_flag(CPU.X);
}

static void inst_LDY(void)
{
	CPU.Y = *read_ptr();
	N_flag(CPU.Y);
	Z_flag(CPU.Y);
}

static void inst_LSR(void)
{
	uint8_t tmp = *read_ptr();
	CPU.SR.bits.carry = tmp & 1;
	tmp >>= 1;
	N_flag(tmp);
	Z_flag(tmp);
	*write_ptr() = tmp;
}

static void inst_NOP(void)
{
	read_ptr();
}

static void inst_ORA(void)
{
	CPU.A |= *read_ptr();
	N_flag(CPU.A);
	Z_flag(CPU.A);
}

static void inst_PHA(void)
{
	stack_push(CPU.A);
}

static void inst_PHP(void)
{
	union StatusReg pushed_sr;
	pushed_sr.byte = CPU.SR.byte;
	pushed_sr.bits.brk = 1;
	stack_push(pushed_sr.byte);
}

static void inst_PLA(void)
{
	CPU.A = stack_pull();
	N_flag(CPU.A);
	Z_flag(CPU.A);
}

static void inst_PLP(void)
{
	CPU.SR.byte = stack_pull();
	CPU.SR.bits.unused = 1;
	CPU.SR.bits.brk = 0;
}

static void inst_ROL(void)
{
	int tmp = (*read_ptr()) << 1;
	tmp |= CPU.SR.bits.carry & 1;
	CPU.SR.bits.carry = tmp > 0xFF;
	tmp &= 0xFF;
	N_flag(tmp);
	Z_flag(tmp);
	*write_ptr() = tmp;
}

static void inst_ROR(void)
{
	int tmp = *read_ptr();
	tmp |= CPU.SR.bits.carry << 8;
	CPU.SR.bits.carry = tmp & 1;
	tmp >>= 1;
	N_flag(tmp);
	Z_flag(tmp);
	*write_ptr() = tmp;
}

static void inst_RTI(void)
{
	CPU.SR.byte = stack_pull();
	CPU.SR.bits.unused = 1;
	CPU.PC = stack_pull();
	CPU.PC |= stack_pull() << 8;
	jumping = 1;
}

static void inst_RTS(void)
{
	CPU.PC = stack_pull();
	CPU.PC |= stack_pull() << 8;
	CPU.PC += 1;
	jumping = 1;
}

static void inst_SBC(void)
{
	uint8_t operand = *read_ptr();
	unsigned int tmp, lo, hi;
	tmp = CPU.A - operand - 1 + (CPU.SR.bits.carry & 1);
	CPU.SR.bits.overflow = ((CPU.A^tmp)&(CPU.A^operand)&0x80) != 0;
	if (CPU.SR.bits.decimal) {
		lo = (CPU.A & 0x0f) - (operand & 0x0f) - 1 + CPU.SR.bits.carry;
		hi = (CPU.A >> 4) - (operand >> 4);
		if (lo & 0x10) lo -= 6, hi--;
		if (hi & 0x10) hi -= 6;
		CPU.A = (hi << 4) | (lo & 0x0f);
	}
	else {
		CPU.A = tmp & 0xFF;
	}
	CPU.SR.bits.carry = tmp < 0x100;
	N_flag(CPU.A);
	Z_flag(CPU.A);
}

static void inst_SEC(void)
{
	CPU.SR.bits.carry = 1;
}

static void inst_SED(void)
{
	CPU.SR.bits.decimal = 1;
}

static void inst_SEI(void)
{
	CPU.SR.bits.interrupt = 1;
}

static void inst_STA(void)
{
	*write_ptr() = CPU.A;
	CPU.extra_cycles = 0;
}

static void inst_STX(void)
{
	*write_ptr() = CPU.X;
}

static void inst_STY(void)
{
	*write_ptr() = CPU.Y;
}

static void inst_TAX(void)
{
	CPU.X = CPU.A;
	N_flag(CPU.X);
	Z_flag(CPU.X);
}

static void inst_TAY(void)
{
	CPU.Y = CPU.A;
	N_flag(CPU.Y);
	Z_flag(CPU.Y);
}

static void inst_TSX(void)
{
	CPU.X = CPU.SP;
	N_flag(CPU.X);
	Z_flag(CPU.X);
}

static void inst_TXA(void)
{
	CPU.A = CPU.X;
	N_flag(CPU.A);
	Z_flag(CPU.A);
}

static void inst_TXS(void)
{
	CPU.SP = CPU.X;
}

static void inst_TYA(void)
{
	CPU.A = CPU.Y;
	N_flag(CPU.A);
	Z_flag(CPU.A);
}

/* ↓地址模式↓ */

static uint8_t *get_IMPL(void)
{
	return &CPU.memory[0];
}

static uint8_t *get_IMM(void)
{
	return &CPU.memory[(uint16_t) (CPU.PC+1)];
}

static uint16_t get_uint16(void)
{
	uint16_t index;
	memcpy(&index, get_IMM(), sizeof(index));
	return index;
}

static uint8_t *get_ZP(void)
{
	return &CPU.memory[*get_IMM()];
}

static uint8_t *get_ZPX(void)
{
	return &CPU.memory[((*get_IMM()) + CPU.X) & 0xFF];
}

static uint8_t *get_ZPY(void)
{
	return &CPU.memory[((*get_IMM()) + CPU.Y) & 0xFF];
}

static uint8_t *get_ACC(void)
{
	return &CPU.A;
}

static uint8_t *get_ABS(void)
{
	return &CPU.memory[get_uint16()];
}

static uint8_t *get_ABSX(void)
{
	uint16_t ptr;
	ptr = (uint16_t)(get_uint16() + CPU.X);
	if ((uint8_t)ptr < CPU.X) CPU.extra_cycles ++;
	return &CPU.memory[ptr];
}

static uint8_t *get_ABSY(void)
{
	uint16_t ptr;
	ptr = (uint16_t)(get_uint16() + CPU.Y);
	if ((uint8_t)ptr < CPU.Y) CPU.extra_cycles ++;
	return &CPU.memory[ptr];
}

static uint8_t * get_IND(void)
{
	uint16_t ptr;
	memcpy(&ptr, get_ABS(), sizeof(ptr));
	return &CPU.memory[ptr];
}

static uint8_t * get_XIND(void)
{
	uint16_t ptr;
	ptr = ((* get_IMM()) + CPU.X) & 0xFF;
	if (ptr == 0xff) {
		ptr = CPU.memory[ptr] + (CPU.memory[ptr & 0xff00] << 8);
	}
	else {
		memcpy(&ptr, &CPU.memory[ptr], sizeof(ptr));
	}
	return &CPU.memory[ptr];
}

static uint8_t * get_INDY(void)
{
	uint16_t ptr;
	ptr = * get_IMM();
	if (ptr == 0xff) {
		ptr = CPU.memory[ptr] + (CPU.memory[ptr & 0xff00] << 8);
	}
	else {
		memcpy(&ptr, &CPU.memory[ptr], sizeof(ptr));
	}
	ptr += CPU.Y;
	if ((uint8_t)ptr < CPU.Y) CPU.extra_cycles ++;
	return &CPU.memory[ptr];
}

static uint8_t *get_REL(void)
{
	return &CPU.memory[(uint16_t) (CPU.PC + (int8_t) * get_IMM())];
}

static uint8_t *get_JMP_IND_BUG(void)
{
	uint8_t *addr;
	uint16_t ptr;
	
	ptr = get_uint16();
	if ((ptr & 0xff) == 0xff) {
		ptr = CPU.memory[ptr] + (CPU.memory[ptr & 0xff00] << 8);
	}
	else {
		addr = &CPU.memory[ptr];
		memcpy(&ptr, addr, sizeof(ptr));
	}
	return &CPU.memory[ptr];
}

/* 重置CPU状态 */
void reset_cpu(int _a, int _x, int _y, int _sp, int _sr, int _pc)
{
	CPU.A = _a;
	CPU.X = _x;
	CPU.Y = _y;
	CPU.SP = _sp;

	CPU.SR.byte = _sr;
	CPU.SR.bits.interrupt = 1;
	CPU.SR.bits.unused = 1;

	if (_pc < 0)
		memcpy(&CPU.PC, &CPU.memory[-_pc], sizeof(CPU.PC));
	else
		CPU.PC = _pc;
	
	CPU.total_cycles = 0;
}

/* 加载ROM文件到内存 */
int load_rom(char *filename, int load_addr)
{
	int loaded_size, max_size;
	memset(CPU.memory, 0, sizeof(CPU.memory));

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("错误: 无法打开文件\n");
		return -1;
	}
	max_size = 0x10000 - load_addr;
	loaded_size = (int)fread(&CPU.memory[load_addr], 1, (size_t)max_size, fp);
	fprintf(stderr, "加载 $%04x 字节: $%04x - $%04x\n", loaded_size, load_addr, load_addr + loaded_size - 1);
	fclose(fp);
	return 0;
}

/* 执行一条指令 */
int step_cpu(int verbose)
{
	inst = instructions[CPU.memory[CPU.PC]];
	if (verbose) {
		printf("%04X  ", CPU.PC);
		if (lengths[inst.mode] == 3)
			printf("%02X %02X %02X", CPU.memory[CPU.PC], CPU.memory[CPU.PC+1], CPU.memory[CPU.PC+2]);
		else if (lengths[inst.mode] == 2)
			printf("%02X %02X   ", CPU.memory[CPU.PC], CPU.memory[CPU.PC+1]);
		else
			printf("%02X      ", CPU.memory[CPU.PC]);
		printf("  %-10s               A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d\n", inst.mnemonic, CPU.A, CPU.X, CPU.Y, CPU.SR.byte, CPU.SP, (int)((CPU.total_cycles * 3) % 341));
	}
	jumping = 0;
	CPU.extra_cycles = 0;
	inst.function();
	if (jumping == 0) CPU.PC += lengths[inst.mode];
	if (inst.cycles == 7) CPU.extra_cycles = 0;
	CPU.total_cycles += inst.cycles + CPU.extra_cycles;
	return inst.cycles + CPU.extra_cycles;
}

/* 内存转储 */
void save_memory(const char *filename)
{
	if (filename == NULL) filename = "memdump";
	FILE *fp = fopen(filename, "w");
	fwrite(&CPU.memory, sizeof(CPU.memory), 1, fp);
	fclose(fp);
}
