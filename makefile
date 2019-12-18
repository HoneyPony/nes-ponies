BIN= main.nes

BUILD_DIR= ./build
SRC_DIR = ./src

SRCS=\
main.s

$(BIN): $(SRCS:%=$(BUILD_DIR)/%.o)
	ld65 $^ -t nes -o $(BIN)

$(BUILD_DIR)/%.s.o: $(SRC_DIR)/%.s
	mkdir -p $(BUILD_DIR)
	ca65 $< -o $@
	
$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	cc65 $< -o $@.s
	ca65 $@.s -o $@
	