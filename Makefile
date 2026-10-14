CC := gcc
CCFLAGS := -Wall -Wextra -Iinclude
DEBUGFLAGS := -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer

SRC_DIR := src
BUILD_DIR := build
TARGET := tack

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) -o $@ $^

debug: CCFLAGS += $(DEBUGFLAGS)
debug: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

run: all
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean
