CC = gcc
CFLAGS = -std=c23 -O2 -g -Wall -Werror -MMD -MP

# Libraries
LIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

# Include paths for internal includes and external includes
INCLUDE_PATHS = -Iinclude -Ilibs/include

# Directories
SRC_DIR = src
LIBS_SRC_DIR = libs/src
BUILD_DIR = build

# Source files
SRCS = $(SRC_DIR)/main.c $(wildcard $(SRC_DIR)/**/*.c)
LIB_SRCS = $(wildcard $(LIBS_SRC_DIR)/**/*.c)

# Object files (internal)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
# Object files (external)
OBJS += $(LIB_SRCS:$(LIBS_SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Final executable
EXEC = $(BUILD_DIR)/equafun

# Default target (compiling the executable)
all: $(EXEC)

# Linking the object files into the final executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LIBS)

# Compilation rule for each internal source file (src/)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Compilation rule for each external source file (lib/src/)
$(BUILD_DIR)/%.o: $(LIBS_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
