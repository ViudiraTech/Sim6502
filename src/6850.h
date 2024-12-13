/*
 *
 *		6850.h
 *		UART控制器头文件
 *
 *		2024/12/13 By MicroFish
 *		基于 MIT 开源协议
 *		Copyright © 2020 ViudiraTech，保留最终解释权。
 *
 */

#ifndef INCLUDE_6850_H_
#define INCLUDE_6850_H_

#include <stdbool.h>

#define CTRL_ADDR 0xA000 // 控制地址
#define DATA_ADDR 0xA001 // 数据地址

/* UART状态位 */
struct UartStatusBits {
	bool RDRF:1;
	bool TDRE:1;
	bool DCD:1;
	bool CTS:1;
	bool FE:1;
	bool OVRN:1;
	bool PE:1;
	bool IRQ:1;
};

/* UART状态寄存器 */
union UartStatusReg {
	struct UartStatusBits bits;
	uint8_t byte;
};

/* 初始化UART */
void init_uart(int is_interactive);

/* 模拟UART读写操作 */
void step_uart(void);

#endif // INCLUDE_6850_H_
