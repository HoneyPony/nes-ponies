include .config

CC=$(LLVMPATH)/bin/mos-nes-nrom-clang

BIN=main.nes

BUILD_DIR=./build
SRC_DIR=./src

SRCS= \
	main.c \
	map.c \
	palette.c \
	player.c \
	graphics.s \
	nmi.s \
	prng.s \
	sprite_ram.s \
	poll_input.s


$(BIN): $(SRCS:%=$(BUILD_DIR)/%.o)
	@$(CC) $^ -o $(BIN) -Os
	@echo "link  $@"

$(BUILD_DIR)/%.s.o : $(SRC_DIR)/%.s | $(BUILD_DIR)/
	@$(CC) -c $< -o $@
	@echo "cc    $@"

%/:
	@mkdir -p $@
	@echo "mkdir $@"
	
$(BUILD_DIR)/%.c.o : $(SRC_DIR)/%.c | $(BUILD_DIR)/
	@$(CC) -MD -c $< -o $@ -Os
	@echo "as    $@"
	
define clean-dir
	@rm -rf $(1)
	@echo "rmdir $(1)"
endef
define clean-file
	@rm -f $(1)
	@echo "rm    $(1)"
endef

clean:
	$(call clean-dir,build)
	$(call clean-file,main.nes)
	$(call clean-file,main.nes.elf)
	$(call clean-file,main.mlb)

.PHONY: clean

-include $(filter %.c.d,$(SRCS:%.c=build/%.c.d))