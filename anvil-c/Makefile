CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./include
LDFLAGS = 

SRC_DIR = src
BUILD_DIR = build
TARGET = dsa_editor_demo

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/stats.c $(SRC_DIR)/queue.c $(SRC_DIR)/trie.c $(SRC_DIR)/stack.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean run

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
