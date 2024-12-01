# Compiler
CC = gcc
CFLAGS = -lm
# -Wall -Wextra -O2 -std=c99

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source and Header Files
SRC_FILES = $(SRC_DIR)/main.c \
            $(SRC_DIR)/graph.c \
            $(SRC_DIR)/recommendation.c \
            $(SRC_DIR)/neuralnetwork.c

HEADERS = $(SRC_DIR)/graph.h \
          $(SRC_DIR)/recommendation.h \
          $(SRC_DIR)/neuralnetwork.h

# Output
TARGET = $(BUILD_DIR)/a

# Build Rules
all: $(TARGET)

$(TARGET): $(SRC_FILES) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean Rule
clean:
	rm -rf $(BUILD_DIR)

# Run Rule
run: $(TARGET)
	$(TARGET)

.PHONY: all clean run

