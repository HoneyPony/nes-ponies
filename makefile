BIN= main.nes

BUILD_DIR= ./build
SRC_DIR = ./src

SRCS=\
test.c\
graphics.s\
sprite_ram.s

$(BIN): $(SRCS:%=$(BUILD_DIR)/%.o)
	ld65 $^ -C nes.cfg -o $(BIN) --lib nes.lib

$(BUILD_DIR)/%.s.o : $(SRC_DIR)/%.s
	mkdir -p $(BUILD_DIR)
	ca65 $< -o $@
	
$(BUILD_DIR)/%.c.o : $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	cc65 $< -o $@.s -t nes
	ca65 $@.s -o $@
	