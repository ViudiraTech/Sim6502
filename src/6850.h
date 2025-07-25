/*
 *
 *      6850.h
 *      UART controller header file
 *
 *      2024/12/13 By MicroFish
 *      Based on MIT open source agreement
 *      Copyright © 2020 ViudiraTech, based on the MIT agreement.
 *
 */

#ifndef INCLUDE_6850_H_
#define INCLUDE_6850_H_

#include <stdbool.h>

#define CTRL_ADDR 0xA000 // Control address
#define DATA_ADDR 0xA001 // Data address

/* UART Status Bits */
struct UartStatusBits {
        bool RDRF : 1;
        bool TDRE : 1;
        bool DCD  : 1;
        bool CTS  : 1;
        bool FE   : 1;
        bool OVRN : 1;
        bool PE   : 1;
        bool IRQ  : 1;
};

/* UART Status Register */
union UartStatusReg {
        struct UartStatusBits bits;
        uint8_t               byte;
};

/* Initialize UART */
void init_uart(int is_interactive);

/* Simulate UART read and write operations */
void step_uart(void);

#endif // INCLUDE_6850_H_
