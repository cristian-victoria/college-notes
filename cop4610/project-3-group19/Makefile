# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I./include
LDFLAGS =

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .
INCLUDE_DIR = include

# Target executable
TARGET = $(BIN_DIR)/filesys

# Source files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/lexer.c $(SRC_DIR)/fat32.c

# Object files
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/lexer.o $(OBJ_DIR)/fat32.o

# Default target
all: $(TARGET)

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile main.c
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(INCLUDE_DIR)/lexer.h $(INCLUDE_DIR)/fat32.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

# Compile lexer.c
$(OBJ_DIR)/lexer.o: $(SRC_DIR)/lexer.c $(INCLUDE_DIR)/lexer.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/lexer.c -o $(OBJ_DIR)/lexer.o

# Compile fat32.c
$(OBJ_DIR)/fat32.o: $(SRC_DIR)/fat32.c $(INCLUDE_DIR)/fat32.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/fat32.c -o $(OBJ_DIR)/fat32.o

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)
	@echo "Clean complete"

# Phony targets
.PHONY: all clean