BUILD_DIR = build
TARGET_NAME = MiProyectoSFML

ifeq ($(OS),Windows_NT)
    CMAKE_GENERATOR = "Unix Makefiles"
    EXECUTABLE = $(BUILD_DIR)/bin/$(TARGET_NAME).exe
else
    CMAKE_GENERATOR = "Unix Makefiles"
    EXECUTABLE = $(BUILD_DIR)/bin/$(TARGET_NAME)
endif

.PHONY: all configure build run clean

all: build

configure:
	cmake -B $(BUILD_DIR) -G $(CMAKE_GENERATOR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=make

build: configure
	cmake --build $(BUILD_DIR) --config Release -j 4

run: build
	./$(EXECUTABLE)

clean:
	cmake -E rm -rf $(BUILD_DIR)