BUILD_DIR = build
TARGET := stm32f407vgtx
RTT_ADDR := 0x2000097c

all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -GNinja
	cd $(BUILD_DIR) && ninja
	@-mv $(BUILD_DIR)/compile_commands.json .

clean:
	rm -rf $(BUILD_DIR)

flash:
	@-pyocd flash -t $(TARGET) $(BUILD_DIR)/fsae-2024.elf

monitor:
	@-pyocd rtt -t $(TARGET) -a $(RTT_ADDR)

.PHONY: all clean flash monitor
