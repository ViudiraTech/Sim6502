/*
 *
 *		Sim6502.c
 *		Sim6502主程序
 *
 *		2024/12/13 By MicroFish
 *		基于 MIT 开源协议
 *		Copyright © 2020 ViudiraTech，保留最终解释权。
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define INCLUDE
#include "6502.h"
#include "6850.h"

struct termios initial_termios;

/* 控制模拟的延迟 */
void step_delay(void)
{
	struct timespec req, rem;

	req.tv_sec = 0;
	req.tv_nsec = STEP_DURATION;

	nanosleep(&req, &rem);
}

/* 运行CPU模拟 */
void run_cpu(uint64_t cycle_stop, int verbose, int mem_dump, int break_pc, int fast)
{
	uint64_t cycles = 0;
	uint64_t cycles_per_step = (CPU_FREQ / (ONE_SECOND / STEP_DURATION));
	for (;;) {
		for (cycles %= cycles_per_step; cycles < cycles_per_step;) {
			if (mem_dump) save_memory(NULL);
			cycles += step_cpu(verbose);
			if ((cycle_stop > 0) && (CPU.total_cycles >= cycle_stop)) goto end;
			step_uart();
			if (break_pc >= 0 && CPU.PC == (uint16_t)break_pc) {
				fprintf(stderr, "break at %04x\n", break_pc);
				save_memory(NULL);
				goto end;
			}
		}
		if (!fast) step_delay();
	}
end:
	return;
}

/* 恢复终端的原始配置 */
void restore_stdin(void)
{
	tcsetattr(0, TCSANOW, &initial_termios);
}

/* 将终端设置为原始模式 */
void raw_stdin(void)
{
	struct termios new_termios;

	tcgetattr(0, &initial_termios);
	new_termios = initial_termios;
	cfmakeraw(&new_termios);
	tcsetattr(0, TCSANOW, &new_termios);
	atexit(restore_stdin);
}

/* 将十六进制字符串转换为整数 */
int hex2int(char *str)
{
	int val;

	if (*str == '$') str++;
	val = strtol(str, NULL, 16);
	return val;
}

/* 程序使用说明 */
void usage(char *argv[])
{
	fprintf(stderr, "使用方法: %s [参数] 文件\n"
			"模拟一个MOS-6502处理器\n"
			"\n参数:\n"
			"  CPU初始化 (使用HEX指定所有值; $nn、0xNN、等.)\n"
			"	-a HEX 设定 A 寄存器 (默认为0)\n"
			"	-x HEX 设定 X 寄存器 (默认为0)\n"
			"	-y HEX 设定 Y 寄存器 (默认为 0)\n"
			"	-s HEX 设定栈指针(默认为$ff)\n"
			"	-p HEX 设定处理器状态寄存器(默认为0)\n"
			"	-r ADDR 设定默认运行地址(默认: 在RST_VEC上加载)\n"
			"\n  模拟器控制参数\n"
			"	-v 在每一个操作时打印CPU信息\n"
			"	-i 向模拟器连接stdin/stdout\n"
			"	-b ADDR 在PC到达此地址时停止, 转储内存, 随后退出\n"
			"	-c NUM 在数字的周期后停止 (默认: 永不)\n"
			"	-f 尽可能以最大速度运行; 没有延迟循环\n"
			"\n  内存初始化\n"
			"	-l ADDR 为ROM文件加载地址(默认为 $c000)\n"
			"	FILE 加载二进制文件\n"
			, argv[0]);
}

/* 程序入口 */
int main(int argc, char *argv[])
{
	int a, x, y, sp, sr, pc, load_addr;
	int verbose, interactive, mem_dump, break_pc, fast;
	uint64_t cycles;
	int opt;

	verbose = 0;
	interactive = 0;
	mem_dump = 0;
	cycles = 0;
	load_addr = 0xC000;
	break_pc = -1;
	fast = 0;
	a = 0;
	x = 0;
	y = 0;
	sp = 0xFF;
	sr = 0;
	pc = -RST_VEC;
	while ((opt = getopt(argc, argv, "hvimfa:b:x:y:r:p:s:g:c:l:")) != -1) {
		switch (opt) {
		case 'v':
			verbose = 1;
			break;
		case 'i':
			interactive = 1;
			break;
		case 'm':
			mem_dump = 1;
			break;
		case 'f':
			fast = 1;
			break;
		case 'b':
			break_pc = hex2int(optarg);
			break;
		case 'a':
			a = hex2int(optarg);
			break;
		case 'x':
			x = hex2int(optarg);
			break;
		case 'y':
			y = hex2int(optarg);
			break;
		case 's':
			sp = hex2int(optarg);
			break;
		case 'p':
			sr = hex2int(optarg);
			break;
		case 'r':
		case 'g':
			pc = hex2int(optarg);
			break;
		case 'c':
			cycles = atol(optarg);
			break;
		case 'l':
			load_addr = hex2int(optarg);
			break;
		case 'h':
		default:
			usage(argv);
			exit(EXIT_FAILURE);
		}
	}
	if (optind >= argc) {
		usage(argv);
		exit(EXIT_FAILURE);
	}
	if (load_rom(argv[optind], load_addr) != 0) {
		printf("加载时出错 \"%s\".\n", argv[optind]);
		return EXIT_FAILURE;
	}
	if (interactive) {
		printf("*** 进入交互模式, CTRL+X 退出 ***\n\n");
		raw_stdin();
	}
	init_uart(interactive);
	reset_cpu(a, x, y, sp, sr, pc);
	run_cpu(cycles, verbose, mem_dump, break_pc, fast);
	return EXIT_SUCCESS;
}
