/*
 *
 *      6850.c
 *      UART Controller
 *
 *      2024/12/13 By MicroFish
 *      Based on MIT open source agreement
 *      Copyright © 2020 ViudiraTech, based on the MIT agreement.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>

#define INCLUDE
#include "6502.h"
#include "6850.h"

static int                 n;
static union UartStatusReg uart_SR;
static uint8_t             incoming_char;
static int                 interactive;

/* Initialize UART */
void init_uart(int is_interactive)
{
    CPU.memory[DATA_ADDR] = 0;

    uart_SR.byte      = 0;
    uart_SR.bits.TDRE = 1;

    uart_SR.bits.RDRF = 0;
    incoming_char     = 0;

    interactive = is_interactive;
}

/* Ready to read data */
int stdin_ready(void)
{
    struct pollfd fds;
    fds.fd     = 0;
    fds.events = POLLIN;
    return poll(&fds, 1, 0) == 1;
}

/* Simulate UART read and write operations */
void step_uart(void)
{
    if (write_addr == &CPU.memory[DATA_ADDR]) {
        putchar(CPU.memory[DATA_ADDR]);
        if (CPU.memory[DATA_ADDR] == '\b') printf(" \b");
        fflush(stdout);
        write_addr = NULL;
    } else if (read_addr == &CPU.memory[DATA_ADDR]) {
        uart_SR.bits.RDRF = 0;
        read_addr         = NULL;
    }
    if ((n++ % 100) == 0) {
        if (!uart_SR.bits.RDRF && stdin_ready()) {
            if (read(0, &incoming_char, 1) != 1) printf("Warning: read() returns 0\n");
            if (interactive) {
                if (incoming_char == 0x18) {
                    printf("\r\n");
                    exit(0);
                }
                if (incoming_char == 0x7F) { incoming_char = '\b'; }
            }
            uart_SR.bits.RDRF = 1;
        }
    }
    CPU.memory[DATA_ADDR] = incoming_char;
    CPU.memory[CTRL_ADDR] = uart_SR.byte;
}
