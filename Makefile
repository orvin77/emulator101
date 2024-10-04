# Specify the project to build, default to 'project1' if none is provided
PROJ ?= 8080_emulator

# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Source and build directories
SRC_DIR = $(PROJ)
BUILD_DIR = build

# Target binary (output binary will be named after the project)
TARGET = $(BUILD_DIR)/$(PROJ)

# Rule to build the project
all: $(TARGET)

# Build rule
$(TARGET): $(SRC_DIR)/main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Run the built project
run: $(TARGET)
	./$(TARGET)

# Clean rule to remove build files
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean
