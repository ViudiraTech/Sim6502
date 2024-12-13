GCC		= gcc

CFLAGS	= -Wall -O3
LDFLAGS	= -O3

SRC_DIR	= ./src/
OBJ		:= $(SRC_DIR)Sim6502.o $(SRC_DIR)6502.o $(SRC_DIR)6850.o

TARGET	= Sim6502

all: info $(TARGET) done

info:
	@printf "Sim6502 Compile Script.\n"
	@printf "Copyright 2020 ViudiraTech. All rights interpretation reserved.\n"
	@printf "Based on the MIT open source license.\n"
	@echo

$(TARGET): $(OBJ)
	$(GCC) $(LDFLAGS) -o $@ $^

done:
	@printf "\n\033[1;32m[Done]\033[0m Compilation complete.\n"

clean:
	rm -f $(TARGET) $(OBJ)

test: $(TARGET)
	./$(TARGET) -i roms/wozmon.bin
