BUILD_DIR = build

all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -GNinja
	cd $(BUILD_DIR) && ninja
	@-mv $(BUILD_DIR)/compile_commands.json .

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
