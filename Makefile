BUILD_DIR = build

all: $(BUILD_DIR)/build.ninja
	cmake --build $(BUILD_DIR)

$(BUILD_DIR)/build.ninja:
	mkdir -p $(BUILD_DIR)
	cmake -B $(BUILD_DIR) -G Ninja -S .

clean:
	rm -rf $(BUILD_DIR)

re:
	rm -rf $(BUILD_DIR)
	cmake -B $(BUILD_DIR) -G Ninja -S .
	cmake --build $(BUILD_DIR)

run:
	./$(BUILD_DIR)/osm_cpp_demo
