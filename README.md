# MOS 6502 Simulator

This is an open-source project that emulates the MOS Technology 6502 microprocessor and its companion 6850 UART controller. The project was released under the MIT open source license and was written by MicroFish on December 13, 2024.

## Features

- Emulates the full instruction set of the 6502 processor.
- Supports analog serial communication via the 6850 UART controller.
- Ability to load ROM files into the emulator's memory.
- Provides a memory dump function.
- Interactive mode is supported, allowing users to enter data at runtime.

## Build & Run

To build and run the emulator, you'll need a compiler that supports C, and a make builder. Here's a brief description of the build process:

1. Clone the project to the local computer:

   ```sh
   git clone https://github.com/ViudiraTech/Sim6502.git
   ```

2. Use make to compile the entire project:

   ```sh
   make
   ```

3. Run the compiled executable file and load the ROM file and set parameters as needed. Or do "make test" to test the ready-made Wozmon.bin

## Directions for use

The emulator accepts several command-line arguments to control its behavior. Here are some of the available parameters:

- `-a`, `-x`, `-y`, `-s`, `-p`:Set the initial values for the A register, the X register, the Y register, the stack pointer, and the processor status register, respectively.
- `-r`, `-g`：Set the default running address.
- `-v`:CPU information is printed at each operation.
- `-i`:Connect stdin/stdout to the emulator.
- `-b`:Stops when the PC reaches the specified address, dumps memory, and then exits.
- `-c`:Stops after the specified period.
- `-f`:Run at maximum speed as much as possible with no delayed loops.
- `-l`:Set the loading address for the ROM file.

## File structure

- `Sim6502.c`:The main program of the emulator.
- `6850.c` & `6850.h`:Simulation of the 6850 UART controller.
- `6502.c` & `6502.h`:Simulation of the 6502 processor.

## Copyright Notice

Copyright © 2020 ViudiraTech, based on the MIT agreement.
