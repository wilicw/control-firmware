BUILD_DIR = build
DEBUGGER := daplink

all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -GNinja
	cd $(BUILD_DIR) && ninja
	@-mv $(BUILD_DIR)/compile_commands.json .

clean:
	rm -rf $(BUILD_DIR)

flash:
	@-openocd -f $(shell pwd)/openocd_$(DEBUGGER).cfg -c "program $(BUILD_DIR)/fsae-2024.elf verify reset exit"

.PHONY: all clean flash
