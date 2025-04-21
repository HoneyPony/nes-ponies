include .config

CC=$(LLVMPATH)/bin/mos-nes-clang
AS=$(LLVMPATH)/bin/llvm-mc
LD=$(LLVMPATH)/bin/ld.lld

BIN= main.nes

BUILD_DIR= ./build
SRC_DIR = ./src

SRCS=\
main.c\
map.c\
palette.c\
player.c\
graphics.s\
nmi.s\
sprite_ram.s\
poll_input.s\
prng.s



$(BIN): $(SRCS:%=$(BUILD_DIR)/%.o)
	$(LD) $^ -o $(BIN)
#	ld65 $^ -C nes.cfg -o $(BIN) --lib smallnes.lib

$(BUILD_DIR)/%.s.o : $(SRC_DIR)/%.s
	mkdir -p $(BUILD_DIR)
	$(AS) $< -o $@ -triple mos --filetype=obj
#	ca65 $< -o $@
	
$(BUILD_DIR)/%.c.o : $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c $< -o $@
#	$(AS) $@.ir -o $@ -triple mos
#	cc65 $< -o $@.s -t nes
#	ca65 $@.s -o $@
	
clean:
	find . -name "*.o" -delete