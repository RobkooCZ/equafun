# Compiler and default flags
CC := gcc
CFLAGS := -std=c23 -Wall -MMD -MP
LIBS := -lglfw -lGL -lpthread -lXrandr -lXi -ldl -lfreetype -lm
INCLUDE_PATHS := -Iinclude -Ilibs/include $(shell pkg-config --cflags freetype2 | awk '{print $1}')

# Directories
SRC_DIR := src
LIBS_SRC_DIR := libs/src
BUILD_DIR := build

# Source files
SRCS := $(shell find $(SRC_DIR) -name "*.c")
LIB_SRCS := $(shell find $(LIBS_SRC_DIR) -name "*.c")

# Object files (internal)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
# Object files (external)
OBJS += $(LIB_SRCS:$(LIBS_SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Final executable
EXEC := $(BUILD_DIR)/equafun

# Default target
all: Release

# Debug build type
Debug: CFLAGS += -g -Og -Wextra -D_DEBUG_NEABLE
Debug: $(EXEC)

# Release build type
Release: CFLAGS += -O3 -funroll-loops
Release: $(EXEC)

# MinSizeRel build type
MinSizeRel: CFLAGS += -Os
MinSizeRel: $(EXEC)

# Flags build type
Flags: CFLAGS += -O2 -Wextra -Werror -Wpedantic -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wconversion -Wuninitialized -Wno-unused-function -Wnull-dereference -Wdouble-promotion -Wfloat-equal -Wlogical-op -Wunreachable-code -Wvla -Wmissing-prototypes -Wredundant-decls -Wformat=2 -Wformat-nonliteral -Wformat-security -Wno-missing-field-initializers -Wstrict-aliasing -Wstrict-prototypes -Wunused-variable -Wunused-parameter -Wunused-but-set-variable -Wcast-align -Wno-implicit-fallthrough -Wdeclaration-after-statement -Winline -Wunsafe-loop-optimizations -Wstack-protector -fstack-protector-all -D_FORTIFY_SOURCE=2 -fno-common -fwrapv -fno-strict-aliasing -fno-builtin -ffast-math -funroll-loops -fno-omit-frame-pointer -fstack-check -g -flto -march=native -mtune=native -fno-omit-frame-pointer -fvisibility=hidden -fsanitize=address -fsanitize=undefined -fsanitize=leak -fno-inline
Flags: $(EXEC)

# Linking the object files into the final executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LIBS)

# Build glad.c without pedantic warnings
$(BUILD_DIR)/glad/glad.o: $(LIBS_SRC_DIR)/glad/glad.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Wno-pedantic $(INCLUDE_PATHS) -c $< -o $@

# Compilation rule for each internal source file (src/)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Compilation rule for each external source file (lib/src/)
$(BUILD_DIR)/%.o: $(LIBS_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

-include $(OBJS:.o=.d)

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

# Copy assets
assets:
	rm -rf $(BUILD_DIR)/data
	cp -r data/ $(BUILD_DIR)/

.PHONY: all clean Debug Release MinSizeRel Flags assets
