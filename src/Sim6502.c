/*
 *
 *      Sim6502.c
 *      Sim6502 main program
 *
 *      2024/12/13 By MicroFish
 *      Based on MIT open source agreement
 *      Copyright © 2020 ViudiraTech, based on the MIT agreement.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define INCLUDE
#include "6502.h"
#include "6850.h"

struct termios initial_termios;

/* Controlling the delay of the simulation */
void step_delay(void)
{
    struct timespec req, rem;

    req.tv_sec  = 0;
    req.tv_nsec = STEP_DURATION;

    nanosleep(&req, &rem);
}

/* Running CPU simulation */
void run_cpu(uint64_t cycle_stop, int verbose, int mem_dump, int break_pc, int fast)
{
    uint64_t cycles          = 0;
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

/* Restore the terminal to its original configuration */
void restore_stdin(void)
{
    tcsetattr(0, TCSANOW, &initial_termios);
}

/* Set the terminal to raw mode */
void raw_stdin(void)
{
    struct termios new_termios;

    tcgetattr(0, &initial_termios);
    new_termios = initial_termios;
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
    atexit(restore_stdin);
}

/* Convert hexadecimal string to integer */
int hex2int(char *str)
{
    int val;

    if (*str == '$') str++;
    val = strtol(str, NULL, 16);
    return val;
}

/* Program Instructions */
void usage(char *argv[])
{
    fprintf(stderr,
            "Usage: %s [parameters] file\n"
            "Simulate a MOS-6502 processor\n"
            "\nparameter:\n"
            "  CPU initialization (use HEX to specify all values; $nn, 0xNN, etc.)\n"
            "	-a HEX set A register (default is 0)\n"
            "	-x HEX Set X register (default is 0)\n"
            "	-y HEX set Y register (default is 0)\n"
            "	-s HEX Set stack pointer (default is $ff)\n"
            "	-p HEX Set processor status register (default is 0)\n"
            "	-r ADDR Set the default run address (default: load on RST_VEC)\n"
            "\n  Simulator Control Parameters\n"
            "	-v Print CPU information for each operation\n"
            "	-i connect stdin/stdout to the emulator\n"
            "	-b ADDR Stop when the PC reaches this address, dump memory, and then exit\n"
            "	-c NUM Stop after NUM periods (default: never)\n"
            "	-f Run at maximum speed possible; no delay loop\n"
            "\n  Memory initialization\n"
            "	-l ADDR is the ROM file loading address (default is $c000)\n"
            "	FILE Load binary file\n",
            argv[0]);
}

/* Program entry */
int main(int argc, char *argv[])
{
    int      a, x, y, sp, sr, pc, load_addr;
    int      verbose, interactive, mem_dump, break_pc, fast;
    uint64_t cycles;
    int      opt;

    verbose     = 0;
    interactive = 0;
    mem_dump    = 0;
    cycles      = 0;
    load_addr   = 0xC000;
    break_pc    = -1;
    fast        = 0;
    a           = 0;
    x           = 0;
    y           = 0;
    sp          = 0xFF;
    sr          = 0;
    pc          = -RST_VEC;
    while ((opt = getopt(argc, argv, "hvimfa:b:x:y:r:p:s:g:c:l:")) != -1) {
        switch (opt) {
            case 'v' :
                verbose = 1;
                break;
            case 'i' :
                interactive = 1;
                break;
            case 'm' :
                mem_dump = 1;
                break;
            case 'f' :
                fast = 1;
                break;
            case 'b' :
                break_pc = hex2int(optarg);
                break;
            case 'a' :
                a = hex2int(optarg);
                break;
            case 'x' :
                x = hex2int(optarg);
                break;
            case 'y' :
                y = hex2int(optarg);
                break;
            case 's' :
                sp = hex2int(optarg);
                break;
            case 'p' :
                sr = hex2int(optarg);
                break;
            case 'r' :
            case 'g' :
                pc = hex2int(optarg);
                break;
            case 'c' :
                cycles = atol(optarg);
                break;
            case 'l' :
                load_addr = hex2int(optarg);
                break;
            case 'h' :
            default :
                usage(argv);
                exit(EXIT_FAILURE);
        }
    }
    if (optind >= argc) {
        usage(argv);
        exit(EXIT_FAILURE);
    }
    if (load_rom(argv[optind], load_addr) != 0) {
        printf("Error loading \"%s\".\n", argv[optind]);
        return EXIT_FAILURE;
    }
    if (interactive) {
        printf("*** Enter interactive mode, CTRL+X to exit ***\n\n");
        raw_stdin();
    }
    init_uart(interactive);
    reset_cpu(a, x, y, sp, sr, pc);
    run_cpu(cycles, verbose, mem_dump, break_pc, fast);
    return EXIT_SUCCESS;
}
