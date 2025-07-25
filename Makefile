# =====================================================
#
#      Makefile
#      Sim6502 compile script
#
#      2024/12/13 By MicroFish
#      Based on MIT open source agreement
#      Copyright © 2020 ViudiraTech, based on the MIT agreement.
#
# =====================================================

GCC        = gcc

CFLAGS     = -Wall -O3
LDFLAGS    = -O3

C_SOURCES := $(shell find * -name "*.c")
S_SOURCES := $(shell find * -name "*.s")
HEADERS   := $(shell find * -name "*.h")

SRC_DIR    = ./src/
OBJ       := $(SRC_DIR)Sim6502.o $(SRC_DIR)6502.o $(SRC_DIR)6850.o

TARGET     = Sim6502

all: info $(TARGET) done

%.fmt: %
	@printf "\033[1;32m[Format]\033[0m $< ...\n"
	@clang-format -i $<

info:
	@printf "Sim6502 Compile Script.\n"
	@printf "Copyright 2020 ViudiraTech. All rights interpretation reserved.\n"
	@printf "Based on the MIT open source license.\n"
	@echo

$(TARGET): $(OBJ)
	$(GCC) $(LDFLAGS) -o $@ $^

done:
	@printf "\n\033[1;32m[Done]\033[0m Compilation complete.\n"

format: $(C_SOURCES:%=%.fmt) $(S_SOURCES:%=%.fmt) $(HEADERS:%=%.fmt)
	@printf "\033[1;32m[Done]\033[0m Code Format complete.\n\n"

clean:
	rm -f $(TARGET) $(OBJ)

test: $(TARGET)
	./$(TARGET) -i roms/wozmon.bin
